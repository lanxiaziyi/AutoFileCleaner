#ifndef SCHEDULERMANAGER_H
#define SCHEDULERMANAGER_H

#include <QObject>
#include <QTimer>
#include <QMap>
#include <QDateTime>
#include "taskmanager.h"

class SchedulerManager : public QObject {
    Q_OBJECT

public:
    explicit SchedulerManager(TaskManager* taskManager, QObject* parent = nullptr);
    ~SchedulerManager();

    // 启动和停止调度
    void start();
    void stop();
    bool isRunning() const;

    // 手动运行任务
    void runTask(const QString& taskId);
    void runAllEnabledTasks();

    // 设置调度检查间隔（秒）
    void setCheckInterval(int seconds);

signals:
    void taskStarted(const QString& taskId);
    void taskFinished(const QString& taskId, bool success, const QString& message);
    void taskError(const QString& taskId, const QString& errorMessage);

private slots:
    void checkSchedule();

private:
    TaskManager* m_taskManager;
    QTimer* m_checkTimer;
    QMap<QString, QDateTime> m_lastRunTimes;
    bool m_isRunning;
    
    bool isTimeToRun(const QString& scheduleTime, const QString& taskId);
    QString formatResult(const FileCleanTask& task, int filesDeleted, qint64 spaceFreed);
};

#endif // SCHEDULERMANAGER_H
