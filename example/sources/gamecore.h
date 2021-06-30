/******************************************************************************
* zconsole - sfml based in-game console module 
* Copyright (C) 2020-2021 Zachary T Harris. All Rights Reserved. 
* Example Program
* 
* File: gamecore.h
* Desc: central game core, runs the show
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
#ifndef __GAMECORE_H__
#define __GAMECORE_H__  

#include "steptimer.h"  
#include "zconsole/zconsole.h"



class GameCore final : sf::NonCopyable { 
public:
	GameCore();
	~GameCore(); 
	 
	void run(); 

private:  
	/*
	Internal functions:                 */
	//----------------------------------//
	zstring getWorkingDirectory() const;
	void initialize(); 
	void updateFramerateSetting();
	void onEvent(const sf::Event& _event); 
	void processRendering();
	void renderDebugText(
		sf::RenderTarget& _rendertarget,
		sf::RenderStates  _renderstates = sf::RenderStates::Default);		
	
	/*
	Member variables:                   */
	//----------------------------------//
	Zconsole*        m_console; 
	bool             m_consoleIsOpen;  
	float            m_framerate;
	bool             m_isRunning;  
	unsigned int     m_linespace; 
	sf::Color        m_refreshColor;
	bool             m_showStats;     
	StepTimer        m_steptimer; 
	sf::Text         m_text; 
	bool             m_vsyncEnabled;
	sf::RenderWindow m_window;

	/*
	Console Commands:                   */
	//----------------------------------//
	void initConsoleCommands();
	void cmd_showfps  (zstrings _args);
	void cmd_shutdown (zstrings _args); 
	void cmd_vsync    (zstrings _args);
	void cmd_title    (zstrings _args); 
	void cmd_setfps   (zstrings _args);
};
/*****************************************************************************/  
#endif //EOF
/*****************************************************************************/  
