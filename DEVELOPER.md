# 开发指南 (Developer Guide)

本文档为希望进一步开发和扩展此项目的开发者提供指导。

## 项目架构

### 分层设计

```
┌─────────────────────────┐
│   User Interface (UI)   │
│  (MainWindow, Dialogs)  │
├─────────────────────────┤
│  Business Logic Layer   │
│ (TaskManager, Scheduler)│
├─────────────────────────┤
│   Core Services Layer   │
│ (FileCleaner, Config)   │
├─────────────────────────┤
│     System Layer        │
│ (Windows Registry, FS)  │
└─────────────────────────┘
```

## 核心模块详解

### 1. TaskManager（任务管理器）

**职责**:
- 管理任务数据的CRUD操作
- 任务数据的序列化/反序列化
- 提供任务查询接口

**文件**: `src/taskmanager.h/cpp`

**主要方法**:
```cpp
bool addTask(const FileCleanTask& task);      // 添加任务
bool updateTask(const FileCleanTask& task);   // 更新任务
bool deleteTask(const QString& taskId);       // 删除任务
FileCleanTask getTask(const QString& taskId); // 获取单个任务
const QVector<FileCleanTask>& getAllTasks();  // 获取所有任务
bool loadTasksFromFile(const QString& filePath);  // 从文件加载
bool saveTasksToFile(const QString& filePath);    // 保存到文件
```

**扩展建议**:
- 添加任务分类功能
- 实现任务搜索和过滤
- 添加任务模板功能

### 2. FileCleaner（文件清理器）

**职责**:
- 递归扫描文件夹
- 判断文件是否应该被删除
- 执行文件删除操作

**文件**: `src/filecleaner.h/cpp`

**关键算法**:
```cpp
QDateTime getFileLastAccessTime();  // 获取文件访问时间
bool shouldDeleteFile();             // 判断是否应删除
void recursiveClean();               // 递归清理
```

**扩展建议**:
- 添加文件类型过滤（如只删除特定扩展名）
- 实现"软删除"（移到回收站而非永久删除）
- 添加文件大小过滤
- 实现清理预览功能

### 3. SchedulerManager（定时调度器）

**职责**:
- 定期检查是否需要执行任务
- 触发任务执行
- 记录任务执行状态

**文件**: `src/schedulermanager.h/cpp`

**关键方法**:
```cpp
void checkSchedule();           // 检查是否需要运行
void runTask(const QString& id); // 执行单个任务
bool isTimeToRun();              // 判断时间
```

**扩展建议**:
- 支持更复杂的时间表（周期性、工作日等）
- 添加任务优先级
- 实现任务依赖关系
- 添加系统负载检查（高负载时推迟execution）

### 4. ConfigManager（配置管理器）

**职责**:
- 管理应用配置
- 处理配置文件读写

**文件**: `src/configmanager.h/cpp`

**扩展建议**:
- 添加更多配置选项
- 支持配置加密
- 实现配置版本管理
- 添加配置备份和恢复

## 数据流程

### 任务创建流程

```
User Input (AddTaskDialog)
    ↓
MainWindow::onAddTask()
    ↓
TaskManager::addTask()
    ↓
TaskModel::addTask()
    ↓
MainWindow::saveTasks()
    ↓
File System
```

### 任务执行流程

```
SchedulerManager::checkSchedule()
    ↓
SchedulerManager::runTask()
    ↓
FileCleaner::cleanFolders()
    ↓
FileCleaner::shouldDeleteFile()
    ↓
FileCleaner::recursiveClean()
    ↓
QFile::remove()
    ↓
UI Update (onTaskFinished)
```

## 编译系统

### qmake 系统

使用传统 Qt qmake 构建系统：

```bash
qmake AutoFileCleaner.pro
nmake              # MSVC
make               # GCC/MinGW
```

### CMake 系统

可选的 CMake 构建系统：

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

## 修改建议

### 添加新的文件过滤选项

1. 在 `FileCleanTask` 结构体中添加字段：
```cpp
struct FileCleanTask {
    // ... existing fields ...
    QStringList excludePatterns;  // 排除模式
    QStringList includeExtensions; // 包含扩展名
};
```

