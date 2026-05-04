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

#include <RevisionFiles.h>
#include <cache/Commit.h>
#include <graph/CommitEntry.h>

#include <QHash>
#include <QMutex>
#include <QObject>
#include <QSharedPointer>

#include <optional>
#include <span>

class GitBase;

/**
 * @brief Thread-safe store for all commit data, references, and revision files of a repository.
 *
 * SacredTimeline is the central data cache populated by GitRepoLoader and read by the UI.
 * It maintains three independent mutex-guarded stores:
 *  - Commits (mCommitsMutex): the ordered commit list, a SHA→pointer map, and the WIP entry.
 *  - Revision files (mRevisionsMutex): per-diff file-change lists keyed by (sha1, sha2).
 *  - References (mReferencesMutex): branches, tags, and remote refs keyed by SHA.
 *
 * The friend class GitRepoLoader calls the private processCommits() and setConfigurationDone()
 * methods to populate the cache after parsing. All other callers use only the public API.
 */
class SacredTimeline : public QObject
{
    Q_OBJECT

signals:
    /** Emitted when references or tags change so connected views can refresh. */
    void cacheUpdated();

public:
    /** Distance of the current local branch relative to its upstream. */
    struct LocalBranchDistances
    {
        int aheadOrigin = 0;
        int behindOrigin = 0;
    };

    explicit SacredTimeline(QObject* parent = nullptr);
    ~SacredTimeline();

    /** Returns the total number of commits currently in the cache, including the WIP entry. */
    int commitCount() const;

    /**
     * @brief Returns a batch of (sha, parents) pairs for use by the graph lane builder.
     *
     * @param from   Zero-based index of the first commit to include.
     * @param count  Maximum number of commits to return.
     */
    QVector<Graph::CommitEntry> getCommitBatch(int from, int count) const;

    /** Returns the commit with the given @a sha, or an empty Commit if not found. */
    Commit commitInfo(const QString& sha);

    /** Returns the commit at row @a row, or an empty Commit if the index is out of range. */
    Commit commitInfo(int row);

    /**
     * @brief Searches commits for one whose fields contain @a text.
     *
     * @param text          Text to search for.
     * @param startingPoint Row index at which to begin the search (wraps around).
     * @param reverse       If true, searches from @a startingPoint toward older commits.
     */
    Commit searchCommitInfo(const QString& text, int startingPoint = 0, bool reverse = false);

    /**
     * @brief Updates the WIP (zero-SHA) commit in place with the current working-tree state.
     *
     * @return @c true if the update was applied; @c false if the cache is not yet fully
     *         configured (setConfigurationDone() has not been called).
     */
    bool updateWipCommit(const QString& parentSha, const RevisionFiles& files);

    /**
     * @brief Inserts a newly created commit just after the WIP entry.
     *
     * Updates parent/child links and remaps all SHA→pointer entries. Called after a
     * successful local commit operation.
     */
    void insertCommit(Commit commit);

    /**
     * @brief Replaces the commit identified by @a oldSha with @a newCommit.
     *
     * Migrates all local branch and tag references from the old SHA to the new one.
     * Called after an amend or similar rewrite operation.
     */
    void updateCommit(const QString& oldSha, Commit newCommit);

    /** Updates mutable fields (e.g. longLog) of the cached commit identified by @a info.sha in place. */
    void updateCommitInfo(const Commit& info);

    /** Returns a span over the internal commit vector. The span is valid until the next mutation. */
    std::span<Commit> getCommits();

    /**
     * @brief Stores the revision-file list for the diff between @a sha1 and @a sha2.
     *
     * @return @c true if the entry was inserted or updated; @c false if it was unchanged.
     */
    bool insertRevisionFiles(const QString& sha1, const QString& sha2, const RevisionFiles& file);

    /**
     * @brief Returns the revision-file list for the diff between @a sha1 and @a sha2.
     * @return The stored RevisionFiles, or @c std::nullopt if not yet cached.
     */
    std::optional<RevisionFiles> revisionFile(const QString& sha1, const QString& sha2) const;

