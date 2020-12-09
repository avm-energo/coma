#ifndef LOG_H
#define LOG_H

#include <QFile>
#include <QMutex>

#define LOG_MAX_SIZE 1048576
namespace
{
constexpr char logStart[] = "=== Log started ===\n";
constexpr char logExt[] = "log";
}

class LogClass : public QObject
{
    Q_OBJECT

public:
    explicit LogClass(QObject *parent = nullptr);
    ~LogClass();
    void Init(const QString &Filename);
    void error(const QString &str);
    void warning(const QString &str);
    void info(const QString &str);
    void intvarvalue(const QString &var, int value);
    void WriteFile(const QString &Prepend, const QString &msg);
    void WriteRaw(const QByteArray &ba);

private:
    QString LogFile;
    bool CanLog;
    QFile *fp;
    QMutex *mtx;

    void CheckAndCompress();
};

#endif // LOG_H
