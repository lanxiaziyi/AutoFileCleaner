#include "autostartutiility_win.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#include <QDebug>

AutoStartUtility::AutoStartUtility() {
}

bool AutoStartUtility::setAutoStart(const QString& appName, const QString& appPath, bool enable) {
    if (enable) {
        return setRegistryValue(appName, appPath);
    } else {
        return removeRegistryValue(appName);
    }
}

bool AutoStartUtility::isAutoStartEnabled(const QString& appName) {
    return checkRegistryValue(appName);
}

bool AutoStartUtility::disableAutoStart(const QString& appName) {
    return removeRegistryValue(appName);
}

#ifdef Q_OS_WIN
bool AutoStartUtility::setRegistryValue(const QString& appName, const QString& appPath) {
    HKEY hKey;
    const wchar_t* subKey = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";
    
    // 打开注册表键
    LONG result = RegOpenKeyExW(HKEY_CURRENT_USER, subKey, 0, KEY_WRITE, &hKey);
    if (result != ERROR_SUCCESS) {
        qWarning() << "Failed to open registry key";
        return false;
    }
    
    // 设置值
    const wchar_t* valueName = (const wchar_t*)appName.utf16();
    const wchar_t* valueData = (const wchar_t*)appPath.utf16();
    
    result = RegSetValueExW(hKey, valueName, 0, REG_SZ, 
                             (const BYTE*)valueData, 
                             (DWORD)(wcslen(valueData) + 1) * sizeof(wchar_t));
    
    RegCloseKey(hKey);
    
    if (result != ERROR_SUCCESS) {
        qWarning() << "Failed to set registry value";
        return false;
    }
    
    qDebug() << "Auto-start enabled for" << appName;
    return true;
}

bool AutoStartUtility::removeRegistryValue(const QString& appName) {
    HKEY hKey;
    const wchar_t* subKey = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";
    
    // 打开注册表键
    LONG result = RegOpenKeyExW(HKEY_CURRENT_USER, subKey, 0, KEY_WRITE, &hKey);
    if (result != ERROR_SUCCESS) {
        qWarning() << "Failed to open registry key";
        return false;
    }
    
    // 删除值
    const wchar_t* valueName = (const wchar_t*)appName.utf16();
    result = RegDeleteValueW(hKey, valueName);
    
    RegCloseKey(hKey);
    
    if (result != ERROR_SUCCESS && result != ERROR_FILE_NOT_FOUND) {
        qWarning() << "Failed to delete registry value";
        return false;
    }
    
    qDebug() << "Auto-start disabled for" << appName;
    return true;
}

bool AutoStartUtility::checkRegistryValue(const QString& appName) {
    HKEY hKey;
    const wchar_t* subKey = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";
    
    // 打开注册表键
    LONG result = RegOpenKeyExW(HKEY_CURRENT_USER, subKey, 0, KEY_READ, &hKey);
    if (result != ERROR_SUCCESS) {
        return false;
    }
    
    // 查询值
    const wchar_t* valueName = (const wchar_t*)appName.utf16();
    DWORD size = 0;
    result = RegQueryValueExW(hKey, valueName, NULL, NULL, NULL, &size);
    
    RegCloseKey(hKey);
    
    return result == ERROR_SUCCESS && size > 0;
}
#else
bool AutoStartUtility::setRegistryValue(const QString& appName, const QString& appPath) {
    qWarning() << "Auto-start is only supported on Windows";
    return false;
}

bool AutoStartUtility::removeRegistryValue(const QString& appName) {
    qWarning() << "Auto-start is only supported on Windows";
    return false;
}

bool AutoStartUtility::checkRegistryValue(const QString& appName) {
    return false;
}
#endif
