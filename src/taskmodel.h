#ifndef TASKMODEL_H
#define TASKMODEL_H

#include <QAbstractTableModel>
#include "taskmanager.h"

class TaskModel : public QAbstractTableModel {
    Q_OBJECT

public:
    explicit TaskModel(QObject *parent = nullptr);
    
    // 模型接口
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    
    // 数据管理
    void setTasks(const QVector<FileCleanTask>& tasks);
    void addTask(const FileCleanTask& task);
    void updateTask(int row, const FileCleanTask& task);
    void removeTask(int row);
    void clear();
    
    // 查询
    FileCleanTask getTask(int row) const;
    int findTaskRow(const QString& taskId) const;

private:
    QVector<FileCleanTask> m_tasks;
};

#endif // TASKMODEL_H
