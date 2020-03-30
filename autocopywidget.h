#ifndef AUTOCOPYWIDGET_H
#define AUTOCOPYWIDGET_H

#include <QWidget>
#include <QFileDialog>
#include <QDebug>
#include <QDate>
#include <QTimer>

namespace Ui {
class autoCopyWidget;
}

class autoCopyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit autoCopyWidget(QWidget *parent = nullptr);
    ~autoCopyWidget();
    QString fileName;
    QString targetFold;
    QString lastCopyFile;
    int copyInterval;
    int maxInterval;
    int minInterval;
    QTimer* progress_timer;
    QTimer* timer;
    qint64 start_time;
    bool isSyncing;

    qint64 getTimestamp();
    bool copyFileToPath(QString sourceFile ,QString toDir, bool coverFileIfExist);
    void startTimer();
    void stopTimer();

private slots:
    void on_file_pushButton_clicked();

    void on_fold_pushButton_clicked();

    bool on_sync_pushButton_clicked();

    void on_spinBox_valueChanged(int arg1);

    void on_stopSync_pushButton_clicked();

private:
    Ui::autoCopyWidget *ui;
};

#endif // AUTOCOPYWIDGET_H
