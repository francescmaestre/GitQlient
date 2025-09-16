#include <main-widgets/GitQlientRepo.h>

#include <GitBase.h>
#include <GitConfig.h>
#include <GitHistory.h>
#include <GitLocal.h>
#include <GitMerge.h>
#include <GitSubmodules.h>
#include <GitTags.h>
#include <GitWip.h>
#include <QLogger>
#include <core/cache/Commit.h>
#include <core/cache/GitCache.h>
#include <core/cache/GraphCache.h>
#include <core/graph/WipHelper.h>
#include <core/system/GitQlientSettings.h>
#include <core/system/GitQlientStyles.h>
#include <core/system/GitRepoLoader.h>
#include <dialogs/GitConfigDlg.h>
#include <dialogs/WaitingDlg.h>
#include <diff-widgets/DiffWidget.h>
#include <history-widgets/BlameWidget.h>
#include <history-widgets/HistoryWidget.h>
#include <main-widgets/ConfigWidget.h>
#include <main-widgets/Controls.h>
#include <main-widgets/MergeWidget.h>
#include <ref-widgets/BranchesWidget.h>

#include <QApplication>
#include <QDirIterator>
#include <QFileDialog>
#include <QGridLayout>
#include <QMessageBox>
#include <QStackedLayout>
#include <QStackedWidget>
#include <QTimer>

using namespace QLogger;

