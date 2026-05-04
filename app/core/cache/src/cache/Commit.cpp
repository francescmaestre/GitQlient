#include <cache/Commit.h>

#include <GitExecResult.h>

#include <QByteArrayView>
#include <QHash>
#include <QStringList>

#include <algorithm>
#include <charconv>
#include <cstring>
#include <ranges>

Commit::Commit(QByteArray commitData, const QString& gpg, bool goodSignature)
    : gpgKey(gpg)
    , mGoodSignature(goodSignature)
{
    parseDiff(commitData, 0);
}

Commit::Commit(QByteArray data) { parseDiff(data, 1); }

// Phase 4: per-thread intern table for committer/author strings.
// Repos with few contributors repeat the same "Name <email>" string on thousands of
// commits. Interning converts those from N QString allocations down to 1 per unique
// author, with a fast hash-table lookup on every subsequent hit.
// thread_local means no mutex and no cross-thread contention.
static QString internUtf8(QByteArrayView raw)
{
    static thread_local QHash<QByteArray, QString> sTable;
    const QByteArray key(raw.constData(), raw.size());
    const auto it = sTable.constFind(key);
    if (it != sTable.cend())
        return *it;
    const auto str = QString::fromUtf8(raw);
    sTable.insert(key, str);
    return str;
}

void Commit::parseDiff(QByteArray& data, qsizetype startingField)
{
    if (data.isEmpty())
        return;

    qsizetype lineStart = 0;

    // Phase 3: return a zero-allocation view into `data` instead of a mid() copy.
    // All callers either pass the view directly to QString::from{Latin1,Utf8}()
    // or read raw bytes — no QByteArray is materialised for any field line.
    auto nextLine = [&]() -> QByteArrayView {
        const auto lineEnd = data.indexOf('\n', lineStart);
        const auto end = lineEnd == -1 ? data.size() : lineEnd;
        QByteArrayView line(data.constData() + lineStart, end - lineStart);
        lineStart = end + 1;
        return line;
    };

    for (qsizetype i = 0; i < startingField; ++i)
        nextLine();

    // SHA field: "%m%HX%P" — merge marker (1 char) + 40-char hex SHA + literal 'X' + parent SHAs.
    {
        const auto shaLine = nextLine();
        sha = QString::fromLatin1(shaLine.sliced(1, 40));

        if (shaLine.size() > 42)
        {
            const auto parentView = shaLine.sliced(42);
            const char* pPos = parentView.constData();
            const char* const pEnd = pPos + parentView.size();
            while (pPos < pEnd)
            {
                const char* const sp
                    = static_cast<const char*>(std::memchr(pPos, ' ', static_cast<size_t>(pEnd - pPos)));
                const char* const tokenEnd = sp ? sp : pEnd;
                if (tokenEnd > pPos)
                    mParentsSha.append(QString::fromLatin1(pPos, static_cast<qsizetype>(tokenEnd - pPos)));
                pPos = sp ? sp + 1 : pEnd;
            }
        }
    }

    committer = internUtf8(nextLine()); // Phase 4
    author = internUtf8(nextLine());    // Phase 4

    // Timestamp: parse ASCII digits directly — no QByteArray or QString allocation.
    {
        const auto tsView = nextLine();
        long long ts = 0;
        std::from_chars(tsView.constData(), tsView.constData() + tsView.size(), ts);
        dateSinceEpoch = std::chrono::seconds(ts);
    }

    shortLog = QString::fromUtf8(nextLine()).trimmed();
}

Commit::Commit(const QString& sha, const QStringList& parents, std::chrono::seconds commitDate, const QString& log)
    : sha(sha)
    , dateSinceEpoch(commitDate)
    , shortLog(log)
    , mParentsSha(parents)
{
}

bool Commit::operator==(const Commit& commit) const
{
    return sha.startsWith(commit.sha) && mParentsSha == commit.mParentsSha && committer == commit.committer
        && author == commit.author && dateSinceEpoch == commit.dateSinceEpoch && shortLog == commit.shortLog
        && longLog == commit.longLog;
}

bool Commit::contains(const QString& value) const
{
    return sha.startsWith(value, Qt::CaseInsensitive) || shortLog.contains(value, Qt::CaseInsensitive)
        || committer.contains(value, Qt::CaseInsensitive) || author.contains(value, Qt::CaseInsensitive);
}

int Commit::parentsCount() const
{
    auto count = mParentsSha.count();

    if (count > 0 && mParentsSha.contains(ZERO_SHA))
        --count;

    return count;
}

QString Commit::firstParent() const { return !mParentsSha.isEmpty() ? mParentsSha.at(0) : QString(); }

QStringList Commit::parents() const { return mParentsSha; }

void Commit::setParents(const QStringList& parents) { mParentsSha = parents; }

bool Commit::isInWorkingBranch() const
{
    return std::ranges::any_of(mChilds, [](const Commit* child) {
        return child->sha == ZERO_SHA;
    });
}

bool Commit::isValid() const
{
    if (sha.size() != 40)
        return false;

    return std::ranges::all_of(sha, [](const QChar ch) {
        return (ch >= u'0' && ch <= u'9') || (ch >= u'a' && ch <= u'f') || (ch >= u'A' && ch <= u'F');
    });
}

void Commit::removeChild(Commit* commit)
{
    if (mChilds.contains(commit))
        mChilds.removeAll(commit);
}

QString Commit::getFirstChildSha() const { return !mChilds.isEmpty() ? mChilds.constFirst()->sha : QString{}; }
