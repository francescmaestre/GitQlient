#pragma once

#include <QFrame>

class FileDiffEditor;
class Highlighter;

class FileEditor : public QFrame
{
    Q_OBJECT

signals:
    void signalEditionClosed();

public:
    explicit FileEditor(bool highlighter = true, QWidget* parent = nullptr);
    ~FileEditor();

    void editFile(const QString& fileName);
    void finishEdition();
    void saveFile();
    void changeFontSize();

private:
    FileDiffEditor* mFileEditor = nullptr;
    Highlighter* mHighlighter = nullptr;
    QString mFileName;
    QString mLoadedContent;
    bool isEditing = false;

    void saveTextInFile(const QString& content) const;
};
