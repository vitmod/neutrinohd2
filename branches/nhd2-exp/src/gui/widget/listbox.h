/*
	Neutrino-GUI  -   DBoxII-Project
	
	$Id: lisbox.h 2013/10/12 mohousch Exp $

	Copyright (C) 2001 Steffen Hehn 'McClean'
	Homepage: http://dbox.cyberphoria.org/

	License: GPL

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/


#ifndef __listbox__
#define __listbox__

#include "menue.h"

#include <driver/framebuffer.h>

#include <string>

class CListBox : public CMenuWidget
{
	protected:
		CFrameBuffer*	frameBuffer;
		bool            modified;
		std::string	caption;
		int		width;
		int		height;
		int		x;
		int		y;

		int		fheight;
		int		theight;

		unsigned int	selected;
		unsigned int	liststart;
		unsigned int	listmaxshow;
	
		int 		ButtonHeight;
		int 		info_height;
		int 		HeadInfoHeight;
		
		int icon_bf_w;
		int icon_bf_h;
		
		bool ItemDetails;
		bool HeadInfo;

		virtual void paintItem(int pos);
		virtual void paint();
		virtual	void paintHead();
		virtual void paintFoot();
		virtual void hide();
		virtual void paintDetails(int index);
		virtual void paintItem2DetailsLine(int pos, int ch_index);
		virtual void clearItem2DetailsLine();
		virtual void paintHeadInfo(int index = 0);
		

		
		//------hier Methoden �berschreiben-------
		//------Fernbedienungsevents--------------
		virtual void onRedKeyPressed(){};
		virtual void onGreenKeyPressed(){};
		virtual void onYellowKeyPressed(){};
		virtual void onBlueKeyPressed(){};
		virtual void onOkKeyPressed(){};
		virtual void onMenuKeyPressed(){};
		virtual void onInfoKeyPressed(){};
		virtual void onRightKeyPressed(){};
		virtual void onLeftKeyPressed(){};
		virtual void onZapKeyPressed(){};
		virtual void onMuteKeyPressed(){};
		virtual void onOtherKeyPressed( int /*key*/ ){};

		//------gibt die Anzahl der Listenitems---
		virtual unsigned int getItemCount();

		//------malen der Items-------------------
		virtual int getItemHeight();
		virtual void paintItem(uint32_t itemNr, int paintNr, bool selected);

		//------Benutzung von setModified---------
		void setModified(void);

	public:
		CListBox(const char * const Caption, int _width = MENU_WIDTH, int _height = MENU_HEIGHT, bool itemDetails = false, bool headInfo = false); 
		virtual int exec(CMenuTarget* parent, const std::string& actionKey);
};


#endif