2. 修改 `FileCleaner::shouldDeleteFile()` 方法

3. 在 `AddTaskDialog` 中添加UI控件

### 添加数据库支持

替换JSON存储为SQLite数据库：

1. 添加 `#include <QtSql>`
2. 创建 `DatabaseManager` 类
3. 修改 `TaskManager` 使用数据库后端
4. 实现迁移机制

### 添加远程监控

添加网络功能：

1. 实现 HTTP/WebSocket 服务器
2. 创建 REST API
3. 添加认证机制
4. 实现远程任务管理

## 测试

### 单元测试框架

推荐使用 Qt Test 框架：

```cpp
#include <QtTest>

class TestTaskManager : public QObject {
    Q_OBJECT
private slots:
    void testAddTask();
    void testDeleteTask();
};
```

### 手动测试清单

- [ ] 创建任务
- [ ] 编辑任务
- [ ] 删除任务
- [ ] 运行任务
- [ ] 定时执行任务
- [ ] 开机自启动
- [ ] 最小化到托盘
- [ ] 恢复窗口
- [ ] 保存和加载任务

## 性能优化

### 大规模文件夹处理

对于包含大量文件的文件夹：

1. **实现多线程处理**:
```cpp
class FileCleanerWorker : public QObject {
    Q_OBJECT
};
QThread workerThread;
```

2. **使用QDirIterator替代QDir::entryInfoList()**

3. **批量操作文件**

### 内存优化

1. 使用流式处理大文件
2. 及时释放临时对象
3. 使用 QFileInfo 的轻量级函数

## 调试技巧

### 启用详细日志

```cpp
qDebug() << "Message:" << variable;
qWarning() << "Warning:" << variable;
qCritical() << "Critical:" << variable;
```

### 使用Qt Creator调试器

1. 设置断点
2. 使用表达式求值
3. 查看变量和对象状态
4. 使用调用堆栈跟踪

### 日志文件分析

```cpp
// 写入日志文件
QFile logFile(configManager.getLogFile());
if (logFile.open(QIODevice::Append | QIODevice::Text)) {
    QTextStream out(&logFile);
    out << QDateTime::currentDateTime().toString() << ": " << message << "\n";
    logFile.close();
}
```

## 常见问题解决

### 问题: 文件没有被删除

**检查项**:
1. 文件是否被占用
2. 权限是否足够
3. 文件访问时间是否正确获取
4. 时间阈值是否设置正确

**调试方法**:
```cpp
qDebug() << "File:" << fileInfo.filePath();
qDebug() << "Last Access:" << lastAccessTime.toString();
qDebug() << "Days to delete:" << lastAccessTime.daysTo(QDateTime::currentDateTime());
```

### 问题: 内存泄漏

**检查项**:
1. 所有 `new` 都有对应的 `delete`
2. 信号连接是否正确（避免循环引用）
3. 定时器是否停止

**工具**:
- 使用 Valgrind（Linux）
- 使用 Dr. Memory（Windows）
- Qt Creator 内存分析工具

## 贡献指南

### 代码风格

1. **命名规范**:
   - 类名: PascalCase (MainWindow)
   - 方法: camelCase (cleanFolder)
   - 成员变量: m_camelCase (m_taskManager)
   - 常量: UPPER_CASE (DEFAULT_THRESHOLD)

2. **注释**:
   - 为公开接口添加注释
   - 使用 Doxygen 风格的注释
   - 解释"为什么"而不仅是"做什么"

3. **格式**:
   - 使用 4 个空格缩进
   - 每行最多 100 字符
   - 避免深度嵌套

### 提交消息

```
[类别] 简短描述

详细描述（如需要）

相关问题: #123
```

## 资源链接

- [Qt 官方文档](https://doc.qt.io/)
- [Qt Creator 用户指南](https://doc.qt.io/qtcreator/)
- [Windows API 文档](https://docs.microsoft.com/en-us/windows/win32/)

## 许可证

本项目遵循 MIT 许可证。

---

**最后更新**: 2024-02-22
