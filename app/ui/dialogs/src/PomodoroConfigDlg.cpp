#include "dialogs/PomodoroConfigDlg.h"
#include "ui_PomodoroConfigDlg.h"

#include <GitBase.h>
#include <system/GitQlientStyles.h>
#include <system/SettingsKeys.h>

#include <QSettings>

using namespace System;

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
    ui->cbAlarmSound->setChecked(settings.value(GlobalKey::Pomodoro::Alarm, false).toBool());
    ui->cbStopResets->setChecked(settings.value(GlobalKey::Pomodoro::StopResets, true).toBool());
    ui->pomodoroDur->setValue(settings.value(GlobalKey::Pomodoro::Duration, 25).toInt());
    ui->breakDur->setValue(settings.value(GlobalKey::Pomodoro::Break, 5).toInt());
    ui->longBreakDur->setValue(settings.value(GlobalKey::Pomodoro::LongBreak, 15).toInt());
    ui->sbLongBreakCount->setValue(settings.value(GlobalKey::Pomodoro::LongBreakTrigger, 4).toInt());
}

PomodoroConfigDlg::~PomodoroConfigDlg() { delete ui; }

void PomodoroConfigDlg::accept()
{
    QSettings settings;
    settings.setValue(GlobalKey::Pomodoro::Alarm, ui->cbAlarmSound->isChecked());
    settings.setValue(GlobalKey::Pomodoro::StopResets, ui->cbStopResets->isChecked());
    settings.setValue(GlobalKey::Pomodoro::Duration, ui->pomodoroDur->value());
    settings.setValue(GlobalKey::Pomodoro::Break, ui->breakDur->value());
    settings.setValue(GlobalKey::Pomodoro::LongBreak, ui->longBreakDur->value());
    settings.setValue(GlobalKey::Pomodoro::LongBreakTrigger, ui->sbLongBreakCount->value());

    QDialog::accept();
}
