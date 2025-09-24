#pragma once

#include <QFrame>

class Commit;
class QLabel;
class QScrollArea;
class ButtonLink;

class CommitInfoPanel : public QFrame
{
    Q_OBJECT

public:
    explicit CommitInfoPanel(QWidget* parent = nullptr);

    void configure(const Commit& commit);
    void clear();

private:
    ButtonLink* mLabelSha = nullptr;
    QLabel* mLabelTitle = nullptr;
    QLabel* mLabelDescription = nullptr;
    QScrollArea* mScrollArea = nullptr;
    QLabel* mLabelAuthor = nullptr;
    QLabel* mLabelDateTime = nullptr;
};
