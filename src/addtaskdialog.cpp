#include "addtaskdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QDir>

AddTaskDialog::AddTaskDialog(QWidget* parent)
    : QDialog(parent) {
    setWindowTitle("新建清理任务");
    setModal(true);
    setGeometry(0, 0, 600, 500);
    initUI();
}

AddTaskDialog::AddTaskDialog(const FileCleanTask& task, QWidget* parent)
    : QDialog(parent) {
    setWindowTitle("编辑清理任务");
    setModal(true);
    setGeometry(0, 0, 600, 500);
    initUI();
    loadTask(task);
}

void AddTaskDialog::initUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // 任务名称
    QHBoxLayout* nameLayout = new QHBoxLayout();
    nameLayout->addWidget(new QLabel("任务名称:"));
    m_nameEdit = new QLineEdit();
    m_nameEdit->setPlaceholderText("输入任务名称");
    nameLayout->addWidget(m_nameEdit);
    mainLayout->addLayout(nameLayout);
    
    // 描述
    QHBoxLayout* descLayout = new QHBoxLayout();
    descLayout->addWidget(new QLabel("任务描述:"));
    m_descriptionEdit = new QPlainTextEdit();
    m_descriptionEdit->setPlaceholderText("输入任务描述和备注");
    m_descriptionEdit->setMaximumHeight(80);
    descLayout->addWidget(m_descriptionEdit);
    mainLayout->addLayout(descLayout);
    
    // 目标文件夹
    QVBoxLayout* folderLayout = new QVBoxLayout();
    folderLayout->addWidget(new QLabel("目标清理文件夹:"));
    
    m_folderList = new QListWidget();
    folderLayout->addWidget(m_folderList);
    
    QHBoxLayout* folderBtnLayout = new QHBoxLayout();
    m_addFolderBtn = new QPushButton("添加文件夹");
    m_removeFolderBtn = new QPushButton("删除选中");
    folderBtnLayout->addWidget(m_addFolderBtn);
    folderBtnLayout->addWidget(m_removeFolderBtn);
    folderBtnLayout->addStretch();
    folderLayout->addLayout(folderBtnLayout);
    mainLayout->addLayout(folderLayout);
    
    // 清理参数
    QHBoxLayout* paramsLayout = new QHBoxLayout();
    
    paramsLayout->addWidget(new QLabel("清理阈值 (天):"));
    m_daysSpinBox = new QSpinBox();
    m_daysSpinBox->setMinimum(1);
    m_daysSpinBox->setMaximum(366);
    m_daysSpinBox->setValue(30);
    paramsLayout->addWidget(m_daysSpinBox);
    
    paramsLayout->addSpacing(20);
    paramsLayout->addWidget(new QLabel("执行时间:"));
    m_timeEdit = new QTimeEdit();
    m_timeEdit->setDisplayFormat("HH:mm");
    paramsLayout->addWidget(m_timeEdit);
    
    paramsLayout->addStretch();
    mainLayout->addLayout(paramsLayout);
    
    // 启用状态
    m_enableCheckBox = new QCheckBox("启用此任务");
    m_enableCheckBox->setChecked(true);
    mainLayout->addWidget(m_enableCheckBox);
    
    // 按钮
    QHBoxLayout* btnLayout = new QHBoxLayout();
    m_okBtn = new QPushButton("确定");
    m_cancelBtn = new QPushButton("取消");
    btnLayout->addStretch();
    btnLayout->addWidget(m_okBtn);
    btnLayout->addWidget(m_cancelBtn);
    mainLayout->addLayout(btnLayout);
    
    // 信号连接
    connect(m_addFolderBtn, &QPushButton::clicked, this, &AddTaskDialog::onAddFolder);
    connect(m_removeFolderBtn, &QPushButton::clicked, this, &AddTaskDialog::onRemoveFolder);
    connect(m_okBtn, &QPushButton::clicked, this, &AddTaskDialog::onAccept);
    connect(m_cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
}

FileCleanTask AddTaskDialog::getTask() const {
    FileCleanTask task;
    task.name = m_nameEdit->text();
    task.description = m_descriptionEdit->toPlainText();
    task.daysThreshold = m_daysSpinBox->value();
    task.scheduleTime = m_timeEdit->time().toString("HH:mm");
    task.enabled = m_enableCheckBox->isChecked();
    
    // 收集文件夹
    task.targetFolders.clear();
    for (int i = 0; i < m_folderList->count(); ++i) {
        task.targetFolders.append(m_folderList->item(i)->text());
    }
    
    return task;
}

void AddTaskDialog::onAddFolder() {
    QString folder = QFileDialog::getExistingDirectory(this, "选择文件夹");
    if (!folder.isEmpty()) {
        // 检查是否为根目录
        if (isRootDirectory(folder)) {
            QMessageBox::warning(this, "路径不合法", 
                "不允许添加根目录！\n\n"
                "出于安全考虑，请选择具体的子目录来清理。\n"
                "例如：C:\\Users\\YourName\\Downloads （而不是 C:\\）");
            return;
        }
        
        // 检查是否已存在
        bool exists = false;
        for (int i = 0; i < m_folderList->count(); ++i) {
            if (m_folderList->item(i)->text() == folder) {
                exists = true;
                break;
            }
        }
        
        if (exists) {
            QMessageBox::warning(this, "提示", "该文件夹已添加");
        } else {
            m_folderList->addItem(folder);
        }
    }
}

bool AddTaskDialog::isRootDirectory(const QString& path) {
    QDir dir(path);
    
    // 在 Windows 上检查 C:\, D:\ 等根目录
    // 在 Unix/Linux 上检查 / 根目录
#ifdef Q_OS_WIN
    // 获取驱动器的根目录
    QFileInfo fileInfo(path);
    QString absolutePath = fileInfo.absolutePath();
    QString fileName = fileInfo.fileName();
    
    // 如果路径的根目录就是它自己，说明是根目录
    // (例如 C:\ 的根是 C:\ 本身)
    return (dir.isRoot() || 
            fileName.isEmpty() || 
            (absolutePath == path && path.length() <= 3)); // C:\ 形式
#else
    // Unix/Linux 系统
    return path == "/" || dir.isRoot();
#endif
}

void AddTaskDialog::onRemoveFolder() {
    QListWidgetItem* item = m_folderList->currentItem();
    if (item) {
        delete m_folderList->takeItem(m_folderList->row(item));
    }
}

void AddTaskDialog::onBrowseFolder() {
    // 此方法可用于快速浏览
}

void AddTaskDialog::onAccept() {
    if (m_nameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "验证错误", "请输入任务名称");
        return;
    }
    
    if (m_folderList->count() == 0) {
        QMessageBox::warning(this, "验证错误", "请至少添加一个文件夹");
        return;
    }
    
    accept();
}

void AddTaskDialog::loadTask(const FileCleanTask& task) {
    m_nameEdit->setText(task.name);
    m_descriptionEdit->setPlainText(task.description);
    m_daysSpinBox->setValue(task.daysThreshold);
    m_timeEdit->setTime(QTime::fromString(task.scheduleTime, "HH:mm"));
    m_enableCheckBox->setChecked(task.enabled);
    
    m_folderList->clear();
    for (const QString& folder : task.targetFolders) {
        m_folderList->addItem(folder);
    }
}
