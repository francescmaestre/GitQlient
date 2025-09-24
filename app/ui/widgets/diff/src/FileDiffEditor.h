#pragma once

#include <custom-widgets/FileDiffView.h>

class FileDiffEditor : public FileDiffView
{
public:
    explicit FileDiffEditor(QColor additionColor, QColor removalColor, QColor commentColor, QWidget* parent = nullptr);

private:
    QTextEdit::ExtraSelection prevSelection;

    void highlightCurrentLine();
};
