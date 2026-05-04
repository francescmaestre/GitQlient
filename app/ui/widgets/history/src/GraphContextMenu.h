#pragma once

/****************************************************************************************
 ** GitQlient is an application to manage and operate one or several Git repositories. With
 ** GitQlient you will be able to add commits, branches and manage all the options Git provides.
 ** Copyright (C) 2021  Francesc Maestre
 **
 ** LinkedIn: https://www.linkedin.com/in/francescmaestre/
 **
 ** This program is free software; you can redistribute it and/or
 ** modify it under the terms of the GNU Lesser General Public
 ** License as published by the Free Software Foundation; either
 ** version 2 of the License, or (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 ** Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public
 ** License along with this library; if not, write to the Free Software
 ** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 ***************************************************************************************/

#include <QMenu>

class SacredTimeline;
namespace Graph
{
    class TemporalLoom;
}
class GitBase;

class GraphContextMenu : public QMenu
{
    Q_OBJECT

signals:
    void fullReload();
    void referencesReload();
    void logReload();
    void signalOpenDiff(const QString& sha);
    void signalAmendCommit(const QString& sha);
    void signalRebaseConflict();
    void signalMergeRequired(const QString& origin, const QString& destination);
    void mergeSqushRequested(const QString& origin, const QString& destination);
    void signalCherryPickConflict(const QStringList& pendingShas = QStringList());
    void signalPullConflict();
    void signalRefreshPRsCache();

public:
    explicit GraphContextMenu(
        const QSharedPointer<SacredTimeline>& cache,
        const QSharedPointer<Graph::TemporalLoom>& graphCache,
        const QSharedPointer<GitBase>& git,
        const QStringList& shas,
        QWidget* parent = nullptr);

private:
    QSharedPointer<SacredTimeline> mCache;
    QSharedPointer<Graph::TemporalLoom> mGraphCache;
    QSharedPointer<GitBase> mGit;
    QStringList mShas;

    void createIndividualShaMenu();
    void createMultipleShasMenu();
    void createBranch();
    void createTag();
    void exportAsPatch();
    void checkoutBranch();
    void createCheckoutBranch();
    void checkoutCommit();
    void cherryPickCommit();
    void applyPatch();
    void applyCommit();
    void push();
    void pull();
    void fetch();
    void revertCommit();
    void resetSoft();
    void resetMixed();
    void resetHard();
    void rebase();
    void merge();
    void mergeSquash();
    void addBranchActions(const QString& sha);
    void showSquashDialog();
    void amendNoEdit();
};