    /** Removes all cached references. Called by GitRepoLoader before re-fetching refs. */
    void clearReferences();

    /** Associates @a reference of @a type with commit @a sha. */
    void insertReference(const QString& sha, References::Type type, const QString& reference);

    /** Removes @a reference of @a type from commit @a sha. */
    void deleteReference(const QString& sha, References::Type type, const QString& reference);

    /** Returns @c true if commit @a sha has at least one associated reference. */
    bool hasReferences(const QString& sha);

    /** Returns all reference names of @a type associated with commit @a sha. */
    QStringList getReferences(const QString& sha, References::Type type);

    /** Returns the SHA of the commit that owns @a referenceName of @a type, or an empty string. */
    QString getShaOfReference(const QString& referenceName, References::Type type) const;

    /**
     * @brief Moves the current-branch reference to @a currentSha.
     *
     * Removes the branch name from whatever SHA previously held it, then re-inserts it
     * at @a currentSha. Called after GitRepoLoader resolves the active branch.
     */
    void reloadCurrentBranchInfo(const QString& currentBranch, const QString& currentSha);

    /** Replaces the untracked-files list used to compute the WIP commit description. */
    void setUntrackedFilesList(QVector<QString> untrackedFiles);

    /**
     * @brief Returns @c true if the WIP commit has staged or unstaged changes beyond untracked files.
     */
    bool pendingLocalChanges();

    /** Returns all (sha, names) pairs for references of @a type (typically local or remote branches). */
    QVector<QPair<QString, QStringList>> getBranches(References::Type type);

    /** Returns a name→sha map for all references of @a tagType. */
    QMap<QString, QString> getTags(References::Type tagType) const;

    /**
     * @brief Merges @a remoteTags into the reference store and emits cacheUpdated().
     *
     * Called by GitTags when the remote-tag fetch completes.
     */
    void updateTags(QMap<QString, QString> remoteTags);

    /**
     * @brief Queries the working tree via GitWip and updates the WIP commit in place.
     *
     * Fetches the current untracked-file list and staged/unstaged diff, then calls
     * setUntrackedFilesList() and updateWipCommit() with the results.
     * @return @c true if the WIP commit was updated; @c false if the cache is not yet
     *         fully configured.
     */
    bool refreshWip(const QSharedPointer<GitBase>& git);

    /** Returns @c true once processCommits() has been called at least once. */
    bool isInitialized() const { return mInitialized; }

private:
    friend class GitRepoLoader;

    bool mInitialized = false;
    bool mConfigured = true;
    QVector<QString> mUntrackedFiles;

    mutable QMutex mCommitsMutex;
    QVector<Commit> mCommitsCache;
    QHash<QString, Commit*> mCommitsMap;

    mutable QMutex mRevisionsMutex;
    QHash<QPair<QString, QString>, RevisionFiles> mRevisionFilesMap;

    mutable QMutex mReferencesMutex;
    QHash<QString, References> mReferences;

    /**
     * @brief Replaces all commit data, rebuilds parent/child links, and inserts the WIP entry.
     *
     * Called exclusively by GitRepoLoader after parsing the git log. Returns a span over
     * the internal commit vector so the graph builder can iterate without copying.
     */
    std::span<Commit> processCommits(const QString& parentSha, const RevisionFiles& files, QVector<Commit> commits);

    /** Marks the cache as fully configured. Called by GitRepoLoader after references are loaded. */
    void setConfigurationDone() { mConfigured = true; }

    bool insertRevisionFile(const QString& sha1, const QString& sha2, const RevisionFiles& file);
    void insertWipRevision(const QString parentSha, const RevisionFiles& files);
    auto searchCommit(const QString& text, int startingPoint = 0) const;
    auto reverseSearchCommit(const QString& text, int startingPoint = 0) const;
    void clearInternalData();
};
