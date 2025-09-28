#pragma once

#include <system/SettingsKeys.h>

#include <QColor>
#include <QFile>
#include <QFontDatabase>
#include <QScopedPointer>
#include <QSettings>
#include <QString>

inline QString getStyles()
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
            QSettings settings;
            const auto fontSize
                = settings
                      .value(
                          System::GlobalKey::UiBaseFontSize,
                          QFontDatabase::systemFont(QFontDatabase::GeneralFont).pointSize())
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
