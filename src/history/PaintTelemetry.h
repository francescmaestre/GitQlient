#pragma once

/****************************************************************************************
 ** GitQlient is an application to manage and operate one or several Git repositories. With
 ** GitQlient you will be able to add commits, branches and manage all the options Git provides.
 ** Copyright (C) 2021  Francesc Maestre
 **
 ** LinkedIn: https://www.linkedin.com/in/francescmaestre/
 **
 ** This program is free software; you can redistribute it and/or
 ** modify it under the terms of the GNU Lesser General Public
 ** License as published by the Free Software Foundation; either
 ** version 2 of the License, or (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 ** Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public
 ** License along with this library; if not, write to the Free Software
 ** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 ***************************************************************************************/

#include <QElapsedTimer>

/**
 * @brief Accumulates row-paint timings and emits a 5-second-window summary via QLogger.
 *
 * Warns immediately if a single row paint exceeds 8 ms (half a 60 fps frame budget).
 */
class PaintTelemetry
{
public:
   struct Stats
   {
      qint64 rows = 0;
      qint64 totalNs = 0;
      qint64 maxNs = 0;
      qint64 slowRows = 0;
   };

   void recordRowPaint(qint64 nsecs, int row, int col);
   Stats snapshot() const;
   void reset();

   static void setEnabled(bool enabled);
   static bool isEnabled();

private:
   QElapsedTimer mWindowTimer;
   qint64 mWindowRows = 0;
   qint64 mWindowNs = 0;
   qint64 mWindowMaxNs = 0;

   qint64 mTotalRows = 0;
   qint64 mTotalNs = 0;
   qint64 mTotalMaxNs = 0;
   qint64 mTotalSlow = 0;
};
