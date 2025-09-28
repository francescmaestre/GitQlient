#pragma once

#include <QColor>
#include <QPalette>

namespace System::Colors
{
    static const QColor filesBlue(87, 155, 213); //#579BD5
    static const QColor gitQlientOrange(216, 144, 0); //#D89000
    static const QColor graphBlue(50, 92, 199); //#325CC7
    static const QColor graphRed(255, 34, 34); //#FF2222
    static const QColor graphGreen(101, 149, 43); //#65952B
    static const QColor graphPastel(205, 144, 119); //#CD9077
    static const QColor graphOrange(255, 147, 32); //#FF9320
    static const QColor graphGrey(132, 132, 132); //#848484
    static const QColor graphTurquoise(64, 224, 208); //#40E0D0
    static const QColor graphAubergine(71, 1, 115); //#470173
    static const QColor graphPink(255, 121, 198); //#FF79C6
    static const QColor graphCoral(230, 94, 108); //#E65E6C
    static const QColor graphDetached(133, 30, 62); //#851E3E
    static const QColor graphTag(222, 195, 195); //#DEC3C3
    static const QColor highlightCommentStart(64, 65, 66); //#404142
    static const QColor highlightCommentEnd(96, 97, 98); //#606162
    static const QColor jenkinsResultSuccess(0, 175, 24); //#00AF18
    static const QColor jenkinsResultFailure(193, 32, 32); //#C12020
    static const QColor jenkinsResultAborted(91, 91, 91); //#5B5B5B
    static const QColor jenkinsResultNotBuilt(200, 200, 200); //#C8C8C8
    static const QColor editorGreenShadowBright(141, 201, 68, 255 * 0.65);
    static const QColor editorGreenShadowDark(141, 201, 68, 255 * 0.45);
    static const QColor editorRedShadowBright(255, 85, 85, 255 * 0.65);
    static const QColor editorRedShadowDark(255, 85, 85, 255 * 0.45);

    static const int kBranchColorsCount = 11;

    static std::array<std::function<QColor()>, kBranchColorsCount> BranchColors{
        []() {
            // This value will change with the OS Theme
            return QPalette().color(QPalette::Text);
        },
        []() {
            return Colors::graphRed;
        },
        []() {
            return Colors::graphBlue;
        },
        []() {
            return Colors::graphGreen;
        },
        []() {
            return Colors::graphOrange;
        },
        []() {
            return Colors::graphAubergine;
        },
        []() {
            return Colors::graphCoral;
        },
        []() {
            return Colors::graphGrey;
        },
        []() {
            return Colors::graphTurquoise;
        },
        []() {
            return Colors::graphPink;
        },
        []() {
            return Colors::graphPastel;
        }};

} // namespace System::Colors
