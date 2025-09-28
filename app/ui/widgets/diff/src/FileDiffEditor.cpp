#include "FileDiffEditor.h"

#include <custom-widgets/LineNumberArea.h>
#include <system/Colors.h>
#include <system/GitQlientStyles.h>

FileDiffEditor::FileDiffEditor(QColor additionColor, QColor removalColor, QColor commentColor, QWidget* parent)
    : FileDiffView(additionColor, removalColor, commentColor, parent)
{
    setReadOnly(false);

    addNumberArea(new LineNumberArea(this, QPalette().color(QPalette::Text)));

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

        selection.format.setBackground(QPalette().color(QPalette::AlternateBase));
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
