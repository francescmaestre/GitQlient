#include <cache/References.h>

void References::addReference(Type type, const QString& value)
{
    auto& list = mReferences[type];
    if (!list.contains(value))
        list.append(value);
}

QStringList References::getReferences(Type type) const { return mReferences.value(type, QStringList()); }

std::optional<std::reference_wrapper<const QStringList>> References::findReferences(Type type) const
{
    const auto it = mReferences.constFind(type);
    if (it == mReferences.cend())
        return std::nullopt;
    return std::cref(*it);
}

int References::removeReference(References::Type type, const QString& value)
{
    const auto removedItems = mReferences[type].removeAll(value);

    if (mReferences[type].isEmpty())
        mReferences.remove(type);

    return removedItems;
}
