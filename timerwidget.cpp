#include "timerwidget.h"

TimerWidget::TimerWidget(QString id, QString name, QString cmd, int min, int sec, bool idle, bool repeat, bool enabled) : QWidget()
{
    settings = new QSettings("timerqt", "timers");
    timer = new QTimer();
    timer->setInterval(1000);

    timerId = id;
    auto vbox = new QVBoxLayout(this);
    auto row1 = new QHBoxLayout();
    auto row2 = new QHBoxLayout();
    vbox->addLayout(row1);
    vbox->addLayout(row2);
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

    row1->addWidget(nameEdit, 1);
    row1->addWidget(cmdEdit, 4);
    row1->addWidget(runBtn, 1);

    row2->addWidget(delBtn, 1);
    row2->addWidget(minSpin, 1);
    row2->addWidget(secSpin, 1);
    row2->addWidget(idleCheck,1);
    row2->addWidget(repeatCheck, 1);
    row2->addWidget(enabledCheck, 1);

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
