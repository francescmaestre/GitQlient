#pragma once

/****************************************************************************************
 ** GitQlient is an application to manage and operate one or several Git repositories.
 ** Copyright (C) 2021  Francesc Martinez
 **
 ** This program is free software; you can redistribute it and/or
 ** modify it under the terms of the GNU Lesser General Public License v2 or later.
 ***************************************************************************************/

#include <QString>

/// Headless paint benchmark, activated via `--benchmark-graph-run=<workload>`.
class BenchmarkHarness
{
public:
   enum class Workload
   {
      None,
      Throughput,
      Scroll
   };

   static void configure(Workload w, QString outputPath);
   static bool pending();
   static int run(const QString &repoPath);
   static QString workloadName();
};
