#pragma once

#include <custom-widgets/DiffInfo.h>

#include <QPlainTextEdit>

class FileDiffHighlighter;

class LineNumberArea;

class FileDiffView : public QPlainTextEdit
{
    Q_OBJECT

signals:
    void signalScrollChanged(int value);

public:
    explicit FileDiffView(QColor additionColor, QColor removalColor, QColor commentColor, QWidget* parent = nullptr);
    ~FileDiffView();

    void addNumberArea(LineNumberArea* numberArea);
    void loadDiff(
        const QString& text,
        const QVector<ChunkDiffInfo::ChunkInfo>& fileDiffInfo = QVector<ChunkDiffInfo::ChunkInfo>());
    void moveScrollBarToPos(int value);
    void setStartingLine(int lineNumber) { mStartingLine = lineNumber; }
    int getHeight() const;
    int getLineHeigth() const;
    QSize sizeHint() const override;

protected:
    void resizeEvent(QResizeEvent* event) override;
    bool eventFilter(QObject* target, QEvent* event) override;

private:
    void updateLineNumberAreaWidth(int newBlockCount);
    void updateLineNumberArea(const QRect& rect, int dy);
    int lineNumberAreaWidth();

    QVector<ChunkDiffInfo::ChunkInfo> mFileDiffInfo;
    LineNumberArea* mLineNumberArea = nullptr;
    FileDiffHighlighter* mDiffHighlighter = nullptr;
    int mStartingLine = 0;
    int mRow = -1;

    friend class LineNumberArea;
};
