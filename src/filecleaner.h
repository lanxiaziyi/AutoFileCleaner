#ifndef FILECLEANER_H
#define FILECLEANER_H

#include <QString>
#include <QStringList>
#include <QVector>
#include <QDateTime>

struct CleaningResult {
    QString folder;
    int filesDeleted = 0;
    qint64 spaceFreed = 0;  // 字节
    QString lastError;
};

class FileCleaner {
public:
    FileCleaner();

    // 主清理函数
    bool cleanFolder(const QString& folderPath, int daysThreshold, CleaningResult& result);
    
    // 批量清理
    QVector<CleaningResult> cleanFolders(const QStringList& folderPaths, int daysThreshold);
    
    // 扫描函数（仅扫描，不删除）
    QVector<QString> scanFolder(const QString& folderPath, int daysThreshold, int& fileCount, qint64& totalSize);
    
private:
    // 获取文件的最后访问时间
    QDateTime getFileLastAccessTime(const QString& filePath);
    
    // 检查文件是否应该被删除
    bool shouldDeleteFile(const QString& filePath, int daysThreshold);
    
    // 递归清理文件夹
    void recursiveClean(const QString& folderPath, int daysThreshold, int& filesDeleted, qint64& spaceFreed);
};

#endif // FILECLEANER_H
