#pragma once

#include <QMap>
#include <QStringList>

class References
{
public:
    enum class Type
    {
        LocalTag,
        RemoteTag,
        LocalBranch,
        RemoteBranche,
    };

    void addReference(Type type, const QString& value);

    QStringList getReferences(Type type) const;

    int removeReference(Type type, const QString& value);

    bool isEmpty() const { return mReferences.isEmpty(); }

    void clear() { mReferences.clear(); }

private:
    QMap<Type, QStringList> mReferences;
};
