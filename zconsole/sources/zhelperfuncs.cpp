/******************************************************************************
* zconsole - sfml based in-game console module 
* Copyright (C) 2020-2021 Zachary T Harris. All Rights Reserved.  
* 
* File: zhelperfuncs.cpp
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
#include <sstream>
#include <iomanip>
#include "zhelperfuncs.hpp"



void 
Zconsole_GetColorCodes(
	unsigned int  _index,
	unsigned int& _rgba,
	zstring&      _cstr) {

	switch (_index) {
	case 0x0: _rgba = 0x087C00FF; _cstr = "Green";     break;
	case 0x1: _rgba = 0x783C00FF; _cstr = "Brown";     break;
	case 0x2: _rgba = 0xFFB000FF; _cstr = "Amber";     break;
	case 0x3: _rgba = 0x808080FF; _cstr = "Grey";      break;
	case 0x4: _rgba = 0x982220FF; _cstr = "Red";       break;
	case 0x5: _rgba = 0x800080FF; _cstr = "Purple";    break;
	case 0x6: _rgba = 0x00FFFFFF; _cstr = "Cyan";      break;
	case 0x7: _rgba = 0xE454ECFF; _cstr = "Magenta";   break;
	case 0x8: _rgba = 0x00FF00FF; _cstr = "Lt Green";  break;
	case 0x9: _rgba = 0x2245FFFF; _cstr = "Blue";      break;
	case 0xA: _rgba = 0xC0C0C0FF; _cstr = "White";     break;
	}
}


int
Zconsole_ArgToBool(
	const zstring& _arg0) {

	if (_arg0 == "enable" || 
		_arg0 == "on"     ||
		_arg0 == "true"   || 
		_arg0 == "1") 
		return 1;
	if (_arg0 == "disable" ||
		_arg0 == "off"     ||
		_arg0 == "false"   || 
		_arg0 == "0") 
		 return 0; 
	return -1;
} 


zstring 
Zconsole_StringToLower(
	const zstring& _strIn) { 

	zstring strOut(_strIn);
	std::transform(
		strOut.begin(), strOut.end(), strOut.begin(), tolower);
	return strOut;
}


float
Zconsole_StringToFloat(
	const zstring& _in) { 

	float              out;
	std::istringstream str;
	str = std::istringstream(_in);
	if (str >> out)
		return out;
	return 0.f;
}


zstring 
Zconsole_UintToHexString(
	unsigned int   _hexvalue,
	const zstring& _hexformat) {

	zstring            prefix;
	std::streamsize    width;
	std::ostringstream conversion;

	prefix = "0x";
	/*
	determine format:*/
	if (_hexformat.find('n', 5) != std::string::npos)
		prefix = "";
	if (_hexformat.find('1', 4) != std::string::npos)
		width = 1;
	else if (_hexformat.find('2', 4) != std::string::npos)
		width = 2;
	else if (_hexformat.find('8', 4) != std::string::npos)
		width = 8;
	/*
	build the string:*/
	conversion
		<< std::uppercase
		<< std::right
		<< std::setfill('0')
		<< std::setw(width)
		<< std::hex
		<< _hexvalue;
	return conversion.str();
}


unsigned int
Zconsole_HexStringToUint(zstring _hexstring) {  
	unsigned int       value;
	std::istringstream converter;

	_hexstring = Zconsole_StringToLower(_hexstring);
	if (_hexstring.length() > 1) {
		if (_hexstring[0] == '0' && _hexstring[1] == 'x') {
			_hexstring.erase(_hexstring.begin());
			_hexstring.erase(_hexstring.begin());
		}
	}
	converter = std::istringstream(_hexstring);
	converter >> std::hex >> value;
	return value;
}


sf::Color 
Zconsole_GetColorFromUint(
	unsigned int _color) {

	sf::Color color;
	color.r = (_color & 0xFF000000) >> 0x18;
	color.g = (_color & 0x00FF0000) >> 0x10;
	color.b = (_color & 0x0000FF00) >> 0x08;
	color.a = (_color & 0x000000FF);
	return color;
}


zstrings 
Zconsole_TokenizeString(
	const zstring& _string, 
	char           _token) {

	zstrings          output;
	std::stringstream stream;
	std::string       tokenizedstr;

	if (!_string.empty()) {
		stream = std::stringstream(_string);

		while (std::getline(stream, tokenizedstr, _token)) {
			if (!tokenizedstr.empty())
				output.push_back(tokenizedstr);
		}
	}
	return output;
}


sf::FloatRect
Zconsole_GetVerticesBounds(
	const zvertices& _vertices) {

	unsigned int  it;
	sf::FloatRect bounds;
	float         px, py, pr, pb;
	sf::Vector2f  position;

	if (!_vertices.empty()) {
		px = _vertices[0].position.x;
		py = _vertices[0].position.y;
		pr = _vertices[0].position.x;
		pb = _vertices[0].position.y;

		for (it = 1; it < _vertices.size(); ++it) {
			position = _vertices[it].position;

			if (position.x < px)
				px = position.x;
			else if (position.x > pr)
				pr = position.x;
			if (position.y < py)
				py = position.y;
			else if (position.y > pb)
				pb = position.y;
		}
		bounds.left   = px;
		bounds.top    = py;
		bounds.width  = (pr - px); // * scale_width
		bounds.height = (pb - py); // * scale_height
	} 
	return bounds;
}
/*****************************************************************************/  
//EOF
/*****************************************************************************/
