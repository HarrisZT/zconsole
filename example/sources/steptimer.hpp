/******************************************************************************
* zconsole - sfml based in-game console module 
* Copyright (C) 2020-2021 Zachary T Harris. All Rights Reserved. 
* Example Program
* 
* File: steptimer.hpp 
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
#ifndef __STEPTIMER_HPP__
#define __STEPTIMER_HPP__

#include "zconsole/ztypes.hpp"



class StepTimer final : sf::NonCopyable {
public: 
	/*
	Initialize the steptimer subsystem.
	This function must be called before using the interface.
	@_fps  : specified desired framerate (60 default)*/
	void init(unsigned int _fps = 60);
	 
	/*
	Specify the application's target framerate
	@_targetFps: the framerate (60 default)*/
	void setTargetFps(unsigned int _targetFps);

	/*
	Get the fixed logical timestep value*/
	float getDelta() const;

	/*
	This function should be called before the start of the
	gameloop, before logical updates are initiated.*/
	void restart();

	/*
	Reports whether there is time remaining for additional logic updates.
	If this functions returns true, then the gameloop should iterate 
	once more, otherwise the gameloop should break from logical updates.
	@return: true if time remaining, else false*/
	bool hasTimeRemaining() const;

	/*
	Update time remaining this cycle.
	The main gameloop should call this function inside
	the logical updates section of code. */
	void update();

	/*
	Returns timing data as a string vector.
	This data can be used for displaying debug info to the screen */
	zstrings data() const;

private:
	typedef std::vector<sf::Time> timelist;
    typedef timelist::size_type   tsizetype;  
    
    float      m_tdelta;
	sf::Clock  m_clock[2]; 
	float      m_fpsAverage;
    float      m_accumulator; 
    sf::Time   m_currentTime; 
    float      m_totalRuntime;
    float      m_timeElapsed;     
    sf::Time   m_sampleAccumulator;
    timelist   m_sampleData;
    tsizetype  m_sampleIndex;  
    float      m_frequencyMinimum;
    float      m_frequencyMaximum;
    float      m_frequencyAverage;
    float      m_frequencyCurrent;
    sf::Uint64 m_frequencyElapsed; 
    sf::Time   m_timerangeMinimum;
    sf::Time   m_timerangeMaximum;
    sf::Time   m_timerangeAverage;
    sf::Time   m_timerangeCurrent;
    sf::Time   m_timerangeElapsed;  
}; 
/*****************************************************************************/  
#endif //EOF
/*****************************************************************************/
