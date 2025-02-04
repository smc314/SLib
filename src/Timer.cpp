/*
 * Copyright (c) 2001,2002 Steven M. Cherry. All rights reserved.
 *
 * This file is a part of slib - a c++ utility library
 *
 * The slib project, including all files needed to compile 
 * it, is free software; you can redistribute it and/or use it and/or modify 
 * it under the terms of the GNU Lesser General Public License as published by 
 * the Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * 
 * You should have received a copy of the GNU Lesser General Public License 
 * along with this program.  See file COPYING for details.
 */

#include "Timer.h"
#include <chrono>

using namespace SLib;

Timer::Timer() : 
	m_start_time(std::chrono::high_resolution_clock::now()),
	m_end_time(m_start_time)
{
}

Timer::~Timer() = default;

void Timer::Start()
{
	m_start_time = std::chrono::high_resolution_clock::now();
}

void Timer::Finish()
{
	m_end_time = std::chrono::high_resolution_clock::now();
}

double Timer::Duration() const
{
	auto duration = m_end_time - m_start_time;
	return std::chrono::duration<double>(duration).count();
}

uint64_t Timer::GetCycleCount()
{
	auto now = std::chrono::high_resolution_clock::now();
	auto duration = now.time_since_epoch();
	return std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
}


