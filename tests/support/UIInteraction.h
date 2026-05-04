#pragma once

#include <QCoreApplication>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPoint>
#include <QWidget>

/**
 * @brief Utility functions for simulating user input in widget tests.
 *
 * Replaces Qt::Test interaction helpers (QTest::mouseClick, QTest::keyClick, etc.)
 * using QCoreApplication::sendEvent directly. Requires a QApplication instance and
 * QT_QPA_PLATFORM=offscreen for headless/CI environments.
 */
namespace UI
{
    inline void mouseClick(QWidget* widget, Qt::MouseButton button = Qt::LeftButton,
                           Qt::KeyboardModifiers modifiers = Qt::NoModifier,
                           QPoint pos = QPoint())
    {
        if (pos.isNull())
            pos = widget->rect().center();

        QMouseEvent press(QEvent::MouseButtonPress, pos, widget->mapToGlobal(pos), button, button, modifiers);
        QCoreApplication::sendEvent(widget, &press);

        QMouseEvent release(QEvent::MouseButtonRelease, pos, widget->mapToGlobal(pos), button, Qt::NoButton, modifiers);
        QCoreApplication::sendEvent(widget, &release);
    }

    inline void mouseMove(QWidget* widget, QPoint pos, Qt::MouseButtons buttons = Qt::NoButton,
                          Qt::KeyboardModifiers modifiers = Qt::NoModifier)
    {
        QMouseEvent event(QEvent::MouseMove, pos, widget->mapToGlobal(pos), Qt::NoButton, buttons, modifiers);
        QCoreApplication::sendEvent(widget, &event);
    }

    inline void keyClick(QWidget* widget, Qt::Key key, Qt::KeyboardModifiers modifiers = Qt::NoModifier,
                         const QString& text = QString())
    {
        QKeyEvent press(QEvent::KeyPress, key, modifiers, text);
        QCoreApplication::sendEvent(widget, &press);

        QKeyEvent release(QEvent::KeyRelease, key, modifiers, text);
        QCoreApplication::sendEvent(widget, &release);
    }

    inline void keyClicks(QWidget* widget, const QString& sequence, Qt::KeyboardModifiers modifiers = Qt::NoModifier)
    {
        for (const QChar& ch : sequence)
            keyClick(widget, Qt::Key_unknown, modifiers, ch);
    }
} // namespace UI
