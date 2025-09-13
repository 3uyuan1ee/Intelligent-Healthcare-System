#!/bin/bash

# 智能医疗管理系统 - 快速启动脚本

set -e

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
PURPLE='\033[0;35m'
NC='\033[0m'

# 项目信息
PROJECT_NAME="智能医疗管理系统"
VERSION="1.0.0"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"

# 显示横幅
show_banner() {
    echo -e "${CYAN}"
    cat << 'EOF'
╔══════════════════════════════════════════════════════════════╗
║                                                              ║
║                 智能医疗管理系统                               ║
║             Intelligent Healthcare System                     ║
║                                                              ║
║                    基于 Qt + C++ 开发                         ║
║                                                              ║
╚══════════════════════════════════════════════════════════════╝
EOF
    echo -e "${NC}"
}

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

log_menu() {
    echo -e "${PURPLE}[MENU]${NC} $1"
}

# 显示主菜单
show_menu() {
    echo
    echo -e "${CYAN}请选择操作:${NC}"
    echo
    echo -e "  ${GREEN}1${NC}) 构建项目"
    echo -e "  ${GREEN}2${NC}) 启动服务器"
    echo -e "  ${GREEN}3${NC}) 启动客户端"
    echo -e "  ${GREEN}4${NC}) 运行测试"
    echo -e "  ${GREEN}5${NC}) 创建发布版本"
    echo -e "  ${GREEN}6${NC}) 查看项目信息"
    echo -e "  ${GREEN}7${NC}) 环境检查"
    echo -e "  ${GREEN}8${NC}) 清理项目"
    echo -e "  ${GREEN}0${NC}) 退出"
    echo
    echo -n -e "${BLUE}请输入选项 [0-8]: ${NC}"
}

# 显示项目信息
show_info() {
    echo
    echo -e "${CYAN}项目信息:${NC}"
    echo
    echo -e "  ${GREEN}项目名称:${NC} $PROJECT_NAME"
    echo -e "  ${GREEN}版本:${NC} $VERSION"
    echo -e "  ${GREEN}项目目录:${NC} $PROJECT_DIR"
    echo -e "  ${GREEN}脚本目录:${NC} $SCRIPT_DIR"
    echo -e "  ${GREEN}技术栈:${NC} Qt 5/6, C++17, Boost.Asio, MySQL"
    echo
    echo -e "${CYAN}项目特色:${NC}"
    echo -e "  • 多角色支持 (患者/医生/管理员)"
    echo -e "  • 实时网络通信 (TCP + JSON)"
    echo -e "  • 数据持久化 (MySQL)"
    echo -e "  • 跨平台支持 (Windows/macOS/Linux)"
    echo
}

# 环境检查
check_environment() {
    echo
    log_info "检查运行环境..."
    
    local issues=0
    
    # 检查 CMake
    if command -v cmake &> /dev/null; then
        local cmake_version=$(cmake --version | head -n1 | cut -d' ' -f3)
        echo -e "  ✅ CMake: $cmake_version"
    else
        echo -e "  ❌ CMake: 未安装"
        issues=$((issues + 1))
    fi
    
    # 检查编译器
    if command -v g++ &> /dev/null; then
        local gcc_version=$(g++ --version | head -n1)
        echo -e "  ✅ GCC: $gcc_version"
    elif command -v clang++ &> /dev/null; then
        local clang_version=$(clang++ --version | head -n1)
        echo -e "  ✅ Clang: $clang_version"
    else
        echo -e "  ❌ C++ 编译器: 未安装"
        issues=$((issues + 1))
    fi
    
    # 检查 Qt
    if command -v qmake &> /dev/null; then
        local qt_version=$(qmake --version | grep -o 'Qt version [0-9.]*')
        echo -e "  ✅ Qt: $qt_version"
    elif command -v qmake6 &> /dev/null; then
        local qt_version=$(qmake6 --version | grep -o 'Qt version [0-9.]*')
        echo -e "  ✅ Qt: $qt_version"
    else
        echo -e "  ⚠️  Qt: 未找到 (客户端构建可能失败)"
    fi
    
    # 检查 MySQL
    if command -v mysql &> /dev/null; then
        echo -e "  ✅ MySQL: 已安装"
    else
        echo -e "  ⚠️  MySQL: 未找到 (服务器需要 MySQL)"
    fi
    
    # 检查 Boost
    if pkg-config --exists boost_system 2>/dev/null; then
        echo -e "  ✅ Boost: 已安装"
    else
        echo -e "  ⚠️  Boost: 未找到 (服务器需要 Boost)"
    fi
    
    echo
    if [[ $issues -eq 0 ]]; then
        log_success "环境检查通过!"
    else
        log_warning "发现 $issues 个问题，可能影响构建"
    fi
    
    echo
    read -p "按回车键继续..."
}

