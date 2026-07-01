#include "abatcher.h"

#include <avm-widgets/styleloader.h>
#include <avm-gen/settings.h>

#include <QCoreApplication>
#include <QCheckBox>
#include <QVBoxLayout>

void Abatcher::setupUI() 
{
//     QString caption(QCoreApplication::applicationName());
//     caption.append(" v").append(QCoreApplication::applicationVersion());
//     setWindowTitle(caption);
//     setMinimumSize(QSize(1150, 700));
    
//     auto wdgt = new QWidget(this);
//     auto lyout = new QVBoxLayout(wdgt);
//     auto hlyout = new QHBoxLayout;
//     hlyout->addWidget(createToolBar(), Qt::AlignTop | Qt::AlignLeft);
    
//     m_alarmW = new AlarmWidget(this);
//     hlyout->addWidget(m_alarmW, Qt::AlignCenter);
//     lyout->addLayout(hlyout);
//     lyout->addWidget(least());
//     wdgt->setLayout(lyout);
    
//     setCentralWidget(wdgt);
    
//     StyleLoader::setStyle(Settings::get("theme", "Light"));
}

QCheckBox *Abatcher::createDryCheckBox()
{
    QCheckBox *dryCB = new QCheckBox(this);

    return dryCB;
}