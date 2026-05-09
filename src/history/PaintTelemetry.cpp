#include "PaintTelemetry.h"

#include <QLogger.h>

#include <algorithm>

using namespace QLogger;

namespace
{
bool gEnabled = false;
}

void PaintTelemetry::setEnabled(bool enabled)
{
   gEnabled = enabled;
}

bool PaintTelemetry::isEnabled()
{
   return gEnabled;
}

void PaintTelemetry::recordRowPaint(qint64 nsecs, int row, int col)
{
   if (!mWindowTimer.isValid())
      mWindowTimer.start();

   mWindowNs += nsecs;
   mWindowMaxNs = std::max(mWindowMaxNs, nsecs);
   ++mWindowRows;

   if (nsecs > 8'000'000)
   {
      QLog_Warning("Paint",
                   QString("Slow row paint: %1 ms (row %2, col %3)")
                       .arg(nsecs / 1'000'000.0, 0, 'f', 2)
                       .arg(row)
                       .arg(col));
   }

   if (mWindowTimer.elapsed() >= 5'000 && mWindowRows > 0)
   {
      const auto avgUs = mWindowNs / 1000.0 / mWindowRows;
      const auto maxMs = mWindowMaxNs / 1'000'000.0;
      const auto rowsPs = mWindowRows * 1000LL / std::max(1LL, mWindowTimer.elapsed());

      QLog_Info("Paint",
                QString("Paint stats [5s window]: %1 rows painted, avg %2 µs/row, max %3 ms/row, %4 rows/s")
                    .arg(mWindowRows)
                    .arg(avgUs, 0, 'f', 1)
                    .arg(maxMs, 0, 'f', 2)
                    .arg(rowsPs));

      mWindowRows = 0;
      mWindowNs = 0;
      mWindowMaxNs = 0;
      mWindowTimer.restart();
   }
}
