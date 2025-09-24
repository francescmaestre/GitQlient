#pragma once

#include <QDialog>

class GitConfig;

namespace Ui
{
    class CreateRepoDlg;
}

enum class CreateRepoDlgType
{
    CLONE,
    INIT
};

class CreateRepoDlg : public QDialog
{
    Q_OBJECT

signals:
    void signalOpenWhenFinish(const QString& path);

public:
    explicit CreateRepoDlg(CreateRepoDlgType type, QSharedPointer<GitConfig> git, QWidget* parent = nullptr);
    ~CreateRepoDlg() override;

    void accept() override;
    QPair<QString, QString> getCloneInfo() const { return qMakePair(mCloneUrl, mClonePath); }

private:
    Ui::CreateRepoDlg* ui;
    CreateRepoDlgType mType;
    QSharedPointer<GitConfig> mGit;
    QString mCloneUrl;
    QString mClonePath;

    void selectFolder();
    void addDefaultName(const QString& url);
    void showGitControls();
    void verifyDefaultFolder();
    void saveConfigAndAccept(const QString& fullPath);
};
