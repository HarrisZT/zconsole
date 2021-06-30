/******************************************************************************
* zconsole - sfml based in-game console module 
* Copyright (C) 2020-2021 Zachary T Harris. All Rights Reserved.  
* 
* File: zhistory.h
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
#ifndef __ZHISTORY_H__
#define __ZHISTORY_H__

#include "zconsole/zconsole.h"
 


class Zconsole::Zhistory final  {
public: 
	Zhistory(Zconsole* const _lpconsole) :
		m_lpconsole(_lpconsole) {}
	~Zhistory() {}	

	/*
	Initialize the history storage module
	@_path: where to load/save external file*/
	void initialize(const zstring&  _path);

	/*
	Returns the size of the string buffer used for storing hitsory*/
	unsigned int size() const;

	/*
	Updates the contents stored in history*/
	void update();

	/*
	Specify a new item to store in history.
	@_index: where in history to store item
	@_item : the item to store at index*/
	void setItem(unsigned int _index, const zstring& _item);

	/*
	Returns an item that was stored in history.
	@_index: where in history the item was stored*/
	const zstring& getItem(unsigned int _index);

	/*
	Save the contents stored in history to an external file*/
	void save(bool _append = true);

	/*
	Load history data from an external file.
	@return: true if loaded successfully, else false*/
	bool load();

	/*
	Parsing the data stored in an external file*/
	void read(int _changeindex);

	/*
	Clears the contents stored in history*/
	void clear();

	/*
	Displays the contents of history to the console screen*/
	void print();

	/*
	Specifies where to save/load file*/
	void setDirectory(const zstring& _directory);

private:
	Zconsole* const m_lpconsole;
	int             m_index;    //index in history 
    zstrings        m_buffer;   //the history buffer  
    zstring         m_filepath; //path to external file
};
/*****************************************************************************/  
#endif //EOF
/*****************************************************************************/  