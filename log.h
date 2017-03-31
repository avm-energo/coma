/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2016  <copyright holder> <email>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

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
    void Info(QByteArray &ba);
    void intvarvalue(const QString &var, int value);
    void WriteFile(const QString &Prepend, const QString &msg);
    
private:
    QString LogFile;
    bool CanLog;
    QFile *fp;

    void CheckAndGz();
};

#endif // LOG_H
