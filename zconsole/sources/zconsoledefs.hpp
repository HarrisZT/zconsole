/******************************************************************************
* zconsole - sfml based in-game console module 
* Copyright (C) 2020-2021 Zachary T Harris. All Rights Reserved.  
* 
* File: zconsoledefs.hpp
* Desc: common global console constants
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
#ifndef __ZCONSOLEDEFS_HPP__
#define	__ZCONSOLEDEFS_HPP__

#define ZCONSOLE_PROMPT         ">: "  
#define ZCONSOLE_WIDTH          80
#define ZCONSOLE_HEIGHT         25
#define ZCONSOLE_FONTSIZE       64
#define ZCONSOLE_HISTORYSIZE    100 
#define ZCONSOLE_TEXTCOLOR      0x783C00FFU
#define ZCONSOLE_MAXMSGSAVE     0x00000BB8U 
#define ZCONSOLE_DEFAULTSCALE   0.25f
#define ZCONSOLE_MAXLINES       (ZCONSOLE_HEIGHT - 2) 
#define ZCONSOLE_MAXECHOS       (ZCONSOLE_MAXLINES - 3) 
#define ZCONSOLE_TOTALSIZE      (ZCONSOLE_WIDTH * ZCONSOLE_HEIGHT) 
#define ZCONSOLE_ENTRYKEY       sf::Keyboard::Tilde 
#define ZCONSOLE_CURSORBLINK    350 
#define ZCONSOLE_CURSOREND      0x000186A0 
#define ZCONSOLE_CURSORGLYPH    0x00002588 
#define ZCONSOLE_CURSORHOME    -0x000186A0  
#define ZCONSOLE_CURSORMINX     0x00000004 
#define ZCONSOLE_SCROLLBEGIN   -0x3B9ACA00  
#define ZCONSOLE_SCROLLEND      0x3B9ACA00  

/*****************************************************************************/  
#endif //EOF
/*****************************************************************************/  
