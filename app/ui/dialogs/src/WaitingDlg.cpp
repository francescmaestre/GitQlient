#include "dialogs/WaitingDlg.h"

#include <system/GitQlientStyles.h>

#include <QCoreApplication>
#include <QKeyEvent>
#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>

WaitingDlg::WaitingDlg(const QString& labelText)
    : QDialog()
{
    mLabel = new QLabel(labelText);
    mProgressBar = new QProgressBar();
    mProgressBar->setRange(0, 0);
    mProgressBar->setTextVisible(false);

    const auto layout = new QVBoxLayout(this);
    layout->addWidget(mLabel);
    layout->addWidget(mProgressBar);

    setAttribute(Qt::WA_DeleteOnClose);
    setWindowModality(Qt::ApplicationModal);
    setWindowFlags(Qt::FramelessWindowHint);
    setMinimumWidth(300);
}

void WaitingDlg::updateMessage(const QString& message)
{
    mLabel->setText(message);
    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
}

void WaitingDlg::updateProgress(int done, int total)
{
    if (mProgressBar->maximum() != total)
        mProgressBar->setRange(0, total);
    mProgressBar->setValue(done);
    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
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
