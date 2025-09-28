#pragma once

#include <QVector>

class ProjectListManager
{
public:
    ProjectListManager() = delete;
    static void setProjectOpened(const QString& projectPath);
    static QStringList getRecentProjects();
    static void saveRecentProjects(const QString& projectPath);
    static void clearRecentProjects();
    static void saveMostUsedProjects(const QString& projectPath);
    static void clearMostUsedProjects();
    static QStringList getMostUsedProjects();

private:
};
