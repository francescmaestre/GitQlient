#include <FileDiffEditor.h>

#include <LineNumberArea.h>
#include <core/system/Colors.h>
#include <core/system/GitQlientSettings.h>
#include <core/system/GitQlientStyles.h>

FileDiffEditor::FileDiffEditor(QColor additionColor, QColor removalColor, QColor commentColor, QWidget *parent)
   : FileDiffView(additionColor, removalColor, commentColor, parent)
{
   setReadOnly(false);

   const auto textColor = QSettings().value("colorSchema", 0).toInt() == 1 ? textColorBright : textColorDark;

   addNumberArea(new LineNumberArea(this, textColor));

   connect(this, &FileDiffView::cursorPositionChanged, this, &FileDiffEditor::highlightCurrentLine);

   highlightCurrentLine();
}

void FileDiffEditor::highlightCurrentLine()
{
   QList<QTextEdit::ExtraSelection> extraSelections;

   if (!isReadOnly())
   {
      prevSelection.format.setBackground(QBrush("transparent"));
      prevSelection.format.setProperty(QTextFormat::FullWidthSelection, true);
      prevSelection.cursor.clearSelection();

      QTextEdit::ExtraSelection selection;

      selection.format.setBackground(GitQlientStyles::getGraphSelectionColor());
      selection.format.setProperty(QTextFormat::FullWidthSelection, true);
      selection.cursor = textCursor();
      selection.cursor.clearSelection();

      extraSelections.append(prevSelection);
      extraSelections.append(selection);

      prevSelection = selection;
   }

   setExtraSelections(extraSelections);

   viewport()->update();
}
