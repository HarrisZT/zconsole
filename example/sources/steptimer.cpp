/******************************************************************************
* zconsole - sfml based in-game console module 
* Copyright (C) 2020-2021 Zachary T Harris. All Rights Reserved. 
* Example Program
* 
* File: steptimer.cpp 
* Desc: interface for tracking framerate at a fixed timestep
*******************************************************************************


This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
******************************************************************************/ 
#include <sstream>
#include <iomanip> 
#include "steptimer.h"

#define SAMPLEDEPTH      100
#define GetSampleDepth() ((sf::Int64)(m_sampleData.size()))
#define NumericLimits(t) ((std::numeric_limits<t>::max)())
   



 
void StepTimer::init(unsigned int _fps) { 
	setTargetFps(_fps); 

	m_fpsAverage = 0.f;	 

	m_accumulator  = 0.f;
	m_totalRuntime = 0.f;
	m_timeElapsed  = 0.f;	 
	m_currentTime  = m_clock[1].getElapsedTime();

	m_sampleData.resize(SAMPLEDEPTH);

	m_frequencyMinimum = NumericLimits(float);
	m_timerangeMinimum = sf::microseconds(NumericLimits(sf::Int64));
	 
}  

 
void StepTimer::setTargetFps(unsigned int _fps) {
	m_tdelta = (float)(1.f / _fps);
}


float StepTimer::getDelta() const {
	return m_tdelta;
}


bool StepTimer::hasTimeRemaining() const {
	return m_accumulator >= m_tdelta;
}


void StepTimer::update() {
	m_accumulator -= m_tdelta;
	m_totalRuntime += m_tdelta;
} 
 

void StepTimer::restart() {
	m_timerangeCurrent = m_clock[0].getElapsedTime();

	m_sampleAccumulator -= m_sampleData[m_sampleIndex];
	m_sampleData[m_sampleIndex] = m_timerangeCurrent;
	m_sampleAccumulator += m_timerangeCurrent;
	m_timerangeElapsed += m_timerangeCurrent;

	if (++m_sampleIndex >= m_sampleData.size()) 
		m_sampleIndex = 0; 
	if (m_timerangeCurrent != sf::microseconds(0))  
		m_frequencyCurrent = 1.0F / m_timerangeCurrent.asSeconds();
	 
	if (m_sampleAccumulator != sf::microseconds(0)) {
		float depth = (float)GetSampleDepth();
		m_frequencyAverage = depth / m_sampleAccumulator.asSeconds();
		m_fpsAverage = (float)(m_frequencyAverage);
	} 
	sf::Int64 depth    = GetSampleDepth();
	sf::Int64 smoothMs = m_sampleAccumulator.asMicroseconds() / depth;

	m_timerangeAverage = sf::microseconds(smoothMs);

	if (m_frequencyCurrent < m_frequencyMinimum)
		m_frequencyMinimum = m_frequencyCurrent;
	if (m_frequencyCurrent > m_frequencyMaximum)
		m_frequencyMaximum = m_frequencyCurrent;
	if (m_timerangeCurrent < m_timerangeMinimum)
		m_timerangeMinimum = m_timerangeCurrent;
	if (m_timerangeCurrent > m_timerangeMaximum)
		m_timerangeMaximum = m_timerangeCurrent;

	++m_frequencyElapsed;
	m_clock[0].restart();

	auto newtime = m_clock[1].getElapsedTime();
	float frametime = 
		newtime.asSeconds() - 
		m_currentTime.asSeconds();
	m_currentTime = newtime;
	m_accumulator += frametime; 
} 


zstrings StepTimer::data() const { 
	zstrings dataOut;

	static auto	FormatText = [](
		const zstring& _name,
		const zstring& _resolution,
		const auto&    _value)->zstring {

		std::ostringstream os;
		os.precision(4);
		os << std::left << std::setw(5);
		os << _name << " : ";
		os << std::setw(5);
		if (!_value)
			os << "null";
		else os << _value << " " << _resolution;
		return os.str(); 
	}; 

#define pushdata(s1, s2, v) \
    dataOut.push_back(FormatText(s1, s2, v)) 

	pushdata("TIME",  "(sec)", m_timerangeElapsed.asSeconds());
	pushdata("FRAME", "",      m_frequencyElapsed); 
	pushdata("FPS",   "",      m_frequencyCurrent);
	pushdata(" min.", "",      m_frequencyMinimum);
	pushdata(" avg.", "",      std::abs(m_fpsAverage));
	pushdata(" max.", "",      m_frequencyMaximum);
	pushdata("DELTA", "(ms)",  m_timerangeCurrent.asMilliseconds());
	pushdata(" min.", "(ms)",  m_timerangeMinimum.asMilliseconds());
	pushdata(" avg.", "(ms)",  m_timerangeAverage.asMilliseconds());
	pushdata(" max.", "(ms)",  m_timerangeMaximum.asMilliseconds()); 

#undef pushdata 
	return dataOut;
}
/*****************************************************************************/  
//EOF
/*****************************************************************************/