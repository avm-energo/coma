#pragma once

#include <QObject>

namespace LimeReport
{
class ReportEngine;
}

class TuneReporter : public QObject
{
    Q_OBJECT
private:
    LimeReport::ReportEngine *m_engine;

public:
    explicit TuneReporter(QObject *parent = nullptr) noexcept;
    void setTemplatePath(const QString &templatePath) noexcept;
    void setupReportData(const std::map<QString, QString> &reportData) noexcept;
    void saveToFile(const QString &filepath) noexcept;
};
