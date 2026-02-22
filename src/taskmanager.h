#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <QString>
#include <QVector>
#include <QVariant>
#include <QJsonObject>
#include <QDateTime>

struct FileCleanTask {
    QString id;                         // 任务ID (UUID)
    QString name;                       // 任务名称
    QString description;                // 任务描述/备注
    QStringList targetFolders;          // 目标文件夹列表
    int daysThreshold;                  // 多少天未访问的文件才删除
    QString scheduleTime;               // 执行时间 (HH:mm格式)
    bool enabled;                       // 是否启用
    QDateTime created;                  // 创建时间
    QDateTime lastRun;                  // 最后运行时间

    // JSON序列化
    QJsonObject toJson() const;
    static FileCleanTask fromJson(const QJsonObject& json);
};

class TaskManager {
public:
    TaskManager();

    // 任务操作
    bool addTask(const FileCleanTask& task);
    bool updateTask(const FileCleanTask& task);
    bool deleteTask(const QString& taskId);
    bool enableTask(const QString& taskId, bool enable);
    
    // 查询操作
    FileCleanTask getTask(const QString& taskId) const;
    const QVector<FileCleanTask>& getAllTasks() const;
    QVector<FileCleanTask> getEnabledTasks() const;
    
    // 文件操作
    bool loadTasksFromFile(const QString& filePath);
    bool saveTasksToFile(const QString& filePath) const;
    
    // 统计信息
    int getTaskCount() const;
    int getEnabledTaskCount() const;

private:
    QVector<FileCleanTask> m_tasks;
};

#endif // TASKMANAGER_H
