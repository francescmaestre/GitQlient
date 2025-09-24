#pragma once

#include <QDateTime>
#include <QFrame>

class GitBase;
class QScrollArea;
class ButtonLink;
class QLabel;
class GitCache;

class FileBlameWidget : public QFrame
{
    Q_OBJECT

signals:
    void signalCommitSelected(const QString& sha);

public:
    explicit FileBlameWidget(
        const QSharedPointer<GitCache>& cache, const QSharedPointer<GitBase>& git, QWidget* parent = nullptr);

    void setup(const QString& fileName, const QString& currentSha, const QString& previousSha);
    void reload(const QString& currentSha, const QString& previousSha);
    QString getCurrentSha() const;
    QString getCurrentFile() const { return mCurrentFile; }

private:
    QSharedPointer<GitCache> mCache;
    QSharedPointer<GitBase> mGit;
    QFrame* mAnotation = nullptr;
    QLabel* mCurrentSha = nullptr;
    QLabel* mPreviousSha = nullptr;
    QScrollArea* mScrollArea = nullptr;
    QFont mInfoFont;
    QFont mCodeFont;
    QString mCurrentFile;

    struct Annotation
    {
        QString sha;
        QString author;
        QDateTime dateTime;
        int line = 0;
        QString content;
    };

    QVector<Annotation> processBlame(const QString& blame);
    void formatAnnotatedFile(const QVector<Annotation>& annotations);
    QLabel* createDateLabel(const Annotation& annotation, bool isFirst);
    QLabel* createAuthorLabel(const QString& author, bool isFirst);
    ButtonLink* createMessageLabel(const QString& sha, bool isFirst);
    QLabel* createNumLabel(const Annotation& annotation, int row);
    QLabel* createCodeLabel(const QString& content);
};
