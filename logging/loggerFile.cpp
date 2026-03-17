/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/

#include "loggerFile.h"

LoggerFile::LoggerFile() 
    : mLogFile{mLogFileName}
{
}

LoggerFile::~LoggerFile()
{
}

void LoggerFile::WriteLog(string message)
{
    mLogFile << message;
    mLogFile << std::endl;
}