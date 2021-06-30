/******************************************************************************
* zconsole - sfml based in-game console module 
* Copyright (C) 2020-2021 Zachary T Harris. All Rights Reserved. 
* Example Program
* 
* File: gamecore.cpp
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
#include <sys/types.h>
#include <sys/stat.h> 
#include <iostream>
#include <algorithm>
#include <sstream>
#include <fstream> 

#if defined(_WIN32)
#  include <Windows.h>
#  include <shlobj.h> 
#  if defined(_MSC_VER)
#    include <direct.h>  
#  endif 
#else
#  include <libgen.h>
#  include <dirent.h> 
#  include <fcntl.h>
#  include <unistd.h>
#  include <sys/stat.h> 
#  if defined(__linux__)
#    define MAX_PATH 512
#    include <string.h>
#    include <stdlib.h> 
#  elif defined(__APPLE__)
#    define MAX_PATH PATH_MAX
#    include <CoreServices/CoreServices.h>
#    include "../detail/ResourcePath.hpp"
#  endif 
#endif  
#include "steptimer.h"  
#include "gamecore.h" 
 
 



/* Section 1:
** Public Interface
******************************************************************************/ 
GameCore::~GameCore() {} 
GameCore::GameCore() :  
	m_console       (Zconsole::getSingleton()), 
	m_consoleIsOpen (false),
	m_isRunning     (false),
    m_showStats     (false) {}


void GameCore::run() { 
	initialize();
	while (m_isRunning == true) { 
		float delta = m_steptimer.getDelta();
		m_steptimer.restart();

		sf::Event event; 
		while (m_window.pollEvent(event)) {
			onEvent(event);
		}
		while (m_steptimer.hasTimeRemaining()) {
			m_steptimer.update();
			if (m_consoleIsOpen) {
				if (!m_console->onUpdate(delta)) {
					m_console->close();
					m_consoleIsOpen = false;
				}
			}
		}
		processRendering();
	} 
} 





/* Section 2:
** Internal Interface
******************************************************************************/ 
zstring GameCore::getWorkingDirectory() const {
	zstring dir;
#ifdef _WIN32
	TCHAR output[FILENAME_MAX];
	if (GetCurrentDirectory(FILENAME_MAX, output) != 0) {
		dir = zstring(output);
		std::replace(dir.begin(), dir.end(), '\\', '/');
	}
#else 
	char output[FILENAME_MAX];
	if (getcwd(output, FILENAME_MAX) != 0)  
	    dir = zstring(output);
#endif  
	dir += "/";
	return dir;
}


void GameCore::updateFramerateSetting() {
	if (m_vsyncEnabled) {
		m_window.setFramerateLimit(0);
		m_window.setVerticalSyncEnabled(true);
	}
	else {
		m_window.setFramerateLimit(m_framerate);
		m_window.setVerticalSyncEnabled(false);
	}
}


void GameCore::initialize() {
	if (!m_isRunning) {   
		m_showStats    = true;  
		m_framerate    = (float)60; 
		m_vsyncEnabled = true;
		m_refreshColor = sf::Color::Black;

		m_steptimer.init();

		{//initialize window:
			sf::VideoMode mode;
			mode = sf::VideoMode::getDesktopMode(); 
			mode.width /= 2;
			mode.height /= 2;
			m_window.create(mode, "ConsoleExample", sf::Style::Default); 
			m_window.setKeyRepeatEnabled(false);
			updateFramerateSetting();
		}
		{//initialize console:
			zstring dir = getWorkingDirectory();
			bool bResult = m_console->initialize(dir);
			if (bResult) {
				initConsoleCommands();
				m_console->open();
				m_consoleIsOpen = true;
			} 
		}
		{//initialize debug text:
			auto* font = m_console->getFont();
			m_linespace = (unsigned int)font->getLineSpacing(64);
			m_text.setFont(*font);
			m_text.setCharacterSize(64);
			m_text.setFillColor(sf::Color::Green);
		}
		m_isRunning = true;  
	} 
}


void GameCore::onEvent(const sf::Event& _event) {
	switch (_event.type) { 
	case sf::Event::LostFocus: 
		m_window.requestFocus();		
		break; 
	case sf::Event::Closed:
		cmd_shutdown({});
		return;
	case sf::Event::KeyPressed:
		switch (_event.key.code) {
		case sf::Keyboard::Tilde:
			if (!m_consoleIsOpen) {
				m_consoleIsOpen = true;
				m_console->open();
			} break; 
		}
	} 
	if (m_consoleIsOpen)
		m_console->onEvent(_event);
} 

 
void GameCore::processRendering() {
	if (m_consoleIsOpen)
		m_console->onPresent(m_window);
	else { 
		if (m_showStats)
			renderDebugText(m_window);
	}
	m_window.setView(m_window.getDefaultView()); 
	m_window.display();
	m_window.clear(m_refreshColor);
}


