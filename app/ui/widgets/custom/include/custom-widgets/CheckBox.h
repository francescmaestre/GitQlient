#pragma once

#include <QCheckBox>
#include <QStyle>

class CheckBox : public QCheckBox
{
    Q_OBJECT

public:
    explicit CheckBox(QWidget* parent = 0);
    explicit CheckBox(const QString& text, QWidget* parent = 0);

    QString getIndicator(QStyle::State state) const;

protected:
    bool event(QEvent* e);
    void paintEvent(QPaintEvent* e);
};
