#pragma once

#include "../s2/s2datatypes.h"

#include <QByteArray>
#include <QObject>
#include <gen/datamanager/typesproxy.h>
#include <gen/stdfunc.h>

class S2RequestService final : public QObject
{
    Q_OBJECT
private:
    UniquePointer<DataTypesProxy> m_proxy;
    quint16 m_lastRequestedFile;

private slots:
    void responseReceived(const QVariant &var);

public:
    explicit S2RequestService(QObject *parent = nullptr);

public slots:
    void request(const S2::FilesEnum filenum, bool withCheck = false);

signals:
    void response(const QByteArray &response);
    void noConfigurationError();
};