void GameCore::renderDebugText(
	sf::RenderTarget& _rendertarget,
	sf::RenderStates  _renderstates) {

	sf::Vector2f pos = { 5, 5 };

	const auto  data = m_steptimer.data();
	const auto& view = _rendertarget.getView();
	const auto  size = _rendertarget.getSize();
	const auto  rect = sf::FloatRect(0.f, 0.f, (float)size.x, (float)size.y);

	auto transform = sf::Transform::Identity;
	transform.scale(0.25f, 0.25f);
	_renderstates.transform *= transform;
	_rendertarget.setView(sf::View(rect)); 
	
	m_text.setFillColor(sf::Color::Green);

	for (const auto& it : data) {
		m_text.setString(it);
		m_text.setPosition(floor(pos.x), floor(pos.y));
		pos.y += m_linespace;
		_rendertarget.draw(m_text, _renderstates);
	} 
	pos.y += m_linespace * 4;
	m_text.setString(
		"Console window is currently closed. Press tilde key to open it");
	m_text.setPosition(floor(pos.x), floor(pos.y));
	m_text.setFillColor(sf::Color::White);
	_rendertarget.draw(m_text, _renderstates);

	_rendertarget.setView(view);
} 
 



 
/* Section 3:
** Console Interface 
******************************************************************************/ 
void GameCore::cmd_showfps(zstrings _args) {
	if (!_args.empty()) {
		switch (Zconsole_ArgToBool(_args[0])) {
		case true:  m_showStats = true;  break;
		case false: m_showStats = false; break;
		case -1:
			m_console->print("invalid argument. must be boolean");
			return;
		}
	}
}


void GameCore::cmd_shutdown(zstrings _args) { 
	m_isRunning = false;
}


void GameCore::cmd_vsync(zstrings _args) {
	if (!_args.empty()) {
		switch (Zconsole_ArgToBool(_args[0])) {
		case 1:  m_vsyncEnabled = true;  break;
		case 0:  m_vsyncEnabled = false; break;
		case -1: 
			m_console->print("invalid argument. must be boolean");
			return;
		}
		updateFramerateSetting(); 
	}
}


void GameCore::cmd_title(zstrings _args) {
	if (!_args.empty())
		m_window.setTitle(_args[0]);
}


void GameCore::cmd_setfps(zstrings _args) {
	if (!_args.empty()) {
		float fps;
		std::istringstream str;
		str = std::istringstream(_args[0]);
		if (str >> fps) {
			floor(fps);
			if (fps < 0) {
				m_console->print("invalid value: must be unsigned int");
				return;
			}
			m_framerate = fps;
			if (m_vsyncEnabled) {
				m_console->print("note that vsync is currently enabled.");
				m_console->print("this function will not take effect");
                m_console->print("until vsync is disabled");
			}
			updateFramerateSetting();
		}
	}
}
	
 


void GameCore::initConsoleCommands() {
#define BIND(x) \
std::bind(&GameCore::x, this, std::placeholders::_1) 
	zcommand command;
	{
		command.func = BIND(cmd_showfps);
		command.name = "showfps";
		command.help = {
			"show or hide frames per second",
			"showfps [bool] - (bool) a specifying boolean value",
			"                 value can be any of the following:",
			" true, false, 1, 0, on, off",
			"example: \"showfps true\" will enable fps rendering",
		};
		m_console->insertCommand(command);
	}
	{
		command.func = BIND(cmd_shutdown);
		command.name = "shutdown";
		command.help = {
			"terminates the program (both the console and the game)",
			"and return to operating system",
			"",
			"example: \"shutdown\"",
		};
		m_console->insertCommand(command);
	}
	{
		command.func = BIND(cmd_vsync);
		command.name = "vsync";
		command.help = {
			"enable or disable vertical syncronization.", 
			"",
			"example: \"vsync true\" will enable vsync",
		};
		m_console->insertCommand(command);
	}
	{
		command.func = BIND(cmd_title);
		command.name = "title";
		command.help = {
			"specifies a new string for the window titlebar.", 
			"",
			"example: \"title Game\" will set the titlebar to \"Game\"",
		};
		m_console->insertCommand(command);
	}
	{
		command.func = BIND(cmd_setfps);
		command.name = "setfps";
		command.help = {
			"specifies the framerate the game will run at.", 
			"",
			"example: \"setfps 60\" will set the framerate to 60",
		};
		m_console->insertCommand(command);
	}
#undef BIND
}
/*****************************************************************************/  
//EOF
/*****************************************************************************/ 