#include "RefListDelegate.h"
#include "GitQlientBranchItemRole.h"

#include <system/GitQlientStyles.h>

#include <QPainter>

using namespace GitQlient;

constexpr auto DefaultHeight = 30.0;

RefListDelegate::RefListDelegate(bool isTag, QObject *parent)
   : QStyledItemDelegate(parent)
   , mIsTag(isTag)
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
