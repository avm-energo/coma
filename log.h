#ifndef LOG_H
#define LOG_H

#include <QObject>
#include <QFile>

#define LOG_MAX_SIZE    1048576

class Log : public QObject
{
    Q_OBJECT

public:
    explicit Log(QObject *parent=0);
    ~Log();
    void Init(const QString &Filename);
    void error(const QString &str);
    void warning(const QString &str);
    void info(const QString &str);
//    void Info(QByteArray &ba);
    void intvarvalue(const QString &var, int value);
    void WriteFile(const QString &Prepend, const QString &msg);
    void WriteRaw(const QByteArray &ba);
    
private:
    QString LogFile;
    bool CanLog;
    QFile *fp;

    void CheckAndGz();
};

#endif // LOG_H
