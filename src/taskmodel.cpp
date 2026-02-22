#include "taskmodel.h"
#include <QDebug>

TaskModel::TaskModel(QObject *parent)
    : QAbstractTableModel(parent) {
}

int TaskModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return m_tasks.size();
}

int TaskModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid()) {
        return 0;
    }
    // 任务名 | 描述 | 目标文件夹数 | 清理阈值 | 运行时间 | 启用状态 | 最后运行
    return 7;
}

QVariant TaskModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= m_tasks.size()) {
        return QVariant();
    }
    
    const FileCleanTask& task = m_tasks.at(index.row());
    
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (index.column()) {
            case 0: // 任务名
                return task.name;
            case 1: // 描述
                return task.description;
            case 2: // 文件夹数
                return task.targetFolders.size();
            case 3: // 清理阈值
                return QString("%1 天").arg(task.daysThreshold);
            case 4: // 运行时间
                return task.scheduleTime;
            case 5: // 启用状态
                return task.enabled ? "已启用" : "已禁用";
            case 6: // 最后运行
                if (task.lastRun.isValid()) {
                    return task.lastRun.toString("yyyy-MM-dd HH:mm:ss");
                }
                return "未运行";
            default:
                break;
        }
    }
    
    if (role == Qt::TextAlignmentRole) {
        return QVariant(int(Qt::AlignLeft | Qt::AlignVCenter));
    }
    
    return QVariant();
}

QVariant TaskModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
            case 0: return "任务名称";
            case 1: return "描述";
            case 2: return "文件夹数";
            case 3: return "清理阈值";
            case 4: return "运行时间";
            case 5: return "启用";
            case 6: return "最后运行";
            default: break;
        }
    }
    return QVariant();
}

void TaskModel::setTasks(const QVector<FileCleanTask>& tasks) {
    beginResetModel();
    m_tasks = tasks;
    endResetModel();
}

void TaskModel::addTask(const FileCleanTask& task) {
    beginInsertRows(QModelIndex(), m_tasks.size(), m_tasks.size());
    m_tasks.append(task);
    endInsertRows();
}

void TaskModel::updateTask(int row, const FileCleanTask& task) {
    if (row < 0 || row >= m_tasks.size()) {
        return;
    }
    m_tasks[row] = task;
    emit dataChanged(index(row, 0), index(row, columnCount() - 1));
}

void TaskModel::removeTask(int row) {
    if (row < 0 || row >= m_tasks.size()) {
        return;
    }
    beginRemoveRows(QModelIndex(), row, row);
    m_tasks.removeAt(row);
    endRemoveRows();
}

void TaskModel::clear() {
    beginResetModel();
    m_tasks.clear();
    endResetModel();
}

FileCleanTask TaskModel::getTask(int row) const {
    if (row < 0 || row >= m_tasks.size()) {
        return FileCleanTask();
    }
    return m_tasks.at(row);
}

int TaskModel::findTaskRow(const QString& taskId) const {
    for (int i = 0; i < m_tasks.size(); ++i) {
        if (m_tasks[i].id == taskId) {
            return i;
        }
    }
    return -1;
}
