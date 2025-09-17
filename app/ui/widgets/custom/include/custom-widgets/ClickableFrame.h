#pragma once

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
   explicit ClickableFrame(QWidget *parent = nullptr);
   explicit ClickableFrame(const QString &title, QWidget *parent = nullptr);

   void setTitle(const QString &title);
   QString title() const;
   void setCount(int count);
   void setCount(const QString &count);
   void updateTitleWithCount();

   void setContentWidget(QWidget *content);
   QWidget *contentWidget() const { return mContentWidget; }

   void setExpandable(bool expandable);
   bool isExpandable() const { return mIsExpandable; }
   void setExpanded(bool expanded);
   bool isExpanded() const { return mIsExpanded; }

   void setLinkStyle() { mHasLinkStyles = true; }
   void showExpandArrow(bool show);

protected:
   void mousePressEvent(QMouseEvent *e) override;
   void mouseReleaseEvent(QMouseEvent *e) override;
   void enterEvent(QEnterEvent *event) override;
   void leaveEvent(QEvent *event) override;

private:
   QWidget *mHeaderWidget = nullptr;
   QLabel *mTitleLabel = nullptr;
   QLabel *mCountLabel = nullptr;
   QLabel *mArrowLabel = nullptr;
   QHBoxLayout *mHeaderLayout = nullptr;

   QWidget *mContentWidget = nullptr;
   QVBoxLayout *mMainLayout = nullptr;

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
