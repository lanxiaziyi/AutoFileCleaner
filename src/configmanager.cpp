#include "configmanager.h"
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

ConfigManager::ConfigManager()
    : m_settings(nullptr), m_appName("AutoFileCleaner") {
    m_configDirectory = getConfigDirectory();
    ensureDirectoriesExist();
    
    // 初始化 QSettings
    QString configPath = m_configDirectory + "/config.ini";
    m_settings = new QSettings(configPath, QSettings::IniFormat);
}

bool ConfigManager::loadConfig() {
    m_settings->sync();
    qDebug() << "Config loaded from:" << m_settings->fileName();
    return true;
}

bool ConfigManager::saveConfig() {
    m_settings->sync();
    qDebug() << "Config saved to:" << m_settings->fileName();
    return true;
}

QString ConfigManager::getTasksFile() const {
    return m_configDirectory + "/tasks.json";
}

QString ConfigManager::getLogFile() const {
    return m_configDirectory + "/logs.txt";
}

bool ConfigManager::isAutoStartEnabled() const {
    return m_settings->value("App/AutoStart", true).toBool();
}

bool ConfigManager::isMinimizeToTray() const {
    return m_settings->value("App/MinimizeToTray", true).toBool();
}

QString ConfigManager::getAppDataPath() const {
    return m_configDirectory;
}

void ConfigManager::setAutoStartEnabled(bool enabled) {
    m_settings->setValue("App/AutoStart", enabled);
    m_settings->sync();
}

void ConfigManager::setMinimizeToTray(bool enabled) {
    m_settings->setValue("App/MinimizeToTray", enabled);
    m_settings->sync();
}

bool ConfigManager::ensureDirectoriesExist() {
    QDir dir(m_configDirectory);
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            qWarning() << "Failed to create config directory:" << m_configDirectory;
            return false;
        }
    }
    
    // 创建日志目录
    QString logDir = m_configDirectory + "/logs";
    QDir logDirObj(logDir);
    if (!logDirObj.exists()) {
        logDirObj.mkpath(".");
    }
    
    return true;
}

QString ConfigManager::getConfigDirectory() {
    // 在 Windows 上，配置文件通常保存在 AppData\Local 目录
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    
    if (dataPath.isEmpty()) {
        dataPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/" + m_appName;
    }
    
    return dataPath;
}
