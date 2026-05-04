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
