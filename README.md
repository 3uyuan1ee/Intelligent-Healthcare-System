# 智能医疗管理系统 (Intelligent Healthcare System)

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20macOS%20%7C%20Linux-green.svg)](#)
[![Qt](https://img.shields.io/badge/Qt-5%2F6-brightgreen.svg)](#)
[![C++](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](#)

一个基于 Qt 框架和 C++ 开发的智能医疗管理系统，支持患者、医生和管理员三种用户角色的完整医疗信息化解决方案。

## 🏥 项目特色

- **👥 多角色支持**: 患者、医生、管理员三种用户角色，权限分级管理
- **🔄 实时通信**: 基于 TCP 套接字的客户端-服务器架构，JSON 消息协议
- **💾 数据持久化**: MySQL 数据库存储，支持医疗数据、预约、病例等完整业务
- **🎨 现代化界面**: Qt Widgets 构建，支持跨平台部署
- **🔒 安全认证**: 完整的用户登录注册系统，会话管理
- **📱 模块化设计**: 清晰的代码架构，易于维护和扩展

## 🏗️ 系统架构

```
智能医疗管理系统
├── 客户端 (Qt/C++)
│   ├── 用户认证模块
│   ├── 患者管理模块
│   ├── 医生工作台
│   ├── 预约系统
│   ├── 病例管理
│   ├── 处方管理
│   └── 通知系统
├── 服务器端 (C++/Boost.Asio)
│   ├── TCP 服务器
│   ├── 数据库连接池
│   ├── 业务逻辑处理
│   └── JSON 消息路由
└── 数据库 (MySQL)
    ├── 用户信息表
    ├── 预约记录表
    ├── 病例数据表
    ├── 处方信息表
    └── 通知消息表
```

## 🚀 核心功能

### 患者功能
- 用户注册登录
- 个人信息管理
- 医生查询与预约
- 病例查看
- 处方查看
- 健康问卷填写
- 系统通知接收

### 医生功能
- 工作台管理
- 患者接诊签到
- 病例录入与编辑
- 处方开具
- 工作时间管理
- 通知发布
- 患者信息查看

### 管理员功能
- 系统用户管理
- 医生信息审核
- 患者签到统计
- 系统通知管理
- 数据统计分析
- 系统配置管理

## 🛠️ 技术栈

### 客户端
- **Qt 5/6**: 跨平台 GUI 框架
- **C++17**: 现代C++标准
- **TCP套接字**: 网络通信
- **JSON**: 数据交换格式
- **SQLite**: 本地缓存

### 服务器端
- **Boost.Asio**: 异步网络编程
- **MySQL**: 关系型数据库
- **nlohmann/json**: JSON处理库
- **多线程**: 并发处理

### 构建系统
- **CMake**: 跨平台构建
- **Qt Creator**: IDE支持

## 📦 安装与运行

### 环境要求

- **操作系统**: Windows 10+, macOS 10.14+, Ubuntu 18.04+
- **Qt版本**: Qt 5.15+ 或 Qt 6.x
- **编译器**: 支持C++17的编译器
- **数据库**: MySQL 5.7+
- **CMake**: 3.16+

### 客户端构建

```bash
# 克隆项目
git clone https://github.com/yourusername/Intelligent-Healthcare-System.git
cd Intelligent-Healthcare-System

# 创建构建目录
mkdir build && cd build

# 配置项目
cmake ..

# 编译
make

# 运行
./INTELLIGENT_HEALTHCARE_Client
```

### 服务器端构建

```bash
# 安装依赖
# Ubuntu/Debian
sudo apt-get install libboost-all-dev libmysqlclient-dev

# CentOS/RHEL
sudo yum install boost-devel mysql-devel

# 编译服务器
g++ -std=c++17 -I/usr/include/mysql -L/usr/lib/mysql -lmysqlclient -lboost_system -lboost_thread -o server Server/server.cpp

# 运行服务器
./server
```

### 数据库配置

1. 创建数据库：
```sql
CREATE DATABASE SmartMedical CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
```

2. 修改服务器配置（Server/server.cpp）：
```cpp
constexpr char dbip[] = "your_database_ip";
constexpr char dbuser[] = "your_username";
constexpr char dbpassword[] = "your_password";
constexpr char dbname[] = "SmartMedical";
constexpr int dbport = 3306;
```

## 📁 项目结构

```
Intelligent-Healthcare-System/
├── CMakeLists.txt              # 主构建文件
├── README.md                   # 项目说明
├── LICENSE                     # 开源协议
├── CLAUDE.md                   # 开发指南
├── Standard.h                  # 通用头文件
├── main.cpp                    # 程序入口
├── resources.qrc              # Qt资源文件
├── resources/                 # 资源文件
│   └── icons/                 # 图标资源
├── Client/                    # 客户端界面
│   ├── patient_client.*       # 患者端
│   ├── doctor_client.*        # 医生端
│   └── admin_client.*         # 管理员端
├── NetWork/                   # 网络模块
│   └── tcpclient.*           # TCP客户端
├── Instance/                  # 实例管理
│   ├── UserSession.*         # 用户会话
│   └── StateManager.*        # 状态管理
├── Fun/                      # 功能模块
│   ├── function.*            # 工具函数
│   ├── JsonMessageBuilder.*  # JSON消息构建
│   └── DataManager.*         # 数据管理
├── infoClient/               # 信息管理
├── appointClient/            # 预约管理
├── caseClient/               # 病例管理
├── adviceClient/             # 处方管理
├── checkInClient/            # 签到管理
├── QuestionClient/           # 问卷管理
├── Notice/                   # 通知系统
├── Server/                   # 服务器端
│   └── server.cpp            # 服务器主程序
├── build/                    # 构建目录
└── Cache/                    # 缓存目录
```

## 🎯 使用说明

### 首次运行

1. **启动服务器**
   ```bash
   cd Server
   ./server
   ```

2. **启动客户端**
   ```bash
   ./INTELLIGENT_HEALTHCARE_Client
   ```

3. **注册管理员账户**
   - 使用管理员界面注册第一个管理员账户
   - 管理员可审核医生信息和管理系统用户

### 用户角色流程

```
患者注册 → 医生注册(需审核) → 预约挂号 → 就诊签到 → 病例录入 → 处方开具 → 查看结果
```

## 🧪 测试

项目包含以下测试类型：

- **单元测试**: 工具函数和数据处理模块
- **集成测试**: 网络通信和数据库操作
- **UI测试**: 界面交互和用户体验
- **压力测试**: 多用户并发访问

## 🤝 贡献指南

欢迎提交 Issue 和 Pull Request！

### 开发流程

1. Fork 本仓库
2. 创建特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 创建 Pull Request

### 代码规范

- 遵循 Qt 编码风格
- 使用有意义的变量名和函数名
- 添加适当的注释
- 确保代码通过编译和测试

## 📄 开源协议

本项目采用 MIT 协议开源 - 查看 [LICENSE](LICENSE) 文件了解详情。

## 🙏 致谢

- Qt 开发团队提供优秀的跨平台框架
- Boost 库提供高质量的网络编程支持
- nlohmann 提供便捷的 JSON 处理库

## 📞 联系方式

如有问题或建议，欢迎通过以下方式联系：

- GitHub Issues
- Email: [1481059602@qq.com]

---

⭐ 如果这个项目对您有帮助，请考虑给个 Star！