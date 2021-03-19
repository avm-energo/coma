#include "ipctrl.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
IPCtrl::IPCtrl(QWidget *parent) : QFrame(parent)
{
    setFrameShape(QFrame::StyledPanel);
    setFrameShadow(QFrame::Sunken);

    QHBoxLayout *pLayout = new QHBoxLayout(this);
    setLayout(pLayout);
    pLayout->setContentsMargins(0, 0, 1, 0);
    pLayout->setSpacing(0);

    for (int i = 0; i != QTUTL_IP_SIZE; ++i)
    {
        if (i != 0)
        {
            QLabel *pDot = new QLabel(".", this);
            pLayout->addWidget(pDot);
            pLayout->setStretch(pLayout->count(), 0);
        }

        m_pLineEdit.at(i) = new QLineEdit(this);
        QLineEdit *pEdit = m_pLineEdit.at(i);

        int pixelsWide = pEdit->fontMetrics().horizontalAdvance(QString::number(255));
        // Here is костыль
        pEdit->setMaximumWidth(pixelsWide * 2);
        pEdit->installEventFilter(this);

        pLayout->addWidget(pEdit);
        pLayout->setStretch(pLayout->count(), 1);

        pEdit->setFrame(false);
        pEdit->setAlignment(Qt::AlignCenter);

        QFont font = pEdit->font();
        font.setStyleHint(QFont::Monospace);
        font.setFixedPitch(true);
        pEdit->setFont(font);

        QRegExp rx("^(0|[1-9]|[1-9][0-9]|1[0-9][0-9]|2([0-4][0-9]|5[0-5]))$");
        QValidator *validator = new QRegExpValidator(rx, pEdit);
        pEdit->setValidator(validator);
    }

    //  setMaximumWidth(30 * QTUTL_IP_SIZE);

    connect(this, &IPCtrl::signalTextChanged, this, &IPCtrl::slotTextChanged, Qt::QueuedConnection);
}

IPCtrl::~IPCtrl()
{
}

void IPCtrl::slotTextChanged(QLineEdit *pEdit)
{
    for (unsigned int i = 0; i != QTUTL_IP_SIZE; ++i)
    {
        if (pEdit == m_pLineEdit.at(i))
        {
            if ((pEdit->text().size() == MAX_DIGITS && pEdit->text().size() == pEdit->cursorPosition())
                || (pEdit->text() == "0"))
            {
                // auto-move to next item
                if (i + 1 != QTUTL_IP_SIZE)
                {
                    m_pLineEdit.at(i + 1)->setFocus();
                    m_pLineEdit.at(i + 1)->selectAll();
                }
            }
        }
    }
}

bool IPCtrl::eventFilter(QObject *obj, QEvent *event)
{
    bool bRes = QFrame::eventFilter(obj, event);

    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *pEvent = dynamic_cast<QKeyEvent *>(event);
        if (pEvent)
        {
            for (unsigned int i = 0; i != QTUTL_IP_SIZE; ++i)
            {
                QLineEdit *pEdit = m_pLineEdit[i];
                if (pEdit == obj)
                {
                    switch (pEvent->key())
                    {
                    case Qt::Key_Left:
                        if (pEdit->cursorPosition() == 0)
                        {
                            // user wants to move to previous item
                            movePrevLineEdit(i);
                        }
                        break;

                    case Qt::Key_Right:
                        if (pEdit->text().isEmpty() || (pEdit->text().size() == pEdit->cursorPosition()))
                        {
                            // user wants to move to next item
                            moveNextLineEdit(i);
                        }
                        break;

                    case Qt::Key_0:
                        if (pEdit->text().isEmpty() || pEdit->text() == "0")
                        {
                            pEdit->setText("0");
                            // user wants to move to next item
                            moveNextLineEdit(i);
                        }
                        emit signalTextChanged(pEdit);
                        break;

                    case Qt::Key_Backspace:
                        if (pEdit->text().isEmpty() || pEdit->cursorPosition() == 0)
                        {
                            // user wants to move to previous item
                            movePrevLineEdit(i);
                        }
                        break;

                    case Qt::Key_Comma:
                    case Qt::Key_Period:
                        moveNextLineEdit(i);
                        break;

                    default:
                        emit signalTextChanged(pEdit);
                        break;
                    }
                }
            }
        }
    }

    return bRes;
}

std::array<quint8, IPCtrl::QTUTL_IP_SIZE> IPCtrl::getIP() const
{
    std::array<quint8, QTUTL_IP_SIZE> ipAddr;
    std::transform(m_pLineEdit.cbegin(), m_pLineEdit.cend(), ipAddr.begin(),
        [](const QLineEdit *lineEdit) -> quint8 { return lineEdit->text().toUInt(); });
    return ipAddr;
}

void IPCtrl::setIP(std::array<quint8, IPCtrl::QTUTL_IP_SIZE> ipAddr)
{
    for (auto i = 0; i != QTUTL_IP_SIZE; ++i)
    {
        m_pLineEdit.at(i)->setText(QString::number(ipAddr.at(i)));
    }
}

void IPCtrl::moveNextLineEdit(int i)
{
    if (i + 1 != QTUTL_IP_SIZE)
    {
        m_pLineEdit.at(i + 1)->setFocus();
        m_pLineEdit.at(i + 1)->setCursorPosition(0);
        m_pLineEdit.at(i + 1)->selectAll();
    }
}

void IPCtrl::movePrevLineEdit(int i)
{
    if (i != 0)
    {
        m_pLineEdit.at(i - 1)->setFocus();
        m_pLineEdit.at(i - 1)->setCursorPosition(m_pLineEdit[i - 1]->text().size());
        // m_pLineEdit[i-1]->selectAll();
    }
}