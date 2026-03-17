/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/


#ifndef NULLLOGGER_H
#define NULLLOGGER_H

#include "logger.h"

#pragma once

class NullLogger : public Logger
{
public:
    NullLogger();
    ~NullLogger();

    void WriteLog(string logMessage) override;
    
private:

};

#endif