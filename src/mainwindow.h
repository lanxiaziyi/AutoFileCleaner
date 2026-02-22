#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QTableView>
#include <QLabel>
#include <QSystemTrayIcon>
#include <QCheckBox>
#include "taskmanager.h"
#include "taskmodel.h"
#include "configmanager.h"
#include "schedulermanager.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;
    void changeEvent(QEvent *event) override;

private slots:
    // 任务管理
    void onAddTask();
    void onEditTask();
    void onDeleteTask();
    void onRunTask();
    void onRunAllTasks();
    void onToggleTask();
    
    // 调度相关
    void onTaskStarted(const QString& taskId);
    void onTaskFinished(const QString& taskId, bool success, const QString& message);
    void onTaskError(const QString& taskId, const QString& errorMessage);
    
    // 设置
    void onSettings();
    void onAbout();
    void onAutoStartToggled(bool checked);
    
    // 系统托盘
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void createTrayIcon();
    
    // 数据保存和加载
    void loadTasks();
    void saveTasks();

private:
    void initUI();
    void initConnections();
    void startScheduler();
    void stopScheduler();
    void refreshTaskList();

    // UI 组件
    QTableView* m_taskTable;
    TaskModel* m_taskModel;
    QPushButton* m_addBtn;
    QPushButton* m_editBtn;
    QPushButton* m_deleteBtn;
    QPushButton* m_runBtn;
    QPushButton* m_runAllBtn;
    QLabel* m_statusLabel;
    QLabel* m_enabledCountLabel;
    QCheckBox* m_autoStartCheckBox;
    
    // 业务逻辑
    TaskManager m_taskManager;
    ConfigManager m_configManager;
    SchedulerManager* m_schedulerManager;
    
    // 系统托盘
    QSystemTrayIcon* m_trayIcon;
    QMenu* m_trayMenu;
    
    // 状态
    bool m_schedulerRunning;
};

#endif // MAINWINDOW_H
