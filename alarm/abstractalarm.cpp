#include "abstractalarm.h"

#include "../gen/colors.h"
#include "../gen/error.h"
#include "../widgets/wd_func.h"

#include <QApplication>
#include <QBoxLayout>
#include <QCursor>
#include <QDialog>
#include <QDir>
#include <QGroupBox>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QProgressBar>
#include <QPushButton>
#include <QSettings>
#include <QSplashScreen>
#include <QStandardPaths>
#include <QStringListModel>
#include <QToolBar>

AbstractAlarm::AbstractAlarm(QWidget *parent) : QWidget(parent)
{
}
