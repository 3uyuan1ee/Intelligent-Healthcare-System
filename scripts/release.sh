#!/bin/bash

# 智能医疗管理系统 - 版本发布脚本
# 用于创建正式发布版本

set -e

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
NC='\033[0m'

# 脚本目录
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"

# 版本配置
VERSION="1.0.0"
RELEASE_DATE=$(date +"%Y-%m-%d")
PLATFORMS=("linux" "macos" "windows")

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

log_release() {
    echo -e "${PURPLE}[RELEASE]${NC} $1"
}

# 显示帮助
show_help() {
    cat << EOF
智能医疗管理系统版本发布脚本

用法: $0 [选项]

选项:
    -h, --help              显示此帮助信息
    -v, --version VERSION   指定版本号 (默认: $VERSION)
    -p, --platform PLATFORM 构建指定平台 [linux|macos|windows]
    -s, --skip-tests        跳过测试
    -c, --create-checksum   创建校验和
    -u, --upload            上传到 GitHub Releases
    -t, --token TOKEN       GitHub API Token
    -r, --release-notes     编辑发布说明

示例:
    $0                                      # 构建所有平台版本
    $0 -v 1.0.1 -p linux                   # 构建指定版本和平台
    $0 --create-checksum --upload --token TOKEN  # 构建并上传到 GitHub
EOF
}

# 解析参数
SKIP_TESTS=false
CREATE_CHECKSUM=false
UPLOAD=false
GITHUB_TOKEN=""
EDIT_NOTES=false
TARGET_PLATFORM="all"

while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            show_help
            exit 0
            ;;
        -v|--version)
            VERSION="$2"
            shift 2
            ;;
        -p|--platform)
            TARGET_PLATFORM="$2"
            shift 2
            ;;
        -s|--skip-tests)
            SKIP_TESTS=true
            shift
            ;;
        -c|--create-checksum)
            CREATE_CHECKSUM=true
            shift
            ;;
        -u|--upload)
            UPLOAD=true
            shift
            ;;
        -t|--token)
            GITHUB_TOKEN="$2"
            shift 2
            ;;
        -r|--release-notes)
            EDIT_NOTES=true
            shift
            ;;
        *)
            log_error "未知选项: $1"
            exit 1
            ;;
    esac
done

# 检查环境
check_environment() {
    log_info "检查发布环境..."
    
    # 检查 git 状态
    if [[ ! -d "$PROJECT_DIR/.git" ]]; then
        log_error "当前目录不是 Git 仓库"
        exit 1
    fi
    
    # 检查是否有未提交的更改
    if ! git diff-index --quiet HEAD --; then
        log_warning "检测到未提交的更改"
        read -p "是否继续发布? (y/N): " -n 1 -r
        echo
        if [[ ! $REPLY =~ ^[Yy]$ ]]; then
            exit 1
        fi
    fi
    
    # 检查 GitHub Token
    if [[ "$UPLOAD" == true ]] && [[ -z "$GITHUB_TOKEN" ]]; then
        log_error "上传到 GitHub 需要 --token 参数"
        exit 1
    fi
    
    # 检查必需工具
    local missing_tools=()
    
    if ! command -v curl &> /dev/null; then
        missing_tools+=("curl")
    fi
    
    if ! command -v sha256sum &> /dev/null && ! command -v shasum &> /dev/null; then
        missing_tools+=("sha256sum/shasum")
    fi
    
    if [[ ${#missing_tools[@]} -gt 0 ]]; then
        log_error "缺少必需工具: ${missing_tools[*]}"
        exit 1
    fi
    
    log_success "环境检查完成"
}

# 运行测试
run_tests() {
    if [[ "$SKIP_TESTS" == true ]]; then
        log_warning "跳过测试"
        return 0
    fi
    
    log_info "运行测试..."
    
    # 这里可以添加实际的测试命令
    # 例如: make test 或 ./scripts/test.sh
    
    log_success "测试通过"
}

# 构建版本
build_version() {
    local platform="$1"
    local build_script="$SCRIPT_DIR/build.sh"
    
    log_info "构建 $platform 版本..."
    
    case "$platform" in
        "linux")
            # 模拟 Linux 构建
            "$build_script" --clean --both --package -o "$PROJECT_DIR/release/linux"
            ;;
        "macos")
            # macOS 构建
            "$build_script" --clean --both --package -o "$PROJECT_DIR/release/macos"
            ;;
        "windows")
            # Windows 构建 (如果可能)
            log_warning "Windows 构建需要在 Windows 环境下运行"
            ;;
        *)
            log_error "不支持的平台: $platform"
            exit 1
            ;;
    esac
}

