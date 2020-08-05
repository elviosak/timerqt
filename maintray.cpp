#include "maintray.h"


MainTray::MainTray(QWidget *parent)
    : QSystemTrayIcon(parent)
{
    icons["Regular"] = QPixmap(":/timerqt");
    icons["Dark Theme"] = QPixmap(":/timerqt-dark");
    icons["Light Theme"] = QPixmap(":/timerqt-light");
    updateMenu();
    setContextMenu(menu);
    connect(menu, &QMenu::aboutToShow, this, &MainTray::updateMenu);
    connect(this, &QSystemTrayIcon::activated, this, &MainTray::handleTray);
    createWindow();
    loadSettings();
    connect(winIconCombo, &QComboBox::currentTextChanged, this, &MainTray::updateWinIcon);
    connect(trayIconCombo, &QComboBox::currentTextChanged, this, &MainTray::updateTrayIcon);
}

void MainTray::createWindow(){
    win = new QMainWindow();
    win->setAttribute(Qt::WA_QuitOnClose, false);
    win->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    win->setWindowIcon(icons["Regular"]);
    win->setWindowTitle("Timer Qt");
    auto central = new QWidget();
    vbox = new QVBoxLayout();
    central->setLayout(vbox);
    auto frame = new QFrame();
    frame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    frame->setLineWidth(4);

    //icons config
    QGridLayout * configGrid = new QGridLayout();
    vbox->addLayout(configGrid);
    winIconCombo = new QComboBox();
    fillIconCombo(winIconCombo);
    configGrid->addWidget(new QLabel("App Icon:"), 0, 0, 1, 1);
    configGrid->addWidget(winIconCombo, 0, 1, 1, 1);

    trayIconCombo = new QComboBox();
    fillIconCombo(trayIconCombo);
    configGrid->addWidget(new QLabel("Tray Icon:"), 0, 2, 1, 1);
    configGrid->addWidget(trayIconCombo, 0, 3, 1, 1);

    adder = new QGridLayout();



    addName = new QLineEdit();
    addCmd = new QLineEdit();
    addHour = new QSpinBox();
    addHour->setRange(0, 24);
    addHour->setSuffix(" hours");
    addHour->setValue(0);
    addMin = new QSpinBox();
    addMin->setRange(0, 59);
    addMin->setSuffix(" minutes");
    addMin->setValue(0);
    addSec = new QSpinBox();
    addSec->setRange(0, 59);
    addSec->setSuffix(" seconds");
    addSec->setValue(10);

    connect(addHour, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int hour){
        if (hour == 0 && addMin->value() == 0  && addSec->value() == 0)
            addHour->setValue(1);
    });
    connect(addMin, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int min){
        if (min == 0 && addHour->value() == 0  &&addSec->value() == 0)
            addMin->setValue(1);
    });
    connect(addSec, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int sec){
        if (sec == 0 && addHour->value() == 0  &&addMin->value() == 0 )
            addSec->setValue(1);
    });


    addIdle = new QCheckBox("Idle");
    addIdle->setToolTip("System Idle time");
    addRepeat = new QCheckBox("Repeat");
    addRepeat->setToolTip("Repeat after first run");
    addEnabled = new QCheckBox("Enabled");
    addEnabled->setToolTip("Start timer after adding");
    auto addBtn = new QPushButton("Add");
    connect(addBtn, &QPushButton::clicked,this, &MainTray::addTimer);
    adder->addWidget(new QLabel("Name:"), 1, 0, 1, 1);
    adder->addWidget(addName, 1, 1, 1, 3);
    adder->addWidget(new QLabel("Command:"), 2, 0, 1, 1);
    adder->addWidget(addCmd, 2, 1, 1, 3);
    adder->addWidget(new QLabel("Timer:"), 3, 0, 1, 1);
    adder->addWidget(addHour, 3, 1, 1, 1);
    adder->addWidget(addMin, 3, 2, 1, 1);
    adder->addWidget(addSec, 3, 3, 1, 1);

    adder->addWidget(new QLabel("Options:"), 4, 0, 1, 1);
    adder->addWidget(addIdle, 4, 1, 1, 1);
    adder->addWidget(addRepeat, 4, 2, 1, 1);
    adder->addWidget(addEnabled, 4, 3, 1, 1);
    adder->addWidget(addBtn, 5, 3, 1, 1);
    frame->setLayout(adder);
    vbox->addWidget(frame);

    timers = new QVBoxLayout();
    scroll = new QScrollArea();
    auto timersFrame = new QWidget();

    timersFrame->setLayout(timers);
    scroll->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
    scroll->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    scroll->setWidgetResizable(true);
    scroll->setWidget(timersFrame);
    vbox->addWidget(scroll);
    win->setCentralWidget(central);
    win->setMinimumWidth(700);
}
void MainTray::fillIconCombo(QComboBox *combo){
    combo->addItem(icons["Regular"], "Regular", "Regular");
    combo->addItem(icons["Dark Theme"], "Dark Theme", "Dark Theme");
    combo->addItem(icons["Light Theme"], "Light Theme", "Light Theme");
}
void MainTray::updateTrayIcon(QString trayIcon){
    settings->setValue("trayIcon", trayIcon);
    setIcon(icons[trayIcon]);
}
void MainTray::updateWinIcon(QString winIcon){
    settings->setValue("winIcon", winIcon);
    win->setWindowIcon(icons[winIcon]);
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
    a = menu->addAction("Show");
    connect(a, &QAction::triggered, this, [=] { win->show(); win->activateWindow(); });
    a = menu->addAction("Hide");
    connect(a, &QAction::triggered, this, [=] { win->hide(); });
    a = menu->addAction(QIcon::fromTheme("exit"), "Quit");
    connect(a, &QAction::triggered, this, [=] { exit(0); });
}

