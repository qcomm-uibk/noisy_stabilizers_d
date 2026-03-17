/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/


#include "loggerCout.h"
#include <iostream>


LoggerCout::LoggerCout()
{
}

LoggerCout::~LoggerCout()
{
}

void LoggerCout::WriteLog(string message)
{
    cout << message << std::endl;
}