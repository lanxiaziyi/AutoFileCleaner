#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QString>
#include <QSettings>

class ConfigManager {
public:
    ConfigManager();

    // 初始化配置
    bool loadConfig();
    bool saveConfig();

    // 获取和设置配置
    QString getTasksFile() const;
    QString getLogFile() const;
    bool isAutoStartEnabled() const;
    bool isMinimizeToTray() const;
    QString getAppDataPath() const;

    // 设置配置
    void setAutoStartEnabled(bool enabled);
    void setMinimizeToTray(bool enabled);

    // 确保必要的目录存在
    bool ensureDirectoriesExist();

private:
    QSettings* m_settings;
    QString m_configDirectory;
    QString m_appName;

    QString getConfigDirectory();
};

#endif // CONFIGMANAGER_H
