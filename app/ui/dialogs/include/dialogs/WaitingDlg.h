#pragma once

#include <QDialog>

class WaitingDlg : public QDialog
{
public:
    explicit WaitingDlg(const QString& labelText);

    void close();

protected:
    void keyPressEvent(QKeyEvent* e) override;
    void closeEvent(QCloseEvent* e) override;

private:
    bool mPrepareToClose = false;
};
