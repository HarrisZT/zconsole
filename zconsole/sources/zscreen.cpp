/******************************************************************************
* zconsole - sfml based in-game console module 
* Copyright (C) 2020-2021 Zachary T Harris. All Rights Reserved.  
* 
* File: zscreen.cpp
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
#include "zconsoledefs.hpp"
#include "zscreen.hpp"



/* Section 1:
** internal data
******************************************************************************/
#define CELLMATRIX_FRAME_UL   0x00002554 //ascii char- upper left corner 
#define CELLMATRIX_FRAME_UR   0x00002557 //ascii char- upper right corner
#define CELLMATRIX_FRAME_BL   0x0000255A //ascii char- bottom left corner
#define CELLMATRIX_FRAME_BR   0x0000255D //ascii char- bottom right corner
#define CELLMATRIX_FRAME_RGBA 0xFFFFFFFF //frame character color code
#define CELLMATRIX_FRAME_HBAR 0x00002550 //ascii char- horizontal bar
#define CELLMATRIX_FRAME_VBAR 0x00002551 //ascii char- vertical bar

const Zconsole::Zscreen::LineEntry
Zconsole::Zscreen::emptyline;





/* Section 2:
** internal interface functions
******************************************************************************/
void Zconsole::Zscreen::initLineEntry(
	LineEntry*      _line, 
	const zstring&  _string, 
	const zustring& _colors) {

	_line->text = _string;
	_line->rgba = _colors;

	if (_line->text.empty()) {
		_line->text = " "; 
	} 
	_line->rgba.resize(
		_line->text.size(), 
		m_lpconsole->m_textcolor); 
}


const zustring& 
Zconsole::Zscreen::getWideColor(int _index) {
	if (_index < 0) 
		_index = m_widemessage.size() + _index;
	_index += m_firstmsg;
	if (_index < 0 || (unsigned int)_index >= m_widemessage.size())
		return emptyline.rgba;
	return m_widemessage[_index].rgba;
} 


const zstring& 
Zconsole::Zscreen::getWideMessage(int _index) {
	if (_index < 0) 
		_index = m_widemessage.size() + _index;
	_index += m_firstmsg;
	if (_index < 0 || (unsigned int)_index >= m_widemessage.size())
		return emptyline.text;
	return m_widemessage[_index].text;
}  


unsigned int
Zconsole::Zscreen::pushWideMessages(
	const LineEntry& _textline,
	LineBatch*       _widemsgs,
	unsigned int     _width) {

	unsigned int ret       = 0u;
	unsigned int start     = 0u;
	unsigned int charcount = 0u;	

	//push pieces of str if they go over width or if we encounter a newline
	for (unsigned int i = 0u; i < _textline.text.size(); ++i) {
		++charcount;
		if (_textline.text[i] == '\n' || charcount >= _width) {
			if (_textline.text[i] == '\n') --charcount;
			if (_widemsgs) {
				LineEntry line;
				line.text = _textline.text.substr(start, charcount);
				line.rgba = _textline.rgba.substr(start, charcount);
				_widemsgs->push_back(line);
			}
			++ret;
			start = i + 1u;
			charcount = 0u;
		}
	} 
	//push last piece if loop didn't
	if (charcount != 0u) {
		if (_widemsgs) {
			LineEntry line;
			line.text  = _textline.text.substr(start, charcount);
			line.rgba = _textline.rgba.substr(start, charcount);
			_widemsgs->push_back(line);
		}
		++ret;
	}
	return ret;
}  


void 
Zconsole::Zscreen::processLineEcho(
	const zstring&  _string, 
	const zustring& _colors) {

	unsigned int msgOffset;

	LineEntry line;
	initLineEntry(&line, _string, _colors);

	m_message.push_back(line);
	pushWideMessages(
		line, 
		&m_widemessage, 
		ZCONSOLE_WIDTH - 2);

	if (m_message.size() > ZCONSOLE_MAXMSGSAVE) {
		msgOffset = 
			pushWideMessages(
			*m_message.begin(), 0x0, ZCONSOLE_WIDTH - 2);

		m_message.erase(m_message.begin());
		m_widemessage.erase(
			m_widemessage.begin(), 
			m_widemessage.begin() + msgOffset);
	}
	scroll(ZCONSOLE_SCROLLEND); //make this conditional?
	++m_lpconsole->m_pendingUpdates;
}  


Zconsole::Zscreen::Cell*
Zconsole::Zscreen::getCellsAt(unsigned int _coordx, unsigned int _coordy) {
	unsigned int index;
	if (_coordx + 1 < ZCONSOLE_WIDTH &&
		_coordy + 1 < ZCONSOLE_HEIGHT) {

		index = _coordx + ZCONSOLE_WIDTH * _coordy;
		return &m_matrix[index];
	}
	return nullptr; 
} 





/* Section 3:
** public interface functions
******************************************************************************/
void Zconsole::Zscreen::clear() {
	m_firstmsg = 0;
	m_message.clear();
	m_widemessage.clear(); 
}  


void Zconsole::Zscreen::echoLine(const zstring& _string) {
	LineEntry line;
	line.text = _string;
	line.rgba.resize(line.text.size(), m_lpconsole->m_textcolor);
	processLineEcho(line.text, line.rgba);
} 


