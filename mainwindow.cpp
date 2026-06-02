#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QAction>
#include <QSettings>
#include <QValidator>
#include <QTime>

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
    ui->settingsRestTimeSpinBox->setValue(timer_time_resting);
    ui->settingsWorkTimeSpinBox->setRange(1, DEFAULT_VALUES::MAX_WORK_REST_TIME);
    ui->settingsWorkTimeSpinBox->setValue(timer_time_working);


    //obj setups
    timer = new QTimer(this);
    timer->setInterval(TIMER_INTERVAL_DEFAULT);

    //connections

    //  timers
    connect(timer, &QTimer::timeout, this, &MainWindow::handleIntervalTimeout);
    connect(timer, &QTimer::timeout, this, &MainWindow::drawTimerLabel);

    //  buttons timer start and stop
    connect(ui->startButton, &QPushButton::clicked, timer, [this]() {
        timer->start();
        elapsed_timer.start();
    });
    connect(ui->stopButton, &QPushButton::clicked, timer, &QTimer::stop);

    //  buttons change current tab
    connect(ui->actionSettings, &QAction::triggered, this, [this]() {
        ui->stackedWidget->setCurrentIndex(1);
    });
    connect(ui->actionHome_2, &QAction::triggered, this, [this]() {
        ui->stackedWidget->setCurrentIndex(0);
    });

    //  settings buttons
    connect(ui->settingsApplyButton, &QPushButton::clicked, this, &MainWindow::applyNewSettings);
    connect(ui->settingsDefaultButton, &QPushButton::clicked, this, &MainWindow::settingsSetDefaultValues);

    //tray
    tray_icon = new QSystemTrayIcon(this);
    QIcon icon(":/icons/temp_tray.png");
    qDebug() << "Is icon null " << icon.isNull();
    tray_icon->setIcon(icon);
    tray_icon->show();

    //obj starts
    drawTimerLabel();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::handleIntervalTimeout()
{
    timer_current_remaining -= elapsed_timer.restart();
    if (timer_current_remaining <= 0) {
        timer->stop();
        elapsed_timer.invalidate();
        timer_status = timer_status == Working ? Resting : Working;
        timer_current_remaining = timer_status == Working ? timer_time_working : timer_time_resting;
    }
}

void MainWindow::drawTimerLabel()
{
    QTime time = QTime::fromMSecsSinceStartOfDay(timer_current_remaining);
    ui->mainTimerLabel->setText(time.toString("mm:ss"));
    ui->msTimerLabel->setText(time.toString(".zzz"));
}

void MainWindow::applyNewSettings()
{
    timer_time_working = ui->settingsWorkTimeSpinBox->value();
    timer_time_resting = ui->settingsRestTimeSpinBox->value();

    saveSettings();
}

void MainWindow::settingsSetDefaultValues()
{
    ui->settingsRestTimeSpinBox->setValue(TIMER_TIME_RESTING_DEFAULT);
    ui->settingsWorkTimeSpinBox->setValue(TIMER_TIME_WORKING_DEFAULT);
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
