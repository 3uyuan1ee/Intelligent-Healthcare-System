#!/bin/bash

# 单元测试运行脚本
set -e

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m'

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

log_test() {
    echo -e "${PURPLE}[TEST]${NC} $1"
}

log_result() {
    echo -e "${CYAN}[RESULT]${NC} $1"
}

# 显示帮助
show_help() {
    cat << EOF
智能医疗管理系统单元测试运行脚本

用法: $0 [选项]

选项:
    -h, --help              显示此帮助信息
    -v, --verbose           详细输出模式
    -c, --clean             清理测试文件
    -b, --build             编译测试文件
    -r, --run               运行测试
    -a, --all               执行完整流程（清理+编译+运行）
    -t, --test <name>       运行指定测试
    --coverage              生成测试覆盖率报告
    --timeout <seconds>     设置测试超时时间（默认：300秒）
EOF
}

# 解析参数
VERBOSE=false
CLEAN=false
BUILD=false
RUN=false
ALL=false
COVERAGE=false
SPECIFIC_TEST=""
TIMEOUT=300

while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            show_help
            exit 0
            ;;
        -v|--verbose)
            VERBOSE=true
            shift
            ;;
        -c|--clean)
            CLEAN=true
            shift
            ;;
        -b|--build)
            BUILD=true
            shift
            ;;
        -r|--run)
            RUN=true
            shift
            ;;
        -a|--all)
            ALL=true
            CLEAN=true
            BUILD=true
            RUN=true
            shift
            ;;
        -t|--test)
            SPECIFIC_TEST="$2"
            RUN=true
            shift 2
            ;;
        --coverage)
            COVERAGE=true
            shift
            ;;
        --timeout)
            TIMEOUT="$2"
            shift 2
            ;;
        *)
            log_error "未知选项: $1"
            show_help
            exit 1
            ;;
    esac
done

# 获取脚本目录和项目目录
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"

# 检查环境
check_environment() {
    log_info "检查测试环境..."

    # 检查Qt环境
    if ! command -v qmake &> /dev/null && ! command -v qmake6 &> /dev/null; then
        log_error "未找到qmake或qmake6，请确保Qt开发环境已安装"
        exit 1
    fi

    # 检查CMake
    if ! command -v cmake &> /dev/null; then
        log_warning "未找到CMake，将使用qmake构建系统"
    fi

    # 检查必要的源文件
    local required_files=(
        "$PROJECT_DIR/Instance/UserSession.h"
        "$PROJECT_DIR/Instance/StateManager.h"
        "$PROJECT_DIR/NetWork/tcpclient.h"
        "$PROJECT_DIR/Fun./JsonMessageBuilder.h"
        "$PROJECT_DIR/Standard.h"
    )

    for file in "${required_files[@]}"; do
        if [[ ! -f "$file" ]]; then
            log_error "找不到必要文件: $file"
            exit 1
        fi
    done

    log_success "环境检查通过"
}

# 清理测试文件
clean_tests() {
    log_info "清理测试文件..."

    # 清理编译产物
    find "$SCRIPT_DIR" -name "*.o" -delete 2>/dev/null || true
    find "$SCRIPT_DIR" -name "moc_*" -delete 2>/dev/null || true
    find "$SCRIPT_DIR" -name "*.a" -delete 2>/dev/null || true

    # 清理测试可执行文件
    find "$SCRIPT_DIR" -name "*_test" -type f -delete 2>/dev/null || true
    find "$SCRIPT_DIR" -name "Makefile" -delete 2>/dev/null || true

    # 清理构建目录
    if [[ -d "$SCRIPT_DIR/build" ]]; then
        rm -rf "$SCRIPT_DIR/build"
    fi

    # 清理临时文件
    find "$SCRIPT_DIR" -name "*.tmp" -delete 2>/dev/null || true
    find "$SCRIPT_DIR" -name "*.log" -delete 2>/dev/null || true

    # 清理缓存目录
    if [[ -d "$SCRIPT_DIR/cache" ]]; then
        rm -rf "$SCRIPT_DIR/cache"
    fi

    # 清理测试数据目录
    if [[ -d "$SCRIPT_DIR/tests_temp" ]]; then
        rm -rf "$SCRIPT_DIR/tests_temp"
    fi

    log_success "清理完成"
}

# 创建构建目录
setup_build_dir() {
    local build_dir="$SCRIPT_DIR/build"
    mkdir -p "$build_dir"
    echo "$build_dir"
}

# 查找Qt版本
find_qt_version() {
    if command -v qmake6 &> /dev/null; then
        echo "6"
    elif command -v qmake &> /dev/null; then
        echo "5"
    else
        echo "none"
    fi
}

