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

#include <diff-widgets/IDiffWidget.h>

#include <QFrame>
#include <custom-widgets/DiffInfo.h>

class FileDiffView;
class QPushButton;
class CheckBox;
class FileEditor;
class QStackedWidget;
class QLabel;
class QLineEdit;
class QPlainTextEdit;
class QVBoxLayout;
class QSpacerItem;
class HunkWidget;
class ButtonLink;

class FileDiffWidget : public IDiffWidget
{
    Q_OBJECT

signals:
    void exitRequested();
    void fileStaged(const QString& fileName);
    void fileReverted(const QString& fileName);

public:
    explicit FileDiffWidget(
        const QSharedPointer<GitBase>& git, QSharedPointer<SacredTimeline> cache, QWidget* parent = nullptr);

    void clear();
    bool reload() override;
    void updateFontSize() override;
    void hideHunks() const;
    bool setup(
        const QString& file,
        bool isCached,
        bool editMode = false,
        QString currentSha = QString(),
        QString previousSha = QString());
    QString getCurrentFile() const { return mCurrentFile; }

private:
    enum View
    {
        Hunks,
        Unified,
        Split,
        Edition
    };

    QString mCurrentFile;
    bool mIsCached = false;
    QPushButton* mBack = nullptr;
    QPushButton* mGoPrevious = nullptr;
    QPushButton* mGoNext = nullptr;
    QPushButton* mEdition = nullptr;
    QPushButton* mHunksView = nullptr;
    QPushButton* mFullView = nullptr;
    QPushButton* mSplitView = nullptr;
    QPushButton* mSave = nullptr;
    QPushButton* mStage = nullptr;
    QPushButton* mRevert = nullptr;
    ButtonLink* mFileNameLabel = nullptr;
    QFrame* mTitleFrame = nullptr;
    FileDiffView* mUnifiedFile = nullptr;
    FileDiffView* mNewFile = nullptr;
    QLineEdit* mSearchOld = nullptr;
    FileDiffView* mOldFile = nullptr;
    QVector<int> mModifications;
    DiffInfo mChunks;
    int mCurrentChunkLine = 0;
    FileEditor* mFileEditor = nullptr;
    QVector<HunkWidget*> mHunks;
    QVBoxLayout* mHunksLayout = nullptr;
    QFrame* mHunksFrame = nullptr;
    QSpacerItem* mHunkSpacer = nullptr;
    QStackedWidget* mViewStackedWidget = nullptr;

    bool configure(const QString& file, bool isCached, QString currentSha = QString(), QString previousSha = QString());

    void setSplitViewEnabled(bool enable);

    void setFullViewEnabled(bool enable);

    void setHunksViewEnabled(bool enable);

    void hideBackButton() const;

    void moveChunkUp();
    void moveChunkDown();

    void enterEditionMode(bool enter);

    void endEditFile();
    void stageFile();
    void revertFile();

    void processHunks(const QString& file);

    void createAndAddHunk(const QString& file, const QString& header, const QString& hunk);

    void deleteHunkView();
};
