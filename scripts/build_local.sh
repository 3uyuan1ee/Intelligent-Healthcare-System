#!/bin/bash

# 本地开发构建脚本
# 支持构建本地开发版本的服务器

set -e

# 颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# 日志函数
log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# 项目根目录
PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SERVER_DIR="$PROJECT_DIR/Server"
DATABASE_DIR="$PROJECT_DIR/database"

# 加载环境变量
ENV_FILE="$PROJECT_DIR/.env"
if [ -f "$ENV_FILE" ]; then
    export $(cat "$ENV_FILE" | grep -v '^#' | xargs)
else
    echo "错误: 找不到.env文件，请从.env.example复制并配置"
    exit 1
fi

# 检查必要的环境变量
if [ -z "$DB_PASSWORD" ]; then
    echo "错误: DB_PASSWORD未在.env文件中设置"
    exit 1
fi

# 检查必要工具
check_dependencies() {
    log_info "检查构建依赖..."

    local missing_deps=()

    # 检查编译器
    if ! command -v g++ &> /dev/null && ! command -v clang++ &> /dev/null; then
        missing_deps+=("g++ 或 clang++")
    fi

    # 检查 pkg-config
    if ! command -v pkg-config &> /dev/null; then
        missing_deps+=("pkg-config")
    fi

    # 在macOS上检查brew安装的包
    if [[ "$OSTYPE" == "darwin"* ]]; then
        # 检查 MySQL
        if ! brew list mysql &> /dev/null && ! [ -d "/usr/local/mysql" ]; then
            missing_deps+=("mysql")
        fi

        # 检查 Boost
        if ! brew list boost &> /dev/null; then
            missing_deps+=("boost")
        fi

        # 检查 nlohmann-json
        if ! brew list nlohmann-json &> /dev/null; then
            missing_deps+=("nlohmann-json")
        fi
    else
        # Linux系统的检查
        if ! pkg-config --exists libmysqlclient; then
            missing_deps+=("libmysqlclient-dev")
        fi

        if ! pkg-config --exists boost_system; then
            missing_deps+=("libboost-all-dev")
        fi

        if ! pkg-config --exists nlohmann_json; then
            missing_deps+=("nlohmann-json3-dev")
        fi
    fi

    if [ ${#missing_deps[@]} -ne 0 ]; then
        log_error "缺少以下依赖:"
        printf '  %s\n' "${missing_deps[@]}"
        echo
        log_info "安装命令:"
        if [[ "$OSTYPE" == "darwin"* ]]; then
            echo "  macOS: brew install ${missing_deps[*]}"
        else
            echo "  Ubuntu/Debian: sudo apt-get install ${missing_deps[*]}"
        fi
        exit 1
    fi

    log_success "所有依赖已满足"
}

# 检查MySQL服务
check_mysql_service() {
    log_info "检查MySQL服务状态..."

    if command -v mysql &> /dev/null; then
        if mysql -h 127.0.0.1 -u root -p"$DB_PASSWORD" -e "SELECT 1;" &> /dev/null; then
            log_success "MySQL服务正常运行"
            return 0
        else
            log_warning "MySQL服务可能未运行或密码不正确"
            return 1
        fi
    else
        log_error "MySQL未安装"
        return 1
    fi
}

# 初始化数据库
init_database() {
    log_info "检查数据库初始化状态..."

    if mysql -h 127.0.0.1 -u root -p"$DB_PASSWORD" -e "USE SmartMedical; SELECT 1;" &> /dev/null; then
        log_success "数据库已存在"
        return 0
    fi

    if [ -f "$DATABASE_DIR/init_database.sql" ]; then
        log_info "正在初始化数据库..."
        if mysql -h 127.0.0.1 -u root -p"$DB_PASSWORD" < "$DATABASE_DIR/init_database.sql"; then
            log_success "数据库初始化成功"
        else
            log_error "数据库初始化失败"
            exit 1
        fi
    else
        log_error "找不到数据库初始化脚本: $DATABASE_DIR/init_database.sql"
        exit 1
    fi
}

# 构建本地服务器
build_local_server() {
    log_info "构建本地开发版服务器..."

    cd "$SERVER_DIR"

    local output_file="server_local"
    local include_paths=""
    local library_paths=""
    local libraries=""
    local compile_flags=""

    # macOS 特定配置
    if [[ "$OSTYPE" == "darwin"* ]]; then
        # Homebrew 路径 (Apple Silicon Mac)
        if [ -d "/opt/homebrew" ]; then
            HOMEBREW_PREFIX="/opt/homebrew"
        # Homebrew 路径 (Intel Mac)
        elif [ -d "/usr/local" ]; then
            HOMEBREW_PREFIX="/usr/local"
        else
            log_error "找不到 Homebrew 安装路径"
            exit 1
        fi

        # MySQL 路径
        if [ -d "$HOMEBREW_PREFIX/opt/mysql" ]; then
            MYSQL_INCLUDE="$HOMEBREW_PREFIX/opt/mysql/include"
            MYSQL_LIB="$HOMEBREW_PREFIX/opt/mysql/lib"
        elif [ -d "/usr/local/mysql" ]; then
            MYSQL_INCLUDE="/usr/local/mysql/include"
            MYSQL_LIB="/usr/local/mysql/lib"
        else
            log_error "找不到 MySQL 安装路径"
            exit 1
        fi

        # Boost 路径
        if [ -d "$HOMEBREW_PREFIX/opt/boost" ]; then
            BOOST_INCLUDE="$HOMEBREW_PREFIX/opt/boost/include"
            BOOST_LIB="$HOMEBREW_PREFIX/opt/boost/lib"
        else
            BOOST_INCLUDE="$HOMEBREW_PREFIX/include"
            BOOST_LIB="$HOMEBREW_PREFIX/lib"
        fi

        # nlohmann-json 路径
        if [ -d "$HOMEBREW_PREFIX/opt/nlohmann-json" ]; then
            JSON_INCLUDE="$HOMEBREW_PREFIX/opt/nlohmann-json/include"
        else
            JSON_INCLUDE="$HOMEBREW_PREFIX/include"
        fi

        include_paths="-I$MYSQL_INCLUDE -I$BOOST_INCLUDE -I$JSON_INCLUDE -I/usr/local/include"
        library_paths="-L$MYSQL_LIB -L$BOOST_LIB"
        libraries="-lmysqlclient -lboost_system -lpthread"
        compile_flags="-std=c++17 -DLOCAL_DEV"

        log_info "macOS 编译参数:"
        echo "  MySQL Include: $MYSQL_INCLUDE"
        echo "  MySQL Library: $MYSQL_LIB"
        echo "  Boost Include: $BOOST_INCLUDE"
        echo "  Boost Library: $BOOST_LIB"

    else
        # Linux 系统，使用 pkg-config
        MYSQL_CFLAGS=$(pkg-config --cflags libmysqlclient 2>/dev/null || echo "")
        MYSQL_LIBS=$(pkg-config --libs libmysqlclient 2>/dev/null || echo "-lmysqlclient")
        BOOST_FLAGS=$(pkg-config --cflags --libs boost_system 2>/dev/null || echo "-lboost_system")

        if pkg-config --exists nlohmann_json 2>/dev/null; then
            JSON_FLAGS=$(pkg-config --cflags nlohmann_json 2>/dev/null)
        else
            JSON_FLAGS=""
        fi

        include_paths="$MYSQL_CFLAGS $JSON_FLAGS -I/usr/include"
        library_paths=""
        libraries="$MYSQL_LIBS $BOOST_FLAGS -lpthread"
        compile_flags="-std=c++17 -DLOCAL_DEV"

        log_info "Linux 编译参数:"
        echo "  MySQL CFLAGS: $MYSQL_CFLAGS"
        echo "  MySQL LIBS: $MYSQL_LIBS"
        echo "  Boost FLAGS: $BOOST_FLAGS"
    fi

    # 编译命令
    local compile_cmd="g++ $compile_flags $include_paths $library_paths server_local.cpp $libraries -o $output_file"

    log_info "执行编译命令:"
    echo "  $compile_cmd"
    echo

    # 执行编译
    if eval "$compile_cmd" 2>&1; then
        log_success "本地服务器编译成功: $SERVER_DIR/$output_file"
    else
        log_error "编译失败"
        log_info "请检查以下路径是否存在："
        if [[ "$OSTYPE" == "darwin"* ]]; then
            echo "  MySQL: $MYSQL_INCLUDE, $MYSQL_LIB"
            echo "  Boost: $BOOST_INCLUDE, $BOOST_LIB"
        fi
        exit 1
    fi
}

# 创建启动脚本
create_launcher() {
    local launcher="$PROJECT_DIR/start_local_server.sh"

    cat > "$launcher" << EOF
#!/bin/bash

# 本地服务器启动脚本

set -e

GREEN='\033[0;32m'
BLUE='\033[0;34m'
RED='\033[0;31m'
NC='\033[0m'

PROJECT_DIR="\$(cd "\$(dirname "\${BASH_SOURCE[0]}")" && pwd)"
SERVER_DIR="\$PROJECT_DIR/Server"

# 加载环境变量
ENV_FILE="\$PROJECT_DIR/.env"
if [ -f "\$ENV_FILE" ]; then
    export \$(cat "\$ENV_FILE" | grep -v '^#' | xargs)
else
    echo -e "\${RED}错误: 找不到.env文件，请从.env.example复制并配置\${NC}"
    exit 1
fi

# 检查必要的环境变量
if [ -z "\$DB_PASSWORD" ]; then
    echo -e "\${RED}错误: DB_PASSWORD未在.env文件中设置\${NC}"
    exit 1
fi

echo -e "\${BLUE}=== 启动智能医疗系统本地服务器 ===\${NC}"
echo "项目目录: \$PROJECT_DIR"
echo "服务器: \$SERVER_DIR/server_local"
echo

# 检查服务器文件
if [ ! -f "\$SERVER_DIR/server_local" ]; then
    echo "错误: 服务器文件不存在，请先运行: ./scripts/build_local.sh"
    exit 1
fi

# 检查MySQL服务
if ! mysql -h 127.0.0.1 -u root -p"\$DB_PASSWORD" -e "SELECT 1;" &> /dev/null; then
    echo -e "\${RED}错误: MySQL服务未运行或连接失败\${NC}"
    echo "请检查:"
    echo "1. MySQL是否已启动: brew services start mysql"
    echo "2. 密码是否正确: 编辑.env文件中的DB_PASSWORD"
    exit 1
fi

echo -e "\${GREEN}✓ 环境检查通过，启动服务器...\${NC}"
cd "\$SERVER_DIR"
./server_local
EOF

    chmod +x "$launcher"
    log_success "创建启动脚本: $launcher"
}

# 主函数
main() {
    echo
    echo "========================================"
    echo "  智能医疗系统 - 本地开发构建"
    echo "========================================"
    echo

    check_dependencies

    if ! check_mysql_service; then
        log_warning "MySQL服务检查失败，但继续构建..."
        echo "请确保MySQL已启动: brew services start mysql"
    fi

    init_database
    build_local_server
    create_launcher

    echo
    log_success "构建完成！"
    echo
    echo "下一步操作:"
    echo "1. 确保MySQL服务运行: brew services start mysql"
    echo "2. 修改数据库密码: 编辑 Server/server_local.cpp 中的 dbpassword"
    echo "3. 启动本地服务器: ./start_local_server.sh"
    echo "4. 构建客户端: ./scripts/build.sh"
    echo
}

# 运行主函数
main "$@"