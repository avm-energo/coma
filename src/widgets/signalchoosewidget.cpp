#include "signalchoosewidget.h"

#include "../widgets/wd_func.h"

#include <QVBoxLayout>

SignalChooseWidget::SignalChooseWidget(const QStringList &snames, const QStringList &discr, QWidget *parent)
    : QScrollArea(parent)
{
    auto widget = new QWidget(this);
    QVBoxLayout *lyout = new QVBoxLayout;
    for (int i = 0; i < snames.size(); ++i)
    {
        int idx = snames.size() - i - 1; // инверсия индекса
        QString name = snames.at(idx);
        QHBoxLayout *hlyout = new QHBoxLayout;
        QCheckBox *chb = WDFunc::NewChB2(this, name, "");
        connect(chb, &QAbstractButton::toggled, this, [=](bool isChecked) { emit signalToggled(name, isChecked); });
        hlyout->addWidget(chb);
        auto msWidget = new MarkSignalWidget(name, this);
        msWidget->setToolTip(discr.at(idx));
        // w->setObjectName(name);
        connect(msWidget, &MarkSignalWidget::clicked, this, [=] { emit signalChoosed(name); });
        hlyout->addWidget(msWidget);
        lyout->addLayout(hlyout);
    }
    widget->setLayout(lyout);
    setWidget(widget);
}

void SignalChooseWidget::setChecked(QString signame, bool checked)
{
    WDFunc::SetChBData(this, signame, checked);
}

MarkSignalWidget::MarkSignalWidget(const QString &text, QWidget *parent) : QLabel(parent)
{
    setText(text);
    setMouseTracking(true);
}

void MarkSignalWidget::mousePressEvent(QMouseEvent *e)
{
    emit clicked();
    e->accept();
}
