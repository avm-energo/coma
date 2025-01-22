#include <widgets/estatusbar.h>

#include <QHBoxLayout>
#include <QLabel>

EStatusBar::EStatusBar(QWidget *parent) : QStatusBar(parent)
{
    auto bar = new QStatusBar(parent);
    bar->setMaximumHeight(parent->height() / 20);

    auto widget = new QWidget(parent);
    widget->setMaximumHeight(bar->height());

    auto layout = new QHBoxLayout;
    layout->setSpacing(parent->width() / 20);
    layout->setContentsMargins(1, 1, 1, 1);

    auto queueSize = new QLabel(bar);
    queueSize->setObjectName("QueueSize");
    layout->addWidget(queueSize);

    auto msgModel = new QLabel(bar);
    msgModel->setObjectName("Model");
    layout->addWidget(msgModel);

    auto msgSerialNumber = new QLabel(bar);
    msgSerialNumber->setObjectName("SerialNumber");
    layout->addWidget(msgSerialNumber);

    auto msgConnectionState = new QLabel(bar);
    msgConnectionState->setObjectName("ConnectionState");
    layout->addWidget(msgConnectionState);

    auto msgConnectionImage = new QLabel(bar);
    msgConnectionImage->setObjectName("ConnectionImage");
    layout->addWidget(msgConnectionImage);

    auto msgConnectionType = new QLabel(bar);
    msgConnectionType->setObjectName("ConnectionType");
    layout->addWidget(msgConnectionType);

    auto height = bar->height() - layout->contentsMargins().bottom();
    for (auto i = 0; i < layout->count(); ++i)
    {
        layout->itemAt(i)->widget()->setFixedHeight(height);
    }

    widget->setLayout(layout);
    bar->addPermanentWidget(widget);
    return bar;
}