# 构建项目
build_project() {
    echo
    log_info "开始构建项目..."
    
    if [[ ! -f "$SCRIPT_DIR/build.sh" ]]; then
        log_error "构建脚本不存在: $SCRIPT_DIR/build.sh"
        read -p "按回车键继续..."
        return 1
    fi
    
    echo
    echo -e "${CYAN}构建选项:${NC}"
    echo "  1) 完整构建 (服务器 + 客户端)"
    echo "  2) 只构建客户端"
    echo "  3) 只构建服务器"
    echo "  4) 清理并重新构建"
    echo "  0) 返回主菜单"
    echo
    echo -n -e "${BLUE}请选择构建类型 [0-4]: ${NC}"
    read -r build_choice
    
    case $build_choice in
        1)
            echo "构建服务器和客户端..."
            "$SCRIPT_DIR/build.sh" --clean --both --package
            ;;
        2)
            echo "构建客户端..."
            "$SCRIPT_DIR/build.sh" --clean --client --package
            ;;
        3)
            echo "构建服务器..."
            "$SCRIPT_DIR/build.sh" --clean --server --package
            ;;
        4)
            echo "清理并重新构建..."
            "$SCRIPT_DIR/build.sh" --clean --both --package
            ;;
        0)
            return
            ;;
        *)
            log_error "无效选择"
            return 1
            ;;
    esac
    
    if [[ $? -eq 0 ]]; then
        log_success "构建完成!"
    else
        log_error "构建失败"
    fi
    
    read -p "按回车键继续..."
}

# 启动服务器
start_server() {
    echo
    log_info "启动服务器..."
    
    local server_binary="$PROJECT_DIR/build/server/server"
    if [[ ! -f "$server_binary" ]]; then
        log_warning "服务器未构建，是否现在构建? (y/N)"
        read -r response
        if [[ "$response" =~ ^[Yy]$ ]]; then
            "$SCRIPT_DIR/build.sh" --clean --server
        else
            return 1
        fi
    fi
    
    echo
    echo -e "${YELLOW}服务器将在新的终端窗口中启动${NC}"
    echo -e "${YELLOW}服务器将在端口 1437 上监听连接${NC}"
    echo
    
    # 根据平台选择启动方式
    case "$(uname -s)" in
        Darwin)
            # macOS
            osascript -e "tell app \"Terminal\" to do script \"cd '$PROJECT_DIR/build/server' && ./server\""
            ;;
        Linux)
            # Linux
            if command -v gnome-terminal &> /dev/null; then
                gnome-terminal -- bash -c "cd '$PROJECT_DIR/build/server' && ./server; exec bash"
            elif command -v xterm &> /dev/null; then
                xterm -e "cd '$PROJECT_DIR/build/server' && ./server" &
            else
                log_error "无法找到支持的终端模拟器"
                return 1
            fi
            ;;
        *)
            log_error "不支持的平台"
            return 1
            ;;
    esac
    
    log_success "服务器启动中..."
    sleep 2
    read -p "按回车键继续..."
}

# 启动客户端
start_client() {
    echo
    log_info "启动客户端..."
    
    local client_binary="$PROJECT_DIR/build/client/INTELLIGENT_HEALTHCARE_Client"
    if [[ ! -f "$client_binary" ]]; then
        log_warning "客户端未构建，是否现在构建? (y/N)"
        read -r response
        if [[ "$response" =~ ^[Yy]$ ]]; then
            "$SCRIPT_DIR/build.sh" --clean --client
        else
            return 1
        fi
    fi
    
    echo
    echo -e "${YELLOW}客户端将在新的窗口中启动${NC}"
    echo
    
    # 根据平台选择启动方式
    case "$(uname -s)" in
        Darwin)
            # macOS
            osascript -e "tell app \"Terminal\" to do script \"cd '$PROJECT_DIR/build/client' && ./INTELLIGENT_HEALTHCARE_Client\""
            ;;
        Linux)
            # Linux
            if command -v gnome-terminal &> /dev/null; then
                gnome-terminal -- bash -c "cd '$PROJECT_DIR/build/client' && ./INTELLIGENT_HEALTHCARE_Client; exec bash"
            elif command -v xterm &> /dev/null; then
                xterm -e "cd '$PROJECT_DIR/build/client' && ./INTELLIGENT_HEALTHCARE_Client" &
            else
                log_error "无法找到支持的终端模拟器"
                return 1
            fi
            ;;
        *)
            log_error "不支持的平台"
            return 1
            ;;
    esac
    
    log_success "客户端启动中..."
    sleep 2
    read -p "按回车键继续..."
}

