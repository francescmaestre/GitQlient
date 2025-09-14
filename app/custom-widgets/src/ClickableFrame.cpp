#include <custom-widgets/ClickableFrame.h>

#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QMouseEvent>
#include <QVBoxLayout>

ClickableFrame::ClickableFrame(QWidget *parent)
   : QFrame(parent)
{
   setupLayout();
}

ClickableFrame::ClickableFrame(const QString &title, QWidget *parent)
   : QFrame(parent)
   , mTitle(title)
{
   setupLayout();
   setTitle(title);
}

void ClickableFrame::setupLayout()
{
   mMainLayout = new QVBoxLayout(this);
   mMainLayout->setContentsMargins(0, 0, 0, 0);
   mMainLayout->setSpacing(0);

   mHeaderWidget = new QWidget();
   mHeaderLayout = new QHBoxLayout(mHeaderWidget);
   mHeaderLayout->setContentsMargins(10, 5, 10, 5);
   mHeaderLayout->setSpacing(5);

   mTitleLabel = new QLabel();
   mCountLabel = new QLabel();
   mArrowLabel = new QLabel();
   mArrowLabel->setFixedSize(16, 16);

   mHeaderLayout->addWidget(mTitleLabel);
   mHeaderLayout->addWidget(mCountLabel);
   mHeaderLayout->addStretch();
   mHeaderLayout->addWidget(mArrowLabel);

   mMainLayout->addWidget(mHeaderWidget);

   setAttribute(Qt::WA_DeleteOnClose);
   setObjectName("ClickableFrame");

   updateArrowIcon();
}

void ClickableFrame::setTitle(const QString &title)
{
   mTitle = title;
   mTitleLabel->setText(QString("<b>%1</b>").arg(title));
}

QString ClickableFrame::title() const
{
   return mTitle;
}

void ClickableFrame::setCount(int count)
{
   mCount = QString::number(count);
   updateTitleWithCount();
}

void ClickableFrame::updateTitleWithCount()
{
   if (mTitleLabel)
   {
      QString displayText = mTitle;
      if (!mCount.isEmpty() && mCount != "0")
      {
         displayText = QString("(%1)")
                           .arg(mCount);
      }
      else if (!mCount.isEmpty())
      {
         displayText = QString("(0)");
      }

      mCountLabel->setText(displayText);
   }
}

void ClickableFrame::setContentWidget(QWidget *content)
{
   if (mContentWidget)
   {
      mMainLayout->removeWidget(mContentWidget);
      mContentWidget->deleteLater();
   }

   mContentWidget = content;
   if (mContentWidget)
   {
      mMainLayout->addWidget(mContentWidget);
      mContentWidget->setVisible(mIsExpanded);
   }
}

void ClickableFrame::setExpandable(bool expandable)
{
   mIsExpandable = expandable;
   setCursor(expandable ? Qt::PointingHandCursor : Qt::ArrowCursor);
   mArrowLabel->setVisible(expandable && mShowArrow);
   updateArrowIcon();
}

void ClickableFrame::showExpandArrow(bool show)
{
   mShowArrow = show;
   mArrowLabel->setVisible(mIsExpandable && show);
}

void ClickableFrame::setExpanded(bool _expanded)
{
   if (mIsExpanded != _expanded)
   {
      mIsExpanded = _expanded;
      updateExpandedState();
      updateArrowIcon();
      emit expanded(mIsExpanded);
   }
}

void ClickableFrame::updateExpandedState()
{
   if (mContentWidget)
   {
      mContentWidget->setVisible(mIsExpanded);
   }
}

void ClickableFrame::updateArrowIcon()
{
   if (mArrowLabel && mIsExpandable && mShowArrow)
   {
      const auto icon = QIcon(mIsExpanded ? ":/icons/remove" : ":/icons/add");
      mArrowLabel->setPixmap(icon.pixmap(QSize(16, 16)));
   }
}

void ClickableFrame::mousePressEvent(QMouseEvent *e)
{
   if (e->button() == Qt::LeftButton)
   {
      if (!mHeaderWidget || mHeaderWidget->geometry().contains(e->pos()))
      {
         mPressed = true;
      }
   }
   QFrame::mousePressEvent(e);
}

void ClickableFrame::mouseReleaseEvent(QMouseEvent *e)
{
   if (mPressed && e->button() == Qt::LeftButton)
   {
      if (!mHeaderWidget || mHeaderWidget->geometry().contains(e->pos()))
      {
         if (mIsExpandable)
         {
            setExpanded(!mIsExpanded);
         }
         emit clicked();
      }
   }
   mPressed = false;
   QFrame::mouseReleaseEvent(e);
}

void ClickableFrame::enterEvent(QEnterEvent *event)
{
   if (mHasLinkStyles && mTitleLabel)
   {
      QFont f = mTitleLabel->font();
      f.setUnderline(true);
      mTitleLabel->setFont(f);
   }
   QFrame::enterEvent(event);
}

void ClickableFrame::leaveEvent(QEvent *event)
{
   if (mHasLinkStyles && mTitleLabel)
   {
      QFont f = mTitleLabel->font();
      f.setUnderline(false);
      mTitleLabel->setFont(f);
   }
   QFrame::leaveEvent(event);
}
