/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2011 Morgan Leborgne
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "QProgressIndicator.h"

#include <QPainter>
#include <QTimer>

QProgressIndicator::QProgressIndicator(QWidget *parent)
    : QWidget(parent)
    , m_angle(0)
    , m_delay(40)
    , m_displayedWhenStopped(false)
    , m_color(Qt::black)
    , m_timer(new QTimer(this))
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setFocusPolicy(Qt::NoFocus);
    m_timer->setSingleShot(false);
    m_timer->setInterval(m_delay);
    connect(m_timer, &QTimer::timeout, this, &QProgressIndicator::timerEvent);
}

int QProgressIndicator::animationDelay() const noexcept
{
    return m_delay;
}

bool QProgressIndicator::isAnimated() const noexcept
{
    return m_timer->isActive();
}

void QProgressIndicator::setDisplayedWhenStopped(bool state) noexcept
{
    m_displayedWhenStopped = state;
    update();
}

bool QProgressIndicator::isDisplayedWhenStopped() const noexcept
{
    return m_displayedWhenStopped;
}

void QProgressIndicator::startAnimation()
{
    m_angle = 0;
    if (!isAnimated())
        m_timer->start();
}

void QProgressIndicator::stopAnimation()
{
    if (isAnimated())
        m_timer->stop();
    update();
}

void QProgressIndicator::setAnimationDelay(int delay) noexcept
{
    m_delay = delay;
    m_timer->setInterval(m_delay);
}

const QColor &QProgressIndicator::color() const noexcept
{
    return m_color;
}

void QProgressIndicator::setColor(const QColor &color) noexcept
{
    m_color = color;
    update();
}

QSize QProgressIndicator::sizeHint() const noexcept
{
    return QSize(20, 20);
}

int QProgressIndicator::heightForWidth(int w) const noexcept
{
    return w;
}

void QProgressIndicator::timerEvent()
{
    m_angle = (m_angle + 30) % 360;
    update();
}

void QProgressIndicator::paintEvent(QPaintEvent * /*event*/)
{
    if (!m_displayedWhenStopped && !isAnimated())
        return;

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    int width = std::min(this->width(), this->height());
    int outerRadius = (width - 1) * 0.5;
    int innerRadius = (width - 1) * 0.5 * 0.38;
    int capsuleHeight = outerRadius - innerRadius;
    int capsuleWidth = (width > 32) ? capsuleHeight * .23 : capsuleHeight * .35;
    int capsuleRadius = capsuleWidth / 2;

    for (int i = 0; i < 12; i++)
    {
        QColor color = m_color;
        color.setAlphaF(1.0f - (i / 12.0f));
        p.setPen(Qt::NoPen);
        p.setBrush(color);
        p.save();
        p.translate(rect().center());
        p.rotate(m_angle - i * 30.0f);
        p.drawRoundedRect(-capsuleWidth * 0.5, -(innerRadius + capsuleHeight), capsuleWidth, capsuleHeight,
            capsuleRadius, capsuleRadius);
        p.restore();
    }
}
