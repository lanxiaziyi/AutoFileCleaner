#include "mainwindow.h"
#include "addtaskdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableView>
#include <QHeaderView>
#include <QMessageBox>
#include <QApplication>
#include <QMenu>
#include <QAction>
#include <QSystemTrayIcon>
#include <QClipboard>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_schedulerRunning(false) {
    
    setWindowTitle("自动文件清理工具");
    setGeometry(100, 100, 1000, 600);
    
    m_configManager.loadConfig();
    initUI();
    initConnections();
    
    // 创建托盘图标
    createTrayIcon();
    
    // 加载任务
    loadTasks();
    
    // 创建调度器
    m_schedulerManager = new SchedulerManager(&m_taskManager, this);
    connect(m_schedulerManager, &SchedulerManager::taskStarted, this, &MainWindow::onTaskStarted);
    connect(m_schedulerManager, &SchedulerManager::taskFinished, this, &MainWindow::onTaskFinished);
    connect(m_schedulerManager, &SchedulerManager::taskError, this, &MainWindow::onTaskError);
    
    // 启动调度器
    startScheduler();
}

MainWindow::~MainWindow() {
    stopScheduler();
}

void MainWindow::initUI() {
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    
    // 任务表格
    m_taskTable = new QTableView();
    m_taskModel = new TaskModel(this);
    m_taskTable->setModel(m_taskModel);
    m_taskTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_taskTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_taskTable->setAlternatingRowColors(true);
    m_taskTable->horizontalHeader()->setStretchLastSection(true);
    m_taskTable->setColumnWidth(0, 120);
    m_taskTable->setColumnWidth(1, 150);
    m_taskTable->setColumnWidth(2, 80);
    m_taskTable->setColumnWidth(3, 100);
    m_taskTable->setColumnWidth(4, 100);
    m_taskTable->setColumnWidth(5, 80);
    
    mainLayout->addWidget(new QLabel("清理任务列表:"));
    mainLayout->addWidget(m_taskTable);
    
    // 按钮行
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    m_addBtn = new QPushButton("新建任务");
    m_editBtn = new QPushButton("编辑选中");
    m_deleteBtn = new QPushButton("删除选中");
    m_runBtn = new QPushButton("运行选中");
    m_runAllBtn = new QPushButton("运行全部");
    
    buttonLayout->addWidget(m_addBtn);
    buttonLayout->addWidget(m_editBtn);
    buttonLayout->addWidget(m_deleteBtn);
    buttonLayout->addSpacing(20);
    buttonLayout->addWidget(m_runBtn);
    buttonLayout->addWidget(m_runAllBtn);
    buttonLayout->addStretch();
    
    mainLayout->addLayout(buttonLayout);
    
    // 状态栏
    m_statusLabel = new QLabel("调度器: 已启动");
    m_enabledCountLabel = new QLabel("已启用任务: 0");
    
    QHBoxLayout* statusLayout = new QHBoxLayout();
    statusLayout->addWidget(m_statusLabel);
    statusLayout->addSpacing(20);
    statusLayout->addWidget(m_enabledCountLabel);
    statusLayout->addStretch();
    
    mainLayout->addLayout(statusLayout);
    
    setCentralWidget(centralWidget);
}

void MainWindow::initConnections() {
    connect(m_addBtn, &QPushButton::clicked, this, &MainWindow::onAddTask);
    connect(m_editBtn, &QPushButton::clicked, this, &MainWindow::onEditTask);
    connect(m_deleteBtn, &QPushButton::clicked, this, &MainWindow::onDeleteTask);
    connect(m_runBtn, &QPushButton::clicked, this, &MainWindow::onRunTask);
    connect(m_runAllBtn, &QPushButton::clicked, this, &MainWindow::onRunAllTasks);
}

void MainWindow::onAddTask() {
    AddTaskDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        FileCleanTask task = dialog.getTask();
        if (m_taskManager.addTask(task)) {
            m_taskModel->addTask(task);
            saveTasks();
            m_enabledCountLabel->setText(QString("已启用任务: %1").arg(m_taskManager.getEnabledTaskCount()));
            QMessageBox::information(this, "成功", "任务已创建");
        } else {
            QMessageBox::critical(this, "错误", "创建任务失败");
        }
    }
}

void MainWindow::onEditTask() {
    QModelIndex index = m_taskTable->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, "提示", "请选择要编辑的任务");
        return;
    }
    
    FileCleanTask task = m_taskModel->getTask(index.row());
    AddTaskDialog dialog(task, this);
    if (dialog.exec() == QDialog::Accepted) {
        FileCleanTask updatedTask = dialog.getTask();
        updatedTask.id = task.id;
        updatedTask.created = task.created;
        updatedTask.lastRun = task.lastRun;
        
        if (m_taskManager.updateTask(updatedTask)) {
            m_taskModel->updateTask(index.row(), updatedTask);
            saveTasks();
            m_enabledCountLabel->setText(QString("已启用任务: %1").arg(m_taskManager.getEnabledTaskCount()));
            QMessageBox::information(this, "成功", "任务已更新");
        } else {
            QMessageBox::critical(this, "错误", "更新任务失败");
        }
    }
}

void MainWindow::onDeleteTask() {
    QModelIndex index = m_taskTable->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, "提示", "请选择要删除的任务");
        return;
    }
    
    if (QMessageBox::question(this, "确认", "确定要删除此任务吗?") == QMessageBox::Yes) {
        FileCleanTask task = m_taskModel->getTask(index.row());
        if (m_taskManager.deleteTask(task.id)) {
            m_taskModel->removeTask(index.row());
            saveTasks();
            m_enabledCountLabel->setText(QString("已启用任务: %1").arg(m_taskManager.getEnabledTaskCount()));
            QMessageBox::information(this, "成功", "任务已删除");
        }
    }
}

