#include <commit-widgets/CommitInfoPanel.h>

#include <cache/Commit.h>
#include <custom-widgets/ButtonLink.h>

#include <QApplication>
#include <QClipboard>
#include <QLabel>
#include <QScrollArea>
#include <QToolTip>
#include <QVBoxLayout>

CommitInfoPanel::CommitInfoPanel(QWidget *parent)
   : QFrame(parent)
   , mLabelSha(new ButtonLink())
   , mLabelTitle(new QLabel())
   , mLabelDescription(new QLabel())
   , mLabelAuthor(new QLabel())
   , mLabelDateTime(new QLabel())
{
   mLabelSha->setObjectName("labelSha");
   mLabelSha->setAlignment(Qt::AlignCenter);
   mLabelSha->setWordWrap(true);

   QFont font1;
   font1.setBold(true);
   font1.setWeight(QFont::Bold);
   mLabelTitle->setFont(font1);
   mLabelTitle->setAlignment(Qt::AlignCenter);
   mLabelTitle->setWordWrap(true);
   mLabelTitle->setObjectName("labelTitle");

   mLabelDescription->setWordWrap(true);
   mLabelDescription->setObjectName("labelDescription");

   mScrollArea = new QScrollArea();
   mScrollArea->setWidget(mLabelDescription);
   mScrollArea->setWidgetResizable(true);
   mScrollArea->setFixedHeight(50);

   mLabelAuthor->setObjectName("labelAuthor");

   mLabelDateTime->setObjectName("labelDateTime");

   const auto wipSeparator = new QFrame();
   wipSeparator->setObjectName("separator");

   const auto descriptionLayout = new QVBoxLayout(this);
   descriptionLayout->setContentsMargins(0, 0, 0, 0);
   descriptionLayout->setSpacing(0);
   descriptionLayout->addWidget(mLabelSha);
   descriptionLayout->addWidget(mLabelTitle);
   descriptionLayout->addWidget(mScrollArea);
   descriptionLayout->addWidget(wipSeparator);
   descriptionLayout->addWidget(mLabelAuthor);
   descriptionLayout->addWidget(mLabelDateTime);

   connect(mLabelSha, &ButtonLink::clicked, this, [this]() {
      QApplication::clipboard()->setText(mLabelSha->data().toString());
      auto pal = qApp->palette();
      auto textColor = pal.color(QPalette::Text);
      auto backgroundColor = pal.color(QPalette::Base);
      QToolTip::showText(QCursor::pos(), tr("<div style='color: %1; background-color: %2'>Copied!</div>").arg(textColor.name(), backgroundColor.name()), mLabelSha);
   });
}

void CommitInfoPanel::configure(const Commit &commit)
{
   mLabelSha->setText(commit.sha.left(8));
   mLabelSha->setData(commit.sha);
   mLabelSha->setToolTip("Click to save");

   mLabelTitle->setText(commit.shortLog);
   mLabelAuthor->setText(commit.committer.split("<").first());

   mLabelDateTime->setText(QDateTime::fromSecsSinceEpoch(commit.dateSinceEpoch.count()).toString("dd/MM/yyyy hh:mm"));

   if (const auto description = commit.longLog;
       description.isEmpty())
   {
      mScrollArea->setVisible(false);
   }
   else
   {
      QFontMetrics fm(mLabelDescription->font());
      const auto neededsize = fm.boundingRect(QRect(0, 0, 300, 250), Qt::TextWordWrap, description);
      auto height = neededsize.height();

      if (height > 250)
         height = 250;
      else if (height < 50)
         height = 50;

      mScrollArea->setFixedHeight(height);

      auto f = mLabelDescription->font();
      f.setItalic(description.isEmpty());
      mLabelDescription->setFont(f);
      mLabelDescription->setText(description);
      mScrollArea->setVisible(true);
   }
}

void CommitInfoPanel::clear()
{
   mLabelSha->clear();
   mLabelTitle->clear();
   mLabelAuthor->clear();
   mLabelDateTime->clear();
   mLabelDescription->clear();
}
