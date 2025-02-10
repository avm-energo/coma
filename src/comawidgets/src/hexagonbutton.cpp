#include <widgets/hexagonbutton.h> //

HexagonButton::HexagonButton(QWidget *parent) : QPushButton(parent)
{
    setAttribute(Qt::WA_Hover);
    setAttribute(Qt::WA_X11NetWmWindowTypeToolBar);
}

void HexagonButton::setData(const QString &data)
{
    m_data = data;
}

QString HexagonButton::data()
{
    return m_data;
}
