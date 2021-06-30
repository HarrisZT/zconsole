/******************************************************************************
* zconsole - sfml based in-game console module 
* Copyright (C) 2020-2021 Zachary T Harris. All Rights Reserved.  
* 
* File: zcursor.h
* Desc: interface for console cursor
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
#ifndef __ZCURSOR_H__
#define __ZCURSOR_H__

#include "ztimers.h"
#include "zconsole/zconsole.h"



class Zconsole::Zcursor final  {
public: 
	Zcursor(Zconsole* const _lpconsole) :
		m_lpconsole(_lpconsole) {}
	~Zcursor() {}	

	/*
	Initialize the console cursor interface.*/
	void initialize();

	/*
	This function resets the vertices used to create the
	cursor within the cell matrix. It is called during updates.
	@_coordx  : x coordinate to position the cursor
	@_coordy  : y coordinate to position the cursor
	@_vertices: the console vertices*/
	void reset(float _coordx, float _coordy);

	/*
	Returns the current x coordinate of the cursor*/
	int getPosition() const;

	/*
	Specify the x coordinate for the console cursor
	@_coordx: the x coordinate*/
	void setPosition(int _coordx);

	/*
	Insert a character in the cell matrix
	@_char: the character to be inserted*/
	void addChar(char _char);

	/*
	Delete the character in the cell at the current coordinate
	and returns to previous x coordinate*/
	void backspace();

	/*
	Delete the character in the cell at the current coordinate*/
	void erase();

	/*
	Move cursor by given amount of characters-
	will be clipped to [0, lastlinesize] automatically,
	pass CONSOLE_CURSOR_END or CONSOLE_CURSOR_HOME
	to go to lastline or 0 respectively. use this for Home/End key
	press and for Left/Right arrow keys*/
	void move(int _offset);

	/*
	Move cursor by one word left or right.
	@_direction: +1 = right, -1 = left*/
	void moveOneWord(int _direction);

	/*
	Update the blinking animation for the cursor.
	@_vertices: the console vertices*/
	void update();

private:
	void validatePosition();
	int find1stSkipChar(const zstring& _line, int _iter, int _start);

	int             m_xcoord;  //x coordinate
    Ztimer          m_timer;   //blink timer 
    bool            m_visible; //visibility state
    sf::Color       m_color;   //color (blink state)
    sf::Glyph       m_glyph;   //glyph data 
    unsigned int    m_uchar;   //unicode key for glyph
    int             m_index;   //vertex index 
	Zconsole* const m_lpconsole;
};
/*****************************************************************************/  
#endif //EOF
/*****************************************************************************/  