#pragma once

#include <QObject>
#include <QWidget>
#include <vector>

class SwjModel;
namespace QXlsx
{
class Document;
class Worksheet;
}

class SwjPackConvertor : public QObject
{
    Q_OBJECT
private:
    QWidget *m_parent;
    std::vector<SwjModel> m_data;

    void readFile(const QString &swjfilepath);
    void sortData();
    void writeHeader(QXlsx::Worksheet *sheet);
    void writeData(QXlsx::Worksheet *sheet);
    void writeToFile(const QString &filepath);

public:
    explicit SwjPackConvertor(QWidget *parent = nullptr);
    void selectDirectory();
};
