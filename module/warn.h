#ifndef WARN_H
#define WARN_H

#include <QBitArray>
#include <QShowEvent>
#include <QWidget>
#include <bitset>

#define CIRCLE_RADIUS 15

class Warn : public QWidget
{
    Q_OBJECT
public:
    int m_warnBdNum;
    int m_startWarnAddress;
    std::bitset<32> m_warnFlags; // '1' equals warning

    explicit Warn(QWidget *parent = nullptr);
    int realWarnSize();
    void updatePixmap(bool isset, int position);

public slots:
    //    void Update(std::bitset<32> &states);

    // signals:
    //    void updateWarn(int position, bool value);

private:
    int m_realWarnSize;

protected:
    void showEvent(QShowEvent *e);
    void setupUI(const QStringList &events);
};

#endif // WARN_H
