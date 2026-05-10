#include "BenchmarkHarness.h"

#include <CommitHistoryModel.h>
#include <CommitHistoryView.h>
#include <GitBase.h>
#include <GitCache.h>
#include <GitQlientSettings.h>
#include <GitRepoLoader.h>
#include <PaintTelemetry.h>
#include <RepositoryViewDelegate.h>

#include <QApplication>
#include <QCoreApplication>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QFile>
#include <QImage>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPainter>
#include <QScrollBar>
#include <QSharedPointer>
#include <QStyleOptionViewItem>
#include <QSysInfo>
#include <QThreadPool>
#include <QTimer>

#include <algorithm>
#include <vector>

namespace
{
BenchmarkHarness::Workload gWorkload = BenchmarkHarness::Workload::None;
QString gOutputPath;

bool loadCache(const QString &repoPath, QSharedPointer<GitBase> &git, QSharedPointer<GitCache> &cache,
               QSharedPointer<GitQlientSettings> &settings)
{
   git = QSharedPointer<GitBase>::create(repoPath);
   if (git->run("git rev-parse --git-dir").output.trimmed().isEmpty())
   {
      qWarning("BenchmarkHarness: not a git repository: %s", qPrintable(repoPath));
      return false;
   }

   cache = QSharedPointer<GitCache>::create();
   settings = QSharedPointer<GitQlientSettings>::create(repoPath);

   GitRepoLoader loader(git, cache, settings);

   bool finished = false;
   QEventLoop loop;
   QObject::connect(&loader, &GitRepoLoader::signalLoadingFinished, &loop, [&finished, &loop](bool) {
      finished = true;
      loop.quit();
   });

   loader.loadAll();
   QTimer::singleShot(600'000, &loop, &QEventLoop::quit);
   loop.exec();

   if (!finished)
   {
      qWarning("BenchmarkHarness: load did not finish within 10 minutes");
      return false;
   }

   QThreadPool::globalInstance()->waitForDone(60'000);
   return true;
}

void writeJson(const QJsonObject &obj)
{
   const auto bytes = QJsonDocument(obj).toJson(QJsonDocument::Indented);

   if (gOutputPath.isEmpty())
   {
      if (QFile out;
          out.open(stdout, QIODevice::WriteOnly))
      {
         out.write(bytes);
         out.write("\n");
         out.close();
      }

      return;
   }

   QFile f(gOutputPath);
   if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate))
   {
      qWarning("BenchmarkHarness: cannot write to %s", qPrintable(gOutputPath));
      return;
   }

   f.write(bytes);
   f.write("\n");
   f.close();
}

QJsonObject baseJson(const QString &workload)
{
   QJsonObject obj;
   obj["workload"] = workload;
   obj["os"] = QSysInfo::productType();
   obj["arch"] = QSysInfo::currentCpuArchitecture();
   obj["kernel"] = QSysInfo::kernelVersion();
   obj["sha"] = QString::fromUtf8(SHA_VER);
   return obj;
}

int runThroughput(const QSharedPointer<GitBase> &git, const QSharedPointer<GitCache> &cache,
                  const QSharedPointer<GitQlientSettings> &settings)
{
   const auto rowCount = cache->commitCount();
   if (rowCount == 0)
   {
      qWarning("BenchmarkHarness: cache is empty");
      return 1;
   }

   CommitHistoryView view(cache, git, settings);
   auto *model = new CommitHistoryModel(cache, git, &view);
   view.setModel(model);
   view.resize(1200, 800);

   const auto columns = model->columnCount();

   RepositoryViewDelegate delegate(cache, git, /*gitServerCache=*/ {}, &view);

   QImage backbuffer(1200, ROW_HEIGHT, QImage::Format_ARGB32_Premultiplied);
   backbuffer.setDevicePixelRatio(1.0);

   QStyleOptionViewItem opt;
   opt.rect = QRect(0, 0, backbuffer.width(), backbuffer.height());
   opt.font = QApplication::font();
   opt.palette = QApplication::palette();
   opt.state = QStyle::State_Enabled;

   QPainter painter(&backbuffer);

   QElapsedTimer wall;
   wall.start();

   for (int row = 0; row < rowCount; ++row)
   {
      for (int col = 0; col < columns; ++col)
      {
         backbuffer.fill(Qt::transparent);
         delegate.paint(&painter, opt, model->index(row, col));
      }
   }

   const auto wallMs = wall.elapsed();

   PaintTelemetry::Stats stats {};
   if (const auto *t = delegate.telemetry())
      stats = t->snapshot();

   const auto rowsPainted = stats.rows;
   const auto avgUs = rowsPainted ? stats.totalNs / 1000.0 / rowsPainted : 0.0;
   const auto maxMs = stats.maxNs / 1'000'000.0;

   auto obj = baseJson("throughput");
   obj["rows"] = static_cast<double>(rowCount);
   obj["columns"] = columns;
   obj["paints"] = static_cast<double>(rowsPainted);
   obj["avg_us_per_row"] = avgUs;
   obj["max_ms_per_row"] = maxMs;
   obj["slow_rows"] = static_cast<double>(stats.slowRows);
   obj["wall_ms"] = static_cast<double>(wallMs);
   writeJson(obj);

   return 0;
}

int runScroll(const QSharedPointer<GitBase> &git, const QSharedPointer<GitCache> &cache,
              const QSharedPointer<GitQlientSettings> &settings)
{
   const auto rowCount = cache->commitCount();
   if (rowCount == 0)
   {
      qWarning("BenchmarkHarness: cache is empty");
      return 1;
   }

   CommitHistoryView view(cache, git, settings);
   auto *model = new CommitHistoryModel(cache, git, &view);
   view.setModel(model);
   view.resize(1200, 800);

   auto *delegate = new RepositoryViewDelegate(cache, git, /*gitServerCache=*/ {}, &view);
   view.setItemDelegate(delegate);

   view.show();
   QCoreApplication::processEvents();

   auto *sb = view.verticalScrollBar();
   const auto smin = sb->minimum();
   const auto smax = sb->maximum();

   // Warm-up: ensures fonts/styles are resolved, first-paint cost doesn't
   // pollute the sample.
   sb->setValue(smin);
   view.viewport()->repaint();

   constexpr int kSteps = 100;
   std::vector<qint64> frameNs;
   frameNs.reserve(kSteps);

   QElapsedTimer wall;
   wall.start();

   for (int i = 1; i <= kSteps; ++i)
   {
      const auto target = smin + (smax - smin) * i / kSteps;
      sb->setValue(target);
      QCoreApplication::sendPostedEvents();

      QElapsedTimer t;
      t.start();
      view.viewport()->repaint();
      frameNs.push_back(t.nsecsElapsed());
   }

   const auto wallMs = wall.elapsed();

   auto sorted = frameNs;
   std::sort(sorted.begin(), sorted.end());
   auto pct = [&sorted](double p) -> double {
      if (sorted.empty())
         return 0.0;
      const auto idx = std::min<size_t>(sorted.size() - 1, static_cast<size_t>(p * sorted.size()));
      return sorted[idx] / 1'000'000.0;
   };

   PaintTelemetry::Stats stats {};
   if (const auto *t = delegate->telemetry())
      stats = t->snapshot();

   auto obj = baseJson("scroll");
   obj["rows"] = static_cast<double>(rowCount);
   obj["steps"] = kSteps;
   obj["frame_p50_ms"] = pct(0.50);
   obj["frame_p95_ms"] = pct(0.95);
   obj["frame_p99_ms"] = pct(0.99);
   obj["frame_max_ms"] = sorted.empty() ? 0.0 : sorted.back() / 1'000'000.0;
   obj["paints"] = static_cast<double>(stats.rows);
   obj["slow_rows"] = static_cast<double>(stats.slowRows);
   obj["wall_ms"] = static_cast<double>(wallMs);
   writeJson(obj);

   return 0;
}
} // namespace

void BenchmarkHarness::configure(Workload w, QString outputPath)
{
   gWorkload = w;
   gOutputPath = std::move(outputPath);

   PaintTelemetry::setEnabled(w != Workload::None);
}

bool BenchmarkHarness::pending()
{
   return gWorkload != Workload::None;
}

QString BenchmarkHarness::workloadName()
{
   switch (gWorkload)
   {
      case Workload::Throughput:
         return "throughput";
      case Workload::Scroll:
         return "scroll";
      case Workload::None:
         break;
   }
   return {};
}

int BenchmarkHarness::run(const QString &repoPath)
{
   QSharedPointer<GitBase> git;
   QSharedPointer<GitCache> cache;
   QSharedPointer<GitQlientSettings> settings;

   if (!loadCache(repoPath, git, cache, settings))
      return 1;

   switch (gWorkload)
   {
      case Workload::Throughput:
         return runThroughput(git, cache, settings);
      case Workload::Scroll:
         return runScroll(git, cache, settings);
      case Workload::None:
         break;
   }
   return 1;
}
