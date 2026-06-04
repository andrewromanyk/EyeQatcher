#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QElapsedTimer>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QSoundEffect>

constexpr int TIMER_TIME_WORKING_DEFAULT    = 4000; //= 20 * 60 * 1000; // 20 minuts = 20 * 60 (s in m) * 1000 (ms in s)
constexpr int TIMER_TIME_RESTING_DEFAULT    = 3000; //20 * 1000; // 20 seconds = 20  * 1000 (ms in s)
constexpr int TIMER_INTERVAL_DEFAULT        = 25; // in case user spams start-stop; the timer should count those small intervals
constexpr int NOTIFICATION_VOLUME_DEFAULT   = 50; // 0.5

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

enum TimerStatus {
    Working,
    Resting,
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QTimer* timer;
    QElapsedTimer elapsed_timer;

    QSystemTrayIcon* tray_icon;
    QMenu* tray_menu;

    QSoundEffect* sound_effect = new QSoundEffect(this);


    int timer_time_working          = TIMER_TIME_WORKING_DEFAULT;
    int timer_time_resting          = TIMER_TIME_RESTING_DEFAULT;
    int timer_current_remaining     = timer_time_working;
    TimerStatus timer_status        = Working;
    int notification_volume         = NOTIFICATION_VOLUME_DEFAULT;

    void loadSettings();
    void saveSettings();
    double getCurrentVolumeFloat() {
        return notification_volume / 100.0;
    }
    int getCurrentTimerStatusTime() {
        return timer_status == Working ? timer_time_working : timer_time_resting;
    }
    void switchStatus() {
        timer_status = timer_status == Working ? Resting : Working;
    }
    QString getCurrentTextLabelMain() {
        if (timer->isActive()) {
            return timer_status == Working ? "Working" : "Resting";
        }

        return "Waiting for action...";
    }

signals:
    void timerRanOut();

public slots:
    void handleIntervalTimeout();
    void drawTimerLabel();
    void applyNewSettings();
    void settingsSetDefaultValues();
    void resetTimer();
    void setMainPageStatusLabel();
};
#endif // MAINWINDOW_H