GitQlientRepo::GitQlientRepo(const QSharedPointer<GitBase> &git, const QSharedPointer<GitQlientSettings> &settings,
                             QWidget *parent)
   : QFrame(parent)
   , mGitQlientCache(new GitCache())
   , mGraphCache(new Graph::Cache())
   , mGitBase(git)
   , mSettings(settings)
   , mGitLoader(new GitRepoLoader(mGitBase, mGitQlientCache, mGraphCache, mSettings))
   , mAutoFilesUpdate(new QTimer())
{
   setAttribute(Qt::WA_DeleteOnClose);

   QLog_Info("UI", QString("Initializing GitQlient for repo %1").arg(git->getGitDir()));

   setObjectName("mainWindow");
   setWindowTitle("GitQlient");
   setAttribute(Qt::WA_DeleteOnClose);

   mStackedLayout = new QStackedLayout();

   mHistoryWidget = new HistoryWidget(mGitQlientCache, mGraphCache, mGitBase, mSettings, this);
   mHistoryWidget->setContentsMargins(QMargins(5, 5, 5, 5));

   mDiffWidget = new DiffWidget(mGitBase, mGitQlientCache, this);
   mDiffWidget->setContentsMargins(QMargins(5, 5, 5, 5));

   mBlameWidget = new BlameWidget(mGitQlientCache, mGraphCache, mGitBase, mSettings, this);
   mBlameWidget->setContentsMargins(QMargins(5, 5, 5, 5));

   mMergeWidget = new MergeWidget(mGitQlientCache, mGitBase, this);
   mMergeWidget->setContentsMargins(QMargins(5, 5, 5, 5));

   mIndexMap[ControlsMainViews::History] = mStackedLayout->addWidget(mHistoryWidget);
   mIndexMap[ControlsMainViews::Diff] = mStackedLayout->addWidget(mDiffWidget);
   mIndexMap[ControlsMainViews::Blame] = mStackedLayout->addWidget(mBlameWidget);
   mIndexMap[ControlsMainViews::Merge] = mStackedLayout->addWidget(mMergeWidget);

   mControls = new Controls(mGitQlientCache, mGitBase, this);

   const auto mainLayout = new QVBoxLayout();
   mainLayout->setSpacing(0);
   mainLayout->setContentsMargins(QMargins());
   mainLayout->addWidget(mControls);
   mainLayout->addLayout(mStackedLayout);

   setLayout(mainLayout);

   showHistoryView();

   mAutoFilesUpdate->setInterval(mSettings->localValue("AutoRefresh", 60).toInt() * 1000);

   connect(mAutoFilesUpdate, &QTimer::timeout, this, &GitQlientRepo::updateUiFromWatcher);

   connect(mControls, &Controls::requestFullReload, this, &GitQlientRepo::fullReload);
   connect(mControls, &Controls::requestFullReload, this, &GitQlientRepo::updateUiFromWatcher);
   connect(mControls, &Controls::requestReferencesReload, this, &GitQlientRepo::referencesReload);
   connect(mControls, &Controls::signalGoMerge, this, &GitQlientRepo::showMergeView);
   connect(mControls, &Controls::signalPullConflict, mControls, &Controls::activateMergeWarning);
   connect(mControls, &Controls::signalPullConflict, this, &GitQlientRepo::showWarningMerge);

   connect(mHistoryWidget, &HistoryWidget::signalAllBranchesActive, mGitLoader.data(), &GitRepoLoader::setShowAll);
   connect(mHistoryWidget, &HistoryWidget::fullReload, this, &GitQlientRepo::fullReload);
   connect(mHistoryWidget, &HistoryWidget::referencesReload, this, &GitQlientRepo::referencesReload);
   connect(mHistoryWidget, &HistoryWidget::logReload, this, &GitQlientRepo::logReload);

   connect(mHistoryWidget, &HistoryWidget::signalOpenSubmodule, this, &GitQlientRepo::signalOpenSubmodule);
   connect(mHistoryWidget, &HistoryWidget::changesCommitted, this, &GitQlientRepo::onChangesCommitted);
   connect(mHistoryWidget, &HistoryWidget::signalShowFileHistory, this, &GitQlientRepo::showFileHistory);
   connect(mHistoryWidget, &HistoryWidget::signalRebaseConflict, mControls, &Controls::activateMergeWarning);
   connect(mHistoryWidget, &HistoryWidget::signalRebaseConflict, this, &GitQlientRepo::showRebaseConflict);
   connect(mHistoryWidget, &HistoryWidget::signalMergeConflicts, mControls, &Controls::activateMergeWarning);
   connect(mHistoryWidget, &HistoryWidget::signalMergeConflicts, this, &GitQlientRepo::showWarningMerge);
   connect(mHistoryWidget, &HistoryWidget::signalCherryPickConflict, mControls, &Controls::activateMergeWarning);
   connect(mHistoryWidget, &HistoryWidget::signalCherryPickConflict, this, &GitQlientRepo::showCherryPickConflict);
   connect(mHistoryWidget, &HistoryWidget::signalPullConflict, mControls, &Controls::activateMergeWarning);
   connect(mHistoryWidget, &HistoryWidget::signalPullConflict, this, &GitQlientRepo::showWarningMerge);
   connect(mHistoryWidget, &HistoryWidget::signalUpdateWip, this, &GitQlientRepo::updateWip);

   connect(mDiffWidget, &DiffWidget::signalShowFileHistory, this, &GitQlientRepo::showFileHistory);
   connect(mDiffWidget, &DiffWidget::signalDiffEmpty, this, &GitQlientRepo::showPreviousView);

   connect(mBlameWidget, &BlameWidget::showFileDiff, this, &GitQlientRepo::loadFileDiff);

   connect(mMergeWidget, &MergeWidget::signalMergeFinished, this, &GitQlientRepo::showHistoryView);
   connect(mMergeWidget, &MergeWidget::signalMergeFinished, mGitLoader.data(), &GitRepoLoader::loadAll);
   connect(mMergeWidget, &MergeWidget::signalMergeFinished, mControls, &Controls::disableMergeWarning);

   connect(mGitLoader.data(), &GitRepoLoader::signalLoadingStarted, this, &GitQlientRepo::createProgressDialog);
   connect(mGitLoader.data(), &GitRepoLoader::signalLoadingFinished, this, &GitQlientRepo::onRepoLoadFinished);

   m_loaderThread = new QThread();
   mGitLoader->moveToThread(m_loaderThread);
   mGitQlientCache->moveToThread(m_loaderThread);
   connect(this, &GitQlientRepo::loadRepo, mGitLoader.data(), &GitRepoLoader::loadAll);
   connect(this, &GitQlientRepo::fullReload, mGitLoader.data(), &GitRepoLoader::loadAll);
   connect(this, &GitQlientRepo::referencesReload, mGitLoader.data(), &GitRepoLoader::loadReferences);
   connect(this, &GitQlientRepo::logReload, mGitLoader.data(), &GitRepoLoader::loadLogHistory);
   m_loaderThread->start();

   mGitLoader->setShowAll(mSettings->localValue("ShowAllBranches", true).toBool());
}

