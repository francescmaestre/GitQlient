#include <QApplication>
#include <QFontDatabase>
#include <QIcon>
#include <QTimer>
#include <QTranslator>

#include <BenchmarkHarness.h>
#include <GitQlient.h>
#include <GitQlientSettings.h>
#include <QLogger.h>

using namespace QLogger;

namespace
{
// QT_QPA_PLATFORM is read at QApplication construction; switching after that
// is a no-op, so the bench flag has to be detected before parseArguments.
bool benchmarkRequested(int argc, char *argv[])
{
   for (int i = 1; i < argc; ++i)
   {
      const auto a = QString::fromLocal8Bit(argv[i]);
      if (a == "--benchmark-graph-run" || a.startsWith("--benchmark-graph-run="))
         return true;
   }
   return false;
}
}

int main(int argc, char *argv[])
{
   if (benchmarkRequested(argc, argv) && qEnvironmentVariableIsEmpty("QT_QPA_PLATFORM"))
      qputenv("QT_QPA_PLATFORM", "offscreen");

   QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

   QApplication app(argc, argv);

   QApplication::setOrganizationName("CescSoftware");
   QApplication::setOrganizationDomain("francescmm.com");
   QApplication::setApplicationName("GitQlient");
   QApplication::setApplicationVersion(VER);
   QApplication::setWindowIcon(QIcon(":/icons/GitQlientLogoIco"));

   QFontDatabase::addApplicationFont(":/DejaVuSans");
   QFontDatabase::addApplicationFont(":/DejaVuSansMono");

   const auto languageFile = GitQlientSettings().globalValue("UILanguage", "gitqlient_en").toString();
   QTranslator qtTranslator;

   if (qtTranslator.load(languageFile, QString::fromUtf8(":/translations/")))
      app.installTranslator(&qtTranslator);

   QStringList repos;
   if (!GitQlient::parseArguments(app.arguments(), &repos))
      return 0;

   if (BenchmarkHarness::pending())
   {
      if (repos.isEmpty())
      {
         qWarning("--benchmark-graph-run requires a repository path");
         return 1;
      }
      return BenchmarkHarness::run(repos.first());
   }

   GitQlient mainWin;
   mainWin.setRepositories(repos);
   mainWin.restorePinnedRepos();
   mainWin.show();

   return app.exec();
}
