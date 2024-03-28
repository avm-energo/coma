#pragma once

#include <QPushButton>

class AlarmButton : public QPushButton
{
    Q_OBJECT
public:
    explicit AlarmButton(QWidget *parent = nullptr);
    void setPixmap(const QPixmap &pixmap);
    virtual QSize sizeHint() const override;

protected:
    virtual void paintEvent(QPaintEvent *e) override;

private:
    QPixmap m_pixmap;
};
