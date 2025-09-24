#pragma once

#include <QColor>
#include <QScopedPointer>
#include <QString>

#include <array>

class GitQlientSettings;

class GitQlientStyles
{
private:
    static const int kBranchColors = 11; /*!< Total of branch colors. */

public:
    static GitQlientStyles* getInstance();
    static QString getStyles();
    static QColor getBlue();
    static QColor getRed();
    static QColor getGreen();
    static QColor getOrange();
    static int getTotalBranchColors() { return kBranchColors + 1; }
    static QColor getBranchColorAt(int index);

private:
    static GitQlientStyles* INSTANCE;
    GitQlientStyles() = default;
    static std::array<QColor, kBranchColors> getBranchColors();
};
