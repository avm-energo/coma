/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef FLOWLAYOUT_H
#define FLOWLAYOUT_H

#include <QLayout>
#include <QRect>
#include <QStyle>

class FlowLayout : public QLayout
{
    Q_OBJECT
public:
    explicit FlowLayout(QWidget *parent, int margin = -1, int hSpacing = -1, int vSpacing = -1)
        : QLayout(parent), m_hSpace(hSpacing), m_vSpace(vSpacing)
    {
        setContentsMargins(margin, margin, margin, margin);
    }
    explicit FlowLayout(int margin = -1, int hSpacing = -1, int vSpacing = -1) : m_hSpace(hSpacing), m_vSpace(vSpacing)
    {
        setContentsMargins(margin, margin, margin, margin);
    }
    ~FlowLayout() override
    {
        QLayoutItem *item;
        while ((item = FlowLayout::takeAt(0)))
            delete item;
    }

    void addItem(QLayoutItem *item) override
    {
        itemList.append(item);
    }
    int horizontalSpacing() const
    {
        if (m_hSpace >= 0)
        {
            return m_hSpace;
        }
        else
        {
            return smartSpacing(QStyle::PM_LayoutHorizontalSpacing);
        }
    }
    int verticalSpacing() const
    {
        if (m_vSpace >= 0)
        {
            return m_vSpace;
        }
        else
        {
            return smartSpacing(QStyle::PM_LayoutVerticalSpacing);
        }
    }
    Qt::Orientations expandingDirections() const override
    {
        return {};
    }
    bool hasHeightForWidth() const override
    {
        return true;
    }
    int heightForWidth(int width) const override
    {
        int height = doLayout(QRect(0, 0, width, 0), true);
        return height;
    }
    int count() const override
    {
        return itemList.size();
    }
    QLayoutItem *itemAt(int index) const override
    {
        return itemList.value(index);
    }
    QSize minimumSize() const override;
    void setGeometry(const QRect &rect) override
    {
        QLayout::setGeometry(rect);
        doLayout(rect, false);
    }
    QSize sizeHint() const override
    {
        return minimumSize();
    }
    QLayoutItem *takeAt(int index) override
    {
        if (index >= 0 && index < itemList.size())
            return itemList.takeAt(index);
        return nullptr;
    }

private:
    int doLayout(const QRect &rect, bool testOnly) const;
    int smartSpacing(QStyle::PixelMetric pm) const;

    QList<QLayoutItem *> itemList;
    int m_hSpace;
    int m_vSpace;
};

#endif // FLOWLAYOUT_H
