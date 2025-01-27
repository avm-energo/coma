#pragma once
#include <QLabel>
#include <QStatusBar>
#include <QWidget>

class EStatusBar : public QStatusBar
{
    Q_OBJECT
public:
    EStatusBar(QWidget *parent = nullptr);

public slots:
    void setQueueSize(const QString &sizestr);
    void setModelString(const QString &str);
    void setSerialNumber(quint32 number);
    void setConnectionType(const QString &type);
    void setConnectionState(const QString &state);
    void setConnectionPixmap(const QPixmap &pixmap);

private:
    QLabel *m_queueSizeLbl, *m_msgModelLbl, *m_msgSNLbl, *m_connStateLbl, *m_connTypeLbl, *m_connImageLbl;
};
