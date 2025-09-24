#pragma once

#include <QSyntaxHighlighter>
#include <custom-widgets/DiffInfo.h>

class FileDiffHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    explicit FileDiffHighlighter(
        QColor additionColor, QColor removalColor, QColor commentColor, QTextDocument* document);

    void highlightBlock(const QString& text) override;
    void setDiffInfo(const QVector<ChunkDiffInfo::ChunkInfo>& fileDiffInfo) { mFileDiffInfo = fileDiffInfo; }

private:
    QVector<ChunkDiffInfo::ChunkInfo> mFileDiffInfo;
    QColor mShadowGreen;
    QColor mShadowRed;
    QColor mCommentColor;
};
