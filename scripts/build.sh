#!/bin/bash

# 智能医疗管理系统 - 构建脚本
# 支持多平台构建和打包

set -e  # 遇到错误立即退出

# 颜色定义
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

# 获取脚本目录
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"

# 默认配置
BUILD_TYPE="Release"
BUILD_DIR="$PROJECT_DIR/build"
PLATFORM="$(uname -s)"
CLEAN_BUILD=false
BUILD_SERVER=false
BUILD_CLIENT=true
PACKAGE=false

# 显示帮助信息
show_help() {
    cat << EOF
智能医疗管理系统构建脚本

用法: $0 [选项]

选项:
    -h, --help          显示此帮助信息
    -c, --clean         清理构建目录
    -d, --debug         构建调试版本
    -r, --release       构建发布版本 (默认)
    -s, --server        只构建服务器
    -t, --client        只构建客户端 (默认)
    -b, --both          构建服务器和客户端
    -p, --package       打包应用程序
    -o, --output DIR    指定输出目录

示例:
    $0                  # 构建客户端发布版本
    $0 -c -b -p         # 清理并构建打包所有组件
    $0 -d -s            # 构建调试版本服务器
    $0 -r -o ./dist     # 构建发布版本到指定目录
EOF
}

# 解析命令行参数
parse_args() {
    while [[ $# -gt 0 ]]; do
        case $1 in
            -h|--help)
                show_help
                exit 0
                ;;
            -c|--clean)
                CLEAN_BUILD=true
                shift
                ;;
            -d|--debug)
                BUILD_TYPE="Debug"
                shift
                ;;
            -r|--release)
                BUILD_TYPE="Release"
                shift
                ;;
            -s|--server)
                BUILD_SERVER=true
                BUILD_CLIENT=false
                shift
                ;;
            -t|--client)
                BUILD_SERVER=false
                BUILD_CLIENT=true
                shift
                ;;
            -b|--both)
                BUILD_SERVER=true
                BUILD_CLIENT=true
                shift
                ;;
            -p|--package)
                PACKAGE=true
                shift
                ;;
            -o|--output)
                BUILD_DIR="$2"
                shift 2
                ;;
            *)
                log_error "未知选项: $1"
                show_help
                exit 1
                ;;
        esac
    done
}

# 检查依赖
check_dependencies() {
    log_info "检查构建依赖..."
    
    # 检查 CMake
    if ! command -v cmake &> /dev/null; then
        log_error "未找到 CMake，请安装 CMake 3.16+"
        exit 1
    fi
    
    # 检查编译器
    if ! command -v g++ &> /dev/null && ! command -v clang++ &> /dev/null; then
        log_error "未找到 C++ 编译器，请安装 GCC 或 Clang"
        exit 1
    fi
    
    # 检查 Qt
    if ! command -v qmake &> /dev/null && ! command -v qmake6 &> /dev/null; then
        log_warning "未找到 Qt，可能会影响构建"
    fi
    
    # 检查服务器依赖
    if [[ "$BUILD_SERVER" == true ]]; then
        if ! pkg-config --exists libmysqlclient; then
            log_error "未找到 MySQL 开发库，请安装 libmysqlclient-dev"
            exit 1
        fi
        
        if ! pkg-config --exists boost_system; then
            log_error "未找到 Boost 库，请安装 libboost-all-dev"
            exit 1
        fi
    fi
    
    log_success "依赖检查完成"
}

# 清理构建目录
clean_build() {
    if [[ -d "$BUILD_DIR" ]]; then
        log_info "清理构建目录: $BUILD_DIR"
        rm -rf "$BUILD_DIR"
    fi
}

# 创建构建目录
setup_build_dir() {
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
}

# 构建客户端
build_client() {
    log_info "构建客户端 (构建类型: $BUILD_TYPE)..."
    
    # 创建客户端构建目录
    mkdir -p client
    cd client
    
    # 配置 CMake
    cmake \
        -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
        -DCMAKE_PREFIX_PATH="$(qmake -query QT_INSTALL_PREFIX 2>/dev/null || qmake6 -query QT_INSTALL_PREFIX 2>/dev/null)" \
        "$PROJECT_DIR"
    
    # 编译
    if [[ "$PLATFORM" == "Darwin" ]]; then
        make -j$(sysctl -n hw.ncpu)
    else
        make -j$(nproc)
    fi
    
    # 检查编译结果
    if [[ -f "INTELLIGENT_HEALTHCARE_Client" ]]; then
        log_success "客户端构建成功"
    else
        log_error "客户端构建失败"
        exit 1
    fi
    
    cd ..
}

