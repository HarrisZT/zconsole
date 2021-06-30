/******************************************************************************
* zconsole - sfml based in-game console module 
* Copyright (C) 2020-2021 Zachary T Harris. All Rights Reserved.  
* 
* File: zconsole.cpp
* Desc: interface for console 
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
#include "zfontdata.h"
#include "ztimers.h"
#include "zhelperfuncs.h" 
#include "zcommandtable.h"  
#include "zscreen.h"
#include "zhistory.h"
#include "zcursor.h" 
#include "zconsole/zconsole.h" 

 


Zconsole::Zconsole() {}
Zconsole::~Zconsole() {}


bool Zconsole::initialize(const zstring& _directory) {  
	static bool initialized = false;
	if (!initialized) {  

		m_font.reset(new sf::Font());
		m_font->loadFromMemory(
		    s_zconsole_fontdata, 
			sizeof(s_zconsole_fontdata));

		m_entrykeyflag = true;  
		m_renderscale  = ZCONSOLE_DEFAULTSCALE; 

		m_partition.clear();
		while (m_partition.length() < ZCONSOLE_WIDTH - 2) {
			m_partition += "-";
		}
		m_cursor.reset(new Zcursor(this));
	    m_history.reset(new Zhistory(this)); 
		m_screen.reset(new Zscreen(this));
		m_commandtable.reset(new ZcommandTable(this));     
	   
	    m_cursor->initialize();
	    m_history->initialize(_directory); 
		m_screen->initialize();
		m_commandtable->initialize();

	    parseLastLine(false); 
		initialized = true;
	} 
	return initialized;
} 


void Zconsole::release() {
	m_history->save(false);
	m_screen.reset();
	m_cursor.reset();
	m_history.reset();
	m_commandtable.reset();	 
} 


bool Zconsole::insertCommand(const zcommand& _command) {
	return m_commandtable->insert(_command);
}  


bool Zconsole::removeCommand(const zcommand& _command) {
	return m_commandtable->remove(_command);
}


void Zconsole::print(const zstring& _string) {
	m_screen->echoLine(_string);
}


void Zconsole::setDirectory(const zstring& _directory) {
	m_history->setDirectory(_directory);
}


const sf::Font* const Zconsole::getFont() const {
	return m_font.get();
}


void Zconsole::open() {  
	m_entrykeyflag = true;  
	m_isOpenFlag   = true;  
	
	m_screen->clear(); 
	print(getVersion());
	print("(C) 2015-2021 Zachary T Harris. All rights reserved.");
	print(" ");
}  


void Zconsole::close() { 
	 m_screen->clear(); 
	 m_isOpenFlag = false;  
}  


void Zconsole::onEvent(const sf::Event& _event) {   
	if (!m_isOpenFlag) {
		return;
	}
	if (m_entrykeyflag) {
		if (!sf::Keyboard::isKeyPressed(ZCONSOLE_ENTRYKEY))
			m_entrykeyflag = false;
		return;
	} 
	switch (_event.type) {
	case sf::Event::TextEntered:
		m_cursor->addChar((char)(_event.text.unicode));
		break;

	case sf::Event::KeyPressed:
		if (_event.key.control) {
			switch (_event.key.code) {
			case sf::Keyboard::Left:
				m_cursor->moveOneWord(-1);
				break;
			case sf::Keyboard::Right:
				m_cursor->moveOneWord(+1);
				break;
			case sf::Keyboard::Up:
				m_screen->scroll(-1);
				break;
			case sf::Keyboard::Down: 
				m_screen->scroll(1);
				break;
			case sf::Keyboard::Home:  
				m_screen->scroll(ZCONSOLE_SCROLLBEGIN);
				break;
			case sf::Keyboard::End:  
				m_screen->scroll(ZCONSOLE_SCROLLEND);
				break;
			case sf::Keyboard::PageDown:
				m_screen->scroll(ZCONSOLE_HEIGHT - 3);
				break;
			case sf::Keyboard::PageUp: 
				m_screen->scroll(-(ZCONSOLE_HEIGHT - 3));
				break; 
			}
		}
		else {
			switch (_event.key.code) {
			case sf::Keyboard::Backspace:
				m_cursor->backspace();
				break;
			case sf::Keyboard::Delete:
				m_cursor->erase();
				break;
			case sf::Keyboard::Enter:
				parseLastLine(1);
				break;
			case sf::Keyboard::Left:
				m_cursor->move(-1);
				break;
			case sf::Keyboard::Right: 
				m_cursor->move(1);
				break;
			case sf::Keyboard::End:  
				m_cursor->move(ZCONSOLE_CURSOREND);
				break;
			case sf::Keyboard::Home: 
				m_cursor->move(ZCONSOLE_CURSORHOME);
				break;
			case sf::Keyboard::Up:      
				scrollHistory(-1);
				break;
			case sf::Keyboard::Down:   
				scrollHistory(1);
				break;
			case sf::Keyboard::Escape:  
				close();
				return; 
			}
		} 
	}  
} 


void Zconsole::onPresent(
	sf::RenderTarget& _rendertarget,
	sf::RenderStates  _renderstates) {

	if (!m_isOpenFlag) {
		return;
	}
	const auto& originalview = _rendertarget.getView();

	_renderstates.texture = &m_font->getTexture(ZCONSOLE_FONTSIZE);

	auto transform = sf::Transform::Identity;
	transform.scale(m_renderscale, m_renderscale);
	_renderstates.transform *= transform;

	const auto size = _rendertarget.getSize();
	const auto rect = 
		sf::FloatRect(
			-15.f, -10.f, 
			(float)size.x, (float)size.y);

	_rendertarget.setView(sf::View(rect));
	_rendertarget.draw(
		m_vertices.data(),
		m_vertices.size(),
		sf::Triangles,
		_renderstates);

	_rendertarget.setView(originalview);
}


bool Zconsole::onUpdate(float _delta) {    
	pollInput();
	m_cursor->update();	 

	int   ix, iy, ir, ib;
	float px, py, pr, pb; 
	 
	static unsigned int lastpendingupdates = 0;
	if (!m_font ||
		lastpendingupdates == m_pendingUpdates) {
		return m_isOpenFlag;
	} 
	lastpendingupdates = m_pendingUpdates;  
	auto* screen = m_screen->getCellMatrix(); 
	/*--------------------------------------------*/ 
	/*--------------------------------------------*/ 
	
	
	 
	//reserve 6 vertices(2 triangles) for background
	m_vertices.clear(); 
	m_vertices.push_back(sf::Vertex());
	m_vertices.push_back(sf::Vertex());
	m_vertices.push_back(sf::Vertex());
	m_vertices.push_back(sf::Vertex());	 
	m_vertices.push_back(sf::Vertex());
	m_vertices.push_back(sf::Vertex());

	//Precompute the variables needed by the algorithm
	float hspace  = m_font->getGlyph(L' ', ZCONSOLE_FONTSIZE, 0).advance;
	float vspace  = m_font->getLineSpacing(ZCONSOLE_FONTSIZE);
	float xoffset = 0.f;					  
	float yoffset = ZCONSOLE_FONTSIZE; 

	unsigned int prevChar = 0u;  

	 
	/*-----------------------------------------------------------------------*/ 
	/*-----------------------------------------------------------------------*/
	for (unsigned int A = 0; A < ZCONSOLE_TOTALSIZE; ++A) {
		unsigned int curChar = screen[A].mchar; 
		/*move the reserved vertices so they won't affect the bounds*/
		if (A == 1u) {
			m_vertices[0].position = m_vertices[6].position;
			m_vertices[1].position = m_vertices[6].position;
			m_vertices[2].position = m_vertices[6].position;
			m_vertices[3].position = m_vertices[6].position;
			m_vertices[4].position = m_vertices[6].position;
			m_vertices[5].position = m_vertices[6].position; 
		}
		/*Apply the kerning offset*/
		xoffset += m_font->getKerning(
			prevChar, 
			curChar, 
			ZCONSOLE_FONTSIZE); 

		prevChar = curChar; 
	
	    /*add a cursor under the glyph if this is the right position*/		 
		const int cursorposOffset = 
			(ZCONSOLE_WIDTH * (ZCONSOLE_HEIGHT - 2)) +
			((int)(m_cursor->getPosition() - m_lastlineOffset));

		if (cursorposOffset == A) { 
			m_cursor->reset(xoffset, yoffset); 
		}
		/*Handle spaces*/
		switch (curChar) {
		case '\t': case '\n': case ' ':
			xoffset += hspace;
			continue;
		} 
		/*Extract the current glyph's description*/
		const sf::Glyph& glyph = 
			m_font->getGlyph(curChar, ZCONSOLE_FONTSIZE, false);

		ix = (int)(floor(glyph.bounds.left) + xoffset);
		iy = (int)(floor(glyph.bounds.top) + yoffset);
		ir = (int)(ceil(glyph.bounds.left + glyph.bounds.width) + xoffset);
		ib = (int)(ceil(glyph.bounds.top + glyph.bounds.height) + yoffset);

		px = (float)ix;
		py = (float)iy;
		pr = (float)ir;
		pb = (float)ib;

		const float ux = (float)(glyph.textureRect.left);
		const float uy = (float)(glyph.textureRect.top);
		const float ur = (float)(ux + glyph.textureRect.width);
		const float ub = (float)(uy + glyph.textureRect.height);

		{/*Add a quad for the current character*/
			const sf::Color col = 
				Zconsole_GetColorFromUint(screen[A].color);

		    m_vertices.push_back(sf::Vertex({ px, py }, col, { ux, uy }));
		    m_vertices.push_back(sf::Vertex({ pr, py }, col, { ur, uy }));
		    m_vertices.push_back(sf::Vertex({ px, pb }, col, { ux, ub }));
		    m_vertices.push_back(sf::Vertex({ px, pb }, col, { ux, ub }));
		    m_vertices.push_back(sf::Vertex({ pr, py }, col, { ur, uy }));
		    m_vertices.push_back(sf::Vertex({ pr, pb }, col, { ur, ub }));
		}													   
		{/*Advance to the next character*/
			xoffset += glyph.advance;
			if ((A + 1) % ZCONSOLE_WIDTH == 0) {
				yoffset += vspace;
				xoffset = 0;
				prevChar = '\n';
			}
		}
	}   
	/*-----------------------------------------------------------------------*/ 
	/*-----------------------------------------------------------------------*/
	{//fill the reserved background vertices	 
		const auto bounds = Zconsole_GetVerticesBounds(m_vertices); 

		px = floor(bounds.left);
		py = floor(bounds.top);
		pr = ceil(bounds.left + bounds.width);
		pb = ceil(bounds.top + bounds.height);
		 
		m_vertices[0] = sf::Vertex({ px, py }, sf::Color::Black, { 1.f, 1.f }); 
		m_vertices[1] = sf::Vertex({ pr, py }, sf::Color::Black, { 1.f, 1.f }); 
		m_vertices[2] = sf::Vertex({ px, pb }, sf::Color::Black, { 1.f, 1.f }); 
		m_vertices[3] = sf::Vertex({ px, pb }, sf::Color::Black, { 1.f, 1.f }); 
		m_vertices[4] = sf::Vertex({ pr, py }, sf::Color::Black, { 1.f, 1.f }); 
		m_vertices[5] = sf::Vertex({ pr, pb }, sf::Color::Black, { 1.f, 1.f });  
	} 
	return m_isOpenFlag;
} 


