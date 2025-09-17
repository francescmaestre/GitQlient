#include <custom-widgets/FileDiffView.h>
#include <custom-widgets/LineNumberArea.h>

#include <QSettings>

#include <QIcon>
#include <QPainter>
#include <QTextBlock>

LineNumberArea::LineNumberArea(FileDiffView *editor, QColor textColor, bool allowComments, QColor commentColor, bool skipDeletions)
   : QWidget(editor)
   , mTextColor(textColor)
   , mCommentsAllowed(allowComments)
   , mCommentsColor(commentColor)
   , mSkipDeletions(skipDeletions)
{
   fileDiffWidget = editor;
   setMouseTracking(true);
}

int LineNumberArea::widthInDigitsSize()
{
   return 3;
}

QSize LineNumberArea::sizeHint() const
{
   return { fileDiffWidget->lineNumberAreaWidth(), 0 };
}

void LineNumberArea::setEditor(FileDiffView *editor)
{
   fileDiffWidget = editor;
   setParent(editor);
}

void LineNumberArea::paintEvent(QPaintEvent *event)
{
   QPainter painter(this);

   const auto fontWidth = fileDiffWidget->fontMetrics().horizontalAdvance(QLatin1Char(' '));
   const auto offset = fontWidth * (mCommentsAllowed ? 4 : 1);
   auto block = fileDiffWidget->firstVisibleBlock();
   auto blockNumber = block.blockNumber() + fileDiffWidget->mStartingLine + 1;
   auto top = fileDiffWidget->blockBoundingGeometry(block).translated(fileDiffWidget->contentOffset()).top();
   auto bottom = top + fileDiffWidget->blockBoundingRect(block).height();
   auto lineCorrection = 0;

   while (block.isValid() && top <= event->rect().bottom())
   {
      if (block.isVisible() && bottom >= event->rect().top())
      {
         painter.setPen(mTextColor);

         const auto number = blockNumber + lineCorrection;
         const auto height = fileDiffWidget->fontMetrics().height();

         if (mCommentsAllowed)
         {
            if (mBookmarks.contains(number))
            {
               painter.drawPixmap(6, static_cast<int>(top), height, height,
                                  QIcon(":/icons/comments").pixmap(height, height));

               painter.setPen(mCommentsColor);
            }
            else if (fileDiffWidget->mRow == number)
            {
               painter.drawPixmap(width() - height - fontWidth, static_cast<int>(top), height, height,
                                  QIcon(":/icons/add_comment").pixmap(height, height));

               painter.setPen(mCommentsAllowed);
            }
         }

         auto paintNumber = true;
         if (mSkipDeletions)
         {
            const auto skipDeletion
                = std::find_if(fileDiffWidget->mFileDiffInfo.cbegin(), fileDiffWidget->mFileDiffInfo.cend(),
                               [blockNumber](const ChunkDiffInfo::ChunkInfo &hunk) {
                                  return !hunk.addition && hunk.startLine <= blockNumber && blockNumber <= hunk.endLine;
                               })
                != fileDiffWidget->mFileDiffInfo.cend();

            if (skipDeletion)
            {
               paintNumber = false;
               --lineCorrection;
            }
         }

         if (paintNumber)
            painter.drawText(0, static_cast<int>(top), width() - offset, height, Qt::AlignRight,
                             QString::number(number));
      }

      block = block.next();
      top = bottom;
      bottom = top + fileDiffWidget->blockBoundingRect(block).height();
      ++blockNumber;
   }
}

void LineNumberArea::mouseMoveEvent(QMouseEvent *e)
{
   if (mCommentsAllowed)
   {
      if (rect().contains(e->pos()))
      {
         const auto height = width();
         const auto helpPos = mapFromGlobal(QCursor::pos());
         const auto x = helpPos.x();
         if (x >= 0 && x <= height)
         {
            QTextCursor cursor = fileDiffWidget->cursorForPosition(helpPos);
            const auto textRow = cursor.block().blockNumber();
            auto found = false;

            for (const auto &diff : std::as_const(fileDiffWidget->mFileDiffInfo))
            {
               if (textRow + 1 >= diff.startLine && textRow + 1 <= diff.endLine)
               {
                  fileDiffWidget->mRow = textRow + fileDiffWidget->mStartingLine + 1;
                  found = true;
                  break;
               }
            }

            if (!found)
               fileDiffWidget->mRow = -1;

            repaint();
         }
      }
      else
      {
         fileDiffWidget->mRow = -1;
         repaint();
      }
   }
}

void LineNumberArea::mousePressEvent(QMouseEvent *e)
{
   if (mCommentsAllowed)
      mPressed = rect().contains(e->pos());
}

void LineNumberArea::mouseReleaseEvent(QMouseEvent *e)
{
   if (mCommentsAllowed && mPressed && rect().contains(e->pos()))
   {
      const auto height = width();
      const auto helpPos = mapFromGlobal(QCursor::pos());
      const auto x = helpPos.x();
      if (x >= 0 && x <= height)
      {
         const auto cursor = fileDiffWidget->cursorForPosition(helpPos);
         const auto position = cursor.block().blockNumber() + 1;
         const auto row = position + fileDiffWidget->mStartingLine;
         const auto linkId = mBookmarks.value(row, -1);

         if (linkId == -1)
            emit addComment(row);
         else
            emit gotoReview(linkId);
      }
   }

   mPressed = false;
}
