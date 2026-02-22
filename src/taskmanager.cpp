#include "taskmanager.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QUuid>
#include <QDebug>

QJsonObject FileCleanTask::toJson() const {
    QJsonObject json;
    json["id"] = id;
    json["name"] = name;
    json["description"] = description;
    json["daysThreshold"] = daysThreshold;
    json["scheduleTime"] = scheduleTime;
    json["enabled"] = enabled;
    json["created"] = created.toString(Qt::ISODate);
    json["lastRun"] = lastRun.toString(Qt::ISODate);
    
    QJsonArray folders;
    for (const QString& folder : targetFolders) {
        folders.append(folder);
    }
    json["targetFolders"] = folders;
    
    return json;
}

FileCleanTask FileCleanTask::fromJson(const QJsonObject& json) {
    FileCleanTask task;
    task.id = json["id"].toString();
    task.name = json["name"].toString();
    task.description = json["description"].toString();
    task.daysThreshold = json["daysThreshold"].toInt();
    task.scheduleTime = json["scheduleTime"].toString();
    task.enabled = json["enabled"].toBool();
    task.created = QDateTime::fromString(json["created"].toString(), Qt::ISODate);
    task.lastRun = QDateTime::fromString(json["lastRun"].toString(), Qt::ISODate);
    
    task.targetFolders.clear();
    QJsonArray folders = json["targetFolders"].toArray();
    for (const QJsonValue& folder : folders) {
        task.targetFolders.append(folder.toString());
    }
    
    return task;
}

TaskManager::TaskManager() {
}

bool TaskManager::addTask(const FileCleanTask& task) {
    // 检查ID是否已存在
    for (const auto& t : m_tasks) {
        if (t.id == task.id) {
            return false;
        }
    }
    
    FileCleanTask newTask = task;
    if (newTask.id.isEmpty()) {
        newTask.id = QUuid::createUuid().toString();
    }
    if (!newTask.created.isValid()) {
        newTask.created = QDateTime::currentDateTime();
    }
    
    m_tasks.append(newTask);
    return true;
}

bool TaskManager::updateTask(const FileCleanTask& task) {
    for (int i = 0; i < m_tasks.size(); ++i) {
        if (m_tasks[i].id == task.id) {
            m_tasks[i] = task;
            return true;
        }
    }
    return false;
}

bool TaskManager::deleteTask(const QString& taskId) {
    for (int i = 0; i < m_tasks.size(); ++i) {
        if (m_tasks[i].id == taskId) {
            m_tasks.removeAt(i);
            return true;
        }
    }
    return false;
}

bool TaskManager::enableTask(const QString& taskId, bool enable) {
    for (auto& task : m_tasks) {
        if (task.id == taskId) {
            task.enabled = enable;
            return true;
        }
    }
    return false;
}

FileCleanTask TaskManager::getTask(const QString& taskId) const {
    for (const auto& task : m_tasks) {
        if (task.id == taskId) {
            return task;
        }
    }
    return FileCleanTask();
}

const QVector<FileCleanTask>& TaskManager::getAllTasks() const {
    return m_tasks;
}

QVector<FileCleanTask> TaskManager::getEnabledTasks() const {
    QVector<FileCleanTask> enabled;
    for (const auto& task : m_tasks) {
        if (task.enabled) {
            enabled.append(task);
        }
    }
    return enabled;
}

bool TaskManager::loadTasksFromFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open tasks file:" << filePath;
        return false;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isArray()) {
        qWarning() << "Invalid tasks file format";
        return false;
    }
    
    m_tasks.clear();
    QJsonArray array = doc.array();
    for (const QJsonValue& value : array) {
        if (value.isObject()) {
            m_tasks.append(FileCleanTask::fromJson(value.toObject()));
        }
    }
    
    return true;
}

bool TaskManager::saveTasksToFile(const QString& filePath) const {
    QJsonArray array;
    for (const auto& task : m_tasks) {
        array.append(task.toJson());
    }
    
    QJsonDocument doc(array);
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open tasks file for writing:" << filePath;
        return false;
    }
    
    file.write(doc.toJson());
    file.close();
    
    return true;
}

int TaskManager::getTaskCount() const {
    return m_tasks.size();
}

int TaskManager::getEnabledTaskCount() const {
    int count = 0;
    for (const auto& task : m_tasks) {
        if (task.enabled) {
            count++;
        }
    }
    return count;
}
