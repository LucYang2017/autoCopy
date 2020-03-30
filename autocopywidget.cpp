#include "autocopywidget.h"
#include "ui_autocopywidget.h"


autoCopyWidget::autoCopyWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::autoCopyWidget)
{
    ui->setupUi(this);

    isSyncing = false;

    copyInterval = 60; // 默认拷贝时间间隔为60s
    maxInterval = 3600;
    minInterval = 1;
    ui->spinBox->setValue(copyInterval);
    ui->spinBox->setRange(minInterval,maxInterval);

    ui->progressBar->setRange(0, 1000 * copyInterval);

    ui->plainTextEdit->appendPlainText("欢迎使用自动备份程序。\n");
    ui->plainTextEdit->appendPlainText("开始备份前请先选择待备份文件与目标文件夹，并设置备份时间间隔。\n");
    ui->plainTextEdit->appendPlainText("若选择的文件、文件夹不存在或不具有读写权限，将导致备份失败。\n");
    ui->plainTextEdit->appendPlainText("备份时间间隔最小为1 s，最大为3600 s。若文件大于100 MB，建议设置时间间隔大于30 s。\n");

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [=]{
        copyFileToPath(fileName,targetFold,1);
        start_time = getTimestamp();
    });

    progress_timer = new QTimer(this);
    progress_timer->setInterval(100);

    connect(progress_timer, &QTimer::timeout, this, [=]{
        if (timer->interval() == 0 || !timer->isActive()) // 没有进行同步
            return ;
        int prop = (getTimestamp() - start_time) * ui->progressBar->maximum() / timer->interval();
        ui->progressBar->setValue(prop);
    });
}

autoCopyWidget::~autoCopyWidget()
{
    delete ui;
}

qint64 autoCopyWidget::getTimestamp()
{
    return QDateTime::currentMSecsSinceEpoch();
}

void autoCopyWidget::startTimer()
{
    timer->start();
    progress_timer->start();
    start_time = getTimestamp();
}

void autoCopyWidget::stopTimer()
{
    timer->stop();
    progress_timer->stop();
    ui->progressBar->setValue(0);
}

void autoCopyWidget::on_file_pushButton_clicked()
{
    stopTimer();
    fileName = QFileDialog::getOpenFileName(this,
                                            tr("选择文件"),
                                            "C:/",
                                            tr("Word文档(*.doc *.docx);;演示文稿(*.ppt *.pptx);;Excel工作簿(*.xls *.xlsx);;文本文档(*.txt)"));
    ui->file_lineEdit->setText(fileName);
    if(isSyncing)
    {
        ui->plainTextEdit->appendPlainText("文件已改变，请点击按钮开始备份。\n");
    }
}

void autoCopyWidget::on_fold_pushButton_clicked()
{
    stopTimer();
    targetFold = QFileDialog::getExistingDirectory(this,tr("选择文件夹"),"C:/");
    ui->fold_lineEdit->setText(targetFold);
    if(isSyncing)
    {
        ui->plainTextEdit->appendPlainText("文件夹已改变，请点击按钮开始备份。\n");
    }
}

bool autoCopyWidget::copyFileToPath(QString sourceFile ,QString toDir, bool coverFileIfExist)
{
    toDir.replace("\\","/");

    if (!QFile::exists(sourceFile)){
        qDebug()<<"ERRNO. 1: File not exists.";
        ui->plainTextEdit->appendPlainText("待备份文件不存在！请重新选择。");
        return false;
    }

    QFileInfo dirinfo;
    dirinfo = QFileInfo(toDir);
    if (!dirinfo.isDir()){
        qDebug()<<"ERRNO. 2: Fold not exists.";
        ui->plainTextEdit->appendPlainText("目标文件夹不存在！请重新选择。");
        return false;
    }

    QDir *createfile = new QDir;
    bool exist = createfile->exists(toDir);
    if (exist){
        if(coverFileIfExist){
            createfile->remove(toDir);
        }
    }//end if

    QFileInfo fileInfo;
    fileInfo = QFileInfo(sourceFile);

    QDateTime currentTime =QDateTime::currentDateTime();
    QString current_date =currentTime.toString("/yyyyMMdd-hhmmss");

    toDir = toDir.append(current_date + " " + fileInfo.fileName());

    if(!QFile::copy(sourceFile, toDir))
    {
        qDebug() << "ERRNO. 2: File copy failed.";
        ui->plainTextEdit->appendPlainText("文件拷贝失败！\n");
        return false;
    }
    ui->plainTextEdit->appendPlainText("文件拷贝成功。\n" + toDir + "\n");
    ui->progressBar->setValue(ui->progressBar->maximum());
    return true;
}

void autoCopyWidget::on_spinBox_valueChanged(int arg1)
{
    stopTimer(); // 当时间间隔发生变化时，重置Timer

    copyInterval = arg1;

    ui->progressBar->setRange(0, 1000 * copyInterval);

    timer->setInterval(copyInterval * 1000);

    if(isSyncing) startTimer();
}

bool autoCopyWidget::on_sync_pushButton_clicked()
{


    if (!QFile::exists(fileName)){
        qDebug()<<"ERRNO. 1: File not exists.";
        ui->plainTextEdit->appendPlainText("待备份文件不存在！请重新选择。\n");
        return false;
    }

    targetFold.replace("\\","/");
    QFileInfo dirinfo;
    dirinfo = QFileInfo(targetFold);
    if (!dirinfo.isDir()){
        qDebug()<<"ERRNO. 2: Fold not exists.";
        ui->plainTextEdit->appendPlainText("目标文件夹不存在！请重新选择。\n");
        return false;
    }

    ui->plainTextEdit->appendPlainText("开始备份！\n");
    timer->setInterval(copyInterval * 1000);
    startTimer();
    isSyncing = true;
    return true;
}

void autoCopyWidget::on_stopSync_pushButton_clicked()
{
    qDebug() << isSyncing;
    if(isSyncing)
    {
        isSyncing = false;
        ui->progressBar->setValue(0);
        ui->plainTextEdit->appendPlainText("停止备份！\n");
        stopTimer();
    }
    else {
        ui->plainTextEdit->appendPlainText("请先点击\"开始备份\"按钮。\n");
        isSyncing = false;
    }
}