Zconsole::Zscreen::Cell* const
Zconsole::Zscreen::getCellMatrix() { 
	update();
	if (!m_matrix.empty())
		return &m_matrix[0];
	return nullptr;
} 


void Zconsole::Zscreen::scroll(int _linecount) {
	int lastmsg;
	lastmsg = (ZCONSOLE_HEIGHT - 3) - (int)m_widemessage.size();

	m_firstmsg += _linecount;
	//ensure we go no further than last or first line
	m_firstmsg = std::max<int>(m_firstmsg, lastmsg);
	m_firstmsg = std::min(m_firstmsg, 0);

	++m_lpconsole->m_pendingUpdates;
} 


void Zconsole::Zscreen::update() { 
	static unsigned int lastupdate = 0;
	if (lastupdate == m_lpconsole->m_pendingUpdates)
		return; 

	lastupdate = m_lpconsole->m_pendingUpdates; 

	for (unsigned int i = 1; i < (ZCONSOLE_HEIGHT - 2); ++i) {
		const auto& wstr  = getWideMessage(i - (ZCONSOLE_HEIGHT - 2)); 
		const auto& rgba  = getWideColor(i - (ZCONSOLE_HEIGHT - 2)); 
		auto* const cells = getCellsAt(1u, i);

		for (unsigned int x = 0; x < (ZCONSOLE_WIDTH - 2); ++x) {
			cells[x].mchar = ' ';
			cells[x].color = CELLMATRIX_FRAME_RGBA; 
		}
		for (unsigned int x = 0u; x < wstr.size(); ++x) {
			cells[x].mchar = wstr[x];
			cells[x].color = rgba[x];
		}
	}
	auto* const cells = getCellsAt(1, ZCONSOLE_HEIGHT - 2);

	for (unsigned int x = 0; x < (ZCONSOLE_WIDTH - 2); ++x) {
		cells[x].mchar = ' ';
		cells[x].color = m_lpconsole->m_textcolor;
	}
	for (unsigned int x = 0; x < (ZCONSOLE_WIDTH - 2) &&
		 (m_lpconsole->m_lastlineOffset + x) < 
		m_lpconsole->m_lastline.size(); ++x) {
		cells[x].mchar =
			m_lpconsole->m_lastline[m_lpconsole->m_lastlineOffset + x];
	}
} 


void Zconsole::Zscreen::initialize() {   
	m_lpconsole->m_pendingUpdates  = 1; 
    m_lpconsole->m_lastlineOffset  = 0;  
	m_lpconsole->m_textcolor       = ZCONSOLE_TEXTCOLOR;  

	m_firstmsg = 0;
	m_widemessage.clear();

	for (unsigned int index = 0; index < m_message.size(); ++index) {
		pushWideMessages(
			m_message[index], &m_widemessage, ZCONSOLE_WIDTH - 2);
	}
	++m_lpconsole->m_pendingUpdates;

	/*
	Initialize the screen matrix with empty cells:*/
	m_matrix.assign(ZCONSOLE_TOTALSIZE, Cell());
	 
	/*
	Define top and bottom sides:
	---------------------------*/
	for (unsigned int index = 0; index < ZCONSOLE_WIDTH; ++index) {

		unsigned int top = (index + ZCONSOLE_WIDTH * 0);
		unsigned int bot = (index + ZCONSOLE_WIDTH * (ZCONSOLE_HEIGHT - 1));
		 
		m_matrix[top].mchar = CELLMATRIX_FRAME_HBAR;
		m_matrix[bot].mchar = CELLMATRIX_FRAME_HBAR;
		m_matrix[top].color = CELLMATRIX_FRAME_RGBA;
		m_matrix[bot].color = CELLMATRIX_FRAME_RGBA;
	}
	/*
	Define left and right sides:
	---------------------------*/
	for (unsigned int index = 0; index < ZCONSOLE_HEIGHT; ++index) {

		unsigned int lft = (0 + ZCONSOLE_WIDTH * index);
		unsigned int rgt = (ZCONSOLE_WIDTH - 1 + ZCONSOLE_WIDTH * index);
		 
		m_matrix[lft].mchar = CELLMATRIX_FRAME_VBAR;
		m_matrix[rgt].mchar = CELLMATRIX_FRAME_VBAR;
		m_matrix[lft].color = CELLMATRIX_FRAME_RGBA;
		m_matrix[rgt].color = CELLMATRIX_FRAME_RGBA;
	}
	/*
	Define corners:
	---------------------------*/
	unsigned int ul = 0 + ZCONSOLE_WIDTH * 0;
	unsigned int bl = 0 + ZCONSOLE_WIDTH * (ZCONSOLE_HEIGHT - 1);
	unsigned int br = ZCONSOLE_WIDTH - 1 + ZCONSOLE_WIDTH * (ZCONSOLE_HEIGHT - 1);
	unsigned int ur = ZCONSOLE_WIDTH - 1 + ZCONSOLE_WIDTH * 0;
	 
	m_matrix[ul].mchar = CELLMATRIX_FRAME_UL; //upper left
	m_matrix[bl].mchar = CELLMATRIX_FRAME_BL; //bottom left
	m_matrix[br].mchar = CELLMATRIX_FRAME_BR; //bottom right
	m_matrix[ur].mchar = CELLMATRIX_FRAME_UR; //upper right
} 
/*****************************************************************************/  
//EOF
/*****************************************************************************/  
