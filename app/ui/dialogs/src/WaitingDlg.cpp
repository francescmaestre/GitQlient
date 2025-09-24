#include "dialogs/WaitingDlg.h"

#include <system/GitQlientStyles.h>

#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>

WaitingDlg::WaitingDlg(const QString& labelText)
    : QDialog()
{
    const auto layout = new QHBoxLayout(this);
    layout->addWidget(new QLabel(labelText));

    setAttribute(Qt::WA_DeleteOnClose);
    setWindowModality(Qt::ApplicationModal);
    setWindowFlags(Qt::FramelessWindowHint);
}

void WaitingDlg::keyPressEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Escape)
        return;

    QDialog::keyPressEvent(e);
}

void WaitingDlg::closeEvent(QCloseEvent* e)
{
    if (!mPrepareToClose)
        e->ignore();
    else
        QDialog::closeEvent(e);
}

void WaitingDlg::close()
{
    mPrepareToClose = true;

    QDialog::close();
}
