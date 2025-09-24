#ifndef BUTTONLINK_H
#define BUTTONLINK_H

#include <QLabel>
#include <QVariant>

class ButtonLink : public QLabel
{
    Q_OBJECT

signals:
    void clicked();

public:
    explicit ButtonLink(QWidget* parent = nullptr);
    explicit ButtonLink(const QString& text, QWidget* parent = nullptr);
    explicit ButtonLink(const QString& text, const QVariant& data, QWidget* parent = nullptr);

    void setData(const QVariant& data) { mData = data; }
    QVariant data() const { return mData; }

protected:
    void mousePressEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    bool mPressed = false;
    QVariant mData;
};

#endif // BUTTONLINK_H
