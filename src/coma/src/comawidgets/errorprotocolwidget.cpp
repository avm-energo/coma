#include "comawidgets/errorprotocolwidget.h"

#include <common/appconfig.h>
#include <avm-gen/colors.h>
#include <avm-gen/error.h>
#include <models/errorprotocolmodel.h>

#include <QHeaderView>
#include <QLabel>
#include <QTableView>
#include <QVBoxLayout>

#ifdef MODELDEBUG
#include <QAbstractItemModelTester>
#endif

ErrorProtocolWidget::ErrorProtocolWidget(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QWidget *w = new QWidget;
    QTableView *tv = new QTableView(this);
    auto Model = new ErrorProtocolModel(this);
    Model->initModel();

#ifdef MODELDEBUG
    // Test the model
    new QAbstractItemModelTester(Model, QAbstractItemModelTester::FailureReportingMode::Warning, this);
    // Test the model
#endif
    Model->setHeaderData(0, Qt::Horizontal, "Дата/время", Qt::EditRole);
    Model->setHeaderData(1, Qt::Horizontal, "Файл", Qt::EditRole);
    Model->setHeaderData(2, Qt::Horizontal, "Строка", Qt::EditRole);
    Model->setHeaderData(3, Qt::Horizontal, "Сообщение", Qt::EditRole);
    tv->setModel(Model);

    if (AppConfiguration::app() == AppConfiguration::Service)
    {
        tv->setColumnHidden(1, true);
        tv->setColumnHidden(2, true);
    }
    tv->resizeColumnsToContents();
    connect(Model, &QAbstractItemModel::dataChanged, tv, &QTableView::resizeColumnsToContents);
    tv->horizontalHeader()->setStretchLastSection(true);

    tv->verticalHeader()->setDefaultSectionSize(tv->verticalHeader()->fontMetrics().height() + 2);
    tv->setGridStyle(Qt::SolidLine);
    tv->setShowGrid(true);
    QFrame *line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    QHBoxLayout *upperLayout = new QHBoxLayout;
    upperLayout->addWidget(line, 2);
    QLabel *lbl = new QLabel("Протокол работы");
    lbl->setEnabled(false);
    upperLayout->addWidget(lbl, 0);
    upperLayout->setAlignment(lbl, Qt::AlignCenter);
    line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    upperLayout->addWidget(line, 2);
    lyout->addLayout(upperLayout, 0);
    lyout->addWidget(tv, 5);
    w->setLayout(lyout);
    lyout = new QVBoxLayout;
    lyout->addWidget(w);
    setLayout(lyout);
}

ErrorProtocolWidget::~ErrorProtocolWidget() { }
