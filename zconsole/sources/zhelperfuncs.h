/******************************************************************************
* zconsole - sfml based in-game console module 
* Copyright (C) 2020-2021 Zachary T Harris. All Rights Reserved.  
* 
* File: zhelperfuncs.h
* Desc: helper functions
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
#ifndef __ZHELPERFUNCS_H__
#define __ZHELPERFUNCS_H__

#include <assert.h>  
#include "zconsole/ztypes.h"



extern void 
Zconsole_GetColorCodes(
	unsigned int  _index, 
	unsigned int& _rgba, 
	zstring&      _cstr); 


extern zstring 
Zconsole_StringToLower(
	const zstring& _string);


extern float 
Zconsole_StringToFloat(
	const zstring& _string);


extern zstring 
Zconsole_UintToHexString(
	unsigned int   _hexvalue,
	const zstring& _hexformat);


extern unsigned int
Zconsole_HexStringToUint(
	zstring _hexstring);


extern sf::Color 
Zconsole_GetColorFromUint(
	unsigned int _color);


extern zstrings
Zconsole_TokenizeString(
	const zstring& _string,	
	char           _token);


extern sf::FloatRect
Zconsole_GetVerticesBounds(
	const zvertices& _vertices);


/*****************************************************************************/  
#endif //EOF
/*****************************************************************************/  