#include "BranchesViewDelegate.h"
#include "GitQlientBranchItemRole.h"

#include <system/GitQlientStyles.h>

#include <QAbstractItemView>
#include <QHelpEvent>
#include <QPainter>
#include <QToolTip>

using namespace GitQlient;

constexpr auto DefaultHeight = 30.0;

BranchesViewDelegate::BranchesViewDelegate(bool isTag, QObject *parent)
   : QStyledItemDelegate(parent)
   , mIsTag(isTag)
{
}

void BranchesViewDelegate::paint(QPainter *p, const QStyleOptionViewItem &o, const QModelIndex &i) const
{
   p->setRenderHints(QPainter::Antialiasing);

   QStyleOptionViewItem newOpt(o);

   if (o.state & QStyle::State_Selected)
   {
      QRect rect(0, o.rect.y(), o.rect.width() + o.rect.x(), o.rect.height());
      auto color = o.palette.color(QPalette::Highlight);
      p->fillRect(rect, color);
   }
   else if (o.state & QStyle::State_MouseOver)
   {
      QRect rect(0, o.rect.y(), o.rect.width() + o.rect.x(), o.rect.height());
      auto color = o.palette.color(QPalette::AlternateBase);
      p->fillRect(rect, color);
   }
   else
   {
      auto background = o.palette.color(QPalette::Base);
      p->fillRect(newOpt.rect, background);
   }

   static const auto iconSize = 20;
   static const auto offset = 5;

   if (i.column() == 0)
   {
      if (i.data(IsLeaf).toBool())
      {
         const auto width = newOpt.rect.x();
         QRect rectIcon(width - offset, newOpt.rect.y(), iconSize, newOpt.rect.height());
         QIcon icon(QString::fromUtf8(mIsTag ? ":/icons/tag_indicator" : ":/icons/repo_indicator"));
         icon.paint(p, rectIcon);
      }
      else
      {
         const auto width = newOpt.rect.x();
         QRect rectIcon(width - offset, newOpt.rect.y(), iconSize, newOpt.rect.height());
         QIcon icon(QString::fromUtf8(":/icons/folder_indicator"));
         icon.paint(p, rectIcon);
      }
   }

   auto foreground = o.palette.color(QPalette::Text);
   p->setPen(foreground);

   newOpt.font.setBold(i.data(Qt::UserRole).toBool());

   if (i.data().toString() == "detached")
      newOpt.font.setItalic(true);

   if (i.column() == 0)
      newOpt.rect.setX(newOpt.rect.x() + iconSize + offset);
   else
      newOpt.rect.setX(newOpt.rect.x() + iconSize - offset);

   p->setFont(newOpt.font);
   p->drawText(newOpt.rect, i.data().toString(), QTextOption(Qt::AlignLeft | Qt::AlignVCenter));
}

QSize BranchesViewDelegate::sizeHint(const QStyleOptionViewItem &, const QModelIndex &) const
{
   return QSize(0, DefaultHeight);
}

bool BranchesViewDelegate::helpEvent(QHelpEvent *event, QAbstractItemView *view, const QStyleOptionViewItem &option, const QModelIndex &index)
{
   if (!event || !view)
      return false;

   if (event->type() == QEvent::ToolTip)
   {
      auto text = index.data(Qt::ToolTipRole).toString();

      if (!text.isEmpty())
      {
         auto pal = view->palette();
         auto backgroundColor = pal.color(QPalette::Base).name();
         auto textColor = pal.color(QPalette::Text).name();

         QToolTip::showText(event->globalPos(), tr("<div style='color: %1; background-color: %2;'>%3</div>").arg(textColor, backgroundColor, text), view);
         return true;
      }
   }
   return QStyledItemDelegate::helpEvent(event, view, option, index);
}
