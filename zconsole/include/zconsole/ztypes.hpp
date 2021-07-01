/******************************************************************************
* zconsole - sfml based in-game console module 
* Copyright (C) 2020-2021 Zachary T Harris. All Rights Reserved.  
* 
* File: ztypes.hpp
* Desc: basic type definitions used by zconsole
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
#ifndef __ZTYPES_HPP__
#define __ZTYPES_HPP__

#include <vector>
#include <string>
#include <memory>  
#include <functional>
#include <sfml/system.hpp>
#include <sfml/window.hpp>
#include <sfml/graphics.hpp>



using zstring   = std::string;
using zstrings  = std::vector<std::string>; 
using zustring  = std::basic_string<unsigned>;
using zvertices = std::vector<sf::Vertex>;

/* 
An std::function<> based console callback, which is templated on a
function signature and accepts a string vector containing arguments.*/
using zcallback = std::function<void(zstrings _args)>;

/*
A structure for storing a callback function and information*/
struct zcommand { 
    zcallback func; //command callback function 
	zstring   name; //name of command name (as entered)
	zstring   emsg; //optional message to print on return
	zstrings  help; //description and help information 
}; 

/*
Helper Function for use with Zconsole commands:
Converts a given argument into an equivalent boolean value.
@_arg  : the argument to convert
@return:  (0)- argument equates to false
          (1)- argument equates to true
		 (-1)- argument did not equate to a boolean value*/
extern int 
Zconsole_ArgToBool(
	const zstring& _arg);


/*****************************************************************************/  
#endif //EOF
/*****************************************************************************/  
