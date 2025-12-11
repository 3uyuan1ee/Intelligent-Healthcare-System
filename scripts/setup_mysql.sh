#!/bin/bash

# MySQL配置脚本
# 用于设置MySQL密码并初始化数据库

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

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

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
DATABASE_DIR="$PROJECT_DIR/database"
SERVER_DIR="$PROJECT_DIR/Server"

# 测试MySQL连接
test_mysql_connection() {
    local password="$1"

    log_info "测试MySQL连接..."

    if mysql -h 127.0.0.1 -u root -p"$password" -e "SELECT 1;" &>/dev/null; then
        return 0
    else
        return 1
    fi
}

# 设置MySQL密码
setup_mysql_password() {
    log_info "设置MySQL root密码..."

    echo
    log_info "请按照提示设置MySQL root密码："
    echo "1. 系统会要求验证当前密码（可能直接按回车）"
    echo "2. 设置新密码"
    echo "3. 确认新密码"
    echo

    # 使用mysql_secure_installation来设置密码
    mysql_secure_installation
}

# 初始化数据库
init_database() {
    local password="$1"

    log_info "初始化SmartMedical数据库..."

    if mysql -h 127.0.0.1 -u root -p"$password" < "$DATABASE_DIR/init_database.sql"; then
        log_success "数据库初始化成功"
        return 0
    else
        log_error "数据库初始化失败"
        return 1
    fi
}

# 更新服务器配置中的密码
update_server_password() {
    local password="$1"

    log_info "更新服务器配置文件中的数据库密码..."

    if [ -f "$SERVER_DIR/server_local.cpp" ]; then
        # 备份原文件
        cp "$SERVER_DIR/server_local.cpp" "$SERVER_DIR/server_local.cpp.backup"

        # 更新密码行
        sed -i.bak "s/constexpr char dbpassword\[\] = \".*\";/constexpr char dbpassword[] = \"$password\";/" "$SERVER_DIR/server_local.cpp"

        log_success "密码已更新到 server_local.cpp"
    else
        log_error "找不到 server_local.cpp 文件"
        return 1
    fi
}

# 主函数
main() {
    echo
    echo "========================================"
    echo "  智能医疗系统 - MySQL配置"
    echo "========================================"
    echo

    # 检查MySQL服务
    if ! brew services list | grep mysql | grep started >/dev/null 2>&1; then
        log_info "启动MySQL服务..."
        brew services start mysql
        sleep 3
    fi

    log_success "MySQL服务已启动"

    # 尝试连接数据库
    log_info "测试数据库连接..."

    # 首先尝试空密码
    if test_mysql_connection ""; then
        log_info "检测到无密码连接，建议设置密码以保护数据库安全"
        read -p "是否要设置root密码? (y/n): " set_password

        if [[ $set_password =~ ^[Yy]$ ]]; then
            setup_mysql_password
            echo
            read -s -p "请输入设置的MySQL root密码: " password
            echo
        else
            password=""
            log_warning "继续使用无密码连接（不推荐）"
        fi
    else
        echo
        log_info "请输入MySQL root密码进行连接测试："
        for attempt in {1..3}; do
            read -s -p "密码 (尝试 $attempt/3): " password
            echo

            if test_mysql_connection "$password"; then
                log_success "连接成功！"
                break
            else
                log_error "密码错误"
                if [ $attempt -eq 3 ]; then
                    echo
                    log_error "连接失败次数过多，请检查密码或重置MySQL"
                    echo "重置密码命令: mysql_secure_installation"
                    exit 1
                fi
            fi
        done
    fi

    # 初始化数据库
    if ! init_database "$password"; then
        log_error "数据库初始化失败，请检查权限"
        exit 1
    fi

    # 更新服务器配置
    if ! update_server_password "$password"; then
        exit 1
    fi

    echo
    log_success "MySQL配置完成！"
    echo
    echo "配置信息:"
    echo "  主机: 127.0.0.1"
    echo "  端口: 3306"
    echo "  用户: root"
    echo "  密码: ${password:-<空>}"
    echo "  数据库: SmartMedical"
    echo
    echo "下一步:"
    echo "1. 构建本地服务器: ./scripts/build_local.sh"
    echo "2. 启动服务器: ./start_local_server.sh"
    echo
}

# 运行主函数
main "$@"