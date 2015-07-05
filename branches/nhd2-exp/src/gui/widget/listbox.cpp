/*
	Neutrino-GUI  -   DBoxII-Project
	
	$Id: listbox.cpp 2013/10/12 mohousch Exp $

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
	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gui/widget/listbox.h>

#include <global.h>
#include <neutrino.h>

#include <gui/widget/icons.h>


CListBox::CListBox(const char * const Caption, const std::string& headIcon, int _width, int _height, bool itemDetails, bool titleInfo, bool paintDate)
{
	frameBuffer = CFrameBuffer::getInstance();
	caption = Caption;
	liststart = 0;
	selected =  0;
	width =  _width;
	height = _height;
	
	if(!headIcon.empty())
		HeadIcon = headIcon;
	
	ItemDetails = itemDetails;
	TitleInfo = titleInfo;
	PaintDate = paintDate;
	
	InfoHeight = 0;
	TitleHeight = 0;
	
	if(ItemDetails)
		InfoHeight = g_Font[SNeutrinoSettings::FONT_TYPE_CHANNELLIST_DESCR]->getHeight() + 10;
	
	if(TitleInfo)
		TitleHeight = g_Font[SNeutrinoSettings::FONT_TYPE_CHANNELLIST_DESCR]->getHeight() + 10;
	
	frameBuffer->getIconSize(NEUTRINO_ICON_BUTTON_RED, &icon_bf_w, &icon_bf_h);
	ButtonHeight = std::max(icon_bf_h, g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL]->getHeight()) + 6;
	
	modified = false;
	
	theight = g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE]->getHeight();
	fheight = g_Font[SNeutrinoSettings::FONT_TYPE_CHANNELLIST]->getHeight();
	listmaxshow = (height - theight - ButtonHeight)/fheight;
	height = theight + ButtonHeight + listmaxshow*fheight; // recalc height

	x = frameBuffer->getScreenX() + ((frameBuffer->getScreenWidth() - width) / 2);
	y = frameBuffer->getScreenY() + ((frameBuffer->getScreenHeight() - (height + InfoHeight)) / 2) + TitleHeight/2;
}

void CListBox::setModified(void)
{
	modified = true;
}

void CListBox::paint()
{
	liststart = (selected/listmaxshow)*listmaxshow;

	for(unsigned int count = 0; count < listmaxshow; count++)
	{
		paintItem(count);
	}

	int ypos = y + theight;
	int sb = fheight*listmaxshow;
	frameBuffer->paintBoxRel(x + width - SCROLLBAR_WIDTH, ypos, SCROLLBAR_WIDTH, sb,  COL_MENUCONTENT_PLUS_1);

	int sbc = ((getItemCount() - 1)/ listmaxshow) + 1;
	float sbh = (sb - 4)/ sbc;
	int sbs = (selected/listmaxshow);

	frameBuffer->paintBoxRel(x + width - 13, ypos + 2 + int(sbs* sbh), 11, int(sbh),  COL_MENUCONTENT_PLUS_3);
}

void CListBox::paintHead()
{
	// headBox
	frameBuffer->paintBoxRel(x, y, width, theight, COL_MENUHEAD_PLUS_0, RADIUS_MID, CORNER_TOP);//round
	
	// icon 
	int iw = 0;
	int ih = 0;
	if(!HeadIcon.empty())
	{
		frameBuffer->getIconSize(HeadIcon.c_str(), &iw, &ih);
		frameBuffer->paintIcon(HeadIcon, x + BORDER_LEFT, y + (theight - ih)/2);
	}
	
	// paint time/date
	int timestr_len = 0;
	if(PaintDate)
	{
		char timestr[18];
		
		time_t now = time(NULL);
		struct tm * tm = localtime(&now);
		
		bool gotTime = g_Sectionsd->getIsTimeSet();

		if(gotTime)
		{
			strftime(timestr, 18, "%d.%m.%Y %H:%M", tm);
			timestr_len = g_Font[SNeutrinoSettings::FONT_TYPE_EVENTLIST_ITEMLARGE]->getRenderWidth(timestr, true); // UTF-8
			
			g_Font[SNeutrinoSettings::FONT_TYPE_EVENTLIST_ITEMLARGE]->RenderString(x + width - (BORDER_RIGHT - timestr_len), y + (theight - g_Font[SNeutrinoSettings::FONT_TYPE_EVENTLIST_ITEMLARGE]->getHeight())/2 + g_Font[SNeutrinoSettings::FONT_TYPE_EVENTLIST_ITEMLARGE]->getHeight(), timestr_len + 1, timestr, COL_MENUHEAD, 0, true); // UTF-8 // 100 is pic_w refresh box
		}
	}
	g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE]->RenderString(x + BORDER_LEFT + iw + 5, y + theight, width - (BORDER_LEFT + BORDER_RIGHT + iw + 5 + timestr_len), caption.c_str() , COL_MENUHEAD, 0, true);
}

void CListBox::paintFoot()
{
	int ButtonWidth = width / 4;
	
	frameBuffer->paintBoxRel(x, y + height - ButtonHeight, width, ButtonHeight, COL_MENUHEAD_PLUS_0, RADIUS_MID, CORNER_BOTTOM);//round

	// button red
	frameBuffer->paintIcon(NEUTRINO_ICON_BUTTON_RED, x + width- 4*ButtonWidth + BORDER_LEFT, y + height - ButtonHeight + (ButtonHeight - icon_bf_h)/2);
	g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL]->RenderString(x + width - 4*ButtonWidth + icon_bf_w + BORDER_LEFT + 5, y + height - ButtonHeight + (ButtonHeight - g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL]->getHeight())/2 + g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL]->getHeight(), width, "red action", COL_INFOBAR);

	// button green
	frameBuffer->paintIcon(NEUTRINO_ICON_BUTTON_GREEN, x + width - 3*ButtonWidth + BORDER_LEFT, y + height - ButtonHeight + (ButtonHeight - icon_bf_h)/2);
	g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL]->RenderString(x + width- 3*ButtonWidth + icon_bf_w + BORDER_LEFT + 5, y + height - ButtonHeight + (ButtonHeight - g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL]->getHeight())/2 + g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL]->getHeight(), width, "green action", COL_INFOBAR);

	// button yellow
	frameBuffer->paintIcon(NEUTRINO_ICON_BUTTON_YELLOW, x + width - 2*ButtonWidth + BORDER_LEFT, y + height - ButtonHeight + (ButtonHeight - icon_bf_h)/2);
	g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL]->RenderString(x + width- 2*ButtonWidth + icon_bf_w + BORDER_LEFT + 5, y + height - ButtonHeight + (ButtonHeight - g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL]->getHeight())/2 + g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL]->getHeight(), width, "yellow action", COL_INFOBAR);

	// button blue
	frameBuffer->paintIcon(NEUTRINO_ICON_BUTTON_BLUE, x + width - ButtonWidth + BORDER_LEFT, y + height - ButtonHeight + (ButtonHeight - icon_bf_h)/2);
	g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL]->RenderString(x + width - ButtonWidth + icon_bf_w + BORDER_LEFT + 5, y + height - ButtonHeight + (ButtonHeight - g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL]->getHeight())/2 + g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL]->getHeight(), width, "blue action", COL_INFOBAR);
}

void CListBox::paintItem(int pos)
{
	paintItem(liststart + pos, pos, (liststart + pos == selected) );
}

void CListBox::hide()
{
	int ypos = y - TitleHeight;
	frameBuffer->paintBackgroundBoxRel(x, ypos, width, height + ButtonHeight + InfoHeight + TitleHeight);
	
	clearItem2DetailsLine();
	
	frameBuffer->blit();
}

unsigned int CListBox::getItemCount()
{
	return listmaxshow;
}

int CListBox::getItemHeight()
{
	return fheight;
}

void CListBox::paintItem(unsigned int itemNr, int paintNr, bool _selected)
{
	int ypos = y + theight + paintNr*getItemHeight();

	uint8_t    color;
	fb_pixel_t bgcolor;
	
	if (_selected)
	{
		color   = COL_MENUCONTENTSELECTED;
		bgcolor = COL_MENUCONTENTSELECTED_PLUS_0;
		
		// itemlines	
		paintItem2DetailsLine(paintNr, itemNr);		
		
		// details
		paintDetails(itemNr);
	}
	else
	{
		color   = COL_MENUCONTENT;
		bgcolor = COL_MENUCONTENT_PLUS_0;
	}

	frameBuffer->paintBoxRel(x, ypos, width - 15, getItemHeight(), bgcolor);
	g_Font[SNeutrinoSettings::FONT_TYPE_CHANNELLIST]->RenderString(x + BORDER_LEFT, ypos + fheight, width - (BORDER_LEFT + BORDER_RIGHT), "demo", color);
}

int CListBox::exec(CMenuTarget* parent, const std::string &/*actionKey*/)
{
	neutrino_msg_t      msg;
	neutrino_msg_data_t data;

	int res = menu_return::RETURN_REPAINT;
	selected = 0;

	if (parent)
		parent->hide();

	paintHead();
	paint();
	paintFoot();
	
	frameBuffer->blit();

	bool loop = true;
	modified = false;
	
	while (loop)
	{
		g_RCInput->getMsg(&msg, &data, g_settings.timing[SNeutrinoSettings::TIMING_EPG]);

		if (( msg == (neutrino_msg_t)g_settings.key_channelList_cancel) || ( msg == CRCInput::RC_home))
		{
			loop = false;
		}
		else if (msg == CRCInput::RC_up || (int) msg == g_settings.key_channelList_pageup)
		{
			if(getItemCount()!=0) 
			{
				int step = 0;
				int prev_selected = selected;

				step = ((int) msg == g_settings.key_channelList_pageup) ? listmaxshow : 1;  // browse or step 1
				selected -= step;
				if((prev_selected-step) < 0)            // because of uint
					selected = getItemCount() - 1;

				paintItem(prev_selected - liststart);
				unsigned int oldliststart = liststart;
				liststart = (selected/listmaxshow)*listmaxshow;

				if(oldliststart!=liststart)
					paint();
				else
					paintItem(selected - liststart);
			}
		}
		else if (msg == CRCInput::RC_down || (int) msg == g_settings.key_channelList_pagedown)
		{
			if(getItemCount() != 0) 
			{
				unsigned int step = 0;
				int prev_selected = selected;

				step = ((int) msg == g_settings.key_channelList_pagedown) ? listmaxshow : 1;  // browse or step 1
				selected += step;

				if(selected >= getItemCount()) 
				{
					if (((getItemCount() / listmaxshow) + 1) * listmaxshow == getItemCount() + listmaxshow) // last page has full entries
						selected = 0;
					else
						selected = ((step == listmaxshow) && (selected < (((getItemCount() / listmaxshow) + 1) * listmaxshow))) ? (getItemCount() - 1) : 0;
				}

				paintItem(prev_selected - liststart);
				unsigned int oldliststart = liststart;
				liststart = (selected/listmaxshow)*listmaxshow;
				if(oldliststart!=liststart)
					paint();
				else
					paintItem(selected - liststart);
			}
		}
		else if( msg ==CRCInput::RC_ok)
		{
			onOkKeyPressed();
			paintTitleInfo();
		}
		else if ( msg ==CRCInput::RC_red)
		{
			onRedKeyPressed();
		}
		else if ( msg ==CRCInput::RC_green)
		{
			onGreenKeyPressed();
		}
		else if ( msg ==CRCInput::RC_yellow)
		{
			onYellowKeyPressed();
		}
		else if ( msg ==CRCInput::RC_blue)
		{
			onBlueKeyPressed();
		}
		else if ( msg ==CRCInput::RC_setup)
		{
			onMenuKeyPressed();
		}
		else if ( msg ==CRCInput::RC_info)
		{
			onInfoKeyPressed();
		}
		else if ( msg ==CRCInput::RC_right)
		{
			onRightKeyPressed();
		}
		else if ( msg ==CRCInput::RC_left)
		{
			onLeftKeyPressed();
		}
		else if ( msg ==CRCInput::RC_spkr)
		{
			onMuteKeyPressed();
		}
		else
		{
			CNeutrinoApp::getInstance()->handleMsg( msg, data );
			// kein canceling...
		}

		frameBuffer->blit();	
	}

	hide();
	
	return res;
}

