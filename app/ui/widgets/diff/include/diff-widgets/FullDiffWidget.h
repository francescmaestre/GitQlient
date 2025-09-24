#pragma once

#include <diff-widgets/IDiffWidget.h>

#include <QSyntaxHighlighter>

class QPlainTextEdit;
class QPushButton;

class FullDiffWidget : public IDiffWidget
{
    Q_OBJECT

public:
    explicit FullDiffWidget(
        const QSharedPointer<GitBase>& git, QSharedPointer<GitCache> cache, QWidget* parent = nullptr);

    bool reload() override;
    void loadDiff(const QString& sha, const QString& diffToSha, const QString& diffData);
    void updateFontSize() override;

private:
    QPushButton* mGoPrevious = nullptr;
    QPushButton* mGoNext = nullptr;
    QString mPreviousDiffText;
    QPlainTextEdit* mDiffWidget = nullptr;
    QVector<int> mFilePositions;

    class DiffHighlighter : public QSyntaxHighlighter
    {
    public:
        DiffHighlighter(QTextDocument* document);

        void highlightBlock(const QString& text) override;
    };

    DiffHighlighter* diffHighlighter = nullptr;

    void processData(const QString& fileChunk);
    void moveChunkUp();
    void moveChunkDown();
};
