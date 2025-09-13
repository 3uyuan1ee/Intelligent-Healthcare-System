# 贡献指南 (Contributing Guide)

感谢您对智能医疗管理系统的关注！我们欢迎各种形式的贡献。

## 🤝 如何贡献

### 报告问题

如果您发现了 bug 或有功能建议，请通过 [GitHub Issues](../../issues) 报告：

1. **Bug 报告**：
   - 使用 "Bug Report" 模板
   - 提供复现步骤
   - 描述期望行为
   - 附加截图或日志

2. **功能请求**：
   - 使用 "Feature Request" 模板
   - 详细描述功能需求
   - 说明使用场景

### 代码贡献

#### 开发环境设置

1. **Fork 仓库**
   ```bash
   # 克隆您的 fork
   git clone https://github.com/your-username/Intelligent-Healthcare-System.git
   cd Intelligent-Healthcare-System
   
   # 添加上游仓库
   git remote add upstream https://github.com/original-username/Intelligent-Healthcare-System.git
   ```

2. **安装依赖**
   ```bash
   # Ubuntu/Debian
   sudo apt-get install build-essential cmake qtbase6-dev libboost-all-dev libmysqlclient-dev
   
   # macOS (使用 Homebrew)
   brew install cmake qt boost mysql
   
   # Windows
   # 使用 Qt Maintenance Tool 安装 Qt，下载 Boost 和 MySQL Connector/C
   ```

3. **构建项目**
   ```bash
   mkdir build && cd build
   cmake ..
   make
   ```

#### 代码规范

### C++ 编码规范

1. **命名约定**：
   ```cpp
   // 类名使用 PascalCase
   class UserManager {
   public:
       // 函数名使用 camelCase
       void getUserInfo();
       
       // 成员变量使用 m_ 前缀
       QString m_username;
       
   private:
       // 私有成员使用下划线后缀
       QString database_connection_;
   };
   
   // 常量使用 UPPER_SNAKE_CASE
   const int MAX_LOGIN_ATTEMPTS = 3;
   ```

2. **代码格式**：
   - 使用 4 个空格缩进
   - 行长度限制在 120 字符
   - 大括号风格：K&R
   ```cpp
   if (condition) {
       // 代码块
   } else {
       // else 块
   }
   ```

3. **注释规范**：
   ```cpp
   /**
    * @brief 用户认证管理类
    * 
    * 负责处理用户登录、注册、会话管理等功能
    */
   class AuthManager {
   public:
       /**
        * @brief 验证用户登录凭证
        * @param username 用户名
        * @param password 密码
        * @return bool 验证是否成功
        */
       bool authenticateUser(const QString& username, const QString& password);
   };
   ```

### Qt 特定规范

1. **信号槽连接**：
   ```cpp
   // 推荐使用函数指针语法
   connect(button, &QPushButton::clicked, this, &MyClass::onButtonClicked);
   
   // 避免使用字符串语法（除非必要）
   connect(button, SIGNAL(clicked()), this, SLOT(onButtonClicked()));
   ```

2. **内存管理**：
   ```cpp
   // Qt 对象树管理
   QWidget* widget = new QWidget(parent);
   
   // 使用智能指针（非 Qt 对象）
   auto data = std::make_unique<DataManager>();
   ```

### Git 工作流

1. **创建功能分支**：
   ```bash
   # 功能分支命名：feature/功能名称
   git checkout -b feature/user-authentication-improvement
   
   # 修复分支命名：fix/问题描述
   git checkout -b fix/login-crash-on-invalid-input
   
   # 文档分支命名：docs/文档内容
   git checkout -b docs/api-documentation-update
   ```

2. **提交规范**：
   ```bash
   # 提交消息格式：<类型>(<范围>): <描述>
   # 
   # 类型：feat, fix, docs, style, refactor, test, chore
   # 范围：模块名称（可选）
   # 
   # 示例：
   git commit -m "feat(auth): 添加双因素认证功能"
   git commit -m "fix(database): 修复预约查询时的内存泄漏"
   git commit -m "docs(readme): 更新安装说明"
   ```

3. **保持同步**：
   ```bash
   # 同步上游更新
   git fetch upstream
   git rebase upstream/main
   ```

4. **推送 Pull Request**：
   ```bash
   # 推送到您的 fork
   git push origin feature/your-feature-name
   
   # 创建 Pull Request 到上游仓库的 main 分支
   ```

### 测试要求

#### 单元测试

```cpp
#include <QtTest>
#include "authmanager.h"

class TestAuthManager : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        // 测试初始化
    }
    
    void testUserAuthentication() {
        AuthManager auth;
        QVERIFY(auth.authenticateUser("testuser", "password"));
        QVERIFY(!auth.authenticateUser("invalid", "credentials"));
    }
    
    void cleanupTestCase() {
        // 测试清理
    }
};

QTEST_APPLESS_MAIN(TestAuthManager)
#include "testauthmanager.moc"
```

#### 测试覆盖率要求

- 新功能：≥ 80% 代码覆盖率
- 关键模块：≥ 90% 代码覆盖率
- 修复 bug：必须包含回归测试

### 代码审查

#### 审查清单

在提交 PR 前，请确保：

- [ ] 代码符合项目编码规范
- [ ] 所有测试通过
- [ ] 新功能包含相应测试
- [ ] 文档已更新
- [ ] 提交消息规范
- [ ] 无安全漏洞
- [ ] 性能影响已评估
- [ ] 向后兼容性考虑

#### 审查流程

1. **自动化检查**：
   - 代码格式检查
   - 编译检查
   - 单元测试
   - 静态代码分析

2. **人工审查**：
   - 至少需要 1 位维护者审查
   - 重点关注代码质量和安全性
   - 确保符合项目架构

### 发布流程

#### 版本号规范

遵循语义化版本控制 (Semantic Versioning)：

- `MAJOR.MINOR.PATCH` 格式
- `MAJOR`: 不兼容的 API 更改
- `MINOR`: 向后兼容的功能新增
- `PATCH`: 向后兼容的问题修复

#### 发布检查清单

- [ ] 版本号更新正确
- [ ] 更新日志完整
- [ ] 文档同步更新
- [ ] 所有测试通过
- [ ] 发布分支创建
- [ ] 标签创建

## 📋 贡献者行为准则

### 我们的承诺

为了营造开放和友好的环境，我们作为贡献者和维护者承诺：

- 使用友好和包容的语言
- 尊重不同的观点和经验
- 优雅地接受建设性批评
- 关注社区最大利益

### 不当行为

不当行为包括：

- 使用性暗示语言或图像
- 人身攻击或侮辱性评论
- 公开或私下骚扰
- 未经明确许可发布他人信息

## 🎯 贡献者权益

### 贡献者等级

1. **贡献者**：提交有效的 PR 或 Issue
2. **活跃贡献者**：多次有意义的贡献
3. **核心贡献者**：持续贡献，对项目有深入理解
4. **维护者**：项目核心成员，拥有合并权限

### 认可方式

- README 贡献者列表
- GitHub 贡献者图表
- 发布说明中的致谢
- 项目博客和社交媒体提及

## 📞 联系方式

如有任何贡献相关问题：

- **技术问题**：GitHub Issues
- **合作洽谈**：1481059602@qq.com
- **社区讨论**：GitHub Discussions

---

**感谢您的贡献！** 🙏