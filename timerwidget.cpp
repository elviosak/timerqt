#include "timerwidget.h"

TimerWidget::TimerWidget(QString id, QString name, QString cmd, int hour, int min, int sec, bool idle, bool repeat, bool enabled) : QFrame()
{
    settings = new QSettings("timerqt", "timers");
    setMinimumHeight(80);
    timer = new QTimer();
    timer->setInterval(1000);
    setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    setLineWidth(4);
    timerId = id;
    nameEdit = new QLineEdit(name);
    cmdEdit = new QLineEdit(cmd);
    hourSpin = new QSpinBox();
    hourSpin->setRange(0, 24);
    hourSpin->setSuffix(" h");
    hourSpin->setValue(hour);
    minSpin = new QSpinBox();
    minSpin->setRange(0, 59);
    minSpin->setSuffix(" m");
    minSpin->setValue(min);
    secSpin = new QSpinBox();
    secSpin->setRange(0, 59);
    secSpin->setSuffix(" s");
    secSpin->setValue(sec);

    totalTime = sec + min * 60 + hour * 60 * 60;
    idleCheck = new QCheckBox("Idle");
    idleCheck->setChecked(idle);
    repeatCheck = new QCheckBox("Repeat");
    repeatCheck->setChecked(repeat);
    enabledCheck = new QCheckBox("Enabled");
    enabledCheck->setChecked(enabled);
    startBtn = new QPushButton("Start");
    runBtn = new QPushButton(QIcon(":/run"), "Run (0)");
    delBtn = new QPushButton(QIcon(":/delete"), "Delete");
    logCombo = new QComboBox();
    logCombo->setToolTip("Show output log");

    auto grid = new QGridLayout();

    grid->addWidget(nameEdit, 0, 0, 1, 1);
    grid->addWidget(cmdEdit, 0, 1, 1, 5);
    grid->addWidget(runBtn, 0, 6, 1, 2);

    grid->addWidget(delBtn, 1, 0, 1, 1);
    auto box = new QHBoxLayout();
    box->addWidget(hourSpin);
    box->addWidget(minSpin);
    box->addWidget(secSpin);
    box->addWidget(idleCheck);
    box->addWidget(repeatCheck);
    box->addWidget(enabledCheck);
    grid->addLayout(box, 1, 1, 1, 5);
    grid->addWidget(logCombo, 1, 6, 1, 2);

    for (int i = 0; i < 8 ; ++i ) {
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
    connect(hourSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int hour){
        if (min == 0 && minSpin->value() == 0 && secSpin->value() == 0){
            minSpin->setValue(1);
        }
        totalTime = sec + min * 60 + hour * 60 * 60;
        settings->setValue(timerId + "/hour", hourSpin->value());
    });
    connect(minSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int min){
        if (min == 0 && hourSpin->value() == 0&& secSpin->value() == 0){
            minSpin->setValue(1);
        }
        totalTime = secSpin->value() + minSpin->value() * 60 + hourSpin->value() * 60 * 60;
        settings->setValue(timerId + "/min", minSpin->value());
    });
    connect(secSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int sec){
        if (sec == 0 && hourSpin->value() == 0 && minSpin->value() == 0 ){
            secSpin->setValue(1);
        }
        totalTime = sec + min * 60 + hour * 60 * 60;
        settings->setValue(timerId + "/sec", secSpin->value());
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
    connect(logCombo, QOverload<int>::of(&QComboBox::activated), this, &TimerWidget::showLog);
}

void TimerWidget::procFinished(int exitCode, QProcess::ExitStatus exitStatus){
    runBtn->setEnabled(true);
    QString timestamp = QDate::currentDate().toString("yyyy-MM-dd_");
    timestamp += QTime::currentTime().toString("hh-mm-ss");
    QString output = QString("Exit Code: %1, Exit Status %2\nLog:\n%3").arg(exitCode).arg(exitStatus).arg(QString(proc->readAllStandardOutput()));
    logger[timestamp] = output;
    logCombo->addItem(timestamp);

    qDebug() << timestamp<< output;
    proc->deleteLater();
    if (!repeatCheck->isChecked())
        enabledCheck->setChecked(false);

    startStop(repeatCheck->isChecked());
}

void TimerWidget::showLog(int index){
    QString timestamp = logCombo->itemText(index);
    if(!timestamp.isEmpty() && logger.contains(timestamp)){
        QMessageBox::information(this, QString("%1 Log: %2").arg(nameEdit->text()).arg(timestamp), logger[timestamp], QMessageBox::Ok);
    }
}
void TimerWidget::run(){
    timer->stop();
    auto text = cmdEdit->text();
    if (text.trimmed().length()>0){
        runBtn->setEnabled(false);
        proc = new QProcess();

        proc->start(text);
    //  FOR QT 5.15+
    //    auto cmdList = QProcess::splitCommand(text);
    //    proc->start(cmdList.takeFirst(), cmdList);
    //
        connect(proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &TimerWidget::procFinished);
    }


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
    currentTime = totalTime;
    runBtn->setText("Run ("+ QString::number(currentTime) + ")");
    timerConn = connect(timer, &QTimer::timeout, this, &TimerWidget::onTimeout);
    timer->start();
}
void TimerWidget::stop(){
    timer->stop();
    runBtn->setText("Run (0)");
}
void TimerWidget::onTimeout(){
    if(idleCheck->isChecked()){
        oldIdle = idle;
        idle = KIdleTime::instance()->idleTime() / 1000;
        if(idle > oldIdle){
            currentTime = totalTime - idle;
        }
        else {
            currentTime = totalTime;
            shouldRunIdle = true;
        }
        if(shouldRunIdle == false){
            currentTime = totalTime;
        }
        else if(currentTime <= 0){
            shouldRunIdle = false;
            run();
        }
        runBtn->setText("Run ("+ QString::number(currentTime) + ")");
    }
    else{
        currentTime = currentTime - 1;
        runBtn->setText("Run ("+ QString::number(currentTime) + ")");
        if(currentTime <= 0){
            run();
        }
    }

}
void TimerWidget::remove(){
    QMessageBox::StandardButton ret = QMessageBox::warning(this, "Confirm Deletion", QString("Do you want to delete timer: %1?").arg(nameEdit->text()), QMessageBox::No | QMessageBox::Yes);
    if (ret == QMessageBox::Yes){
        emit removed(timerId);
        deleteLater();
    }
}
TimerWidget::~TimerWidget(){
}
