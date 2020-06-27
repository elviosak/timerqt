#ifndef TIMERWIDGET_H
#define TIMERWIDGET_H

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QTimer>
#include <QWidget>
#include <QCheckBox>
#include <QProcess>
#include <QSettings>

#include <KIdleTime>

class TimerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TimerWidget(QString id, QString name, QString cmd, int min, int sec, bool idle, bool repeat, bool enabled);
    ~TimerWidget();
    QString timerId;
    QLineEdit * nameEdit;
    QLineEdit * cmdEdit;
    QSpinBox * minSpin;
    QSpinBox * secSpin;
    QCheckBox * idleCheck;
    QCheckBox * repeatCheck;
    QCheckBox * enabledCheck;
    QPushButton * runBtn;
    QPushButton * startBtn;
    QPushButton * delBtn;
    QMetaObject::Connection timerConn;
    bool idleReset = true;
    int currentTime;
    QTimer * timer;
    void toggleEnabled(bool checked);
    void startStop(bool checked);
    void save();
    void run();
    void start();
    void stop();
    void onTimeout();
    void remove();
    QStringList parseCmd(QString cmd);
    QSettings * settings;
    void updateSettings(QString key);
signals:
    void removed(QString timerId);
};

#endif // TIMERWIDGET_H
