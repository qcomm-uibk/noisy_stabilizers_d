/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/

#ifndef LOGGERFILE_H
#define LOGGERFILE_H

#include <iostream>
#include <fstream>


#include "logger.h"

#pragma once

class LoggerFile : public Logger
{
public:
    LoggerFile();
    ~LoggerFile();

    void WriteLog(string logMessage) override;

private:
    string  mLogFileName = "nsfd.log";
    ofstream mLogFile;
};

#endif