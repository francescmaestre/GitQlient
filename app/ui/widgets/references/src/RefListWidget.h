#pragma once

#include <QWidget>

class ClickableFrame;
class QListWidget;
class GitCache;
class GitBase;
class RefListDelegate;

class RefListWidget : public QWidget
{
    Q_OBJECT

signals:
    void itemSelected(const QString& data);
    void itemDoubleClicked(const QString& data);
    void contextMenuRequested(const QPoint& pos, const QString& data);

public:
    explicit RefListWidget(
        const QString& title,
        const QString& settingsKey,
        const QSharedPointer<GitCache>& cache,
        const QSharedPointer<GitBase>& git,
        QWidget* parent = nullptr);

    void setCount(int count);
    void addItem(const QString& text, const QString& data = QString());
    void clear();
    void reloadVisibility();
    QListWidget* listWidget() const { return mList; }

private:
    ClickableFrame* mFrame = nullptr;
    QListWidget* mList = nullptr;
    QString mSettingsKey;
    RefListDelegate* mDelegate = nullptr;

    void saveExpansionState(bool expanded);
};