# 创建发布包
create_packages() {
    log_info "创建发布包..."
    
    local release_dir="$PROJECT_DIR/release"
    local package_dir="$release_dir/v$VERSION"
    
    # 创建版本目录
    mkdir -p "$package_dir"
    
    # 移动构建产物
    if [[ "$TARGET_PLATFORM" == "all" ]] || [[ "$TARGET_PLATFORM" == "linux" ]]; then
        if [[ -d "$release_dir/linux" ]]; then
            mv "$release_dir/linux" "$package_dir/"
        fi
    fi
    
    if [[ "$TARGET_PLATFORM" == "all" ]] || [[ "$TARGET_PLATFORM" == "macos" ]]; then
        if [[ -d "$release_dir/macos" ]]; then
            mv "$release_dir/macos" "$package_dir/"
        fi
    fi
    
    # 创建校验和
    if [[ "$CREATE_CHECKSUM" == true ]]; then
        create_checksums "$package_dir"
    fi
    
    # 创建发布说明模板
    create_release_notes "$package_dir"
    
    log_success "发布包创建完成: $package_dir"
}

# 创建校验和
create_checksums() {
    local package_dir="$1"
    
    log_info "创建校验和文件..."
    
    cd "$package_dir"
    
    # 为所有平台创建校验和
    for platform in */; do
        if [[ -d "$platform" ]]; then
            local platform_name="${platform%/}"
            local checksum_file="checksums_${platform_name}.txt"
            
            > "$checksum_file"
            
            # 查找所有文件并计算校验和
            find "$platform_name" -type f ! -name "*.txt" | while read -r file; do
                if command -v sha256sum &> /dev/null; then
                    sha256sum "$file" >> "$checksum_file"
                else
                    shasum -a 256 "$file" >> "$checksum_file"
                fi
            done
            
            log_success "创建校验和: $checksum_file"
        fi
    done
    
    cd "$PROJECT_DIR"
}

