/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/

#ifndef LOGGERCOUT_H
#define LOGGERCOUT_H

#include "logger.h"
#include <string>

#pragma once

class LoggerCout : public Logger
{
public:
    LoggerCout();
    ~LoggerCout();

    void WriteLog(string logMessage) override;

private:

};

#endif