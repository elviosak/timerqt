#ifndef MAINTRAY_H
#define MAINTRAY_H

#include <QAction>
#include <QCheckBox>
#include <QDebug>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QIcon>
#include <QMainWindow>
#include <QMap>
#include <QMenu>
#include <QPixmap>
#include <QPushButton>
#include <QSettings>
#include <QSpinBox>
#include <QSystemTrayIcon>
#include <QUuid>
#include <QVBoxLayout>
#include <QX11Info>

#include "timerwidget.h"
class MainTray : public QSystemTrayIcon
{
    Q_OBJECT

public:
    MainTray(QWidget *parent = nullptr);
    ~MainTray();
    void addTimer();
    void addFromSettings(QString id, QString name, QString cmd, int min, int sec, bool idle, bool repeat, bool enabled);
    void saveToSettings(QString id, QString name, QString cmd, int min, int sec, bool idle, bool repeat, bool enabled);
    void removeTimer(QString timerId);
    void createWindow();
    void handleTray(QSystemTrayIcon::ActivationReason reason);
    void updateMenu();
    void loadSettings();
    QSettings * settings;
private:
    QMap<QString, TimerWidget*> mTimers;
    QMainWindow * win;
    QMenu * menu;
    QVBoxLayout * vbox;
    QVBoxLayout * timers;
    QFormLayout * adder;
    QLineEdit * addName;
    QLineEdit * addCmd;
    QSpinBox * addMin;
    QSpinBox * addSec;
    QCheckBox * addIdle;
    QCheckBox * addRepeat;
    QCheckBox * addEnabled;
};

#endif // MAINTRAY_H
