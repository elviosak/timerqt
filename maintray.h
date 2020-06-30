#ifndef MAINTRAY_H
#define MAINTRAY_H

#include <QAction>
#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QFormLayout>
#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QIcon>
#include <QMainWindow>
#include <QMap>
#include <QMenu>
#include <QPixmap>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QSettings>
#include <QSpinBox>
#include <QSystemTrayIcon>
#include <QUuid>
#include <QVBoxLayout>

#include "timerwidget.h"
class MainTray : public QSystemTrayIcon
{
    Q_OBJECT

public:
    MainTray(QWidget *parent = nullptr);
    ~MainTray();
    void addTimer();
    void addFromSettings(QString id, QString name, QString cmd, int hour, int min, int sec, bool idle, bool repeat, bool enabled);
    void saveToSettings(QString id, QString name, QString cmd, int hour, int min, int sec, bool idle, bool repeat, bool enabled);
    void removeTimer(QString timerId);
    void createWindow();
    void handleTray(QSystemTrayIcon::ActivationReason reason);
    void updateMenu();
    void loadSettings();
    void updateTrayIcon(QString trayIcon);
    void updateWinIcon(QString winIcon);
    void fillIconCombo(QComboBox * combo);
private:
    QMap<QString, TimerWidget*> mTimers;
    QMap<QString, QIcon> icons;
    QMainWindow * win;
    QMenu * menu = new QMenu();
    QSettings * settings = new QSettings("TimerQt", "timerqt");
    QComboBox * winIconCombo;
    QComboBox * trayIconCombo;
    QVBoxLayout * vbox;
    QVBoxLayout * timers;
    QGridLayout * adder;
    QLineEdit * addName;
    QLineEdit * addCmd;
    QSpinBox * addHour;
    QSpinBox * addMin;
    QSpinBox * addSec;
    QCheckBox * addIdle;
    QCheckBox * addRepeat;
    QCheckBox * addEnabled;
    QScrollArea * scroll;
};

#endif // MAINTRAY_H
