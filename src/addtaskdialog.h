#ifndef ADDTASKDIALOG_H
#define ADDTASKDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QSpinBox>
#include <QTimeEdit>
#include <QListWidget>
#include <QPushButton>
#include <QCheckBox>
#include "taskmanager.h"

class AddTaskDialog : public QDialog {
    Q_OBJECT

public:
    explicit AddTaskDialog(QWidget* parent = nullptr);
    explicit AddTaskDialog(const FileCleanTask& task, QWidget* parent = nullptr);

    FileCleanTask getTask() const;

private slots:
    void onAddFolder();
    void onRemoveFolder();
    void onBrowseFolder();
    void onAccept();

private:
    void initUI();
    void loadTask(const FileCleanTask& task);
    
    QLineEdit* m_nameEdit;
    QPlainTextEdit* m_descriptionEdit;
    QListWidget* m_folderList;
    QSpinBox* m_daysSpinBox;
    QTimeEdit* m_timeEdit;
    QCheckBox* m_enableCheckBox;
    QPushButton* m_addFolderBtn;
    QPushButton* m_removeFolderBtn;
    QPushButton* m_okBtn;
    QPushButton* m_cancelBtn;
};

#endif // ADDTASKDIALOG_H
