/*
	WebTV

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

#define __USE_FILE_OFFSET64 1
#include "filebrowser.h"
#include <stdio.h>
#include <global.h>
#include <libgen.h>
#include <neutrino.h>
#include <driver/screen_max.h>
#include "movieplayer.h"
#include "webtv.h"
#include <gui/widget/buttons.h>


#define DEFAULT_WEBTV_XMLFILE 		CONFIGDIR "/webtv.xml"

extern CMoviePlayerGui * moviePlayerGui;	// defined in neutrino.cpp

CWebTV::CWebTV()
{
	frameBuffer = CFrameBuffer::getInstance();
	
	selected = 0;
	liststart = 0;
	
	parser = NULL;
}

CWebTV::~CWebTV()
{
	if (parser)
	{
		xmlFreeDoc(parser);
		parser = NULL;
	}
}

int CWebTV::exec()
{
	readXml();
	
	return Show();
}

CFile * CWebTV::getSelectedFile()
{
	if ((!(filelist.empty())) && (!(filelist[selected].Name.empty())))
		return &filelist[selected];
	else
		return NULL;
}

// readxml file
bool CWebTV::readXml()
{
	//CFileList flist;
	CFile file;
	WebTVChannels Channels_list;
	 
	channels.clear();
	
	if (parser)
	{
		xmlFreeDoc(parser);
		parser = NULL;
	}
	
	parser = parseXmlFile(DEFAULT_WEBTV_XMLFILE);
	
	if (parser) 
	{
		xmlNodePtr l0 = NULL;
		xmlNodePtr l1 = NULL;
		l0 = xmlDocGetRootElement(parser);
		l1 = l0->xmlChildrenNode;
		
		if (l1) 
		{
			while ((xmlGetNextOccurence(l1, "webtv"))) 
			{
				char * title = xmlGetAttribute(l1, (char *)"title");
				//char * urlkey = xmlGetAttribute(l1, (char *)"urlkey");
				char * url = xmlGetAttribute(l1, (char *)"url");
				char * description = xmlGetAttribute(l1, (char *)"description");
				char * locked = xmlGetAttribute(l1, (char *)"locked");
				
				bool ChLocked = locked ? (strcmp(locked, "1") == 0) : false;
				
				// fill
				Channels_list.title = title;
				//Channels_list.urlkey = urlkey;
				Channels_list.url = url;
				Channels_list.description = description;
				Channels_list.locked = locked;
				
				// parentallock
				if ((g_settings.parentallock_prompt != PARENTALLOCK_PROMPT_ONSIGNAL) && (g_settings.parentallock_prompt != PARENTALLOCK_PROMPT_CHANGETOLOCKED))
					ChLocked = false;			
			
				if(!ChLocked)
					channels.push_back(Channels_list);
				
				file.Name = url;
				filelist.push_back(file);

				l1 = l1->xmlNextNode;
			}
		}
		
		return true;
	}
	
	xmlFreeDoc(parser);
	
	return false;
}

int CWebTV::Show()
{
	bool res = false;
	
	neutrino_msg_t      msg;
	neutrino_msg_data_t data;
	
	// windows size
	width  = w_max ( (frameBuffer->getScreenWidth() / 20 * 17), (frameBuffer->getScreenWidth() / 20 ));
	height = h_max ( (frameBuffer->getScreenHeight() / 20 * 16), (frameBuffer->getScreenHeight() / 20));

	if (channels.empty()) 
		return -1;

	// display channame in vfd	
	CVFD::getInstance()->setMode(CVFD::MODE_MENU_UTF8 );
	
	buttonHeight = 7 + std::min(16, g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL]->getHeight());
	theight = g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE]->getHeight();

	fheight = g_Font[SNeutrinoSettings::FONT_TYPE_CHANNELLIST]->getHeight();
	listmaxshow = (height - theight - buttonHeight)/fheight;
	height = theight + buttonHeight + listmaxshow * fheight;
	info_height = fheight + g_Font[SNeutrinoSettings::FONT_TYPE_CHANNELLIST_DESCR]->getHeight() + 10;
	
	x = frameBuffer->getScreenX() + (frameBuffer->getScreenWidth() - width) / 2;
	y = frameBuffer->getScreenY() + (frameBuffer->getScreenHeight() - (height + info_height)) / 2;
	
	// head
	paintHead();
	
	// paint all
	paint();
		
#if !defined USE_OPENGL
	frameBuffer->blit();
#endif

	int oldselected = selected;

	// loop control
	unsigned long long timeoutEnd = CRCInput::calcTimeoutEnd(g_settings.timing[SNeutrinoSettings::TIMING_CHANLIST]);
	bool loop = true;
	
	while (loop) 
	{
		g_RCInput->getMsgAbsoluteTimeout(&msg, &data, &timeoutEnd );
		
		if ( msg <= CRCInput::RC_MaxRC )
			timeoutEnd = CRCInput::calcTimeoutEnd(g_settings.timing[SNeutrinoSettings::TIMING_CHANLIST]);

		if ( msg == CRCInput::RC_timeout )
		{
			selected = oldselected;
			
			loop = false;
		}
		else if ( msg == CRCInput::RC_up || (int) msg == g_settings.key_channelList_pageup )
                {
                        int step = 0;
                        int prev_selected = selected;

			step =  ((int) msg == g_settings.key_channelList_pageup) ? listmaxshow : 1;  // browse or step 1
                        selected -= step;
                        if((prev_selected-step) < 0)            // because of uint
                                selected = channels.size() - 1;

                        paintItem(prev_selected - liststart);
			
                        unsigned int oldliststart = liststart;
                        liststart = (selected/listmaxshow)*listmaxshow;
                        if(oldliststart!=liststart)
                                paint();
                        else
                                paintItem(selected - liststart);
                }
                else if ( msg == CRCInput::RC_down || (int) msg == g_settings.key_channelList_pagedown )
                {
                        unsigned int step = 0;
                        int prev_selected = selected;

			step =  ((int) msg == g_settings.key_channelList_pagedown) ? listmaxshow : 1;  // browse or step 1
                        selected += step;

                        if(selected >= channels.size()) 
			{
                                if (((channels.size() / listmaxshow) + 1) * listmaxshow == channels.size() + listmaxshow) 	// last page has full entries
                                        selected = 0;
                                else
                                        selected = ((step == listmaxshow) && (selected < (((channels.size() / listmaxshow)+1) * listmaxshow))) ? (channels.size() - 1) : 0;
			}

                        paintItem(prev_selected - liststart);
			
                        unsigned int oldliststart = liststart;
                        liststart = (selected/listmaxshow)*listmaxshow;
                        if(oldliststart != liststart)
                                paint();
                        else
                                paintItem(selected - liststart);
                }
                else if ( msg == CRCInput::RC_red || msg == CRCInput::RC_ok || msg == (neutrino_msg_t) g_settings.mpkey_play) 
		{	  
			g_settings.webtv_url = channels[selected].url;
			g_settings.webtv_name = channels[selected].title;
			
			filelist[selected].Name = channels[selected].url;
			
			res = true;
		
			loop = false;
		}
		else if ( msg == CRCInput::RC_green || msg == CRCInput::RC_home ) 
		{
			loop = false;
		}
		else if ( CNeutrinoApp::getInstance()->handleMsg( msg, data ) & messages_return::cancel_all ) 
		{
			loop = false;
		}
			
#if !defined USE_OPENGL
		frameBuffer->blit();
#endif		
	}
	
	hide();
			
	return (res);
}

void CWebTV::hide()
{
	frameBuffer->paintBackgroundBoxRel(x, y, width + 5, height + info_height + 5);
		
#if !defined USE_OPENGL
	frameBuffer->blit();
#endif	
        clearItem2DetailsLine();	
}

void CWebTV::paintItem(int pos)
{
	if (!channels.size())
		return;

	int ypos = y + theight + pos*fheight;
	uint8_t    color;
	fb_pixel_t bgcolor;
	unsigned int curr = liststart + pos;
	
	if (curr == selected) 
	{
		color   = COL_MENUCONTENTSELECTED;
		bgcolor = COL_MENUCONTENTSELECTED_PLUS_0;
		
		// itemlines	
		paintItem2DetailsLine(pos, curr);		
		
		// details
		paintDetails(curr);

		// infobox
		frameBuffer->paintBoxRel(x, ypos, width- 15, fheight, bgcolor);
	} 
	else 
	{
		color = COL_MENUCONTENT;
		bgcolor = COL_MENUCONTENT_PLUS_0;
		
		// infobox
		frameBuffer->paintBoxRel(x, ypos, width - 15, fheight, bgcolor);
	}

	//name and description
	if(curr < channels.size()) 
	{
		char nameAndDescription[255];
		
		snprintf(nameAndDescription, sizeof(nameAndDescription), "%s", channels[curr].title);
		
		g_Font[SNeutrinoSettings::FONT_TYPE_CHANNELLIST]->RenderString(x + 10, ypos + fheight, width - 40 - 15, nameAndDescription, color, 0, true);
	}
}

#define NUM_LIST_BUTTONS 2
struct button_label CWebTVButtons[NUM_LIST_BUTTONS] =
{
	{ NEUTRINO_ICON_BUTTON_RED, LOCALE_AUDIOPLAYER_PLAY},
	{ NEUTRINO_ICON_BUTTON_GREEN, LOCALE_MENU_EXIT},
};

// paint head
void CWebTV::paintHead()
{

	// head
	frameBuffer->paintBoxRel(x, y, width, theight, COL_MENUHEAD_PLUS_0, RADIUS_MID, CORNER_TOP); //round
	
	int ButtonWidth = (width - 20) / 4;
	
	// foot
	//int ButtonWidth = (width - 20) / 4;
	frameBuffer->paintBoxRel(x, y + (height - buttonHeight), width, buttonHeight - 1, COL_MENUHEAD_PLUS_0, RADIUS_MID, CORNER_BOTTOM); //round
	
	::paintButtons(frameBuffer, g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL], g_Locale, x + 10, y + (height - buttonHeight) + 3, ButtonWidth, NUM_LIST_BUTTONS, CWebTVButtons);
	
	// head icon
	int icon_w, icon_h;
	frameBuffer->getIconSize(NEUTRINO_ICON_STREAMING, &icon_w, &icon_h);
	frameBuffer->paintIcon(NEUTRINO_ICON_STREAMING, x + 10, y + ( theight - icon_h)/2 );
	
	// paint time/date
	int timestr_len = 0;
	char timestr[18];
	
	time_t now = time(NULL);
	struct tm *tm = localtime(&now);
	
	bool gotTime = g_Sectionsd->getIsTimeSet();

	if(gotTime)
	{
		strftime(timestr, 18, "%d.%m.%Y %H:%M", tm);
		timestr_len = g_Font[SNeutrinoSettings::FONT_TYPE_EVENTLIST_ITEMLARGE]->getRenderWidth(timestr, true); // UTF-8
		
		g_Font[SNeutrinoSettings::FONT_TYPE_EVENTLIST_ITEMLARGE]->RenderString(x + width - 20 - timestr_len, y + g_Font[SNeutrinoSettings::FONT_TYPE_EVENTLIST_ITEMLARGE]->getHeight() + 5, timestr_len+1, timestr, COL_MENUHEAD, 0, true); // UTF-8 // 100 is pic_w refresh box
	}
	
	//head title
	g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE]->RenderString(x + 10 + icon_w + 10, y + theight, width - 20 - icon_w - timestr_len, g_Locale->getText(LOCALE_WEBTV_HEAD), COL_MENUHEAD, 0, true); // UTF-8
}

// infos
void CWebTV::paintDetails(int index)
{
	// itembox refresh
	frameBuffer->paintBoxRel(x + 2, y + height + 2, width - 4, info_height - 4, COL_MENUCONTENTDARK_PLUS_0);
	
	// name/description
	g_Font[SNeutrinoSettings::FONT_TYPE_CHANNELLIST]->RenderString(x + 10, y + height + 5 + fheight, width - 30, channels[index].title, COL_MENUCONTENTDARK, 0, true);
	g_Font[SNeutrinoSettings::FONT_TYPE_CHANNELLIST_DESCR]->RenderString (x + 10, y+ height + 5 + 2* fheight- 2, width - 30, channels[index].description, COL_MENUCONTENTDARK, 0, true); // UTF-8
}

void CWebTV::clearItem2DetailsLine ()
{  
	  paintItem2DetailsLine(-1, 0);  
}

void CWebTV::paintItem2DetailsLine(int pos, int ch_index)
{
#define ConnectLineBox_Width	16

	int xpos  = x - ConnectLineBox_Width;
	int ypos1 = y + theight + 0 + pos*fheight;
	int ypos2 = y + height;
	int ypos1a = ypos1 + (fheight/2) - 2;
	int ypos2a = ypos2 + (info_height/2) - 2;
	fb_pixel_t col1 = COL_MENUCONTENT_PLUS_6;
	fb_pixel_t col2 = COL_MENUCONTENT_PLUS_1;

	// Clear
	frameBuffer->paintBackgroundBoxRel(xpos, y, ConnectLineBox_Width, height + info_height);

#if !defined USE_OPENGL
	frameBuffer->blit();
#endif

	// paint Line if detail info (and not valid list pos)
	if (pos >= 0) 
	{ 
		int fh = fheight > 10 ? fheight - 10 : 5;
			
		frameBuffer->paintBoxRel(xpos + ConnectLineBox_Width - 4, ypos1 + 5, 4, fh, col1);
		frameBuffer->paintBoxRel(xpos + ConnectLineBox_Width - 4, ypos1 + 5, 1, fh, col2);			

		frameBuffer->paintBoxRel(xpos+ConnectLineBox_Width - 4, ypos2 + 7, 4, info_height - 14, col1);
		frameBuffer->paintBoxRel(xpos+ConnectLineBox_Width - 4, ypos2 + 7, 1, info_height - 14, col2);			

		// vertical line
		frameBuffer->paintBoxRel(xpos + ConnectLineBox_Width - 16, ypos1a, 4, ypos2a - ypos1a, col1);
		frameBuffer->paintBoxRel(xpos + ConnectLineBox_Width - 16, ypos1a, 1, ypos2a - ypos1a + 4, col2);		

		// Hline Oben
		frameBuffer->paintBoxRel(xpos + ConnectLineBox_Width - 15, ypos1a, 12,4, col1);
		frameBuffer->paintBoxRel(xpos + ConnectLineBox_Width - 16, ypos1a, 12,1, col2);
		
		// Hline Unten
		frameBuffer->paintBoxRel(xpos + ConnectLineBox_Width - 15, ypos2a, 12, 4, col1);
		frameBuffer->paintBoxRel(xpos + ConnectLineBox_Width - 12, ypos2a, 8, 1, col2);

		// untere info box lines
		frameBuffer->paintBoxRel(x, ypos2, width, info_height, col1);
	}
}

// paint
void CWebTV::paint()
{
	liststart = (selected/listmaxshow)*listmaxshow;
	
	// channelslist body
	frameBuffer->paintBoxRel(x, y + theight, width, height - buttonHeight - theight, COL_MENUCONTENT_PLUS_0);

	for(unsigned int count = 0; count < listmaxshow; count++) 
	{
		paintItem(count);
	}

	// sb
	int ypos = y + theight;
	int sb = fheight*listmaxshow;
	
	frameBuffer->paintBoxRel(x + width - 15, ypos, 15, sb,  COL_MENUCONTENT_PLUS_1);

	int sbc= ((channels.size()- 1)/ listmaxshow)+ 1;
	int sbs= (selected/listmaxshow);

	frameBuffer->paintBoxRel(x + width- 13, ypos + 2 + sbs*(sb - 4)/sbc, 11, (sb - 4)/sbc, COL_MENUCONTENT_PLUS_3);
}

