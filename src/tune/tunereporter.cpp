#include "tunereporter.h"

#include "../widgets/epopup.h"

#include <LimeReport>

TuneReporter::TuneReporter(QObject *parent) noexcept : QObject(parent), m_engine(new LimeReport::ReportEngine(this))
{
}

void TuneReporter::setTemplatePath(const QString &templatePath) noexcept
{
    if (!templatePath.isEmpty())
    {
        m_engine->dataManager()->clearUserVariables();
        m_engine->loadFromFile(templatePath);
    }
}

void TuneReporter::setupReportData(const std::map<QString, QString> &reportData) noexcept
{
    for (const auto &[name, value] : reportData)
        m_engine->dataManager()->setReportVariable(name, value);
}

void TuneReporter::saveToFile(const QString &filepath) noexcept
{
    if (!filepath.isEmpty())
    {
        // m_engine->designReport();
        m_engine->printToPDF(filepath);
        // m_engine->previewReport();
    }
}
