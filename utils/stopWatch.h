/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/


#ifndef STOPWATCH_H
#define STOPWATCH_H

#include <chrono>

#pragma once

using namespace std;

class StopWatch
{
public:
    StopWatch();
    ~StopWatch();

    void start();
    void stop();

    long long asMicroseconds();

private:
    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;
};

#endif