GitQlientRepo::~GitQlientRepo()
{
   delete mAutoFilesUpdate;

   m_loaderThread->exit();
   m_loaderThread->wait();

   delete m_loaderThread;
}

QString GitQlientRepo::currentBranch() const
{
   return mGitBase->getCurrentBranch();
}

void GitQlientRepo::updateUiFromWatcher()
{
   QLog_Info("UI", QString("Updating the GitQlient UI from watcher"));

   WipHelper::update(mGitBase, mGitQlientCache);

   mHistoryWidget->updateUiFromWatcher();

   mDiffWidget->reload();
}

void GitQlientRepo::clearWindow()
{
   blockSignals(true);

   mHistoryWidget->clear();
   mDiffWidget->clear();

   blockSignals(false);
}

void GitQlientRepo::setWidgetsEnabled(bool enabled)
{
   mControls->enableButtons(enabled);
   mHistoryWidget->setEnabled(enabled);
   mDiffWidget->setEnabled(enabled);
}

void GitQlientRepo::showFileHistory(const QString &fileName)
{
   mBlameWidget->showFileHistory(fileName);

   showBlameView();
}

void GitQlientRepo::createProgressDialog()
{
   if (!mWaitDlg)
   {
      mWaitDlg = new WaitingDlg(tr("Loading repository..."));
      mWaitDlg->setWindowFlag(Qt::Tool);
      mWaitDlg->open();

      QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
   }
}

void GitQlientRepo::onRepoLoadFinished()
{
   if (!mIsInit)
   {
      mIsInit = true;

      mCurrentDir = mGitBase->getWorkingDir();

      emit repoOpened(mCurrentDir);

      setWidgetsEnabled(true);

      mBlameWidget->init(mCurrentDir);

      mAutoFilesUpdate->start();

      if (GitConfig git(mGitBase); !git.getGlobalUserInfo().isValid() && !git.getLocalUserInfo().isValid())
      {
         QLog_Info("UI", QString("Configuring Git..."));

         GitConfigDlg configDlg(mGitBase);

         configDlg.exec();

         QLog_Info("UI", QString("... Git configured!"));
      }

      QLog_Info("UI", "... repository loaded successfully");
   }

   const auto totalCommits = mGitQlientCache->commitCount();

   if (totalCommits == 0)
   {
      if (mWaitDlg)
         mWaitDlg->close();

      return;
   }

   // Update WIP information on initial load
   WipHelper::update(mGitBase, mGitQlientCache);

   mHistoryWidget->updateGraphView(totalCommits);

   mBlameWidget->onNewRevisions(totalCommits);

   mDiffWidget->reload();

   if (mWaitDlg)
      mWaitDlg->close();

   if (QScopedPointer<GitMerge> gitMerge(new GitMerge(mGitBase)); gitMerge->isInMerge())
   {
      mControls->activateMergeWarning();
      showWarningMerge();

      QMessageBox::warning(this, tr("Merge in progress"),
                           tr("There is a merge conflict in progress. Solve the merge before moving on."));
   }
   else if (QScopedPointer<GitLocal> gitMerge(new GitLocal(mGitBase)); gitMerge->isInCherryPickMerge())
   {
      mControls->activateMergeWarning();
      showCherryPickConflict();

      QMessageBox::warning(
          this, tr("Cherry-pick in progress"),
          tr("There is a cherry-pick in progress that contains with conflicts. Solve them before moving on."));
   }

   emit currentBranchChanged();
}

