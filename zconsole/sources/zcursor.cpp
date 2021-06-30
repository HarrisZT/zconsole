/******************************************************************************
* zconsole - sfml based in-game console module 
* Copyright (C) 2020-2021 Zachary T Harris. All Rights Reserved.  
* 
* File: zcursor.cpp
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
#include "zconsoledefs.h" 
#include "zhelperfuncs.h"
#include "zcursor.h"



int Zconsole::Zcursor::find1stSkipChar(
	const zstring& _line,
	int            _iter,
	int            _start) {

	int  index, linesize;
	bool foundnonskip;

	index        = _start + _iter;
	linesize     = (int)_line.size();
	foundnonskip = false;

	static auto	IgnoreChar = [](char _char)->bool {
		return strchr(" ,.;()[]{}:'\"", _char) != 0;
	};
	for (; 0 <= index && index < linesize; index += _iter) {
		if (foundnonskip) {
			if (IgnoreChar(_line[index]))
				return index;
		}
		else if (!IgnoreChar(_line[index]))
			foundnonskip = true;
	}
	return -1;
}


void Zconsole::Zcursor::validatePosition() {
	if ((unsigned int)m_xcoord <= m_lpconsole->m_lastlineOffset) {
		m_lpconsole->m_lastlineOffset = m_xcoord - 1;
	}
	while ((unsigned int)m_xcoord >
		m_lpconsole->m_lastlineOffset + ZCONSOLE_WIDTH - 2) {
		++m_lpconsole->m_lastlineOffset;
	}
} 


void Zconsole::Zcursor::initialize() {  
    m_color   = Zconsole_GetColorFromUint(m_lpconsole->m_textcolor);
    m_xcoord  = 1;
    m_visible = true;
    m_index   = 0; 
    m_uchar   = ZCONSOLE_CURSORGLYPH; 
}


void Zconsole::Zcursor::reset(float _coordx, float _coordy) {  
	float px, py, pr, pb;

	auto& vertices = m_lpconsole->m_vertices; 

	m_index = vertices.size() - 1;
	m_color = Zconsole_GetColorFromUint(m_lpconsole->m_textcolor);
	m_glyph = m_lpconsole->m_font->getGlyph(m_uchar, ZCONSOLE_FONTSIZE, false);
	 
	px = _coordx + m_glyph.bounds.left;
	py = _coordy + m_glyph.bounds.top;
	pr = px + m_glyph.bounds.width;
	pb = py + m_glyph.bounds.height;

	vertices.push_back(sf::Vertex({ px, py }, m_color, { 1.f, 1.f }));
	vertices.push_back(sf::Vertex({ pr, py }, m_color, { 1.f, 1.f }));
	vertices.push_back(sf::Vertex({ px, pb }, m_color, { 1.f, 1.f }));
	vertices.push_back(sf::Vertex({ px, pb }, m_color, { 1.f, 1.f }));
	vertices.push_back(sf::Vertex({ pr, py }, m_color, { 1.f, 1.f }));
	vertices.push_back(sf::Vertex({ pr, pb }, m_color, { 1.f, 1.f }));
}


void Zconsole::Zcursor::addChar(char _char) { 
	if (_char >= ' ' && _char < CHAR_MAX) {
		m_lpconsole->m_lastline.insert(
			m_lpconsole->m_lastline.begin() +
			(m_xcoord - 1), _char);

		++m_xcoord;
		validatePosition();
		++m_lpconsole->m_pendingUpdates;
	}
} 


int Zconsole::Zcursor::getPosition() const {
	return m_xcoord;
} 


void Zconsole::Zcursor::setPosition(int _xcoord) {
	m_xcoord = _xcoord;
} 


void Zconsole::Zcursor::backspace() {
	if (m_xcoord > ZCONSOLE_CURSORMINX) {
		--m_xcoord;
		m_lpconsole->m_lastline.erase(m_xcoord - 1, 1);
		validatePosition();
		++m_lpconsole->m_pendingUpdates;
	}
} 


void Zconsole::Zcursor::erase() {
	m_lpconsole->m_lastline.erase(m_xcoord - 1, 1);
	++m_lpconsole->m_pendingUpdates;
}


void Zconsole::Zcursor::move(int _offset) {
	const int oldx = m_xcoord;

	m_xcoord += _offset;
	m_xcoord = std::max<int>(m_xcoord, ZCONSOLE_CURSORMINX);
	m_xcoord = std::min<int>(m_lpconsole->m_lastline.size() + 1, m_xcoord);

	validatePosition();  
	if (oldx != m_xcoord)
		++m_lpconsole->m_pendingUpdates;
}


void Zconsole::Zcursor::moveOneWord(int _direction) {  
	int targetxpos, iter, offset;

	iter = _direction >= 0 ? 1 : -1; 

	//see below about why we do 'm_cur - 1' not just 'm_cur'
	targetxpos = 
		find1stSkipChar(
		    m_lpconsole->m_lastline, 
		    iter, 
		    m_xcoord - 1);

	//if target is -1 we failed, so just move to home or end, as needed
	if (targetxpos == -1) {
		offset = 
			iter == -1 ? 
			ZCONSOLE_CURSORHOME : ZCONSOLE_CURSOREND;
		move(offset);
	}
	else {
		//Add 1 if we moved left, we wanna be 
		//on first char of word we just skipped
		if (iter == -1)
			++targetxpos; 

		//unfortunate 'hax' of m_Cur being in 
		//'screen space', not 'line space'
		//so 0th char in line is 1 in m_cur, so we add 1
		m_xcoord = targetxpos + 1;
		validatePosition();
		++m_lpconsole->m_pendingUpdates;
	}
} 


void Zconsole::Zcursor::update() {
	auto& vertices = m_lpconsole->m_vertices;

	if (m_timer.elapsed() >= ZCONSOLE_CURSORBLINK) {
		m_timer.restart();

		m_visible = !m_visible;
		if ((unsigned int)(m_index + 6) < vertices.size()) {
			if (m_visible) {
				vertices[m_index + 1].color = m_color;
				vertices[m_index + 2].color = m_color;
				vertices[m_index + 3].color = m_color;
				vertices[m_index + 4].color = m_color;
				vertices[m_index + 5].color = m_color;
				vertices[m_index + 6].color = m_color;
			}
			else {
				vertices[m_index + 1].color = sf::Color::Transparent;
				vertices[m_index + 2].color = sf::Color::Transparent;
				vertices[m_index + 3].color = sf::Color::Transparent;
				vertices[m_index + 4].color = sf::Color::Transparent;
				vertices[m_index + 5].color = sf::Color::Transparent;
				vertices[m_index + 6].color = sf::Color::Transparent;
			}
		}
	}
}
/*****************************************************************************/  
//EOF
/*****************************************************************************/  