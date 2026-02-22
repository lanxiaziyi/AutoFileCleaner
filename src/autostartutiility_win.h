#ifndef AUTOSTARTUTIILITY_WIN_H
#define AUTOSTARTUTIILITY_WIN_H

#include <QString>

class AutoStartUtility {
public:
    AutoStartUtility();

    // 设置程序自启动
    static bool setAutoStart(const QString& appName, const QString& appPath, bool enable = true);
    
    // 检查是否已启用自启动
    static bool isAutoStartEnabled(const QString& appName);
    
    // 禁用自启动
    static bool disableAutoStart(const QString& appName);
    
private:
    // Windows 注册表操作
    static bool setRegistryValue(const QString& appName, const QString& appPath);
    static bool removeRegistryValue(const QString& appName);
    static bool checkRegistryValue(const QString& appName);
};

#endif // AUTOSTARTUTIILITY_WIN_H
