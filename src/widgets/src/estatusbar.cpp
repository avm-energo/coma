#include <widgets/estatusbar.h>

#include <QHBoxLayout>
#include <QLabel>

EStatusBar::EStatusBar(QWidget *parent) : QStatusBar(parent)
{
    setMaximumHeight(parent->height() / 20);

    auto widget = new QWidget(parent);
    widget->setMaximumHeight(height());

    auto layout = new QHBoxLayout;
    layout->setSpacing(parent->width() / 20);
    layout->setContentsMargins(1, 1, 1, 1);

    m_queueSizeLbl = new QLabel(this);
    m_queueSizeLbl->setObjectName("QueueSize");
    layout->addWidget(m_queueSizeLbl);

    m_msgModelLbl = new QLabel(this);
    m_msgModelLbl->setObjectName("Model");
    layout->addWidget(m_msgModelLbl);

    m_msgSNLbl = new QLabel(this);
    m_msgSNLbl->setObjectName("SerialNumber");
    layout->addWidget(m_msgSNLbl);

    m_connStateLbl = new QLabel(this);
    m_connStateLbl->setObjectName("ConnectionState");
    layout->addWidget(m_connStateLbl);

    m_connImageLbl = new QLabel(this);
    m_connImageLbl->setObjectName("ConnectionImage");
    layout->addWidget(m_connImageLbl);

    m_connTypeLbl = new QLabel(this);
    m_connTypeLbl->setObjectName("ConnectionType");
    layout->addWidget(m_connTypeLbl);

    auto height = this->height() - layout->contentsMargins().bottom();
    for (auto i = 0; i < layout->count(); ++i)
    {
        layout->itemAt(i)->widget()->setFixedHeight(height);
    }

    widget->setLayout(layout);
    addPermanentWidget(widget);
}

void EStatusBar::setQueueSize(const QString &sizestr)
{
    m_queueSizeLbl->setText(sizestr);
}

void EStatusBar::setModelString(const QString &str)
{
    m_msgModelLbl->setText(str);
}

void EStatusBar::setSerialNumber(quint32 number)
{
    m_msgSNLbl->setText(QString::number(number, 16));
}

void EStatusBar::setConnectionType(const QString &type)
{
    m_connTypeLbl->setText(type);
}

void EStatusBar::setConnectionState(const QString &state)
{
    m_connStateLbl->setText(state);
    m_connStateLbl->setForegroundRole(QPalette::Highlight);
    m_connStateLbl->setBackgroundRole(QPalette::HighlightedText);
}

void EStatusBar::setConnectionPixmap(const QPixmap &pixmap)
{
    m_connImageLbl->setPixmap(pixmap);
}
