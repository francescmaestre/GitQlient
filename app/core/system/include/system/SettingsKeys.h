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

namespace System
{
    namespace GlobalKey
    {
        [[maybe_unused]] static const char* SettingsVersion = "SettingsVersion";
        [[maybe_unused]] static const char* PinnedRepos = "Config/PinnedRepos";
        [[maybe_unused]] static const char* GitQlientGeometry = "GitQlientGeometry";
        [[maybe_unused]] static const char* LogsLevel = "logsLevel";
        [[maybe_unused]] static const char* LogsDisabled = "logsDisabled";
        [[maybe_unused]] static const char* RecentProjects
            = "Config/RecentProjects";
        [[maybe_unused]] static const char* UiLanguage = "UILanguage";
        [[maybe_unused]] static const char* UsedProjects = "Config/UsedProjects";
        [[maybe_unused]] static const char* UsedProjectsCount = "Config/UsedProjectsCount";
        [[maybe_unused]] static const char* LogsFolder = "logsFolder";
        [[maybe_unused]] static const char* ShowFeaturesDlg = "ShowFeaturesDlg";
        [[maybe_unused]] static const char* ShowInitConfigDialog = "ShowInitConfigDialog";
        [[maybe_unused]] static const char* MaxCommits = "MaxCommits";
        [[maybe_unused]] static const char* GitLocation = "gitLocation";
        [[maybe_unused]] static const char* CommitTitleMaxLength = "commitTitleMaxLength";
        [[maybe_unused]] static const char* UiBaseFontSize = "UiBaseFontSize";
        [[maybe_unused]] static const char* DefaultDiffView = "DefaultDiffView";
        [[maybe_unused]] static const char* BranchSeparator = "BranchSeparator";
        [[maybe_unused]] static const char* ExternalEditor = "ExternalEditor";
        [[maybe_unused]] static const char* FileExplorer = "FileExplorer";
        [[maybe_unused]] static const char* FileBrowser = "FileBrowser";
        [[maybe_unused]] static const char* AutoRefresh = "AutoRefresh";
        [[maybe_unused]] static const char* ShowAllBranches = "ShowAllBranches";
        [[maybe_unused]] static const char* DevMode = "DevMode";
        [[maybe_unused]] static const char* DefaultCloneLocation = "DefaultCloneLocation";

        namespace Ui
        {

        }

        namespace References
        {
            [[maybe_unused]] static const char* MinimalBranchesView = "MinimalBranchesView";
            [[maybe_unused]] static const char* LocalHeader = "LocalHeader";
            [[maybe_unused]] static const char* RemoteHeader = "RemoteHeader";
            [[maybe_unused]] static const char* TagsHeader = "TagsHeader";
            [[maybe_unused]] static const char* StashesHeader = "StashesHeader";
            [[maybe_unused]] static const char* SubmodulesHeader = "SubmodulesHeader";
            [[maybe_unused]] static const char* SubtreeHeader = "SubtreeHeader";
        } // namespace References

        namespace FileDiffView
        {
            [[maybe_unused]] static const char* FontSize = "FileDiffView/FontSize";
        } // namespace FileDiffView

        namespace History
        {
            [[maybe_unused]] static const char* PreferCommit = "HistoryView/PreferCommit";
            [[maybe_unused]] static const char* FontSize = "HistoryView/FontSize";
        } // namespace History

        namespace Pomodoro
        {
            [[maybe_unused]] static const char* Alarm = "Pomodoro/Alarm";
            [[maybe_unused]] static const char* StopResets = "Pomodoro/StopResets";
            [[maybe_unused]] static const char* Duration = "Pomodoro/Duration";
            [[maybe_unused]] static const char* Break = "Pomodoro/Break";
            [[maybe_unused]] static const char* LongBreak = "Pomodoro/LongBreak";
            [[maybe_unused]] static const char* LongBreakTrigger = "Pomodoro/LongBreakTrigger";
        } // namespace Pomodoro

    } // namespace GlobalKey

    namespace RepoKeys
    {

    }
} // namespace System
