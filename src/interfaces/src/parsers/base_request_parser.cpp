#pragma once

#include "interfaces/parsers/base_request_parser.h"

namespace Interface
{

BaseRequestParser::BaseRequestParser(QObject *parent) : QObject(parent)
{
}

} // namespace Interface
