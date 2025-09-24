#include <system/GitQlientStyles.h>

#include <system/Colors.h>
#include <system/GitQlientSettings.h>

#include <QApplication>
#include <QFile>
#include <QFontDatabase>
#include <QPalette>

GitQlientStyles* GitQlientStyles::INSTANCE = nullptr;

GitQlientStyles* GitQlientStyles::getInstance()
{
    if (INSTANCE == nullptr)
        INSTANCE = new GitQlientStyles();

    return INSTANCE;
}

QString GitQlientStyles::getStyles()
{
    QString styles;
    QFile stylesFile(":/stylesheet");

    if (stylesFile.open(QIODevice::ReadOnly))
    {
        QFile textSizeFile(":/font_sizes");
        QString textSizeContent;
        if (textSizeFile.open(QIODevice::ReadOnly))
        {
            auto css = textSizeFile.readAll().split('\n');
            textSizeFile.close();
            auto baseSize = css.takeFirst().trimmed();

            const auto end = baseSize.indexOf("pt;");
            const auto fontSize
                = GitQlientSettings()
                      .globalValue("UiBaseFontSize", QFontDatabase::systemFont(QFontDatabase::GeneralFont).pointSize())
                      .toInt();
            auto increment = fontSize - baseSize.mid(11, end - 11).toInt();
            for (auto& line : css)
            {
                if (line.contains("font-size"))
                {
                    line = line.trimmed();
                    const auto end = line.indexOf("pt;");
                    auto newVal = line.mid(11, end - 11).toInt() + increment;

                    line = QString("font-size: %1pt;").arg(newVal).toUtf8();
                }
            }

            textSizeContent = QString::fromUtf8(css.join('\n'));
            textSizeContent = textSizeContent.trimmed();
        }

        styles = stylesFile.readAll(); // + colorsCss;
        styles += textSizeContent;

        stylesFile.close();
    }

    return styles;
}

QColor GitQlientStyles::getBlue()
{
    const auto colorSchema = GitQlientSettings().globalValue("colorSchema", 1).toInt();

    return colorSchema == 0 ? graphBlueDark : graphBlueBright;
}

QColor GitQlientStyles::getRed() { return graphRed; }

QColor GitQlientStyles::getGreen() { return graphGreen; }

QColor GitQlientStyles::getOrange() { return graphOrange; }

std::array<QColor, GitQlientStyles::kBranchColors> GitQlientStyles::getBranchColors()
{
    static std::array<QColor, kBranchColors> colors{
        {graphRed,
         getBlue(),
         graphGreen,
         graphOrange,
         graphAubergine,
         graphCoral,
         graphGrey,
         graphTurquoise,
         graphPink,
         graphPastel}};

    return colors;
}

QColor GitQlientStyles::getBranchColorAt(int index)
{
    if (index >= 0 and index < kBranchColors + 1)
    {
        if (index == 0)
            return QPalette().color(QPalette::Text);
        else
            return getBranchColors().at(static_cast<size_t>(index));
    }
    return QColor();
}
