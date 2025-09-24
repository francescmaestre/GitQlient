#pragma once

#include <QWidget>

#include <QMap>

class FileDiffView;

class LineNumberArea : public QWidget
{
    Q_OBJECT

signals:
    void gotoReview(int linkId);
    void addComment(int row);

public:
    using BookmarkLine = int;
    using LinkId = int;

    LineNumberArea(
        FileDiffView* editor,
        QColor textColor,
        bool allowComments = false,
        QColor commentColor = QColor(),
        bool skipDeletions = false);

    int widthInDigitsSize();
    QSize sizeHint() const override;
    void setEditor(FileDiffView* editor);
    bool commentsAllowed() const { return mCommentsAllowed; }
    void setCommentBookmarks(const QMap<BookmarkLine, LinkId>& bookmarks) { mBookmarks = bookmarks; }

protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;

private:
    FileDiffView* fileDiffWidget;
    QColor mTextColor;
    bool mPressed = false;
    bool mCommentsAllowed = false;
    QColor mCommentsColor;
    bool mSkipDeletions = false;
    QMap<BookmarkLine, LinkId> mBookmarks;
};
