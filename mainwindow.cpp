#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QAction>
#include <QSettings>
#include <QValidator>

namespace DEFAULT_VALUES {
    constexpr int MAX_WORK_REST_TIME = 18'000'000;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //load settings
    loadSettings();

    //additional ui setup
    ui->settingsRestTimeSpinBox->setRange(1, DEFAULT_VALUES::MAX_WORK_REST_TIME);
    ui->settingsRestTimeSpinBox->setSuffix(" ms");
    ui->settingsRestTimeSpinBox->setValue(timer_time_resting);
    ui->settingsWorkTimeSpinBox->setRange(1, DEFAULT_VALUES::MAX_WORK_REST_TIME);
    ui->settingsWorkTimeSpinBox->setSuffix(" ms");
    ui->settingsWorkTimeSpinBox->setValue(timer_time_working);

    //other settings

    //obj setups
    timer = new QTimer(this);
    timer->setInterval(TIMER_INTERVAL_DEFAULT);

    //connections
    connect(timer, &QTimer::timeout, this, &MainWindow::handleIntervalTimeout);
    connect(timer, &QTimer::timeout, this, &MainWindow::drawTimerLabel);

    connect(ui->startButton, &QPushButton::clicked, timer, [this]() {
        timer->start();
        elapsed_timer.restart();
    });
    connect(ui->stopButton, &QPushButton::clicked, timer, &QTimer::stop);

    connect(ui->actionSettings, &QAction::triggered, this, [this]() {
        ui->stackedWidget->setCurrentIndex(1);
    });
    connect(ui->actionHome_2, &QAction::triggered, this, [this]() {
        ui->stackedWidget->setCurrentIndex(0);
    });

    //obj starts
    drawTimerLabel();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::handleIntervalTimeout()
{
    if (elapsed_timer.elapsed() >= getCurrentTimerStatusTime()) {
        timer->stop();
        elapsed_timer.invalidate();
        timer_status = timer_status == Working ? Resting : Working;
    }
}

void MainWindow::drawTimerLabel()
{
    int elapsed = elapsed_timer.isValid() ? elapsed_timer.elapsed() : 0;
    int secs = (getCurrentTimerStatusTime() - elapsed) / 1000;
    int minutes = secs / 60;
    secs = secs % 60;

    ui->label->setText(QString("%1:%2").arg(minutes).arg(secs));
}

void MainWindow::applyNewSettings()
{
    timer_time_working = ui->settingsWorkTimeSpinBox->value();
    timer_time_resting = ui->settingsRestTimeSpinBox->value();

    saveSettings();
}

void MainWindow::loadSettings()
{
    QSettings settings;

    if (!settings.contains("working_time"))
        settings.setValue("working_time", TIMER_TIME_WORKING_DEFAULT);

    if (!settings.contains("resting_time"))
        settings.setValue("resting_time", TIMER_TIME_RESTING_DEFAULT);

    timer_time_working = settings.value("working_time").toInt();
    timer_time_resting = settings.value("resting_time").toInt();
}

void MainWindow::saveSettings()
{
    QSettings settings;

    settings.setValue("working_time", timer_time_working);
    settings.setValue("resting_time", timer_time_resting);
}