void MainWindow::onRunTask() {
    QModelIndex index = m_taskTable->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, "提示", "请选择要运行的任务");
        return;
    }
    
    FileCleanTask task = m_taskModel->getTask(index.row());
    m_schedulerManager->runTask(task.id);
}

void MainWindow::onRunAllTasks() {
    if (QMessageBox::question(this, "确认", "确定要运行所有已启用的任务吗?") == QMessageBox::Yes) {
        m_schedulerManager->runAllEnabledTasks();
    }
}

void MainWindow::onTaskStarted(const QString& taskId) {
    FileCleanTask task = m_taskManager.getTask(taskId);
    m_statusLabel->setText(QString("正在运行: %1...").arg(task.name));
    qDebug() << "Task started:" << task.name;
}

void MainWindow::onTaskFinished(const QString& taskId, bool success, const QString& message) {
    FileCleanTask task = m_taskManager.getTask(taskId);
    m_statusLabel->setText(QString("任务完成: %1").arg(task.name));
    
    // 刷新列表显示最后运行时间
    int row = m_taskModel->findTaskRow(taskId);
    if (row >= 0) {
        m_taskModel->updateTask(row, task);
    }
    
    if (success) {
        QMessageBox::information(this, "任务完成", message);
    } else {
        QMessageBox::warning(this, "任务完成", message);
    }
}

void MainWindow::onTaskError(const QString& taskId, const QString& errorMessage) {
    m_statusLabel->setText("任务出错");
    QMessageBox::critical(this, "任务错误", errorMessage);
}

void MainWindow::onToggleTask() {
    QModelIndex index = m_taskTable->currentIndex();
    if (!index.isValid()) {
        return;
    }
    
    FileCleanTask task = m_taskModel->getTask(index.row());
    task.enabled = !task.enabled;
    m_taskManager.updateTask(task);
    m_taskModel->updateTask(index.row(), task);
    saveTasks();
}

void MainWindow::startScheduler() {
    if (!m_schedulerRunning) {
        m_schedulerManager->start();
        m_schedulerRunning = true;
        m_statusLabel->setText("调度器: 已启动");
    }
}

void MainWindow::stopScheduler() {
    if (m_schedulerRunning) {
        m_schedulerManager->stop();
        m_schedulerRunning = false;
        m_statusLabel->setText("调度器: 已停止");
    }
}

void MainWindow::createTrayIcon() {
    m_trayIcon = new QSystemTrayIcon(this);
    
    QIcon icon;
    // 可以设置应用图标
    // icon = QIcon(":/images/app.ico");
    
    m_trayMenu = new QMenu(this);
    
    QAction* showAction = m_trayMenu->addAction("显示");
    connect(showAction, &QAction::triggered, this, &QWidget::showNormal);
    
    QAction* runAllAction = m_trayMenu->addAction("运行所有任务");
    connect(runAllAction, &QAction::triggered, this, &MainWindow::onRunAllTasks);
    
    m_trayMenu->addSeparator();
    
    QAction* exitAction = m_trayMenu->addAction("退出");
    connect(exitAction, &QAction::triggered, qApp, &QApplication::quit);
    
    m_trayIcon->setContextMenu(m_trayMenu);
    m_trayIcon->show();
    
    connect(m_trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::onTrayIconActivated);
}

void MainWindow::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason) {
    if (reason == QSystemTrayIcon::DoubleClick || reason == QSystemTrayIcon::Trigger) {
        if (isVisible()) {
            hide();
        } else {
            showNormal();
            activateWindow();
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (m_trayIcon && m_trayIcon->isVisible()) {
        hide();
        event->ignore();
    } else {
        event->accept();
    }
}

void MainWindow::changeEvent(QEvent *event) {
    if (event->type() == QEvent::WindowStateChange) {
        if (isMinimized() && m_configManager.isMinimizeToTray()) {
            hide();
            event->ignore();
            return;
        }
    }
    QMainWindow::changeEvent(event);
}

void MainWindow::loadTasks() {
    if (m_taskManager.loadTasksFromFile(m_configManager.getTasksFile())) {
        m_taskModel->setTasks(m_taskManager.getAllTasks());
        qDebug() << "Tasks loaded successfully";
    } else {
        qDebug() << "No tasks file found";
    }
    
    m_enabledCountLabel->setText(QString("已启用任务: %1").arg(m_taskManager.getEnabledTaskCount()));
}

void MainWindow::saveTasks() {
    if (m_taskManager.saveTasksToFile(m_configManager.getTasksFile())) {
        qDebug() << "Tasks saved successfully";
    } else {
        qWarning() << "Failed to save tasks";
    }
}

void MainWindow::onSettings() {
    // 待实现
    QMessageBox::information(this, "设置", "设置功能待完善");
}

void MainWindow::onAbout() {
    QMessageBox::about(this, "关于",
        "自动文件清理工具 v1.0\n\n"
        "这是一个Windows系统中自动清理旧文件的工具。\n\n"
        "© 2024 All Rights Reserved"
    );
}

void MainWindow::refreshTaskList() {
    m_taskModel->setTasks(m_taskManager.getAllTasks());
    m_enabledCountLabel->setText(QString("已启用任务: %1").arg(m_taskManager.getEnabledTaskCount()));
}
