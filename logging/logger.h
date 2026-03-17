/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/

#ifndef LOGGER_H
#define LOGGER_H

#include <string>

#pragma once

using namespace std;

class Logger
{
public:
    Logger();
    ~Logger();

    virtual void WriteLog(string logMessage) = 0;

private:

};

#endif