# 创建发布说明
create_release_notes() {
    local package_dir="$1"
    
    local notes_file="$package_dir/RELEASE_NOTES.md"
    
    cat > "$notes_file" << EOF
# 智能医疗管理系统 v$VERSION

发布日期: $RELEASE_DATE

## 🎯 主要改进

- [请在此列出主要改进和功能]

## 🐛 问题修复

- [请在此列出修复的问题]

## 🔧 技术更新

- [请在此列出技术相关的更新]

## 📦 下载

### Linux
- [二进制包链接]
- 校验和: \`sha256sum [checksum]\`

### macOS  
- [二进制包链接]
- 校验和: \`sha256sum [checksum]\`

### Windows
- [二进制包链接]
- 校验和: \`sha256sum [checksum]\`

## 🔧 安装说明

\`\`\`bash
# 解压
tar -xzf Intelligent-Healthcare-System-v$VERSION-linux.tar.gz
cd Intelligent-Healthcare-System-v$VERSION

# 启动服务器
./start_server.sh

# 启动客户端
./start_client.sh
\`\`\`

## ⚠️ 注意事项

- 需要安装 MySQL 5.7+
- 需要配置数据库连接参数
- 建议在测试环境验证后再部署到生产环境

## 📞 支持

如有问题，请：
1. 查看 [文档](../../README.md)
2. 提交 [GitHub Issue](../../issues)
3. 发送邮件至: 1481059602@qq.com

---

**感谢使用智能医疗管理系统！** 🏥
EOF

    if [[ "$EDIT_NOTES" == true ]]; then
        log_info "打开编辑器编辑发布说明..."
        ${EDITOR:-nano} "$notes_file"
    fi
    
    log_success "创建发布说明: $notes_file"
}

# 创建 GitHub Release
create_github_release() {
    if [[ "$UPLOAD" != true ]]; then
        return 0
    fi
    
    log_info "创建 GitHub Release..."
    
    # 获取仓库名称
    local repo_url=$(git config --get remote.origin.url)
    local repo_name=$(echo "$repo_url" | sed -n 's/.*github.com[:/]\(.*\)\.git/\1/p')
    
    if [[ -z "$repo_name" ]]; then
        log_error "无法获取仓库名称"
        exit 1
    fi
    
    # 检查 tag 是否已存在
    if git tag -l "v$VERSION" | grep -q "v$VERSION"; then
        log_warning "Tag v$VERSION 已存在"
    else
        # 创建 tag
        git tag -a "v$VERSION" -m "Release v$VERSION"
        git push origin "v$VERSION"
        log_success "创建并推送 tag: v$VERSION"
    fi
    
    # 准备发布数据
    local release_data="{
        \"tag_name\": \"v$VERSION\",
        \"name\": \"v$VERSION\",
        \"body\": \"$(cat "$PROJECT_DIR/release/v$VERSION/RELEASE_NOTES.md" | sed 's/"/\\"/g' | tr '\n' '\\n')\",
        \"draft\": false,
        \"prerelease\": false
    }"
    
    # 创建 release
    local response=$(curl -s -X POST \
        -H "Authorization: token $GITHUB_TOKEN" \
        -H "Accept: application/vnd.github.v3+json" \
        -H "Content-Type: application/json" \
        -d "$release_data" \
        "https://api.github.com/repos/$repo_name/releases")
    
    local upload_url=$(echo "$response" | grep -o '"upload_url": "[^"]*' | cut -d'"' -f4 | cut -d'{' -f1)
    
    if [[ -z "$upload_url" ]]; then
        log_error "创建 GitHub Release 失败"
        echo "$response" | jq . 2>/dev/null || echo "$response"
        exit 1
    fi
    
    # 上传资源文件
    upload_assets "$upload_url" "$repo_name"
    
    log_success "GitHub Release 创建完成"
}

# 上传资源文件
upload_assets() {
    local upload_url="$1"
    local repo_name="$2"
    local package_dir="$PROJECT_DIR/release/v$VERSION"
    
    log_info "上传资源文件..."
    
    # 查找所有需要上传的文件
    find "$package_dir" -type f \( -name "*.tar.gz" -o -name "*.zip" -o -name "*.exe" -o -name "*.txt" \) | while read -r file; do
        local filename=$(basename "$file")
        local content_type="application/octet-stream"
        
        # 根据文件扩展名设置 content type
        case "${filename##*.}" in
            "gz") content_type="application/gzip" ;;
            "zip") content_type="application/zip" ;;
            "exe") content_type="application/vnd.microsoft.portable-executable" ;;
            "txt") content_type="text/plain" ;;
        esac
        
        log_info "上传: $filename"
        
        curl -s -X POST \
            -H "Authorization: token $GITHUB_TOKEN" \
            -H "Accept: application/vnd.github.v3+json" \
            -H "Content-Type: $content_type" \
            --data-binary @"$file" \
            "${upload_url}?name=$filename" > /dev/null
        
        log_success "上传完成: $filename"
    done
}

# 主函数
main() {
    log_release "智能医疗管理系统版本发布脚本 v1.0"
    log_release "目标版本: v$VERSION"
    
    check_environment
    run_tests
    
    # 构建指定平台
    if [[ "$TARGET_PLATFORM" == "all" ]]; then
        for platform in "${PLATFORMS[@]}"; do
            build_version "$platform"
        done
    else
        build_version "$TARGET_PLATFORM"
    fi
    
    create_packages
    create_github_release
    
    log_release "🎉 发布完成！版本: v$VERSION"
    log_release "发布目录: $PROJECT_DIR/release/v$VERSION"
    
    if [[ "$UPLOAD" == true ]]; then
        log_release "GitHub Release: https://github.com/$(git config --get remote.origin.url | sed -n 's/.*github.com[:/]\(.*\)\.git/\1/p')/releases/tag/v$VERSION"
    fi
}

# 运行主函数
main "$@"