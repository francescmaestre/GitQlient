#include "dialogs/InputShaDlg.h"
#include "ui_InputShaDlg.h"

#include <GitBase.h>
#include <GitBranches.h>
#include <GitConfig.h>
#include <GitStashes.h>
#include <core/cache/GitCache.h>
#include <core/system/GitQlientStyles.h>

#include <QFile>
#include <QMessageBox>

InputShaDlg::InputShaDlg(const QString &branch, QSharedPointer<GitBase> git, QWidget *parent)
   : QDialog(parent)
   , ui(new Ui::InputShaDlg)
   , mGit(git)
   , mBranch(branch)
{
   setStyleSheet(GitQlientStyles::getStyles());

   ui->setupUi(this);

   connect(ui->pbAccept, &QPushButton::clicked, this, &InputShaDlg::accept);
   connect(ui->pbCancel, &QPushButton::clicked, this, &InputShaDlg::reject);
}

InputShaDlg::~InputShaDlg()
{
   delete ui;
}

void InputShaDlg::accept()
{
   if (!ui->leSha->text().isEmpty())
   {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      QScopedPointer<GitBranches> git(new GitBranches(mGit));
      const auto ret = git->resetToSha(mBranch, ui->leSha->text());
      QApplication::restoreOverrideCursor();

      if (ret.success)
         QDialog::accept();
      else
         QMessageBox::critical(this, tr("Reset failed"),
                               tr("There were some problems while fetching. Please try again."));
   }
}
