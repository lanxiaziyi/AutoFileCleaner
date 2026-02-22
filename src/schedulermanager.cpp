#include "schedulermanager.h"
#include "filecleaner.h"
#include <QTime>
#include <QDebug>

SchedulerManager::SchedulerManager(TaskManager* taskManager, QObject* parent)
    : QObject(parent), m_taskManager(taskManager), m_isRunning(false) {
    
    m_checkTimer = new QTimer(this);
    connect(m_checkTimer, &QTimer::timeout, this, &SchedulerManager::checkSchedule);
    m_checkTimer->setInterval(60000);  // 默认每分钟检查一次
}

SchedulerManager::~SchedulerManager() {
    if (m_isRunning) {
        stop();
    }
}

void SchedulerManager::start() {
    if (!m_isRunning) {
        m_checkTimer->start();
        m_isRunning = true;
        qDebug() << "SchedulerManager started";
    }
}

void SchedulerManager::stop() {
    if (m_isRunning) {
        m_checkTimer->stop();
        m_isRunning = false;
        qDebug() << "SchedulerManager stopped";
    }
}

bool SchedulerManager::isRunning() const {
    return m_isRunning;
}

void SchedulerManager::runTask(const QString& taskId) {
    FileCleanTask task = m_taskManager->getTask(taskId);
    if (task.id.isEmpty()) {
        emit taskError(taskId, "任务不存在");
        return;
    }
    
    if (!task.enabled) {
        emit taskError(taskId, "任务已禁用");
        return;
    }
    
    emit taskStarted(taskId);
    
    FileCleaner cleaner;
    QVector<CleaningResult> results = cleaner.cleanFolders(task.targetFolders, task.daysThreshold);
    
    int totalFilesDeleted = 0;
    qint64 totalSpaceFreed = 0;
    QString errorMessages;
    
    for (const auto& result : results) {
        totalFilesDeleted += result.filesDeleted;
        totalSpaceFreed += result.spaceFreed;
        
        if (!result.lastError.isEmpty()) {
            errorMessages += QString("文件夹: %1, 错误: %2\n").arg(result.folder, result.lastError);
        }
    }
    
    // 更新任务的最后运行时间
    task.lastRun = QDateTime::currentDateTime();
    m_taskManager->updateTask(task);
    m_lastRunTimes[taskId] = task.lastRun;
    
    QString message = QString("成功删除 %1 个文件，释放空间 %2 MB")
        .arg(totalFilesDeleted)
        .arg(totalSpaceFreed / (1024 * 1024));
    
    if (!errorMessages.isEmpty()) {
        message += "\n警告:\n" + errorMessages;
    }
    
    emit taskFinished(taskId, errorMessages.isEmpty(), message);
}

void SchedulerManager::runAllEnabledTasks() {
    QVector<FileCleanTask> enabledTasks = m_taskManager->getEnabledTasks();
    for (const auto& task : enabledTasks) {
        runTask(task.id);
    }
}

void SchedulerManager::setCheckInterval(int seconds) {
    m_checkTimer->setInterval(seconds * 1000);
}

void SchedulerManager::checkSchedule() {
    QVector<FileCleanTask> enabledTasks = m_taskManager->getEnabledTasks();
    
    for (const auto& task : enabledTasks) {
        if (isTimeToRun(task.scheduleTime, task.id)) {
            qDebug() << "Running scheduled task:" << task.name;
            runTask(task.id);
        }
    }
}

bool SchedulerManager::isTimeToRun(const QString& scheduleTime, const QString& taskId) {
    // 解析计划时间
    QTime plannedTime = QTime::fromString(scheduleTime, "HH:mm");
    if (!plannedTime.isValid()) {
        return false;
    }
    
    QDateTime now = QDateTime::currentDateTime();
    QDateTime plannedDateTime = QDateTime(now.date(), plannedTime);
    
    // 检查上次运行时间
    QDateTime lastRunTime = m_lastRunTimes.value(taskId, QDateTime());
    
    // 如果计划时间已过，并且上次运行时间不是今天，则运行
    if (now >= plannedDateTime) {
        if (!lastRunTime.isValid() || lastRunTime.date() != now.date()) {
            return true;
        }
    }
    
    return false;
}

QString SchedulerManager::formatResult(const FileCleanTask& task, int filesDeleted, qint64 spaceFreed) {
    return QString("任务 '%1' 完成: 删除 %2 个文件，释放 %3 MB")
        .arg(task.name, QString::number(filesDeleted), QString::number(spaceFreed / (1024 * 1024)));
}
