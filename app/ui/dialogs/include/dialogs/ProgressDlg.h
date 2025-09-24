#pragma once

#include <QProgressDialog>

class ProgressDlg : public QProgressDialog
{
public:
    explicit ProgressDlg(const QString& labelText, const QString& cancelButtonText, int maximum, bool autoClose);

    void keyPressEvent(QKeyEvent* e) override;
    void closeEvent(QCloseEvent* e) override;
    void close();

private:
    bool mPrepareToClose = false;
};
