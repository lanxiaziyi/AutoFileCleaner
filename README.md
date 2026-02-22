# 自动文件清理工具 (Auto File Cleaner)

这是一个基于Qt的Windows应用程序，可以自动定时扫描和删除指定文件夹中老旧文件。

## 功能特点

- ✅ **开机自启动**: 支持Windows系统自启动
- ✅ **定时扫描**: 可自定义每天执行的时间
- ✅ **灵活清理**: 可配置多个清理任务，每个任务可指定多个目标文件夹
- ✅ **时间阈值**: 可设置超过N天未访问的文件自动删除
- ✅ **任务备注**: 每个任务都可以添加描述和备注
- ✅ **图形界面**: 直观的任务管理界面
- ✅ **系统托盘**: 最小化到系统托盘，后台运行
- ✅ **运行日志**: 记录每次清理的详细信息

## 需求

### 系统要求
- Windows 10 或 Windows 11
- Qt 5.12 或更新版本
- Visual Studio 2017 或更新版本（编译器）

### 依赖库
- Qt Core
- Qt GUI
- Qt Widgets
- Qt Network
- Qt SQL

## 编译指南

### 使用 Qt Creator

1. **打开项目**
   - 在 Qt Creator 中打开 `AutoFileCleaner.pro` 项目文件

2. **配置构建**
   - 选择合适的 Qt 版本和编译器
   - 配置为 Release 模式以获得最佳性能

3. **编译**
   - 点击"构建"或按 Ctrl+B

4. **运行**
   - 点击"运行"或按 Ctrl+R

### 使用命令行编译

```bash
# 使用 qmake 和 make
cd QtDemo
qmake AutoFileCleaner.pro -spec win32-msvc
nmake release

# 输出文件位于 build/release/ 目录
```

## 使用说明

### 基本步骤

1. **创建清理任务**
   - 点击"新建任务"按钮
   - 输入任务名称和描述
   - 添加要清理的目标文件夹（可添加多个）
   - 设置清理阈值（多少天未访问的文件删除）
   - 设置执行时间（每天的几点运行）
   - 点击"确定"保存

2. **管理任务**
   - **编辑**: 选择任务并点击"编辑选中"
   - **删除**: 选择任务并点击"删除选中"
   - **运行**: 选择任务并点击"运行选中"立即执行
   - **运行全部**: 点击"运行全部"执行所有已启用的任务

3. **启用自启动**
   - 第一次运行时，可在设置中启用开机自启动
   - 应用会在 Windows 开机时自动启动并后台运行

### 界面说明

- **任务列表**: 显示所有创建的清理任务及其状态
- **按钮区**: 用于创建、编辑、删除和执行任务
- **状态栏**: 显示当前调度器状态和已启用任务数量
- **系统托盘**: 应用可最小化到系统托盘，双击可打开

## 配置文件

应用数据保存在以下位置：

```
%APPDATA%\AutoFileCleaner\
├── config.ini       # 应用配置文件
├── tasks.json       # 任务数据（JSON格式）
└── logs/            # 日志目录
    └── logs.txt     # 运行日志
```

### tasks.json 格式示例

```json
[
  {
    "id": "uuid-string",
    "name": "临时文件清理",
    "description": "清理临时文件夹中的旧文件",
    "targetFolders": [
      "C:\\Users\\Username\\AppData\\Local\\Temp",
      "D:\\TempFiles"
    ],
    "daysThreshold": 30,
    "scheduleTime": "02:00",
    "enabled": true,
    "created": "2024-02-22T10:00:00Z",
    "lastRun": "2024-02-22T02:00:01Z"
  }
]
```

## 开发说明

### 项目结构

```
QtDemo/
├── AutoFileCleaner.pro          # 项目配置文件
├── src/
│   ├── main.cpp                 # 程序入口
│   ├── mainwindow.h/cpp         # 主窗口
│   ├── taskmanager.h/cpp        # 任务管理器
│   ├── taskmodel.h/cpp          # 任务数据模型
│   ├── configmanager.h/cpp      # 配置管理器
│   ├── filecleaner.h/cpp        # 文件清理引擎
│   ├── schedulermanager.h/cpp   # 定时调度器
│   ├── addtaskdialog.h/cpp      # 新建/编辑任务对话框
│   └── autostartutiility_win.h/cpp  # Windows自启动工具
├── resources/
│   └── resources.qrc            # 资源文件
└── build/                       # 构建输出目录
```

### 关键类说明

- **MainWindow**: 主应用窗口，负责UI和用户交互
- **TaskManager**: 任务管理和数据持久化
- **TaskModel**: Qt数据模型，用于表格显示
- **FileCleaner**: 文件扫描和删除的核心逻辑
- **SchedulerManager**: 定时调度任务执行
- **ConfigManager**: 应用配置管理
- **AutoStartUtility**: Windows注册表操作，实现开机自启

## 注意事项

1. **文件删除是不可逆的**：删除的文件无法恢复，请谨慎配置清理规则
2. **访问权限**: 需要对目标文件夹有读写权限
3. **系统文件保护**: 无法删除被系统占用的文件
4. **时区**：程序使用本地时间，请确保系统时间正确

## 常见问题

### Q: 程序无法删除某些文件？
A: 可能是因为：
- 文件被其他程序占用
- 没有删除权限
- 文件被系统保护
检查日志文件了解具体原因。

### Q: 任务没有在指定时间执行？
A: 请检查：
- 系统时间是否正确
- 任务是否已启用
- 任务中是否有有效的文件夹和时间设置

### Q: 如何卸载程序？
A: 
1. 在控制面板中卸载应用
2. 程序会自动删除配置文件
3. 如需保留任务数据，请提前备份 `tasks.json`

## 许可证

本项目基于 MIT 许可证。详见 LICENSE 文件。

## 作者

Created with Qt Framework

## 更新日志

### v1.0.0 (2024-02-22)
- 初始版本发布
- 支持任务创建和管理
- 支持定时执行和手动运行
- 支持开机自启动
- 系统托盘支持

---

**开发提示**: 修改代码后请重新编译。如遇到编译错误，请确保已正确配置 Qt 环境。
