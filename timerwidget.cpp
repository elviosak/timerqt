#include "timerwidget.h"

TimerWidget::TimerWidget(QString id, QString name, QString cmd, int min, int sec, bool idle, bool repeat, bool enabled) : QWidget()
{
    settings = new QSettings("timerqt", "timers");
    timer = new QTimer();
    timer->setInterval(1000);

    timerId = id;
    nameEdit = new QLineEdit(name);
    cmdEdit = new QLineEdit(cmd);
    minSpin = new QSpinBox();
    minSpin->setRange(0, 24*60);
    minSpin->setSuffix(" m");
    minSpin->setValue(min);
    secSpin = new QSpinBox();
    secSpin->setRange(0, 59);
    secSpin->setSuffix(" s");
    secSpin->setValue(sec);

    idleCheck = new QCheckBox("Idle");
    idleCheck->setChecked(idle);
    repeatCheck = new QCheckBox("Repeat");
    repeatCheck->setChecked(repeat);
    enabledCheck = new QCheckBox("Enabled");
    enabledCheck->setChecked(enabled);
    startBtn = new QPushButton("Start");
    runBtn = new QPushButton(QIcon(":/run"), "Run (0)");
    delBtn = new QPushButton(QIcon(":/delete"), "Delete");

    auto grid = new QGridLayout();
    grid->addWidget(nameEdit, 0, 0, 1, 1);
    grid->addWidget(cmdEdit, 0, 1, 1, 4);
    grid->addWidget(runBtn, 0, 5, 1, 1);

    grid->addWidget(delBtn, 1, 0, 1, 1);
    grid->addWidget(minSpin, 1, 1, 1, 1);
    grid->addWidget(secSpin, 1, 2, 1, 1);
    grid->addWidget(idleCheck, 1, 3, 1, 1);
    grid->addWidget(repeatCheck, 1, 4, 1, 1);
    grid->addWidget(enabledCheck, 1, 5, 1, 1);

    for (int i = 0; i < 6 ; ++i ) {
        grid->setColumnStretch(i, 1);
    }
    setLayout(grid);

    if(enabled)
        start();

    connect(nameEdit, &QLineEdit::textChanged, this, [=](QString name){
        settings->setValue(timerId + "/name", name);
    });
    connect(cmdEdit, &QLineEdit::textChanged, this, [=](QString cmd){
        settings->setValue(timerId + "/cmd", cmd);
    });
    connect(minSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int min){
        if (min == 0 && secSpin->value() == 0){
            minSpin->setValue(1);
        }
        else {
            settings->setValue(timerId + "/min", min);
        }
    });
    connect(secSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int sec){
        if (sec == 0 && minSpin->value() == 0 ){
            secSpin->setValue(1);
        }
        else {
            settings->setValue(timerId + "/sec", sec);
        }
    });
    connect(idleCheck, &QCheckBox::toggled, this, [=](bool idle){
        settings->setValue(timerId + "/idle", idle);
    });
    connect(repeatCheck, &QCheckBox::toggled, this, [=](bool repeat){
        settings->setValue(timerId + "/repeat", repeat);
    });
    connect(enabledCheck, &QCheckBox::toggled, this, &TimerWidget::toggleEnabled);
    connect(runBtn, &QPushButton::clicked, this, &TimerWidget::run);
    connect(delBtn, &QPushButton::clicked, this, &TimerWidget::remove);
}


void TimerWidget::run(){
    auto text = cmdEdit->text();
    if (text.trimmed().length()>0){
        QProcess::startDetached(text);
    }
//  FOR QT 5.15+
//    auto cmdList = QProcess::splitCommand(text);
//    if(!cmdList.isEmpty()){
//        QProcess::startDetached(cmdList.takeFirst(), cmdList);
//     }
    if (!repeatCheck->isChecked())
        enabledCheck->setChecked(false);

    startStop(repeatCheck->isChecked());
}
void TimerWidget::toggleEnabled(bool checked){
    settings->setValue(timerId + "/enabled", checked);
    startStop(checked);
}
void TimerWidget::startStop(bool checked){
    disconnect(timerConn);
    if (checked){
        start();
    }else{
        stop();
    }
}
void TimerWidget::start(){
    currentTime = minSpin->value() *  60 + secSpin->value();
    runBtn->setText("Run ("+ QString::number(currentTime) + ")");
    timerConn = connect(timer, &QTimer::timeout, this, &TimerWidget::onTimeout);
    timer->start();
}
void TimerWidget::stop(){
    timer->stop();
    runBtn->setText("Run (0)");
    //;
}
void TimerWidget::onTimeout(){
    if(idleCheck->isChecked()){
        int idleTime = KIdleTime::instance()->idleTime() / 1000;
        currentTime = minSpin->value() *  60 + secSpin->value() - idleTime;
        if(currentTime <= 0 ){
            idleReset = false;
            run();
        }
        else {
            idleReset = true;
        }
        runBtn->setText("Run ("+ QString::number(currentTime) + ")");
    }
    else{
        currentTime = currentTime - 1;
        runBtn->setText("Run ("+ QString::number(currentTime) + ")");
        if(currentTime <= 0 ){
            run();
        }
    }
}
void TimerWidget::remove(){
    //remove from settings
    emit removed(timerId);
    deleteLater();
}
TimerWidget::~TimerWidget(){
}