void Zconsole::processNextCommand() { 
	size_t   comment;
	zstrings commandList;
	zstring  cmd, argString;

	if (m_commandbuffer.empty()) {
		return;
	}
	/*
	Allow commands in quotes as single String:
	-----------------------------------------*/
	comment = m_commandbuffer.find_first_of('"');

	if (comment != zstring::npos) {
		cmd = m_commandbuffer.substr(0u, comment);
		argString = m_commandbuffer.substr(
			m_commandbuffer.find_first_of('"'),
			m_commandbuffer.find_last_of('"'));
	}
	else { cmd = m_commandbuffer; }
	
	/*
	Remove prompt characters:
	-----------------------------------------*/
	for (unsigned int i = 0; i < zstring(ZCONSOLE_PROMPT).size(); ++i) {
		cmd.erase(cmd.begin());
	} 
	/*
	Split into command and arguments:
	-----------------------------------------*/
	commandList = Zconsole_TokenizeString(cmd, ' ');
	if (!commandList.empty()) {
		commandList[0] = 
			Zconsole_StringToLower(commandList[0]);
		commandList.push_back(argString);
		m_commandtable->run(commandList);
	}
} 

 
void Zconsole::parseLastLine(bool _isCommandEntry) {
	print(m_lastline);
	m_history->update();

	m_commandbuffer += m_lastline;

	if (_isCommandEntry) {
		processNextCommand();
	}
	m_commandbuffer.clear();
	m_lastline.clear();
	m_cursor->setPosition(1);
	m_lastlineOffset = 0;
	++m_pendingUpdates;
	/*
	Reset prompt:*/
	m_lastline += zstring(ZCONSOLE_PROMPT);
	m_cursor->moveOneWord(-1);
} 


void Zconsole::pollInput() { 
	static ZtimedInputKey kbackspace(sf::Keyboard::BackSpace);
	static ZtimedInputKey kscrollup(sf::Keyboard::Up);
	static ZtimedInputKey kscrolldown(sf::Keyboard::Down);

	if (kbackspace.isPressed())
		m_cursor->backspace();
	else {
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
			if (kscrollup.isPressed())
				m_screen->scroll(-1);
			else if (kscrolldown.isPressed())
				m_screen->scroll(+1);
		}
	}
}


void Zconsole::scrollHistory(int _direction) {
	m_history->read(_direction);
	m_lastlineOffset = 0;
	m_cursor->move(ZCONSOLE_CURSOREND);
	++m_pendingUpdates;
}
/*****************************************************************************/  
//EOF
/*****************************************************************************/ 