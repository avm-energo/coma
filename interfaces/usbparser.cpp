#include "usbparser.h"

UsbParser::UsbParser(QObject *parent) : QObject(parent)
{
}

void UsbParser::send(char command, char parameter, QByteArray &ba, qint64 datasize)
{
}
