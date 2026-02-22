#include "filecleaner.h"
#include <QDir>
#include <QFileInfo>
#include <QDateTime>
#include <QDebug>
#include <QDirIterator>

FileCleaner::FileCleaner() {
}

bool FileCleaner::cleanFolder(const QString& folderPath, int daysThreshold, CleaningResult& result) {
    result.folder = folderPath;
    result.filesDeleted = 0;
    result.spaceFreed = 0;
    result.lastError.clear();
    
    QDir dir(folderPath);
    if (!dir.exists()) {
        result.lastError = "文件夹不存在";
        return false;
    }
    
    try {
        recursiveClean(folderPath, daysThreshold, result.filesDeleted, result.spaceFreed);
        return true;
    } catch (const std::exception& e) {
        result.lastError = QString::fromStdString(e.what());
        return false;
    }
}

QVector<CleaningResult> FileCleaner::cleanFolders(const QStringList& folderPaths, int daysThreshold) {
    QVector<CleaningResult> results;
    for (const QString& folder : folderPaths) {
        CleaningResult result;
        cleanFolder(folder, daysThreshold, result);
        results.append(result);
    }
    return results;
}

QVector<QString> FileCleaner::scanFolder(const QString& folderPath, int daysThreshold, int& fileCount, qint64& totalSize) {
    QVector<QString> filesToDelete;
    fileCount = 0;
    totalSize = 0;
    
    QDir dir(folderPath);
    if (!dir.exists()) {
        return filesToDelete;
    }
    
    QDirIterator iterator(folderPath, QDirIterator::Subdirectories);
    while (iterator.hasNext()) {
        iterator.next();
        
        QFileInfo fileInfo = iterator.fileInfo();
        
        // 跳过目录
        if (fileInfo.isDir()) {
            continue;
        }
        
        fileCount++;
        totalSize += fileInfo.size();
        
        if (shouldDeleteFile(fileInfo.filePath(), daysThreshold)) {
            filesToDelete.append(fileInfo.filePath());
        }
    }
    
    return filesToDelete;
}

QDateTime FileCleaner::getFileLastAccessTime(const QString& filePath) {
#ifdef Q_OS_WIN
    // Windows 特定实现
    WIN32_FILE_ATTRIBUTE_DATA fileAttrData;
    if (GetFileAttributesExW((LPCWSTR)filePath.utf16(), GetFileExInfoStandard, &fileAttrData)) {
        FILETIME ftLastAccessTime = fileAttrData.ftLastAccessTime;
        ULARGE_INTEGER uli;
        uli.LowPart = ftLastAccessTime.dwLowDateTime;
        uli.HighPart = ftLastAccessTime.dwHighDateTime;
        
        // Windows 时间戳是从 1601 年开始，需要转换
        const qint64 EPOCH_DIFFERENCE = 116444736000000000LL;  // 100-nanosecond intervals
        qint64 msecs = (uli.QuadPart - EPOCH_DIFFERENCE) / 10000;
        
        return QDateTime::fromMSecsSinceEpoch(msecs, Qt::UTC);
    }
#else
    QFileInfo fileInfo(filePath);
    return fileInfo.lastRead();
#endif
    return QDateTime();
}

bool FileCleaner::shouldDeleteFile(const QString& filePath, int daysThreshold) {
    QFileInfo fileInfo(filePath);
    
    // 获取最后访问时间
    QDateTime lastAccessTime = getFileLastAccessTime(filePath);
    
    if (!lastAccessTime.isValid()) {
        // 如果无法获取访问时间，使用修改时间
        lastAccessTime = fileInfo.lastModified();
    }
    
    if (!lastAccessTime.isValid()) {
        return false;
    }
    
    // 计算距离现在的天数
    int daysSinceLastAccess = lastAccessTime.daysTo(QDateTime::currentDateTime());
    
    return daysSinceLastAccess >= daysThreshold;
}

void FileCleaner::recursiveClean(const QString& folderPath, int daysThreshold, int& filesDeleted, qint64& spaceFreed) {
    QDir dir(folderPath);
    
    // 列出所有文件和文件夹
    QFileInfoList entries = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    
    for (const QFileInfo& entry : entries) {
        if (entry.isDir()) {
            // 递归处理子目录
            recursiveClean(entry.filePath(), daysThreshold, filesDeleted, spaceFreed);
        } else {
            // 检查是否应该删除该文件
            if (shouldDeleteFile(entry.filePath(), daysThreshold)) {
                spaceFreed += entry.size();
                if (QFile::remove(entry.filePath())) {
                    filesDeleted++;
                    qDebug() << "Deleted:" << entry.filePath();
                } else {
                    qWarning() << "Failed to delete:" << entry.filePath();
                }
            }
        }
    }
}