# 编译测试
build_tests() {
    log_info "编译测试文件..."

    local qt_version=$(find_qt_version)
    local build_dir=$(setup_build_dir)

    log_info "使用Qt$qt_version进行编译"

    # 查找测试文件
    local test_files=(
        "unit/TcpClient_test.cpp"
        "unit/UserSession_test.cpp"
        "unit/JsonMessageBuilder_test.cpp"
        "unit/StateManager_test.cpp"
    )

    local successful_tests=()
    local failed_tests=()

    for test_file in "${test_files[@]}"; do
        local test_name=$(basename "$test_file" .cpp)
        local test_path="$SCRIPT_DIR/$test_file"

        if [[ ! -f "$test_path" ]]; then
            log_warning "测试文件不存在: $test_file"
            continue
        fi

        log_test "编译测试: $test_name"

        # 创建测试项目文件
        local pro_file="$build_dir/$test_name.pro"
        cat > "$pro_file" << EOF
QT += core network testlib widgets
CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app
TARGET = $test_name

INCLUDEPATH += \\
    $PROJECT_DIR \\
    $PROJECT_DIR/Instance \\
    $PROJECT_DIR/NetWork \\
    $PROJECT_DIR/Fun. \\
    $SCRIPT_DIR \\
    $SCRIPT_DIR/config \\
    $SCRIPT_DIR/mocks

SOURCES += \\
    $test_path \\
    $SCRIPT_DIR/config/test_config.cpp \\
    $SCRIPT_DIR/mocks/MockTcpClient.cpp

HEADERS += \\
    $PROJECT_DIR/Instance/UserSession.h \\
    $PROJECT_DIR/Instance/StateManager.h \\
    $PROJECT_DIR/NetWork/tcpclient.h \\
    $PROJECT_DIR/Fun./JsonMessageBuilder.h \\
    $PROJECT_DIR/Standard.h \\
    $SCRIPT_DIR/config/test_config.h \\
    $SCRIPT_DIR/mocks/MockTcpClient.h
EOF

        # 编译测试
        cd "$build_dir"

        if [[ "$VERBOSE" == true ]]; then
            if qmake$qt_version "$test_name.pro" && make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4); then
                successful_tests+=("$test_name")
                log_success "✓ $test_name 编译成功"
            else
                failed_tests+=("$test_name")
                log_error "✗ $test_name 编译失败"
            fi
        else
            if qmake$qt_version "$test_name.pro" >/dev/null 2>&1 && make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4) >/dev/null 2>&1; then
                successful_tests+=("$test_name")
                log_success "✓ $test_name 编译成功"
            else
                failed_tests+=("$test_name")
                log_error "✗ $test_name 编译失败"
            fi
        fi

        # 清理临时文件
        rm -f "$pro_file" "$build_dir/Makefile"
    done

    # 显示编译结果
    log_result "编译结果: ${#successful_tests[@]} 成功, ${#failed_tests[@]} 失败"

    if [[ ${#successful_tests[@]} -eq 0 ]]; then
        log_error "没有测试文件编译成功"
        exit 1
    fi

    if [[ ${#failed_tests[@]} -gt 0 ]]; then
        log_warning "以下测试文件编译失败:"
        for test in "${failed_tests[@]}"; do
            echo "  - $test"
        done
    fi
}

# 运行测试
run_tests() {
    log_info "运行测试..."

    local build_dir="$SCRIPT_DIR/build"
    local total_tests=0
    local passed_tests=0
    local failed_tests=0
    local test_results=()

    # 设置环境变量
    export QT_QPA_PLATFORM=offscreen  # 无头模式运行
    export LC_ALL=C  # 统一语言环境

    if [[ -n "$SPECIFIC_TEST" ]]; then
        # 运行指定测试
        local test_executable="$build_dir/$SPECIFIC_TEST"
        if [[ -x "$test_executable" ]]; then
            run_single_test "$SPECIFIC_TEST" "$test_executable"
        else
            log_error "测试可执行文件不存在: $test_executable"
            exit 1
        fi
    else
        # 运行所有测试
        for test_exec in "$build_dir"/*_test; do
            if [[ -x "$test_exec" ]]; then
                local test_name=$(basename "$test_exec")
                run_single_test "$test_name" "$test_exec"
            fi
        done
    fi

    # 显示测试结果
    show_test_summary
}

# 运行单个测试
run_single_test() {
    local test_name="$1"
    local test_executable="$2"

    log_test "运行测试: $test_name"

    # 设置超时
    local timeout_cmd=""
    if command -v timeout &> /dev/null; then
        timeout_cmd="timeout $TIMEOUT"
    elif command -v gtimeout &> /dev/null; then
        timeout_cmd="gtimeout $TIMEOUT"
    fi

    # 运行测试
    local test_log="$SCRIPT_DIR/${test_name}.log"
    local start_time=$(date +%s)

    if $timeout_cmd "$test_executable" > "$test_log" 2>&1; then
        local end_time=$(date +%s)
        local duration=$((end_time - start_time))

        test_results+=("$test_name:PASS:$duration")
        passed_tests=$((passed_tests + 1))
        log_success "✓ $test_name 通过 (${duration}s)"

        if [[ "$VERBOSE" == true ]]; then
            echo "测试输出:"
            cat "$test_log"
        fi
    else
        local exit_code=$?
        local end_time=$(date +%s)
        local duration=$((end_time - start_time))

        if [[ $exit_code -eq 124 ]]; then
            test_results+=("$test_name:TIMEOUT:$duration")
            log_error "✗ $test_name 超时 (${duration}s)"
        else
            test_results+=("$test_name:FAIL:$duration")
            log_error "✗ $test_name 失败 (${duration}s)"
        fi

        failed_tests=$((failed_tests + 1))

        if [[ "$VERBOSE" == true ]]; then
            echo "测试输出:"
            cat "$test_log"
        fi
    fi

    total_tests=$((total_tests + 1))

    # 清理测试日志
    if [[ "$VERBOSE" != true ]]; then
        rm -f "$test_log"
    fi
}

# 显示测试总结
show_test_summary() {
    echo
    log_result "测试总结"
    echo "=========="
    echo "总测试数: $total_tests"
    echo "通过: $passed_tests"
    echo "失败: $failed_tests"

    if [[ $total_tests -gt 0 ]]; then
        local success_rate=$((passed_tests * 100 / total_tests))
        echo "成功率: ${success_rate}%"
    fi

    echo
    log_result "详细结果"
    echo "----------"
    for result in "${test_results[@]}"; do
        IFS=':' read -r name status duration <<< "$result"

        case $status in
            "PASS")
                echo -e "  ${GREEN}✓${NC} $name (${duration}s)"
                ;;
            "FAIL")
                echo -e "  ${RED}✗${NC} $name (${duration}s) - 失败"
                ;;
            "TIMEOUT")
                echo -e "  ${YELLOW}⚠${NC} $name (${duration}s) - 超时"
                ;;
        esac
    done

    echo

    # 生成测试报告
    generate_test_report

    # 返回适当的退出码
    if [[ $failed_tests -eq 0 ]]; then
        log_success "所有测试通过！"
        exit 0
    else
        log_error "有 $failed_tests 个测试失败"
        exit 1
    fi
}

# 生成测试报告
generate_test_report() {
    local report_file="$SCRIPT_DIR/test_report.txt"
    local timestamp=$(date '+%Y-%m-%d %H:%M:%S')

    cat > "$report_file" << EOF
智能医疗管理系统单元测试报告
生成时间: $timestamp
测试超时: ${TIMEOUT}秒

测试结果统计:
总测试数: $total_tests
通过: $passed_tests
失败: $failed_tests

测试详情:
EOF

    for result in "${test_results[@]}"; do
        IFS=':' read -r name status duration <<< "$result"
        echo "- $name: $status (${duration}s)" >> "$report_file"
    done

    cat >> "$report_file" << EOF

测试环境:
- 操作系统: $(uname -s)
- Qt版本: $(qmake -query QT_VERSION 2>/dev/null || echo "未知")
- 编译器: $(gcc --version 2>/dev/null | head -1 || echo "未知")

建议:
EOF

    if [[ $failed_tests -eq 0 ]]; then
        echo "- 所有测试通过，代码质量良好" >> "$report_file"
    else
        echo "- 有 $failed_tests 个测试失败，需要检查相关代码" >> "$report_file"
        echo "- 建议修复失败的测试用例" >> "$report_file"
    fi

    log_success "测试报告已生成: $report_file"
}

# 生成覆盖率报告
generate_coverage() {
    log_info "生成测试覆盖率报告..."

    if command -v gcov &> /dev/null; then
        local build_dir=$(setup_build_dir)
        cd "$build_dir"

        # 生成覆盖率数据
        find . -name "*.gcda" -delete
        find . -name "*.gcno" -exec gcov {} \;

        # 生成HTML报告
        if command -v lcov &> /dev/null && command -v genhtml &> /dev/null; then
            lcov --directory . --capture --output-file coverage.info
            lcov --remove coverage.info '/usr/*' --output-file coverage.info
            genhtml -o coverage coverage.info

            log_success "覆盖率报告已生成: $build_dir/coverage/index.html"
        else
            log_warning "lcov或genhtml未安装，无法生成HTML覆盖率报告"
        fi
    else
        log_warning "gcov未安装，无法生成覆盖率报告"
    fi
}

# 主函数
main() {
    log_info "智能医疗管理系统单元测试"
    echo "============================="

    check_environment

    if [[ "$ALL" == true || "$CLEAN" == true ]]; then
        clean_tests
    fi

    if [[ "$ALL" == true || "$BUILD" == true ]]; then
        build_tests
    fi

    if [[ "$ALL" == true || "$RUN" == true ]]; then
        run_tests
    fi

    if [[ "$COVERAGE" == true ]]; then
        generate_coverage
    fi

    if [[ "$ALL" == false && "$CLEAN" == false && "$BUILD" == false && "$RUN" == false ]]; then
        log_warning "未指定任何操作，使用 -h 查看帮助信息"
        show_help
        exit 1
    fi
}

# 运行主函数
main "$@"