# 运行测试
run_tests() {
    echo
    log_info "运行测试..."
    
    if [[ ! -f "$SCRIPT_DIR/test.sh" ]]; then
        log_error "测试脚本不存在"
        read -p "按回车键继续..."
        return 1
    fi
    
    echo
    echo -e "${CYAN}测试选项:${NC}"
    echo "  1) 完整测试套件"
    echo "  2) 只运行单元测试"
    echo "  3) 只运行集成测试"
    echo "  4) 只测试客户端"
    echo "  5) 只测试服务器"
    echo "  0) 返回主菜单"
    echo
    echo -n -e "${BLUE}请选择测试类型 [0-5]: ${NC}"
    read -r test_choice
    
    case $test_choice in
        1)
            "$SCRIPT_DIR/test.sh" -v
            ;;
        2)
            "$SCRIPT_DIR/test.sh" -v -u
            ;;
        3)
            "$SCRIPT_DIR/test.sh" -v -i
            ;;
        4)
            "$SCRIPT_DIR/test.sh" -v -c
            ;;
        5)
            "$SCRIPT_DIR/test.sh" -v -s
            ;;
        0)
            return
            ;;
        *)
            log_error "无效选择"
            return 1
            ;;
    esac
    
    read -p "测试完成，按回车键继续..."
}

# 创建发布版本
create_release() {
    echo
    log_info "创建发布版本..."
    
    if [[ ! -f "$SCRIPT_DIR/release.sh" ]]; then
        log_error "发布脚本不存在"
        read -p "按回车键继续..."
        return 1
    fi
    
    echo
    echo -e "${YELLOW}警告: 发布功能需要配置 Git 和 GitHub${NC}"
    echo -e "${YELLOW}当前配置为本地发布测试${NC}"
    echo
    echo -e "${CYAN}发布选项:${NC}"
    echo "  1) 本地构建发布包"
    echo "  2) 完整发布 (需要 GitHub Token)"
    echo "  0) 返回主菜单"
    echo
    echo -n -e "${BLUE}请选择发布类型 [0-2]: ${NC}"
    read -r release_choice
    
    case $release_choice in
        1)
            "$SCRIPT_DIR/release.sh" --skip-tests --create-checksum
            ;;
        2)
            echo "GitHub Token: "
            read -s token
            "$SCRIPT_DIR/release.sh" --create-checksum --token "$token"
            ;;
        0)
            return
            ;;
        *)
            log_error "无效选择"
            return 1
            ;;
    esac
    
    read -p "按回车键继续..."
}

# 清理项目
clean_project() {
    echo
    log_info "清理项目..."
    
    echo
    echo -e "${YELLOW}警告: 这将删除所有构建文件和缓存${NC}"
    echo -n "确认继续? (y/N): "
    read -r response
    
    if [[ "$response" =~ ^[Yy]$ ]]; then
        # 删除构建目录
        if [[ -d "$PROJECT_DIR/build" ]]; then
            rm -rf "$PROJECT_DIR/build"
            log_success "删除构建目录"
        fi
        
        # 删除发布目录
        if [[ -d "$PROJECT_DIR/release" ]]; then
            rm -rf "$PROJECT_DIR/release"
            log_success "删除发布目录"
        fi
        
        # 清理缓存
        if [[ -d "$PROJECT_DIR/Cache" ]]; then
            rm -rf "$PROJECT_DIR/Cache"/*
            log_success "清理缓存目录"
        fi
        
        # Git 清理
        if [[ -d "$PROJECT_DIR/.git" ]]; then
            git clean -fdx "$PROJECT_DIR" 2>/dev/null || true
            log_success "Git 清理完成"
        fi
        
        log_success "项目清理完成!"
    else
        log_info "取消清理操作"
    fi
    
    read -p "按回车键继续..."
}

# 主循环
main_loop() {
    while true; do
        clear
        show_banner
        show_menu
        
        read -r choice
        
        case $choice in
            1)
                build_project
                ;;
            2)
                start_server
                ;;
            3)
                start_client
                ;;
            4)
                run_tests
                ;;
            5)
                create_release
                ;;
            6)
                show_info
                read -p "按回车键继续..."
                ;;
            7)
                check_environment
                ;;
            8)
                clean_project
                ;;
            0)
                echo
                log_success "感谢使用 $PROJECT_NAME!"
                echo
                exit 0
                ;;
            *)
                log_error "无效选择，请重新输入"
                sleep 1
                ;;
        esac
    done
}

# 检查是否在正确的目录
if [[ ! -f "$PROJECT_DIR/CMakeLists.txt" ]]; then
    log_error "未找到 CMakeLists.txt，请确保在正确的项目目录中运行此脚本"
    exit 1
fi

# 启动主程序
main_loop