void CListBox::paintDetails(int index)
{
	if(ItemDetails == false)
		return;
	
	// infobox refresh
	frameBuffer->paintBoxRel(x + 2, y + height + 2, width - 4, InfoHeight - 4, COL_MENUCONTENTDARK_PLUS_0);
}

void CListBox::paintItem2DetailsLine(int pos, int /*ch_index*/)
{
	if(ItemDetails == false)
		return;
	
	int xpos  = x - ConnectLineBox_Width;
	int ypos1 = y + theight + pos*fheight;
	int ypos2 = y + height;
	int ypos1a = ypos1 + (fheight/2) - 2;
	int ypos2a = ypos2 + (InfoHeight/2) - 2;
	fb_pixel_t col1 = COL_MENUCONTENT_PLUS_6;
	fb_pixel_t col2 = COL_MENUCONTENT_PLUS_1;

	// Clear
	frameBuffer->paintBackgroundBoxRel(xpos, y, ConnectLineBox_Width, height + InfoHeight);

	// blit
	frameBuffer->blit();

	// paint Line if detail info (and not valid list pos)
	if (pos >= 0) 
	{ 
		int fh = fheight > 10 ? fheight - 10 : 5;
				
		frameBuffer->paintBoxRel(xpos + ConnectLineBox_Width - 4, ypos1 + 5, 4, fh, col1);
		frameBuffer->paintBoxRel(xpos + ConnectLineBox_Width - 4, ypos1 + 5, 1, fh, col2);			

		frameBuffer->paintBoxRel(xpos + ConnectLineBox_Width - 4, ypos2 + 7, 4, InfoHeight - 14, col1);
		frameBuffer->paintBoxRel(xpos + ConnectLineBox_Width - 4, ypos2 + 7, 1, InfoHeight - 14, col2);			

		// vertical line
		frameBuffer->paintBoxRel(xpos + ConnectLineBox_Width - 16, ypos1a, 4, ypos2a - ypos1a, col1);
		frameBuffer->paintBoxRel(xpos + ConnectLineBox_Width - 16, ypos1a, 1, ypos2a - ypos1a + 4, col2);		

		// Hline Oben
		frameBuffer->paintBoxRel(xpos + ConnectLineBox_Width - 15, ypos1a, 12,4, col1);
		frameBuffer->paintBoxRel(xpos + ConnectLineBox_Width - 16, ypos1a, 12,1, col2);
			
		// Hline Unten
		frameBuffer->paintBoxRel(xpos + ConnectLineBox_Width - 15, ypos2a, 12, 4, col1);
		frameBuffer->paintBoxRel(xpos + ConnectLineBox_Width - 12, ypos2a, 8, 1, col2);

		// untere info box
		frameBuffer->paintBoxRel(x, ypos2, width, InfoHeight, col1, true);
	}
}

void CListBox::clearItem2DetailsLine()
{
	if(ItemDetails == false)
		return;
	  
	  paintItem2DetailsLine(-1, 0);  
}

void CListBox::paintTitleInfo(int index)
{
	if(TitleInfo == false)
		return;
	
	// infobox refresh
	frameBuffer->paintBoxRel(x, y - TitleHeight, width, TitleHeight, COL_MENUCONTENT_PLUS_6);
	frameBuffer->paintBoxRel(x + 2, y - TitleHeight + 2, width - 4, TitleHeight - 4, COL_MENUCONTENT_PLUS_1);
}


