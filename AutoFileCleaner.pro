QT += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET = AutoFileCleaner
TEMPLATE = app

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/taskmanager.cpp \
    src/taskmodel.cpp \
    src/configmanager.cpp \
    src/filecleaner.cpp \
    src/schedulermanager.cpp \
    src/addtaskdialog.cpp \
    src/autostartutiility_win.cpp

HEADERS += \
    src/mainwindow.h \
    src/taskmanager.h \
    src/taskmodel.h \
    src/configmanager.h \
    src/filecleaner.h \
    src/schedulermanager.h \
    src/addtaskdialog.h \
    src/autostartutiility_win.h

RESOURCES += \
    resources/resources.qrc

# Windows 特定设置
win32 {
    RC_FILE = resources/app.rc
    # registry APIs (RegOpenKeyEx, RegCloseKey, etc.) are in Advapi32
    LIBS += -ladvapi32
}

# 生成路径
CONFIG(debug, debug|release) {
    DESTDIR = build/debug
} else {
    DESTDIR = build/release
}

OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.rcc
UI_DIR = $$DESTDIR/.ui