void MainTray::loadSettings(){
    QString trayIcon = settings->value("trayIcon", "Regular").toString();
    setIcon(icons[trayIcon]);
    trayIconCombo->setCurrentText(trayIcon);
    QString winIcon = settings->value("winIcon", "Regular").toString();
    win->setWindowIcon(icons[winIcon]);
    winIconCombo->setCurrentText(winIcon);
    auto groups = settings->childGroups();
    foreach(QString id, groups){
        settings->beginGroup(id);
        auto name = settings->value("name").toString();
        auto cmd = settings->value("cmd").toString();
        auto hour = settings->value("hour").toInt();
        auto min = settings->value("min").toInt();
        auto sec = settings->value("sec").toInt();
        auto idle = settings->value("idle").toBool();
        auto repeat = settings->value("repeat").toBool();
        auto enabled = settings->value("enabled").toBool();
        addFromSettings(id, name, cmd, hour, min, sec, idle, repeat, enabled);
        settings->endGroup();
    }

}
void MainTray::addFromSettings(QString id, QString name, QString cmd, int hour, int min, int sec, bool idle, bool repeat, bool enabled){
    auto timer = new TimerWidget(id, name, cmd, hour, min, sec, idle, repeat, enabled);
    mTimers[id] = timer;
    connect(timer, &TimerWidget::removed, this, &MainTray::removeTimer);
    timers->addWidget(timer);
}
void MainTray::saveToSettings(QString id, QString name, QString cmd, int hour, int min, int sec, bool idle, bool repeat, bool enabled){
    settings->beginGroup(id);
    settings->setValue("name", name);
    settings->setValue("cmd", cmd);
    settings->setValue("hour", hour);
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
    auto hour = addHour->value();
    auto min = addMin->value();
    auto sec = addSec->value();
    auto idle = addIdle->isChecked();
    auto repeat = addRepeat->isChecked();
    auto enabled = addEnabled->isChecked();

    auto timer = new TimerWidget(id, name, cmd, hour, min, sec, idle, repeat, enabled);
    saveToSettings(id, name, cmd, hour, min, sec, idle, repeat, enabled);
    mTimers[id] = timer;
    connect(timer, &TimerWidget::removed, this, &MainTray::removeTimer);
    timers->insertWidget(0, timer);
    //scroll->ensureWidgetVisible(timer);
    //QScrollBar * bar = scroll->verticalScrollBar();
    //bar->setValue(bar->maximum());

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

