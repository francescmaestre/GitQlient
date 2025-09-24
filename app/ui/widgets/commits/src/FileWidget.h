#pragma once

#include <QFrame>
#include <QIcon>

class QPushButton;
class QLabel;

class FileWidget : public QFrame
{
    Q_OBJECT

signals:
    void clicked();

public:
    explicit FileWidget(const QIcon& icon, const QString& text, QWidget* parent = nullptr);
    explicit FileWidget(const QString& icon, const QString& text, QWidget* parent = nullptr);

    void setIcon(const QString& icon);
    QString text() const;
    void setText(const QString& text);
    QSize sizeHint() const override;
    void setTextColor(const QColor& color);
    QColor getTextColor() const { return mColor; }

private:
    QPushButton* mButton = nullptr;
    QLabel* mText = nullptr;
    QColor mColor;
};