# 构建服务器
build_server() {
    log_info "构建服务器..."
    
    # 创建服务器构建目录
    mkdir -p server
    cd server
    
    # 获取编译标志
    MYSQL_CFLAGS=$(pkg-config --cflags libmysqlclient)
    MYSQL_LIBS=$(pkg-config --libs libmysqlclient)
    BOOST_LIBS=$(pkg-config --libs boost_system boost_thread)
    
    # 编译服务器
    if g++ -std=c++17 \
        -O2 \
        -Wall \
        -Wextra \
        $MYSQL_CFLAGS \
        -I"$PROJECT_DIR/Server" \
        "$PROJECT_DIR/Server/server.cpp" \
        $MYSQL_LIBS $BOOST_LIBS \
        -o "server" \
        -pthread; then
        log_success "服务器构建成功"
    else
        log_error "服务器构建失败"
        exit 1
    fi
    
    cd ..
}

# 打包应用程序
package_app() {
    log_info "打包应用程序..."
    
    local PACKAGE_DIR="$BUILD_DIR/package"
    local VERSION="1.0.0"
    
    # 创建包目录
    mkdir -p "$PACKAGE_DIR"
    
    # 复制可执行文件
    if [[ "$BUILD_CLIENT" == true ]] && [[ -f "$BUILD_DIR/client/INTELLIGENT_HEALTHCARE_Client" ]]; then
        cp "$BUILD_DIR/client/INTELLIGENT_HEALTHCARE_Client" "$PACKAGE_DIR/"
    fi
    
    if [[ "$BUILD_SERVER" == true ]] && [[ -f "$BUILD_DIR/server/server" ]]; then
        cp "$BUILD_DIR/server/server" "$PACKAGE_DIR/"
    fi
    
    # 复制资源文件
    if [[ -d "$PROJECT_DIR/resources" ]]; then
        cp -r "$PROJECT_DIR/resources" "$PACKAGE_DIR/"
    fi
    
    # 复制文档
    cp "$PROJECT_DIR/README.md" "$PACKAGE_DIR/"
    cp "$PROJECT_DIR/LICENSE" "$PACKAGE_DIR/"
    
    # 创建启动脚本
    cat > "$PACKAGE_DIR/start_client.sh" << 'EOF'
#!/bin/bash
cd "$(dirname "$0")"
./INTELLIGENT_HEALTHCARE_Client
EOF
    
    cat > "$PACKAGE_DIR/start_server.sh" << 'EOF'
#!/bin/bash
cd "$(dirname "$0")"
./server
EOF
    
    chmod +x "$PACKAGE_DIR/start_client.sh" "$PACKAGE_DIR/start_server.sh"
    
    # 创建配置文件模板
    cat > "$PACKAGE_DIR/config.example" << EOF
# 数据库配置示例
DB_HOST=localhost
DB_USER=your_username
DB_PASSWORD=your_password
DB_NAME=SmartMedical
DB_PORT=3306

# 服务器配置
SERVER_PORT=1437
SERVER_HOST=0.0.0.0
EOF
    
    # 创建归档文件
    local ARCHIVE_NAME="Intelligent-Healthcare-System-$VERSION-$(uname -s)-$(uname -m)"
    cd "$BUILD_DIR"
    
    if command -v tar &> /dev/null; then
        tar -czf "$ARCHIVE_NAME.tar.gz" package/
        log_success "创建归档: $ARCHIVE_NAME.tar.gz"
    fi
    
    if [[ "$PLATFORM" == "Darwin" ]] && command -v zip &> /dev/null; then
        zip -r "$ARCHIVE_NAME.zip" package/
        log_success "创建归档: $ARCHIVE_NAME.zip"
    fi
}

# 主函数
main() {
    log_info "开始构建智能医疗管理系统..."
    
    parse_args "$@"
    check_dependencies
    
    if [[ "$CLEAN_BUILD" == true ]]; then
        clean_build
    fi
    
    setup_build_dir
    
    if [[ "$BUILD_CLIENT" == true ]]; then
        build_client
    fi
    
    if [[ "$BUILD_SERVER" == true ]]; then
        build_server
    fi
    
    if [[ "$PACKAGE" == true ]]; then
        package_app
    fi
    
    log_success "构建完成！"
    log_info "输出目录: $BUILD_DIR"
}

# 运行主函数
main "$@"