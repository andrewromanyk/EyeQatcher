#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QAction>
#include <QSettings>
#include <QValidator>
#include <QTime>

namespace DEFAULT_VALUES {
    constexpr int MAX_WORK_REST_TIME = 18'000;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //load settings
    loadSettings();
    timer_current_remaining = timer_time_working;

    //additional ui setup
    ui->settingsRestTimeSpinBox->setRange(1, DEFAULT_VALUES::MAX_WORK_REST_TIME);
    ui->settingsRestTimeSpinBox->setValue(timer_time_resting / 1000);
    ui->settingsWorkTimeSpinBox->setRange(1, DEFAULT_VALUES::MAX_WORK_REST_TIME);
    ui->settingsWorkTimeSpinBox->setValue(timer_time_working / 1000);
    ui->settingsVolumeSlider->setValue(notification_volume);

    ui->statusbar->setSizeGripEnabled(false);
    setFixedSize(size());

    //obj setups
    timer = new QTimer(this);
    timer->setInterval(TIMER_INTERVAL_DEFAULT);

    //connections

    ///  timers
    connect(timer, &QTimer::timeout, this, &MainWindow::handleIntervalTimeout);
    connect(timer, &QTimer::timeout, this, &MainWindow::drawTimerLabel);
    connect(timer, &QTimer::timeout, this, &MainWindow::setMainPageStatusLabel);

    ///  buttons timer start and stop
    connect(ui->startButton, &QPushButton::clicked, timer, [this]() {
        timer->start();
        elapsed_timer.start();
    });
    connect(ui->stopButton, &QPushButton::clicked, timer, &QTimer::stop);
    connect(ui->resetButton, &QPushButton::clicked, this, &MainWindow::resetTimer);

    ///  buttons change current tab
    connect(ui->actionSettings, &QAction::triggered, this, [this]() {
        ui->stackedWidget->setCurrentIndex(1);
    });
    connect(ui->actionHome_2, &QAction::triggered, this, [this]() {
        ui->stackedWidget->setCurrentIndex(0);
    });
    connect(ui->actionExit, &QAction::triggered, QApplication::instance(), &QApplication::quit);

    ///  settings buttons
    connect(ui->settingsApplyButton, &QPushButton::clicked, this, &MainWindow::applyNewSettings);
    connect(ui->settingsDefaultButton, &QPushButton::clicked, this, &MainWindow::settingsSetDefaultValues);

    //tray
    tray_icon = new QSystemTrayIcon(this);

    QIcon icon(":/icons/temp_tray.png");
    tray_icon->setIcon(icon);

    tray_menu = new QMenu(this);

    QAction* action_open = new QAction("Open", tray_menu);
    tray_menu->addAction(action_open);
    connect(action_open, &QAction::triggered, this, &MainWindow::show);

    tray_menu->addSeparator();

    QAction* action_exit = new QAction("Exit", tray_menu);
    tray_menu->addAction(action_exit);
    connect(action_exit, &QAction::triggered, qApp, &QApplication::quit);

    connect(tray_icon, &QSystemTrayIcon::activated, this,
        [this](QSystemTrayIcon::ActivationReason reason) {
            if (reason == QSystemTrayIcon::Trigger)
                show();
        });

    tray_icon->setContextMenu(tray_menu);
    tray_icon->show();

    //audio player
    sound_effect->setSource(QUrl("qrc:/sounds/timer_up.wav"));
    sound_effect->setVolume(getCurrentVolumeFloat());

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
        sound_effect->play();
        tray_icon->showMessage( // maybe i should replace the entire norification with smth from Marketplace
            "Timer's up",
            "The timer has run out! (REPLACE IT!)",
            QSystemTrayIcon::Information,   // NoIcon does nothing, i dont want to use custom icon.
            15000);
        timer->stop();
        elapsed_timer.invalidate();
        switchStatus();
        timer_current_remaining = getCurrentTimerStatusTime();
    }
}

void MainWindow::drawTimerLabel()
{
    QTime time = QTime::fromMSecsSinceStartOfDay(timer_current_remaining);
    ui->mainTimerLabel->setText(time.toString("mm:ss"));
    ui->msTimerLabel->setText(time.toString(".zzz"));
    setMainPageStatusLabel();
}

void MainWindow::applyNewSettings()
{
    timer_time_working = ui->settingsWorkTimeSpinBox->value() * 1000;
    timer_time_resting = ui->settingsRestTimeSpinBox->value() * 1000;

    notification_volume = ui->settingsVolumeSlider->value();
    sound_effect->setVolume(getCurrentVolumeFloat());

    ui->statusbar->showMessage("Settings saved.", 5000);

    saveSettings();
}

void MainWindow::settingsSetDefaultValues()
{
    ui->settingsRestTimeSpinBox->setValue(TIMER_TIME_RESTING_DEFAULT / 1000);
    ui->settingsWorkTimeSpinBox->setValue(TIMER_TIME_WORKING_DEFAULT / 1000);
    ui->settingsVolumeSlider->setValue(NOTIFICATION_VOLUME_DEFAULT);

    ui->statusbar->showMessage("Applied default values.", 5000);
}

void MainWindow::resetTimer()
{
    timer->stop();
    timer_current_remaining = getCurrentTimerStatusTime();
    drawTimerLabel();
}

void MainWindow::setMainPageStatusLabel()
{
    ui->currentStatusLabel->setText(getCurrentTextLabelMain());
}

void MainWindow::loadSettings()
{
    QSettings settings;

    if (!settings.contains("working_time"))
        settings.setValue("working_time", TIMER_TIME_WORKING_DEFAULT);

    if (!settings.contains("resting_time"))
        settings.setValue("resting_time", TIMER_TIME_RESTING_DEFAULT);

    if (!settings.contains("notification_volume"))
        settings.setValue("notification_volume", NOTIFICATION_VOLUME_DEFAULT);

    timer_time_working = settings.value("working_time").toInt();
    timer_time_resting = settings.value("resting_time").toInt();
    notification_volume = settings.value("notification_volume").toInt();
}

void MainWindow::saveSettings()
{
    QSettings settings;

    settings.setValue("working_time", timer_time_working);
    settings.setValue("resting_time", timer_time_resting);
    settings.setValue("notification_volume", notification_volume);
}
