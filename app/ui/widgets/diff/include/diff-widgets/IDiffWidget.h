#pragma once

#include <QFrame>

class GitBase;
class GitCache;

class IDiffWidget : public QFrame
{
    Q_OBJECT
signals:

public:
    explicit IDiffWidget(const QSharedPointer<GitBase>& git, QSharedPointer<GitCache> cache, QWidget* parent = nullptr);

    virtual bool reload() = 0;
    virtual void updateFontSize() = 0;
    QString getCurrentSha() const { return mCurrentSha; }
    QString getPreviousSha() const { return mPreviousSha; }

protected:
    QSharedPointer<GitBase> mGit;
    QSharedPointer<GitCache> mCache;
    QString mCurrentSha;
    QString mPreviousSha;
};
