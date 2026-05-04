#pragma once

#include <QEventLoop>
#include <QTimer>

/**
 * @brief Runs the event loop until @a signal fires on @a sender or @a timeoutMs elapses.
 * @return @c true if the signal fired; @c false if the timeout expired first.
 *
 * Safe to use before or after calling the code that triggers the signal — if the signal
 * fires synchronously before exec() is reached, QEventLoop::quit() is queued and exec()
 * returns immediately on entry.
 */
template<typename Sender, typename Signal>
bool waitForSignal(Sender* sender, Signal signal, int timeoutMs = 5000)
{
    QEventLoop loop;
    bool received = false;
    QObject::connect(sender, signal, [&received, &loop] {
        received = true;
        loop.quit();
    });
    QTimer::singleShot(timeoutMs, &loop, &QEventLoop::quit);
    loop.exec();
    return received;
}