void GitQlientRepo::loadFileDiff(const QString &currentSha, const QString &previousSha, const QString &file)
{
   const auto loaded = mDiffWidget->loadFileDiff(currentSha, previousSha, file);

   if (loaded)
      showDiffView();
}

void GitQlientRepo::showHistoryView()
{
   mPreviousView = mStackedLayout->currentIndex();

   mStackedLayout->setCurrentIndex(mIndexMap[ControlsMainViews::History]);
}

void GitQlientRepo::showBlameView()
{
   mPreviousView = mStackedLayout->currentIndex();

   mStackedLayout->setCurrentIndex(mIndexMap[ControlsMainViews::Blame]);
}

void GitQlientRepo::showDiffView()
{
   mPreviousView = mStackedLayout->currentIndex();

   mStackedLayout->setCurrentIndex(mIndexMap[ControlsMainViews::Diff]);
}

// TODO: Optimize
void GitQlientRepo::showWarningMerge()
{
   showMergeView();

   const auto wipCommit = mGitQlientCache->commitInfo(ZERO_SHA);

   WipHelper::update(mGitBase, mGitQlientCache);

   const auto file = mGitQlientCache->revisionFile(ZERO_SHA, wipCommit.firstParent());

   if (file)
      mMergeWidget->configure(file.value(), MergeWidget::ConflictReason::Merge);
}

void GitQlientRepo::showRebaseConflict()
{
   showMergeView();

   const auto wipCommit = mGitQlientCache->commitInfo(ZERO_SHA);

   WipHelper::update(mGitBase, mGitQlientCache);

   const auto files = mGitQlientCache->revisionFile(ZERO_SHA, wipCommit.firstParent());

   if (files)
      mMergeWidget->configureForRebase();
}

// TODO: Optimize
void GitQlientRepo::showCherryPickConflict(const QStringList &shas)
{
   showMergeView();

   const auto wipCommit = mGitQlientCache->commitInfo(ZERO_SHA);

   WipHelper::update(mGitBase, mGitQlientCache);

   const auto files = mGitQlientCache->revisionFile(ZERO_SHA, wipCommit.firstParent());

   if (files)
      mMergeWidget->configureForCherryPick(files.value(), shas);
}

// TODO: Optimize
void GitQlientRepo::showPullConflict()
{
   showMergeView();

   const auto wipCommit = mGitQlientCache->commitInfo(ZERO_SHA);

   WipHelper::update(mGitBase, mGitQlientCache);

   const auto files = mGitQlientCache->revisionFile(ZERO_SHA, wipCommit.firstParent());

   if (files)
      mMergeWidget->configure(files.value(), MergeWidget::ConflictReason::Pull);
}

void GitQlientRepo::showMergeView()
{
   mStackedLayout->setCurrentIndex(mIndexMap[ControlsMainViews::Merge]);
}

void GitQlientRepo::showPreviousView()
{
   mStackedLayout->setCurrentIndex(mPreviousView);
}

void GitQlientRepo::updateWip()
{
   mHistoryWidget->resetWip();

   WipHelper::update(mGitBase, mGitQlientCache);

   mHistoryWidget->updateUiFromWatcher();
}

void GitQlientRepo::reconfigureAutoRefresh(int newInterval)
{
   if (newInterval > 0)
      mAutoFilesUpdate->start(newInterval * 1000);
   else
      mAutoFilesUpdate->stop();
}

void GitQlientRepo::onChangesCommitted()
{
   mHistoryWidget->selectCommit(ZERO_SHA);
   mHistoryWidget->loadBranches(false);
   showHistoryView();
}

void GitQlientRepo::closeEvent(QCloseEvent *ce)
{
   QLog_Info("UI", QString("Closing GitQlient for repository {%1}").arg(mCurrentDir));

   mGitLoader->cancelAll();

   QWidget::closeEvent(ce);
}
