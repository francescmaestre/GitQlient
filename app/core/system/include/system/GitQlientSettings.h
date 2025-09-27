#pragma once

#include <QSettings>
#include <QVector>

class GitQlientSettings
{
public:
    GitQlientSettings() = default;
    GitQlientSettings(const QString& gitRepoPath);
    ~GitQlientSettings() { }
    void setGlobalValue(const QString& key, const QVariant& value);
    QVariant globalValue(const QString& key, const QVariant& defaultValue = QVariant()) const;
    QVariant localValue(const QString& key, const QVariant& defaultValue = QVariant()) const;
    void setProjectOpened(const QString& projectPath);
    QStringList getRecentProjects() const;
    void saveRecentProjects(const QString& projectPath);
    void clearRecentProjects();
    void saveMostUsedProjects(const QString& projectPath);
    void clearMostUsedProjects();
    QStringList getMostUsedProjects() const;
    static QString PinnedRepos;
    static QString SplitFileDiffView;

private:
    QSettings globalSettings;
    QString mGitRepoPath;
};
