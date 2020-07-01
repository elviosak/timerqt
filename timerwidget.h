#ifndef TIMERWIDGET_H
#define TIMERWIDGET_H

#include <QDebug>

#include <QComboBox>
#include <QCheckBox>
#include <QDate>
#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QProcess>
#include <QPushButton>
#include <QSpinBox>
#include <QSettings>
#include <QTimer>

#include <KIdleTime>

class TimerWidget : public QFrame
{
    Q_OBJECT
public:
    explicit TimerWidget(QString id, QString name, QString cmd, int hour, int min, int sec, bool idle, bool repeat, bool enabled);
    ~TimerWidget();
    QString timerId;
    QLineEdit * nameEdit;
    QLineEdit * cmdEdit;
    QSpinBox * hourSpin;
    QSpinBox * minSpin;
    QSpinBox * secSpin;
    QCheckBox * idleCheck;
    QCheckBox * repeatCheck;
    QCheckBox * enabledCheck;
    QPushButton * runBtn;
    QPushButton * startBtn;
    QPushButton * delBtn;
    QComboBox * logCombo;
    QMetaObject::Connection timerConn;
    int totalTime;
    bool shouldRunIdle = true;
    int currentTime;
    int idle = 0;
    int oldIdle;
    QTimer * timer;
    void toggleEnabled(bool checked);
    void startStop(bool checked);
    void save();
    void run();
    void start();
    void stop();
    void onTimeout();
    void remove();
    void showLog(int index);
    QStringList parseCmd(QString cmd);
    QSettings * settings = new QSettings("TimerQt", "timerqt");
    void updateSettings(QString key);
    QProcess * proc;
    void procFinished(int exitCode, QProcess::ExitStatus exitStatus);
    QMap<QString, QString> logger;
signals:
    void removed(QString timerId);
};

#endif // TIMERWIDGET_H
