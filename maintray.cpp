#include "maintray.h"


MainTray::MainTray(QWidget *parent)
    : QSystemTrayIcon(QPixmap(":/timerqt"), parent)
{

    settings = new QSettings("timerqt", "timers");
    menu = new QMenu();
    updateMenu();
    setContextMenu(menu);
    connect(menu, &QMenu::aboutToShow, this, &MainTray::updateMenu);
    connect(this, &QSystemTrayIcon::activated, this, &MainTray::handleTray);
    installEventFilter(this);
    createWindow();

    loadSettings();
}

void MainTray::createWindow(){
    win = new QMainWindow();
    win->setAttribute(Qt::WA_QuitOnClose, false);
    win->setWindowIcon(QIcon(":/timerqt"));
    win->setWindowTitle("Timer Qt");
    auto central = new QWidget();
    vbox = new QVBoxLayout();
    central->setLayout(vbox);
    adder = new QFormLayout();
    addName = new QLineEdit();
    adder->addRow("Name: ", addName);
    addCmd = new QLineEdit();
    adder->addRow("Command: ", addCmd);
    addMin = new QSpinBox();
    addMin->setRange(0, 24*60);
    addMin->setSuffix(" m");
    addMin->setValue(1);
    addSec = new QSpinBox();
    addSec->setRange(0, 59);
    addSec->setSuffix(" s");
    adder->addRow("Minutes (0 - " + QString::number(24*60) + "): ", addMin);
    adder->addRow("Seconds (0 - 59): ", addSec);
    connect(addMin, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int min){
        if (min == 0 && addSec->value() == 0)
            addMin->setValue(1);
    });
    connect(addSec, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int sec){
        if (sec == 0 && addMin->value() == 0 )
            addSec->setValue(1);
    });
    addIdle = new QCheckBox("Idle");
    addIdle->setToolTip("System Idle time");
    addRepeat = new QCheckBox("Repeat");
    addRepeat->setToolTip("Repeat after first run");
    addEnabled = new QCheckBox("Enabled");
    addEnabled->setToolTip("Start timer after adding");
    QHBoxLayout * addOptions = new QHBoxLayout();
    addOptions->addWidget(addIdle);
    addOptions->addWidget(addRepeat);
    addOptions->addWidget(addEnabled);
    adder->addRow("Options: ", addOptions);
    auto addBtn = new QPushButton("Add");
    connect(addBtn, &QPushButton::clicked,this, &MainTray::addTimer);
    adder->addWidget(addBtn);
    vbox->addLayout(adder);
    timers = new QVBoxLayout();
    vbox->addLayout(timers);
    win->setCentralWidget(central);
    win->setMinimumWidth(600);
}
void MainTray::handleTray(QSystemTrayIcon::ActivationReason reason){
    if (reason == QSystemTrayIcon::Trigger){
        if(win->isMinimized())
            win->showNormal();
        else {
            win->setVisible(!win->isVisible());
        }
    }
}
void MainTray::updateMenu(){
    menu->clear();
    QAction * a;
    a = menu->addAction("Timers:");
    connect(a, &QAction::triggered, this, [=] { handleTray(QSystemTrayIcon::ActivationReason::Trigger); });
    menu->addSeparator();
    for (auto i = mTimers.begin(); i != mTimers.end(); ++i) {
        auto timer = i.value();
        QString name = timer->nameEdit->text();
        QString actionName = name + " - " + QString::number(timer->currentTime);
        a = menu->addAction(actionName);
        connect(a, &QAction::triggered, this, [=] { handleTray(QSystemTrayIcon::ActivationReason::Trigger); });
    }
    menu->addSeparator();
    connect(a, &QAction::triggered, this, [=] { qDebug() << "action clicked"; });
    a = menu->addAction(QIcon::fromTheme("exit"), "Quit");
    connect(a, &QAction::triggered, this, [=] { exit(0); });
}

void MainTray::loadSettings(){
    auto groups = settings->childGroups();
    foreach(QString id, groups){
        settings->beginGroup(id);
        auto name = settings->value("name").toString();
        auto cmd = settings->value("cmd").toString();
        auto min = settings->value("min").toInt();
        auto sec = settings->value("sec").toInt();
        auto idle = settings->value("idle").toBool();
        auto repeat = settings->value("repeat").toBool();
        auto enabled = settings->value("enabled").toBool();
        addFromSettings(id, name, cmd, min, sec, idle, repeat, enabled);
        settings->endGroup();
    }

}
void MainTray::addFromSettings(QString id, QString name, QString cmd, int min, int sec, bool idle, bool repeat, bool enabled){
    auto timer = new TimerWidget(id, name, cmd, min, sec, idle, repeat, enabled);
    mTimers[id] = timer;
    connect(timer, &TimerWidget::removed, this, &MainTray::removeTimer);
    timers->addWidget(timer);
}
void MainTray::saveToSettings(QString id, QString name, QString cmd, int min, int sec, bool idle, bool repeat, bool enabled){
    settings->beginGroup(id);
    settings->setValue("name", name);
    settings->setValue("cmd", cmd);
    settings->setValue("min", min);
    settings->setValue("sec", sec);
    settings->setValue("idle", idle);
    settings->setValue("repeat", repeat);
    settings->setValue("enabled", enabled);
    settings->endGroup();
}

void MainTray::addTimer(){
    auto id = QUuid::createUuid().toString();
    auto name = addName->text();
    auto cmd = addCmd->text();
    auto min = addMin->value();
    auto sec = addSec->value();
    auto idle = addIdle->isChecked();
    auto repeat = addRepeat->isChecked();
    auto enabled = addEnabled->isChecked();

    auto timer = new TimerWidget(id, name, cmd, min, sec, idle, repeat, enabled);
    saveToSettings(id, name, cmd, min, sec, idle, repeat, enabled);
    mTimers[id] = timer;
    connect(timer, &TimerWidget::removed, this, &MainTray::removeTimer);
    timers->addWidget(timer);

    addName->setText("");
    addCmd->setText("");
    addName->setFocus();
}
void MainTray::removeTimer(QString timerId){
    mTimers.remove(timerId);
    settings->remove(timerId);
    win->updateGeometry();
    qDebug() << "removed signal";
}

MainTray::~MainTray()
{
}

