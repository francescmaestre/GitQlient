#include <commit-widgets/CommitInfoPanel.h>
#include <commit-widgets/CommitInfoWidget.h>

#include <GitExecResult.h>
#include <commit-widgets/FileListWidget.h>
#include <core/cache/Commit.h>
#include <core/cache/GitCache.h>

#include <QDateTime>
#include <QLabel>
#include <QVBoxLayout>

#include <QLogger>

using namespace QLogger;

CommitInfoWidget::CommitInfoWidget(const QSharedPointer<GitCache> &cache, const QSharedPointer<GitBase> &git,
                                   QWidget *parent)
   : QFrame(parent)
   , mCache(cache)
   , mGit(git)
   , mInfoPanel(new CommitInfoPanel())
   , mFileListWidget(new FileListWidget(mGit, mCache))
{
   setAttribute(Qt::WA_DeleteOnClose);

   mFileListWidget->setObjectName("fileListWidget");

   const auto wipSeparator = new QFrame();
   wipSeparator->setObjectName("separator");

   const auto mainLayout = new QGridLayout(this);
   mainLayout->setSpacing(0);
   mainLayout->setContentsMargins(0, 0, 0, 0);
   mainLayout->addWidget(mInfoPanel, 0, 0);
   mainLayout->addWidget(wipSeparator, 1, 0);
   mainLayout->addWidget(mFileListWidget, 2, 0);
   mainLayout->setRowStretch(1, 0);
   mainLayout->setRowStretch(2, 0);
   mainLayout->setRowStretch(2, 1);

   connect(mFileListWidget, &FileListWidget::itemClicked, this, &CommitInfoWidget::handleItemClick);
   connect(mFileListWidget, &FileListWidget::signalShowFileHistory, this, &CommitInfoWidget::signalShowFileHistory);
   connect(mFileListWidget, &FileListWidget::signalEditFile, this, &CommitInfoWidget::signalEditFile);
}

void CommitInfoWidget::configure(const QString &sha)
{
   if (sha == mCurrentSha)
      return;

   clear();

   mCurrentSha = sha;
   mParentSha = sha;

   if (sha != ZERO_SHA && !sha.isEmpty())
   {
      const auto commit = mCache->commitInfo(sha);

      if (!commit.sha.isEmpty())
      {
         QLog_Info("UI", QString("Loading information of the commit {%1}").arg(sha));
         mCurrentSha = commit.sha;
         mParentSha = commit.firstParent();

         mInfoPanel->configure(commit);

         mFileListWidget->insertFiles(mCurrentSha, mParentSha);
      }
   }
}

QString CommitInfoWidget::getCurrentCommitSha() const
{
   return mCurrentSha;
}

void CommitInfoWidget::clear()
{
   mCurrentSha = QString();
   mParentSha = QString();

   mFileListWidget->clear();
}

void CommitInfoWidget::handleItemClick(QListWidgetItem *item)
{
   if (mLastSelectedItem == item && item->isSelected())
   {
      mFileListWidget->clearSelection();
      mLastSelectedItem = nullptr;

      emit signalReturnToHistory();
   }
   else
   {

      item->setSelected(true);
      mLastSelectedItem = item;

      emit showFileDiff(item->text(), mCurrentSha, mParentSha);
   }
}
