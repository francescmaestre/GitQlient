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

#include <QWidget>

class ClickableFrame;
class QListWidget;
class SacredTimeline;
class GitBase;
class RefListDelegate;

class RefListWidget : public QWidget
{
    Q_OBJECT

signals:
    void itemSelected(const QString& data);
    void itemDoubleClicked(const QString& data);
    void contextMenuRequested(const QPoint& pos, const QString& data);

public:
    explicit RefListWidget(
        const QString& title,
        const QString& settingsKey,
        const QSharedPointer<SacredTimeline>& cache,
        const QSharedPointer<GitBase>& git,
        QWidget* parent = nullptr);

    void setCount(int count);
    void addItem(const QString& text, const QString& data = QString());
    void clear();
    void reloadVisibility();
    QListWidget* listWidget() const { return mList; }

private:
    ClickableFrame* mFrame = nullptr;
    QListWidget* mList = nullptr;
    QString mSettingsKey;
    RefListDelegate* mDelegate = nullptr;

    void saveExpansionState(bool expanded);
};
