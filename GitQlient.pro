#General stuff
CONFIG += qt warn_on c++20 c++1z

QT += widgets core network gui

if (!exists(src/git/.git) || !exists(src/AuxiliarCustomWidgets/.git) || !exists(src/QLogger/.git) || !exists(src/QPinnableTabWidget/.git)) {
    message("Submodule update:")
    system(git submodule update --init --recursive)
}

unix:!macos {
   TARGET = gitqlient
   QMAKE_LFLAGS += -no-pie

   isEmpty(PREFIX) {
      PREFIX = /usr/local
   }

   target.path = $$PREFIX/bin

   application.path = $$PREFIX/share/applications
   application.files = $$PWD/src/resources/gitqlient.desktop
   INSTALLS += application

   iconsvg.path = $$PREFIX/share/icons/hicolor/scalable/apps
   iconsvg.extra = \$(QINSTALL) $$PWD/src/resources/icons/GitQlientLogo.svg \$(INSTALL_ROOT)$${iconsvg.path}/$${TARGET}.svg
   icon16.path = $$PREFIX/share/icons/hicolor/16x16/apps
   icon16.extra = \$(QINSTALL) $$PWD/src/resources/icons/GitQlientLogo16.png \$(INSTALL_ROOT)$${icon16.path}/$${TARGET}.png
   icon24.path = $$PREFIX/share/icons/hicolor/24x24/apps
   icon24.extra = \$(QINSTALL) $$PWD/src/resources/icons/GitQlientLogo24.png \$(INSTALL_ROOT)$${icon24.path}/$${TARGET}.png
   icon32.path = $$PREFIX/share/icons/hicolor/32x32/apps
   icon32.extra = \$(QINSTALL) $$PWD/src/resources/icons/GitQlientLogo32.png \$(INSTALL_ROOT)$${icon32.path}/$${TARGET}.png
   icon48.path = $$PREFIX/share/icons/hicolor/48x48/apps
   icon48.extra = \$(QINSTALL) $$PWD/src/resources/icons/GitQlientLogo48.png \$(INSTALL_ROOT)$${icon48.path}/$${TARGET}.png
   icon64.path = $$PREFIX/share/icons/hicolor/64x64/apps
   icon64.extra = \$(QINSTALL) $$PWD/src/resources/icons/GitQlientLogo64.png \$(INSTALL_ROOT)$${icon64.path}/$${TARGET}.png
   icon96.path = $$PREFIX/share/icons/hicolor/96x96/apps
   icon96.extra = \$(QINSTALL) $$PWD/src/resources/icons/GitQlientLogo96.png \$(INSTALL_ROOT)$${icon96.path}/$${TARGET}.png
   icon128.path = $$PREFIX/share/icons/hicolor/128x128/apps
   icon128.extra = \$(QINSTALL) $$PWD/src/resources/icons/GitQlientLogo128.png \$(INSTALL_ROOT)$${icon128.path}/$${TARGET}.png
   icon256.path = $$PREFIX/share/icons/hicolor/256x256/apps
   icon256.extra = \$(QINSTALL) $$PWD/src/resources/icons/GitQlientLogo256.png \$(INSTALL_ROOT)$${icon256.path}/$${TARGET}.png
   icon512.path = $$PREFIX/share/icons/hicolor/512x512/apps
   icon512.extra = \$(QINSTALL) $$PWD/src/resources/icons/GitQlientLogo512.png \$(INSTALL_ROOT)$${icon512.path}/$${TARGET}.png
   INSTALLS += iconsvg icon16 icon24 icon32 icon48 icon64 icon96 icon128 icon256 icon512
   INSTALLS += target
} else {
   TARGET = GitQlient
}

#project files
SOURCES += src/main.cpp

include(src/App.pri)

OTHER_FILES += \
    $$PWD/LICENSE


isEmpty(VERSION) {
   VERSION = $$system(git describe --abbrev=0)
   VERSION = $$replace(VERSION, "v", "")
}

!defined(GQ_SHA, var) {
   GQ_SHA = $$system(git rev-parse --short HEAD)
}

DEFINES += \
    VER=\\\"$$VERSION\\\" \
    SHA_VER=\\\"$$GQ_SHA\\\"

!win32-msvc* {
    debug {
       DEFINES += DEBUG
       QMAKE_CXXFLAGS += -Wall -pedantic-errors -Werror -Wno-deprecated -Wno-error=deprecated-declarations
    }
}

DEFINES += \
   QT_NO_JAVA_STYLE_ITERATORS \
   QT_NO_CAST_TO_ASCII \
   QT_RESTRICTED_CAST_FROM_ASCII \
   QT_USE_QSTRINGBUILDER

macos{
   isEmpty(ARCH) {
      ARCH = arm64
   }

   isEmpty(CREATEDMGPATH) {
      CREATEDMGPATH = /opt/homebrew/bin
   }

   QMAKE_INFO_PLIST=$$PWD/src/resources/Info.plist
   CONFIG+=sdk_no_version_check
   ICON = $$PWD/src/resources/icon.icns

   BUNDLE_FILENAME = $${TARGET}.app
   DMG_FILENAME = "GitQlient-$${VERSION}-$${ARCH}.dmg"
#Target for pretty DMG generation
   dmg.commands += echo "Generate DMG";
   dmg.commands += rm -f *.dmg && macdeployqt $$BUNDLE_FILENAME &&
   dmg.commands += $${CREATEDMGPATH}/create-dmg \
    --volname $${TARGET} \
    --volicon "$${PWD}/src/resources/icon.icns" \
    --background "$${PWD}/src/resources/dmg_bg.png" \
    --icon "$${TARGET}.app" 125 220 \
    --window-size 600 450 \
    --icon-size 100 \
    --hdiutil-quiet \
    --hide-extension $${BUNDLE_FILENAME} \
    --app-drop-link 475 220 \
    $${DMG_FILENAME} \
    $${BUNDLE_FILENAME}

   QMAKE_EXTRA_TARGETS += dmg
}

TRANSLATIONS += \
   $$PWD/src/resources/translations/gitqlient_en.ts \
   $$PWD/src/resources/translations/gitqlient_zh_CN.ts
