#include "dialogs/PomodoroConfigDlg.h"
#include "ui_PomodoroConfigDlg.h"

#include <GitBase.h>
#include <system/GitQlientSettings.h>
#include <system/GitQlientStyles.h>

PomodoroConfigDlg::PomodoroConfigDlg(const QSharedPointer<GitBase>& git, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::PomodoroConfigDlg)
    , mGit(git)
{
    ui->setupUi(this);

    connect(ui->pomodoroDur, &QSlider::valueChanged, this, [this](int value) {
        ui->pomodoroDurLabel->setText(QString::number(value));
    });
    connect(ui->breakDur, &QSlider::valueChanged, this, [this](int value) {
        ui->pomodoroBreakDurLabel->setText(QString::number(value));
    });
    connect(ui->longBreakDur, &QSlider::valueChanged, this, [this](int value) {
        ui->pomodoroLongBreakLabel->setText(QString::number(value));
    });

    QSettings settings;
    ui->cbAlarmSound->setChecked(settings.value("Pomodoro/Alarm", false).toBool());
    ui->cbStopResets->setChecked(settings.value("Pomodoro/StopResets", true).toBool());
    ui->pomodoroDur->setValue(settings.value("Pomodoro/Duration", 25).toInt());
    ui->breakDur->setValue(settings.value("Pomodoro/Break", 5).toInt());
    ui->longBreakDur->setValue(settings.value("Pomodoro/LongBreak", 15).toInt());
    ui->sbLongBreakCount->setValue(settings.value("Pomodoro/LongBreakTrigger", 4).toInt());
}

PomodoroConfigDlg::~PomodoroConfigDlg() { delete ui; }

void PomodoroConfigDlg::accept()
{
    QSettings settings;
    settings.setValue("Pomodoro/Alarm", ui->cbAlarmSound->isChecked());
    settings.setValue("Pomodoro/StopResets", ui->cbStopResets->isChecked());
    settings.setValue("Pomodoro/Duration", ui->pomodoroDur->value());
    settings.setValue("Pomodoro/Break", ui->breakDur->value());
    settings.setValue("Pomodoro/LongBreak", ui->longBreakDur->value());
    settings.setValue("Pomodoro/LongBreakTrigger", ui->sbLongBreakCount->value());

    QDialog::accept();
}
