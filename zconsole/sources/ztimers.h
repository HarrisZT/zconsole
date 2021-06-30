/******************************************************************************
* zconsole - sfml based in-game console module 
* Copyright (C) 2020-2021 Zachary T Harris. All Rights Reserved.  
* 
* File: ztimers.h
* Desc: utility timiing 
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
#ifndef __ZTIMERS_H__
#define __ZTIMERS_H__
		    
#include "zconsole/ztypes.h"



class Ztimer {
public:  
	inline void restart() { 
		m_clock.restart();
	}
	inline int elapsed() const {
		return m_clock.getElapsedTime().asMilliseconds();
	}
private:
	sf::Clock m_clock;
};





class ZtimedInputKey { 
public:
	ZtimedInputKey() {}
	ZtimedInputKey(sf::Keyboard::Key _key) : m_key(_key) {}

	void setKey(sf::Keyboard::Key _key) { m_key = _key; }

	bool isPressed() {
		if (!sf::Keyboard::isKeyPressed(m_key)) {
			m_timer[0].restart();
		}
		else if (m_timer[0].elapsed() >= 500) {
			if (m_timer[1].elapsed() >= 250) {
				m_timer[1].restart();
				return true;
			}
		}
		return false;
	}
private:
	sf::Keyboard::Key m_key;
	Ztimer            m_timer[2];
}; 
/*****************************************************************************/  
#endif //EOF
/*****************************************************************************/  
