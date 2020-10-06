#ifndef WARN_H
#define WARN_H

#include <QBitArray>
#include <QShowEvent>
#include <QWidget>

#define CIRCLE_RADIUS 15

class Warn : public QWidget
{
    Q_OBJECT
public:
    explicit Warn(QWidget *parent = nullptr);

public slots:
    void Update(QList<bool> states);

signals:

private:
    int m_warnBdNum;
    int m_startWarnAddress;
    QBitArray m_warnFlags; // '1' equals warning

protected:
    void showEvent(QShowEvent *e);
    void UpdatePixmaps(bool isset, int position);
    void SetupUI(const QStringList &events, int counters);
};

#endif // WARN_H
