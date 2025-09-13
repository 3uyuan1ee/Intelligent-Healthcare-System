# 项目概览

### 项目结构
```
├── docs/          # 项目文档
├── scripts/       # 构建和部署脚本
├── assets/        # 项目资源
├── Server/        # 服务器端代码
├── Client/        # 客户端界面
├── NetWork/       # 网络通信
├── Instance/      # 实例管理
├── Fun/           # 功能工具
└── [业务模块]...  # 各个业务功能模块
```

### 构建系统 
- **scripts/build.sh**: Linux/macOS 构建脚本
- **scripts/build.bat**: Windows 构建脚本
- **支持多平台构建**: Linux, macOS, Windows
- **自动打包**: 创建发布包和压缩文件
- **依赖检查**: 自动检测和验证构建依赖

### 测试框架
- **scripts/test.sh**: 综合测试脚本
- **测试类型**: 单元测试、集成测试、性能测试、代码质量检查
- **自动报告**: 生成测试报告和结果统计

### 发布系统
- **scripts/release.sh**: 版本发布脚本
- **多平台支持**: 自动构建不同平台的发布包
- **校验和**: 自动生成文件校验和
- **GitHub集成**: 支持自动创建 GitHub Release
- **发布说明**: 自动生成发布说明模板

### 工具
- **scripts/start.sh**: 交互式启动脚本
- **环境检查**: 自动检测开发环境
- **快速操作**: 一键构建、测试、启动

### 版本管理
- **语义化版本**: 遵循版本控制规范
- **自动化发布**: 支持自动创建 Git 标签
- **变更追踪**: 完整的发布记录和变更日志

## 🚀 使用指南

### 快速开始
```bash
# 1. 运行交互式启动脚本
./scripts/start.sh

# 2. 直接构建项目
./scripts/build.sh --clean --both --package

# 3. 运行测试
./scripts/test.sh

# 4. 创建发布版本
./scripts/release.sh --create-checksum
```

### 项目特色
1. **完整文档**: 详细的项目说明和使用指南
2. **跨平台**: 支持 Windows, macOS, Linux
3. **自动化**: 完整的构建、测试、发布流程
4. **专业架构**: 模块化设计，易于维护和扩展
5. **开源友好**: 遵循开源最佳实践

## 📁 最终项目结构

```
Intelligent-Healthcare-System/
├── 📄 README.md              # 项目主文档
├── 📄 LICENSE                # MIT 协议
├── 📄 CONTRIBUTING.md        # 贡献指南
├── 📄 .gitignore             # Git 忽略文件
├── 📄 CMakeLists.txt         # 构建配置
├── 📄 main.cpp               # 程序入口
├── 📄 Standard.h             # 通用头文件
├── 📄 resources.qrc          # Qt 资源文件
│
├── 📁 docs/                  # 项目文档
│   └── ARCHITECTURE.md       # 架构设计文档
│
├── 📁 scripts/               # 脚本文件
│   ├── build.sh             # Linux/macOS 构建脚本
│   ├── build.bat            # Windows 构建脚本
│   ├── test.sh              # 测试脚本
│   ├── release.sh           # 发布脚本
│   └── start.sh             # 交互式启动脚本
│
├── 📁 assets/                # 项目资源 (预留)
├── 📁 Server/                # 服务器端
│   └── server.cpp           # 服务器主程序
│
├── 📁 Client/                # 客户端界面
│   ├── patient_client.*     # 患者端
│   ├── doctor_client.*      # 医生端
│   └── admin_client.*       # 管理员端
│
├── 📁 NetWork/               # 网络通信
│   └── tcpclient.*          # TCP 客户端
│
├── 📁 Instance/              # 实例管理
│   ├── UserSession.*        # 用户会话
│   └── StateManager.*       # 状态管理
│
├── 📁 Fun/                   # 功能工具
│   ├── function.*           # 工具函数
│   ├── JsonMessageBuilder.*  # JSON 消息构建
│   └── DataManager.*         # 数据管理
│
├── 📁 [业务模块]...          # 各个业务功能模块
├── 📁 resources/             # Qt 资源
├── 📁 Cache/                 # 缓存目录
└── 📁 build/                 # 构建输出
```

