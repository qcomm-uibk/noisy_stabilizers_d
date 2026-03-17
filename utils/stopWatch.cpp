/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/


#include "stopWatch.h"

StopWatch::StopWatch()
{
}

StopWatch::~StopWatch()
{
}

void StopWatch::start()
{
    this->begin = std::chrono::steady_clock::now();
}

void StopWatch::stop()
{
    this->end = std::chrono::steady_clock::now();
}

long long StopWatch::asMicroseconds()
{
    return std::chrono::duration_cast<std::chrono::microseconds>(this->end - this->begin).count();
}