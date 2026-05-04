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

#include <QFrame>

class QLabel;
class QWidget;
class QVBoxLayout;
class QHBoxLayout;

class ClickableFrame : public QFrame
{
    Q_OBJECT

signals:
    void clicked();
    void expanded(bool isExpanded);

public:
    explicit ClickableFrame(QWidget* parent = nullptr);
    explicit ClickableFrame(const QString& title, QWidget* parent = nullptr);

    void setTitle(const QString& title);
    QString title() const;
    void setCount(int count);
    void setCount(const QString& count);
    void updateTitleWithCount();
    void setContentWidget(QWidget* content);
    QWidget* contentWidget() const { return mContentWidget; }
    void setExpandable(bool expandable);
    bool isExpandable() const { return mIsExpandable; }
    void setExpanded(bool expanded);
    bool isExpanded() const { return mIsExpanded; }
    void setLinkStyle() { mHasLinkStyles = true; }
    void showExpandArrow(bool show);

protected:
    void mousePressEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    QWidget* mHeaderWidget = nullptr;
    QLabel* mTitleLabel = nullptr;
    QLabel* mCountLabel = nullptr;
    QLabel* mArrowLabel = nullptr;
    QHBoxLayout* mHeaderLayout = nullptr;
    QWidget* mContentWidget = nullptr;
    QVBoxLayout* mMainLayout = nullptr;
    QString mTitle;
    QString mCount;
    bool mPressed = false;
    bool mHasLinkStyles = false;
    bool mIsExpandable = false;
    bool mIsExpanded = true;
    bool mShowArrow = true;
    void setupLayout();
    void updateExpandedState();
    void updateArrowIcon();
};
