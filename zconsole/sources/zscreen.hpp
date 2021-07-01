/******************************************************************************
* zconsole - sfml based in-game console module 
* Copyright (C) 2020-2021 Zachary T Harris. All Rights Reserved.  
* 
* File: zscreen.hpp
* Desc: interface for console screen
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
#ifndef __ZSCREEN_HPP__
#define __ZSCREEN_HPP__

#include "zconsole/zconsole.hpp"  



class Zconsole::Zscreen final  {
public:
	/*
    A single cell in the screen matrix
    representing a character and color*/
    struct Cell {
    	unsigned int mchar;
    	unsigned int color;
    };
    /*
    A line that was entered into the console*/
    struct LineEntry {
    	zstring  text;
    	zustring rgba;
    };
    /*
    a vector of LineEntry structures*/
    typedef std::vector<LineEntry> LineBatch;
	static const LineEntry emptyline;

public: 
	Zscreen(Zconsole* const _lpconsole) :
		m_lpconsole(_lpconsole) {}
	~Zscreen() {}	

	/*
	Initialize the console screen (cell matrix)*/
	void initialize();

	/*
	Updates the cell matrix.
	Must be called when updating the screen*/
	void update();

	/*
	Clear the console screen*/
	void clear();

	/*
	Print a string to the console screen
	@_string: the string to print*/
	void echoLine(const zstring& _string);

	/*
	Get a pointer to the screen cell matrix*/
	Cell* const getCellMatrix();

	/*
	Scroll the screen by a given number of lines,
	use this for ctrl + pageup/pagedown/up/down/home/end
	@_linecount: the number of lines to scroll*/
	void scroll(int _linecount);

private:
	void initLineEntry(
		LineEntry*      _line,
		const zstring&  _string,
		const zustring& _colors);

	unsigned int pushWideMessages(
		const LineEntry& _textline,
		LineBatch*       _widemsgs,
		unsigned int     _width);

	void processLineEcho(
		const zstring&  _string, 
		const zustring& _colors); 

	Cell* getCellsAt(unsigned int _coordx, unsigned int _coordy); 

	const zustring& getWideColor(int _index); 

	const zstring& getWideMessage(int _index); 

	int               m_firstmsg;
	std::vector<Cell> m_matrix;
	LineBatch         m_message;
	LineBatch         m_widemessage;
	Zconsole* const   m_lpconsole;
};
/*****************************************************************************/  
#endif //EOF
/*****************************************************************************/  
