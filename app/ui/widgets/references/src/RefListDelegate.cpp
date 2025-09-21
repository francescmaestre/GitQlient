#include "RefListDelegate.h"

#include <system/GitQlientStyles.h>

#include <QAbstractItemView>
#include <QHelpEvent>
#include <QPainter>
#include <QToolTip>

constexpr auto DefaultHeight = 30.0;

RefListDelegate::RefListDelegate(QObject *parent)
   : QStyledItemDelegate(parent)
{
}

void RefListDelegate::paint(QPainter *p, const QStyleOptionViewItem &o, const QModelIndex &i) const
{
   p->setRenderHints(QPainter::Antialiasing);

   QStyleOptionViewItem newOpt(o);

   if (o.state & QStyle::State_Selected)
   {
      auto color = o.palette.color(QPalette::Highlight);
      p->fillRect(o.rect, color);
   }
   else if (o.state & QStyle::State_MouseOver)
   {
      auto color = o.palette.color(QPalette::AlternateBase);
      p->fillRect(o.rect, color);
   }
   else
   {
      auto background = o.palette.color(QPalette::Base);
      p->fillRect(o.rect, background);
   }

   p->setPen(o.palette.color(QPalette::Text));
   p->setFont(o.font);
   auto rect = o.rect;
   rect.setX(rect.x() + 5);
   p->drawText(rect, i.data().toString(), QTextOption(Qt::AlignLeft | Qt::AlignVCenter));
}

QSize RefListDelegate::sizeHint(const QStyleOptionViewItem &, const QModelIndex &) const
{
   return QSize(0, DefaultHeight);
}

bool RefListDelegate::helpEvent(QHelpEvent *event, QAbstractItemView *view, const QStyleOptionViewItem &option, const QModelIndex &index)
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
