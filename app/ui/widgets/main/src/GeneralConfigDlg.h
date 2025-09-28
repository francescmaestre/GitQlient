#pragma once

#include <QDialog>

class QSpinBox;
class CheckBox;
class QComboBox;
class QLabel;
class QPushButton;
class QLineEdit;

class GeneralConfigDlg : public QDialog
{
    Q_OBJECT

public:
    explicit GeneralConfigDlg(QWidget* parent = nullptr);

private:
    CheckBox* mDisableLogs = nullptr;
    QComboBox* mLevelCombo = nullptr;
    QLineEdit* mGitLocation = nullptr;
    bool mShowResetMsg = false;
    QPushButton* mClose = nullptr;
    QPushButton* mReset = nullptr;
    QPushButton* mApply = nullptr;

    void resetChanges();
    void accept() override;
    void importConfig();
    void exportConfig();
};
