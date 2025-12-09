#!/bin/bash

# 智能医疗管理系统测试运行脚本

set -e  # 遇到错误立即退出

echo "======================================"
echo "智能医疗管理系统 - 测试运行脚本"
echo "======================================"

# 检查是否安装了必要的依赖
check_dependencies() {
    echo "检查依赖..."

    # 检查cmake
    if ! command -v cmake &> /dev/null; then
        echo "错误: 未找到cmake，请先安装cmake"
        exit 1
    fi

    # 检查qmake (用于检测Qt)
    if ! command -v qmake &> /dev/null; then
        echo "错误: 未找到qmake，请先安装Qt开发环境"
        exit 1
    fi

    echo "依赖检查完成"
}

# 设置测试环境
setup_environment() {
    echo "设置测试环境..."

    # 创建构建目录
    if [ ! -d "build" ]; then
        mkdir build
    fi

    cd build

    # 配置CMake (使用父目录的Qt版本)
    echo "配置测试环境..."

    # 检查是否在Qt Creator中运行，如果是使用Qt Creator的Qt路径
    if [ -n "$CMAKE_PREFIX_PATH" ]; then
        cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH=$CMAKE_PREFIX_PATH
    else
        # 尝试找到Qt路径
        QTPATH=$(dirname $(dirname $(which qmake)))
        if [ -d "$QTPATH" ]; then
            cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH=$QTPATH
        else
            cmake .. -DCMAKE_BUILD_TYPE=Debug
        fi
    fi

    echo "环境设置完成"
}

# 运行所有测试
run_all_tests() {
    echo "开始运行所有测试..."
    echo "===================="

    # 编译测试
    echo "编译测试程序..."
    # macOS 使用sysctl获取核心数，Linux使用nproc
    if [[ "$OSTYPE" == "darwin"* ]]; then
        CORES=$(sysctl -n hw.ncpu)
    else
        CORES=$(nproc)
    fi
    make -j$CORES

    if [ $? -ne 0 ]; then
        echo "错误: 编译失败"
        exit 1
    fi

    echo "编译成功"
    echo "===================="

    # 运行每个单独的测试
    echo "运行TcpClient测试..."
    ./TcpClient_test

    echo "运行UserSession测试..."
    ./UserSession_test

    echo "运行JsonMessageBuilder测试..."
    ./JsonMessageBuilder_test

    echo "运行StateManager测试..."
    ./StateManager_test

    echo "运行Function测试..."
    ./Function_test

    echo "运行DataManager测试..."
    ./DataManager_test

    echo "===================="
    echo "所有测试完成"
}

# 运行特定测试
run_specific_test() {
    local test_name=$1

    if [ -z "$test_name" ]; then
        echo "错误: 请指定测试名称"
        echo "可用测试: TcpClient_test, UserSession_test, JsonMessageBuilder_test, StateManager_test, Function_test, DataManager_test"
        exit 1
    fi

    echo "运行 $test_name 测试..."

    # 编译特定测试
    cd build
    make $test_name

    if [ $? -eq 0 ]; then
        ./$test_name
    else
        echo "错误: 编译 $test_name 失败"
        exit 1
    fi
}

# 清理构建文件
clean_build() {
    echo "清理构建文件..."
    rm -rf build
    echo "清理完成"
}

# 显示帮助信息
show_help() {
    echo "用法: $0 [选项] [测试名称]"
    echo ""
    echo "选项:"
    echo "  -h, --help     显示此帮助信息"
    echo "  -c, --clean    清理构建文件"
    echo "  -t, --test     运行指定的测试"
    echo ""
    echo "测试名称:"
    echo "  TcpClient_test       TCP客户端测试"
    echo "  UserSession_test     用户会话测试"
    echo "  JsonMessageBuilder_test JSON消息构建器测试"
    echo "  StateManager_test    状态管理器测试"
    echo "  Function_test        功能函数测试"
    echo "  DataManager_test     数据管理器测试"
    echo ""
    echo "示例:"
    echo "  $0                    运行所有测试"
    echo "  $0 -t TcpClient_test   只运行TcpClient测试"
    echo "  $0 --clean            清理构建文件"
}

# 主逻辑
main() {
    case "${1:-}" in
        -h|--help)
            show_help
            ;;
        -c|--clean)
            clean_build
            ;;
        -t|--test)
            check_dependencies
            setup_environment
            run_specific_test "$2"
            ;;
        "")
            check_dependencies
            setup_environment
            run_all_tests
            ;;
        *)
            echo "错误: 未知选项 '$1'"
            echo "使用 '$0 --help' 查看帮助信息"
            exit 1
            ;;
    esac
}

# 执行主函数
main "$@"