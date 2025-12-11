#!/bin/bash

# 本地服务器启动脚本

set -e

GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m'

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SERVER_DIR="$PROJECT_DIR/Server"

echo -e "${BLUE}=== 启动智能医疗系统本地服务器 ===${NC}"
echo "项目目录: $PROJECT_DIR"
echo "服务器: $SERVER_DIR/server_local"
echo

# 检查服务器文件
if [ ! -f "$SERVER_DIR/server_local" ]; then
    echo "错误: 服务器文件不存在，请先运行: ./scripts/build_local.sh"
    exit 1
fi

# 检查MySQL服务
if ! mysql -h 127.0.0.1 -u root -p'Lmy050323' -e "SELECT 1;" &> /dev/null; then
    echo "错误: MySQL服务未运行或连接失败"
    echo "请检查:"
    echo "1. MySQL是否已启动: brew services start mysql"
    echo "2. 密码是否正确: 修改server_local.cpp中的dbpassword"
    exit 1
fi

echo -e "${GREEN}✓ 环境检查通过，启动服务器...${NC}"
cd "$SERVER_DIR"
./server_local
