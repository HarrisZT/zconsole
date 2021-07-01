/******************************************************************************
* zconsole - sfml based in-game console module 
* Copyright (C) 2020-2021 Zachary T Harris. All Rights Reserved.  
* 
* File: zhistory.cpp
* Desc: interface for console command history
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
#include <fstream>  
#include "zconsoledefs.hpp"
#include "zcursor.hpp"
#include "zscreen.hpp"
#include "zhistory.hpp"





void Zconsole::Zhistory::initialize(const zstring& _path) {   
	setDirectory(_path);
	m_buffer.resize(ZCONSOLE_HISTORYSIZE);
	load();
} 


void Zconsole::Zhistory::setDirectory(const zstring& _path) {
	m_filepath = _path + "conhist.txt"; 
} 


unsigned int Zconsole::Zhistory::size() const {
	return m_buffer.size();
} 


void Zconsole::Zhistory::update() {
	m_buffer.push_back(m_lpconsole->m_lastline);
	m_buffer.erase(m_buffer.begin()); 
	m_index = m_buffer.size(); 
}  


void Zconsole::Zhistory::setItem(unsigned int _index, const zstring& _item) {
	if (_index < size()) 
		m_buffer[_index] = _item;
}  


const zstring& Zconsole::Zhistory::getItem(unsigned int _index) {
	if (_index < size())
		return m_buffer[_index];
	return Zscreen::emptyline.text;
}  


void Zconsole::Zhistory::save(bool _append) {
	unsigned int  index;
	std::ofstream file;

	file = std::ofstream(
		m_filepath.c_str(),
		_append ? std::ios::app : std::ios::trunc); 

	for (index = 0; index < size(); ++index)
		file << getItem(index) << std::endl; 

	m_lpconsole->print("history saved");
	m_lpconsole->print(" ");
}  


bool Zconsole::Zhistory::load() {
	unsigned int  iter, index;
	zstring       line;
	zstrings      buff;
	std::ifstream file;

	file = std::ifstream(m_filepath.c_str()); 
	if (!file.is_open() || !size())
		return false; 
	 
	iter = 0; 
	buff = zstrings(size());
	while (std::getline(file, line)) {
		buff[iter % size()] = line;
		++iter;
	}
	if (iter == 0)
		return false;

	for (index = 0; index < size(); ++index) {
		m_buffer[index] = 
			buff[(iter + index) % size()];
	}
	m_lpconsole->print("history loaded");
	m_lpconsole->print(" ");
	return true;
} 


void Zconsole::Zhistory::read(int _changeindex) {
	static zstring savedlastline;

	bool wasPrompt =
		(unsigned int)m_index == size();

	m_index += _changeindex;
	m_index = std::max<int>(m_index, 0);
	m_index = std::min<int>(m_index, size());

	if ((unsigned int)m_index == size()) {
		//if we came back from history, swap last line in
		if (!wasPrompt)
			std::swap(m_lpconsole->m_lastline, savedlastline); 
	}
	else {
		//if we just entered history, swap out last line
		if (wasPrompt)
			std::swap(m_lpconsole->m_lastline, savedlastline); 
		m_lpconsole->m_lastline = m_buffer[m_index]; 
	} 
}  


void Zconsole::Zhistory::clear() {
	m_buffer.clear();  
	m_lpconsole->print("history cleared"); 
} 


void Zconsole::Zhistory::print() { 
	unsigned int index;
	m_lpconsole->print(" "); 
	m_lpconsole->print(m_lpconsole->m_partition);  
	for (index = 0; index < m_buffer.size(); ++index) {
		if (!m_buffer[index].empty()) 
			m_lpconsole->print(m_buffer[index]);  
	} 
}  
/*****************************************************************************/  
//EOF
/*****************************************************************************/  
