#pragma once

#include <QStringList>
#include <QUuid>
#include <QVector>

struct ChunkDiffInfo
{
    struct ChunkInfo
    {
        ChunkInfo() = default;
        ChunkInfo(const QString& _id)
            : id(_id)
        {
        }
        int startLine = -1;
        int endLine = -1;
        bool addition = false;
        QString id;
        bool isValid() const { return startLine != -1 && endLine != -1; }
    };

    ChunkDiffInfo()
        : newFile(ChunkInfo(id))
        , oldFile(ChunkInfo(id))
    {
    }
    ChunkDiffInfo(bool baseOld, const ChunkInfo& _newFile, const ChunkInfo& _oldFile)
        : baseIsOldFile(baseOld)
        , newFile(_newFile)
        , oldFile(_oldFile)
    {
        newFile.id = id;
        oldFile.id = id;
    }
    bool operator==(const ChunkDiffInfo& info) const { return id == info.id; }
    bool isValid() const { return newFile.isValid() || oldFile.isValid(); }

    QString id = QUuid::createUuid().toString();
    bool baseIsOldFile = true;
    ChunkInfo newFile;
    ChunkInfo oldFile;
};

struct DiffInfo
{
    QStringList fullDiff;
    QStringList newFileDiff;
    QStringList oldFileDiff;
    QVector<ChunkDiffInfo> chunks;
};
