#include <system/ProjectListManager.h>
#include <system/SettingsKeys.h>

#include <QSettings>
#include <QVector>

using namespace System;

void ProjectListManager::setProjectOpened(const QString& projectPath)
{
    saveMostUsedProjects(projectPath);

    saveRecentProjects(projectPath);
}

QStringList ProjectListManager::getRecentProjects()
{
    auto projects = QSettings().value(GlobalKey::RecentProjects, QStringList()).toStringList();

    QStringList recentProjects;
    const auto end = std::min(static_cast<int>(projects.count()), 5);

    for (auto i = 0; i < end; ++i)
        recentProjects.append(projects.takeFirst());

    return recentProjects;
}

void ProjectListManager::saveRecentProjects(const QString& projectPath)
{
    QSettings settings;
    auto usedProjects = settings.value(GlobalKey::RecentProjects, QStringList()).toStringList();

    if (usedProjects.contains(projectPath))
    {
        const auto index = usedProjects.indexOf(projectPath);
        usedProjects.takeAt(index);
    }

    usedProjects.prepend(projectPath);

    while (!usedProjects.isEmpty() && usedProjects.count() > 5)
        usedProjects.removeLast();

    settings.setValue(GlobalKey::RecentProjects, usedProjects);
}

void ProjectListManager::clearRecentProjects()
{
    QSettings settings;
    settings.remove(GlobalKey::RecentProjects);
}

void ProjectListManager::saveMostUsedProjects(const QString& projectPath)
{
    QSettings settings;
    auto projects = settings.value(GlobalKey::UsedProjects, QStringList()).toStringList();
    auto timesUsed = settings.value(GlobalKey::UsedProjectsCount, QList<QVariant>()).toList();

    if (projects.contains(projectPath))
    {
        const auto index = projects.indexOf(projectPath);
        timesUsed[index] = QString::number(timesUsed[index].toInt() + 1);
    }
    else
    {
        projects.append(projectPath);
        timesUsed.append(1);
    }

    settings.setValue(GlobalKey::UsedProjects, projects);
    settings.setValue(GlobalKey::UsedProjectsCount, timesUsed);
}

void ProjectListManager::clearMostUsedProjects()
{
    QSettings settings;
    settings.remove(GlobalKey::UsedProjects);
    settings.remove(GlobalKey::UsedProjectsCount);
}

QStringList ProjectListManager::getMostUsedProjects()
{
    QSettings settings;
    const auto projects = settings.value(GlobalKey::UsedProjects, QStringList()).toStringList();
    const auto timesUsed = settings.value(GlobalKey::UsedProjectsCount, QString()).toList();

    QMultiMap<int, QString> projectOrderedByUse;

    const auto projectsCount = projects.count();
    const auto timesCount = timesUsed.count();

    for (auto i = 0; i < projectsCount && i < timesCount; ++i)
        projectOrderedByUse.insert(timesUsed.at(i).toInt(), projects.at(i));

    QStringList recentProjects;
    const auto end = std::min(static_cast<int>(projectOrderedByUse.count()), 5);
    const auto orderedProjects = projectOrderedByUse.values();

    for (auto i = 0; i < end; ++i)
        recentProjects.append(orderedProjects.at(orderedProjects.count() - 1 - i));

    return recentProjects;
}
