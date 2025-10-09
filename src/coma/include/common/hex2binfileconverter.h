#pragma once

#include <QObject>
#include <QWidget>
#include <gen/error.h>
#include <s2/hexparser.h>

class Hex2BinFileConverter : public QObject
{
    Q_OBJECT
public:
    Hex2BinFileConverter(QWidget *parent = nullptr);
    void start();

private:
    QWidget *m_parent;
    bool m_errorHappened;

    QByteArray readFile(const QString &filepath);
    bool toBinConvert(QByteArray &ba);
    bool toS2Convert(QByteArray &ba);
    bool writeToFile(const QString &filepath, const QByteArray &file);

private slots:
    void errorHandler(S2::HexParseError msg);
};
