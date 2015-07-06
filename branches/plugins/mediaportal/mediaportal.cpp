/*
  neutrinoHD2 project
  
  https://code.google.com/p/neutrinohd2/
  
  $Id: mediaportal.cpp 2014/10/03 mohousch Exp $

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

#include <mediaportal.h>	// plugin.h

extern "C" void plugin_exec(void);
extern "C" void plugin_init(void);
extern "C" void plugin_del(void);


#define MAX_WINDOW_WIDTH  		(g_settings.screen_EndX - g_settings.screen_StartX - 40)
#define MAX_WINDOW_HEIGHT 		(g_settings.screen_EndY - g_settings.screen_StartY - 40)	

#define MIN_WINDOW_WIDTH  		((g_settings.screen_EndX - g_settings.screen_StartX)>>1)
#define MIN_WINDOW_HEIGHT 		200	

#define TITLE_BACKGROUND_COLOR 		COL_MENUHEAD_PLUS_0
#define TITLE_FONT_COLOR 		COL_MENUHEAD

#define TITLE_FONT 			g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE]
#define FOOT_FONT 			g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL]

#define INTER_FRAME_SPACE 		4  // space between e.g. upper and lower window
#define TEXT_BORDER_WIDTH 		8

CFont* CMPBrowser::mp_FontFoot = NULL;
CFont* CMPBrowser::mp_FontTitle = NULL;

const neutrino_locale_t mp_localizedItemName[MPB_INFO_MAX_NUMBER + 1] =
{
	LOCALE_MOVIEBROWSER_SHORT_FILENAME,
	LOCALE_MOVIEBROWSER_SHORT_TITLE ,
	LOCALE_MOVIEBROWSER_SHORT_INFO1,
	NONEXISTANT_LOCALE
};

// default row size in pixel for any element
#define	MPB_ROW_WIDTH_FILENAME 		150
#define	MPB_ROW_WIDTH_TITLE		800
#define	MPB_ROW_WIDTH_INFO1		250

const int mp_defaultRowWidth[MPB_INFO_MAX_NUMBER + 1] = 
{
	MPB_ROW_WIDTH_FILENAME ,
	MPB_ROW_WIDTH_TITLE,
	MPB_ROW_WIDTH_INFO1,
	0 //MPB_ROW_WIDTH_MAX_NUMBER 
};

CMPBrowser::CMPBrowser()
{
	dprintf(DEBUG_NORMAL, "$Id: mediaportal browser, v 0.0.1 2014/10/02 17:52:30 mohousch Exp $\n");
	init();
}

CMPBrowser::~CMPBrowser()
{
	dprintf(DEBUG_NORMAL, "CMPBrowser: del\n");
	
	mp_ItemInfo.clear();
	mp_vHandleBrowserList.clear();

	mp_itemSelectionHandler = NULL;

	for(int i = 0; i < LF_MAX_ROWS; i++)
	{
		mp_browserListLines.lineArray[i].clear();
	}
}

void CMPBrowser::init(void)
{
	dprintf(DEBUG_NORMAL, "CMPBrowser::init\n");
	
	initGlobalSettings();
		
	mp_reload_items = true;

	mp_Window = CFrameBuffer::getInstance();
	
	mp_Browser = NULL;
	mp_Info = NULL;
	
	mp_windowFocus = MPB_FOCUS_BROWSER;
	
	mp_itemSelectionHandler = NULL;
	mp_currentBrowserSelection = 0;
 	mp_prevBrowserSelection = 0;
	
	// browser
	if(mp_settings.browserFrameHeight < MIN_MPBROWSER_FRAME_HEIGHT )
       		mp_settings.browserFrameHeight = MIN_MPBROWSER_FRAME_HEIGHT;
	if(mp_settings.browserFrameHeight > MAX_MPBROWSER_FRAME_HEIGHT)
        	mp_settings.browserFrameHeight = MAX_MPBROWSER_FRAME_HEIGHT;
	
	// Browser List 
	if(mp_settings.browserRowNr == 0)
	{
		dprintf(DEBUG_NORMAL, " row error\r\n");
		
		// init browser row elements if not configured correctly by neutrino.config
		mp_settings.browserRowNr = 2;
		mp_settings.browserRowItem[0] = MPB_INFO_TITLE;
		mp_settings.browserRowItem[1] = MPB_INFO_INFO1;
		mp_settings.browserRowWidth[0] = mp_defaultRowWidth[mp_settings.browserRowItem[0]];
		mp_settings.browserRowWidth[1] = mp_defaultRowWidth[mp_settings.browserRowItem[1]]; 
	}

	initFrames();
	
	refreshBrowserList();	
}

void CMPBrowser::initGlobalSettings(void)
{
	dprintf(DEBUG_NORMAL, "CMPBrowser::initGlobalSettings\n");
	
	mp_settings.gui = MPB_GUI_MOVIE_INFO;

	// Browser List
	mp_settings.browserFrameHeight = g_settings.screen_EndY - g_settings.screen_StartY - 20 - ((g_settings.screen_EndY - g_settings.screen_StartY - 20)>>1) - (INTER_FRAME_SPACE>>1);
	
	mp_settings.browserRowNr = 2;
	mp_settings.browserRowItem[0] = MPB_INFO_TITLE;
	mp_settings.browserRowItem[1] = MPB_INFO_INFO1;
	mp_settings.browserRowWidth[0] = mp_defaultRowWidth[mp_settings.browserRowItem[0]];
	mp_settings.browserRowWidth[1] = mp_defaultRowWidth[mp_settings.browserRowItem[1]];
}

void CMPBrowser::initFrames(void)
{
	dprintf(DEBUG_NORMAL, "CMPBrowser::initFrames\n");
	
	mp_FontFoot  = FOOT_FONT;
	mp_FontTitle = TITLE_FONT;
	
	mp_BoxFrame.iX = 			g_settings.screen_StartX + 10;
	mp_BoxFrame.iY = 			g_settings.screen_StartY + 10;
	mp_BoxFrame.iWidth = 			g_settings.screen_EndX - g_settings.screen_StartX - 20;
	mp_BoxFrame.iHeight = 			g_settings.screen_EndY - g_settings.screen_StartY - 20;

	mp_BoxFrameTitleRel.iX =		0;
	mp_BoxFrameTitleRel.iY = 		0;
	mp_BoxFrameTitleRel.iWidth = 		mp_BoxFrame.iWidth;
	mp_BoxFrameTitleRel.iHeight = 		g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getHeight()*2 + 20;

	mp_BoxFrameBrowserList.iX = 		mp_BoxFrame.iX;
	mp_BoxFrameBrowserList.iY = 		mp_BoxFrame.iY + mp_BoxFrameTitleRel.iHeight + INTER_FRAME_SPACE;
	mp_BoxFrameBrowserList.iWidth = 	mp_BoxFrame.iWidth;
	mp_BoxFrameBrowserList.iHeight = 	mp_settings.browserFrameHeight;

	mp_BoxFrameFootRel.iX = 		0;
	mp_BoxFrameFootRel.iY = 		mp_BoxFrame.iHeight - mp_FontFoot->getHeight()*2 + INTER_FRAME_SPACE;
	mp_BoxFrameFootRel.iWidth = 		mp_BoxFrameBrowserList.iWidth;
	mp_BoxFrameFootRel.iHeight = 		mp_FontFoot->getHeight()*2;
	
	mp_BoxFrameInfo.iX = 			mp_BoxFrameBrowserList.iX;
	mp_BoxFrameInfo.iY = 			mp_BoxFrameBrowserList.iY + mp_BoxFrameBrowserList.iHeight + INTER_FRAME_SPACE;
	mp_BoxFrameInfo.iWidth = 		mp_BoxFrameBrowserList.iWidth;
	mp_BoxFrameInfo.iHeight = 		mp_BoxFrame.iHeight - mp_BoxFrameBrowserList.iHeight - 2*INTER_FRAME_SPACE - mp_BoxFrameFootRel.iHeight - mp_BoxFrameTitleRel.iHeight;
}

int CMPBrowser::exec(CMenuTarget * parent, const std::string & actionKey)
{
	int returnval = menu_return::RETURN_REPAINT;

	if(actionKey == "run")
	{
		if(parent) 
			parent->hide ();
		
		exec();
	}
	
	return returnval;
}

int CMPBrowser::exec()
{
	bool res = false;

	dprintf(DEBUG_NORMAL, "CMPBrowser::exec\n");
	
	int timeout = -1;
	int returnDefaultOnTimeout = true;
	neutrino_msg_t      msg;
	neutrino_msg_data_t data;

	CVFD::getInstance()->setMode(CVFD::MODE_MENU_UTF8, "Media Portal");
	
	initGlobalSettings();
	
	// init frames
	initFrames();

	// Clear all, to avoid 'jump' in screen 
	mp_vHandleBrowserList.clear();
	
	mp_itemSelectionHandler = NULL;

	for(int i = 0; i < LF_MAX_ROWS; i++)
	{
		mp_browserListLines.lineArray[i].clear();
	}

	// paint mb
	if(paint() == false)
		return res;// paint failed due to less memory , exit 

	if ( timeout == -1 )
		timeout = g_settings.timing[SNeutrinoSettings::TIMING_FILEBROWSER];

	unsigned long long timeoutEnd = CRCInput::calcTimeoutEnd( timeout );
	
	// reload movies
	if(mp_reload_items == true)
	{
		dprintf(DEBUG_NORMAL, "CYTBrowser::exec\n");
		loadItems();
	}
	else
	{
		// since we cleared everything above, we have to refresh the list now.
		refreshBrowserList();	
	}

	// get old movie selection and set position in windows	
	mp_currentBrowserSelection = mp_prevBrowserSelection;

	mp_Browser->setSelectedLine(mp_currentBrowserSelection);

	// update movie selection
	updateItemSelection();

	// refresh title
	refreshTitle();
	
	// on set guiwindow
	onSetGUIWindow(mp_settings.gui);
	
	// browser paint 
	mp_Browser->paint();
	mp_Window->blit();

	bool loop = true;
	bool result;
	
	while (loop)
	{
		g_RCInput->getMsgAbsoluteTimeout( &msg, &data, &timeoutEnd );

		result = onButtonPress(msg);
		
		if(result == false)
		{
			if (msg == CRCInput::RC_timeout && returnDefaultOnTimeout)
			{
				dprintf(DEBUG_NORMAL, "CMPBrowser::exec: Timerevent\n");
				
				loop = false;
			}
			else if(msg == CRCInput::RC_ok)
			{
				if(mp_itemSelectionHandler != NULL)
				{
					//playing_info = m_movieSelectionHandler;
					res = true;
					loop = false;
				}
			}
			else if (msg == CRCInput::RC_home)
			{
				loop = false;
			}
			else if (CNeutrinoApp::getInstance()->handleMsg(msg, data) & messages_return::cancel_all)
			{
				dprintf(DEBUG_NORMAL, "CYTBrowser::exec: getInstance\r\n");
				
				loop = false;
			}
		}
		
		mp_Window->blit();	

		if ( msg <= CRCInput::RC_MaxRC )
			timeoutEnd = CRCInput::calcTimeoutEnd(timeout); // calcualate next timeout
	}
	
	hide();
	
	//
	mp_prevBrowserSelection = mp_currentBrowserSelection;
	
	return (res);
}

void CMPBrowser::hide(void)
{
	dprintf(DEBUG_NORMAL, "CYTBrowser::hide\n");
	
	mp_Window->paintBackground();
	mp_Window->blit();
	
	if (mp_Browser != NULL)
	{
		mp_currentBrowserSelection = mp_Browser->getSelectedLine();
		delete mp_Browser;
		mp_Browser = NULL;
	}
	
	if (mp_Info != NULL) 
	{
		delete mp_Info;
		mp_Info = NULL;
	}
}

int CMPBrowser::paint(void)
{
	dprintf(DEBUG_NORMAL, "CMPBrowser::paint\n");

	CVFD::getInstance()->setMode(CVFD::MODE_MENU_UTF8, "Media Portal");	

	mp_Browser = new CListFrame(&mp_browserListLines, NULL, CListFrame::SCROLL | CListFrame::HEADER_LINE, &mp_BoxFrameBrowserList);
	mp_Info = new CTextBox(" ", NULL, CTextBox::SCROLL, &mp_BoxFrameInfo);	

	if(mp_Browser == NULL || mp_Info == NULL )
	{
		if (mp_Browser != NULL)
			delete mp_Browser;

		if (mp_Info != NULL) 
			delete mp_Info;

		mp_Info = NULL;
		mp_Browser = NULL;

		return (false);
	} 
	
	return (true);
}

void CMPBrowser::refresh(void)
{
	dprintf(DEBUG_NORMAL, "CMPBrowser::refresh\n");
	
	refreshTitle();

	if (mp_Browser != NULL && mp_showBrowserFiles == true )
		 mp_Browser->refresh();
	
	if (mp_Info != NULL && mp_showItemInfo == true) 
		refreshItemInfo();
		
	refreshFoot();
	refreshLCD();
}

CFile * CMPBrowser::getSelectedFile(void)
{
	dprintf(DEBUG_INFO, "CMPBrowser::getSelectedFile: %s\n", mp_itemSelectionHandler->Name.c_str());

	if(mp_itemSelectionHandler != NULL)
		return(mp_itemSelectionHandler);
	else
		return(NULL);
}

void CMPBrowser::refreshItemInfo(void)
{
	dprintf(DEBUG_INFO, "CMPBrowser::refreshItemInfo: mp_ItemInfo.size %d\n", mp_ItemInfo.size());
	
	std::string emptytext = " ";
	
	if(mp_ItemInfo.size() <= 0) 
	{
		if(mp_Info != NULL)
			mp_Info->setText(&emptytext);
		return;
	}
	
	if (mp_itemSelectionHandler == NULL)
	{
		// There is no selected element, clear LCD
		mp_Info->setText(&emptytext);
	}
	else
	{
		int pich = 0;
		int picw = 0;
		int lx = 0;
		int ly = 0;
		
		std::string fname;

		fname = mp_itemSelectionHandler->thumbnail;
		
		// display screenshot if exists
		if( !access(fname.c_str(), F_OK) )
		{
			pich = mp_BoxFrameInfo.iHeight - 20;
			picw = pich * (4.0 / 3);
			lx = mp_BoxFrameInfo.iX + mp_BoxFrameInfo.iWidth - (picw + 20);
			ly = mp_BoxFrameInfo.iY + 10;
		}
		
		mp_Info->setText(&mp_itemSelectionHandler->Info2, fname, lx, ly, picw, pich);
	}
	
	mp_Window->blit();
}

void CMPBrowser::refreshLCD(void)
{
	if(mp_ItemInfo.size() <= 0) 
		return;

	//CVFD * lcd = CVFD::getInstance();
	if(mp_itemSelectionHandler == NULL)
	{
		// There is no selected element, clear LCD
		//lcd->showMenuText(0, " ", -1, true); // UTF-8
		//lcd->showMenuText(1, " ", -1, true); // UTF-8
	}
	else
	{
		CVFD::getInstance()->showMenuText(0, mp_itemSelectionHandler->Title.c_str(), -1, true); // UTF-8
	} 	
}

void CMPBrowser::refreshBrowserList(void) 
{
	dprintf(DEBUG_INFO, "CMPBrowser::refreshBrowserList\n");
	
	std::string string_item;

	// Initialise and clear list array
	mp_browserListLines.rows = mp_settings.browserRowNr;
	for(int row = 0; row < mp_settings.browserRowNr; row++)
	{
		mp_browserListLines.lineArray[row].clear();
		mp_browserListLines.rowWidth[row] = mp_settings.browserRowWidth[row];
		mp_browserListLines.lineHeader[row]= g_Locale->getText(mp_localizedItemName[mp_settings.browserRowItem[row]]);
	}
	mp_vHandleBrowserList.clear();
	
	if(mp_ItemInfo.size() <= 0) 
	{
		mp_currentBrowserSelection = 0;
		mp_itemSelectionHandler = NULL;
		
		if(mp_Browser != NULL)
			mp_Browser->setLines(&mp_browserListLines);//FIXME last delete test
		return; // exit here if nothing else is to do
	}
	
	CFile * item_handle;
	
	// prepare Browser list for sorting and filtering
	for(unsigned int file = 0; file < mp_ItemInfo.size(); file++)
	{
		item_handle = &(mp_ItemInfo[file]);
		mp_vHandleBrowserList.push_back(*item_handle);
	}

	for(unsigned int handle = 0; handle < mp_vHandleBrowserList.size(); handle++)
	{
		for(int row = 0; row < mp_settings.browserRowNr ;row++)
		{
			if ( getItemInfoItem(mp_vHandleBrowserList[handle], mp_settings.browserRowItem[row], &string_item) == false)
			{
				string_item = "n/a";
				if(mp_settings.browserRowItem[row] == MPB_INFO_TITLE)
					getItemInfoItem(mp_vHandleBrowserList[handle], MPB_INFO_FILENAME, &string_item);
			}
			mp_browserListLines.lineArray[row].push_back(string_item);
		}
	}
	
	mp_Browser->setLines(&mp_browserListLines);

	mp_currentBrowserSelection = mp_Browser->getSelectedLine();
	
	// update selected movie if browser is in the focus
	if (mp_windowFocus == MPB_FOCUS_BROWSER)
	{
		updateItemSelection();	
	}
}

void CMPBrowser::refreshTitle(void) 
{
	dprintf(DEBUG_INFO, "CMPBrowser::refreshTitle\n");
	
	mp_Window->paintBoxRel(mp_BoxFrame.iX + mp_BoxFrameTitleRel.iX, mp_BoxFrame.iY + mp_BoxFrameTitleRel.iY, mp_BoxFrameTitleRel.iWidth, mp_BoxFrameTitleRel.iHeight, COL_MENUCONTENT_PLUS_6 );
	mp_Window->paintBoxRel(mp_BoxFrame.iX + mp_BoxFrameTitleRel.iX + 2, mp_BoxFrame.iY + mp_BoxFrameTitleRel.iY + 2 , mp_BoxFrameTitleRel.iWidth - 4, mp_BoxFrameTitleRel.iHeight - 4, COL_MENUCONTENTSELECTED_PLUS_0);
	
	// title
	std::string title = "neutrinoHD2 Media Portlal (C)";
	int tw = g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getRenderWidth(title, true); // UTF-8
	int xstart = (mp_BoxFrameTitleRel.iWidth - tw) / 2;
	if(xstart < 10)
		xstart = 10;
	g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->RenderString(mp_BoxFrame.iX + mp_BoxFrameTitleRel.iX + xstart, mp_BoxFrame.iY + mp_BoxFrameTitleRel.iY + 4 + 1*g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getHeight(), mp_BoxFrameTitleRel.iWidth - 20, title, COL_MENUCONTENTSELECTED, 0, true); // UTF-8
	
	// info
	std::string info = "Mediatheck Archiv";
	int iw = g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getRenderWidth(info, true); // UTF-8
	xstart = (mp_BoxFrameTitleRel.iWidth - iw) / 2;
	if(xstart < 10)
		xstart = 10;
	g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->RenderString(mp_BoxFrame.iX + mp_BoxFrameTitleRel.iX + xstart, mp_BoxFrame.iY + mp_BoxFrameTitleRel.iY + 4 + 2*g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getHeight(), mp_BoxFrameTitleRel.iWidth - 20, "Mediatheck Archiv", COL_MENUCONTENTSELECTED, 0, true); // UTF-8	
	
	// icon
	// head icon
	std::string icon_head = PLUGINDIR "/mediaportal/mp.png";
	mp_Window->getIconSize(icon_head.c_str(), &icon_head_w, &icon_head_h);
	mp_Window->paintIcon(icon_head.c_str(), mp_BoxFrame.iX + mp_BoxFrameTitleRel.iX + 10, mp_BoxFrame.iY + mp_BoxFrameTitleRel.iY + (mp_BoxFrameTitleRel.iHeight - icon_head_h)/2);
}

void CMPBrowser::refreshFoot(void) 
{
	dprintf(DEBUG_INFO, "CMPBrowser::refreshFoot\n");
	
	mp_Window->paintBoxRel(mp_BoxFrame.iX + mp_BoxFrameFootRel.iX, mp_BoxFrame.iY + mp_BoxFrameFootRel.iY, mp_BoxFrameFootRel.iWidth, mp_BoxFrameFootRel.iHeight, COL_MENUCONTENT_PLUS_6 );
	mp_Window->paintBoxRel(mp_BoxFrame.iX + mp_BoxFrameFootRel.iX + 2, mp_BoxFrame.iY + mp_BoxFrameFootRel.iY + 2 , mp_BoxFrameFootRel.iWidth - 4, mp_BoxFrameFootRel.iHeight - 4, COL_MENUCONTENTSELECTED_PLUS_0);
	
	uint8_t color = COL_MENUHEAD;
	std::string next_text = g_Locale->getText(LOCALE_MOVIEBROWSER_NEXT_FOCUS);
	
	int width = mp_BoxFrameFootRel.iWidth>>2;
	int xpos1 = mp_BoxFrameFootRel.iX + 10;
	int xpos2 = xpos1 + width;
	int xpos3 = xpos2 + width;
	int xpos4 = xpos3 + width;
	
	int icon_w = 0;
	int icon_h = 0;

	// red
	mp_Window->getIconSize(NEUTRINO_ICON_BUTTON_RED, &icon_w, &icon_h);

	mp_Window->getIconSize(NEUTRINO_ICON_BUTTON_RED, &icon_w, &icon_h);
	mp_Window->paintIcon(NEUTRINO_ICON_BUTTON_RED, mp_BoxFrame.iX + xpos1, mp_BoxFrame.iY + mp_BoxFrameFootRel.iY + (mp_BoxFrameFootRel.iHeight - icon_h)/2 );

	mp_FontFoot->RenderString(mp_BoxFrame.iX + xpos1 + 5 + icon_w, mp_BoxFrame.iY + mp_BoxFrameFootRel.iY + (mp_BoxFrameFootRel.iHeight + mp_FontFoot->getHeight())/2, width - 30, g_Locale->getText(LOCALE_YT_PREV_RESULTS), color, 0, true); // UTF-8

	// green
	mp_Window->getIconSize(NEUTRINO_ICON_BUTTON_GREEN, &icon_w, &icon_h);
	mp_Window->paintIcon(NEUTRINO_ICON_BUTTON_GREEN, mp_BoxFrame.iX + xpos2, mp_BoxFrame.iY + mp_BoxFrameFootRel.iY + (mp_BoxFrameFootRel.iHeight - icon_h)/2 );

	mp_FontFoot->RenderString(mp_BoxFrame.iX + xpos2 + 5 + icon_w, mp_BoxFrame.iY + mp_BoxFrameFootRel.iY + (mp_BoxFrameFootRel.iHeight + mp_FontFoot->getHeight())/2, width -30, g_Locale->getText(LOCALE_YT_NEXT_RESULTS), color, 0, true); // UTF-8

	// yellow
	next_text = g_Locale->getText(LOCALE_MOVIEBROWSER_NEXT_FOCUS);

	mp_Window->getIconSize(NEUTRINO_ICON_BUTTON_YELLOW, &icon_w, &icon_h);
	mp_Window->paintIcon(NEUTRINO_ICON_BUTTON_YELLOW, mp_BoxFrame.iX + xpos3, mp_BoxFrame.iY + mp_BoxFrameFootRel.iY + (mp_BoxFrameFootRel.iHeight - icon_h)/2);

	mp_FontFoot->RenderString(mp_BoxFrame.iX + xpos3 + 5 + icon_w, mp_BoxFrame.iY + mp_BoxFrameFootRel.iY + (mp_BoxFrameFootRel.iHeight + mp_FontFoot->getHeight())/2, width - 30, next_text.c_str(), color, 0, true); // UTF-8

	// blue
	mp_Window->getIconSize(NEUTRINO_ICON_BUTTON_BLUE, &icon_w, &icon_h);
	mp_Window->paintIcon(NEUTRINO_ICON_BUTTON_BLUE, mp_BoxFrame.iX + xpos4, mp_BoxFrame.iY + mp_BoxFrameFootRel.iY + (mp_BoxFrameFootRel.iHeight - icon_h)/2);

	mp_FontFoot->RenderString(mp_BoxFrame.iX + xpos4 + 5 + icon_w, mp_BoxFrame.iY + mp_BoxFrameFootRel.iY + (mp_BoxFrameFootRel.iHeight + mp_FontFoot->getHeight())/2, width-30, g_Locale->getText(LOCALE_MOVIEBROWSER_SCAN_FOR_MOVIES), color, 0, true); // UTF-8
}

bool CMPBrowser::onButtonPress(neutrino_msg_t msg)
{
	dprintf(DEBUG_INFO, "CMPBrowser::onButtonPress %d\n", msg);
	
	bool result = false;
	
	result = onButtonPressMainFrame(msg);

	if(result == false)
	{
		// if Main Frame didnot process the button, the focused window may do
		switch(mp_windowFocus)
		{
			case MPB_FOCUS_BROWSER:
			 	result = onButtonPressBrowserList(msg);		
				break;
				
			case MPB_FOCUS_MOVIE_INFO:
			 	result = onButtonPressItemInfoList(msg);		
				break;
				
			default:
				break;
		}
	}
	
	return (result);
}

bool CMPBrowser::onButtonPressMainFrame(neutrino_msg_t msg)
{
	dprintf(DEBUG_INFO, "CMPBrowser::onButtonPressMainFrame: %d\n", msg);
	
	bool result = true;

	if (msg == CRCInput::RC_home)
	{
		result = false;
	}
	else if (msg == CRCInput::RC_green) 
	{
		;
	}
	else if (msg == CRCInput::RC_yellow) 
	{
		onSetFocusNext();
	}
	else if (msg == CRCInput::RC_blue) 
	{
		loadItems();
		refresh();
	}
	else if (msg == CRCInput::RC_red ) 
	{	
		;
	}
	else if ( msg == CRCInput::RC_info) 
	{
		if(mp_itemSelectionHandler != NULL)
		{
			mp_Window->paintBackground();
			mp_Window->blit();
	  
			//mp_ItemInfo.showItemInfo(*mp_itemSelectionHandler);
			showFileInfo();
			
			refresh();
		}
	}
	else if (msg == CRCInput::RC_setup) 
	{
		//showYTMenu();	
		// setup
	}
	else
	{
		dprintf(DEBUG_INFO, "CMPBrowser::onButtonPressMainFrame: none\r\n");
		
		result = false;
	}

	return (result);
}

bool CMPBrowser::onButtonPressBrowserList(neutrino_msg_t msg) 
{
	dprintf(DEBUG_INFO, "CMPBrowser::onButtonPressBrowserList %d\n", msg);
	
	bool result = true;
	
	if(msg == CRCInput::RC_up)
	{
		mp_Browser->scrollLineUp(1);
	}
	else if (msg == CRCInput::RC_down)
	{
		mp_Browser->scrollLineDown(1);
	}
	else if (msg == CRCInput::RC_page_up)
	{
		mp_Browser->scrollPageUp(1);
	}
	else if (msg == CRCInput::RC_page_down)
	{
		mp_Browser->scrollPageDown(1);
	}
	else if (msg == CRCInput::RC_left)
	{
		mp_Browser->scrollPageUp(1);
	}
	else if (msg == CRCInput::RC_right)
	{
		mp_Browser->scrollPageDown(1);
	}
	else
	{
		// default
		result = false;
	}
	
	if(result == true)
		updateItemSelection();

	return (result);
}

bool CMPBrowser::onButtonPressItemInfoList(neutrino_msg_t msg) 
{
	dprintf(DEBUG_INFO, "CMPBrowser::onButtonPressItemInfoList: %d\n", msg);
	
	bool result = true;
	
	if(msg == CRCInput::RC_up)
	{
		mp_Info->scrollPageUp(1);
	}
	else if (msg == CRCInput::RC_down)
	{
		mp_Info->scrollPageDown(1);
	}
	else
	{
		// default
		result = false;
	}	

	return (result);
}

void CMPBrowser::onSetGUIWindow(MPB_GUI gui)
{
	mp_settings.gui = gui;
	
	if(gui == MPB_GUI_MOVIE_INFO)
	{
		dprintf(DEBUG_NORMAL, "CMPBrowser::onSetGUIWindow:\n");
		
		// Paint these frames ...
		mp_showItemInfo = true;
		mp_showBrowserFiles = true;
		
		mp_Browser->paint();
		onSetFocus(MPB_FOCUS_BROWSER);
		mp_Info->paint();
		refreshItemInfo();
	}
}

void CMPBrowser::onSetFocus(MPB_FOCUS new_focus)
{
	dprintf(DEBUG_INFO, "CMPBrowser::onSetFocus: %d\n", new_focus);
	
	mp_windowFocus = new_focus;
	
	if(mp_windowFocus == MPB_FOCUS_BROWSER)
	{
		mp_Browser->showSelection(true);
	}
	else if(mp_windowFocus == MPB_FOCUS_MOVIE_INFO)
	{
		mp_Browser->showSelection(false);
	}
	
	updateItemSelection();
	refreshFoot();	
}

void CMPBrowser::onSetFocusNext(void) 
{
	dprintf(DEBUG_INFO, "CMPBrowser::onSetFocusNext:\n");
	
	if(mp_settings.gui == MPB_GUI_MOVIE_INFO)
	{
		if(mp_windowFocus == MPB_FOCUS_BROWSER)
		{
			dprintf(DEBUG_NORMAL, "CMPBrowser::onSetFocusNext: MPB_FOCUS_MOVIE_INFO\r\n");
			
			onSetFocus(MPB_FOCUS_MOVIE_INFO);
			mp_windowFocus = MPB_FOCUS_MOVIE_INFO;
		}
		else
		{
			dprintf(DEBUG_NORMAL, "CMPBrowser::onSetFocusNext: MPB_FOCUS_BROWSER\r\n");
			onSetFocus(MPB_FOCUS_BROWSER);
		}
	}
}

void CMPBrowser::updateItemSelection(void)
{
	dprintf(DEBUG_INFO, "CMPBrowser::updateItemSelection: %d\n", mp_windowFocus);
	
	if (mp_ItemInfo.size() == 0) 
		return;
	
	bool new_selection = false;
	 
	unsigned int old_item_selection;
	
	if(mp_windowFocus == MPB_FOCUS_BROWSER)
	{
		if(mp_vHandleBrowserList.size() == 0)
		{
			// There are no elements in the Filebrowser, clear all handles
			mp_currentBrowserSelection = 0;
			mp_itemSelectionHandler = NULL;
			new_selection = true;
		}
		else
		{
			old_item_selection = mp_currentBrowserSelection;
			mp_currentBrowserSelection = mp_Browser->getSelectedLine();
			
			if(mp_currentBrowserSelection != old_item_selection)
				new_selection = true;
			
			if(mp_currentBrowserSelection < mp_vHandleBrowserList.size())
				mp_itemSelectionHandler = &mp_vHandleBrowserList[mp_currentBrowserSelection];
		}
	}
	
	if(new_selection == true)
	{
		refreshItemInfo();
		refreshLCD();
	}
}

void CMPBrowser::addMusicDeluxe(void)
{
	//
	CFile file;
		
	file.Title = "Music deluxe";
	file.Info1 = "stream";
	file.Info2 = "Musik Sender";
	file.thumbnail = PLUGINDIR "/mediaportal/deluxemusic.png";
	file.Name = "Music Deluxe";
	file.Url = "rtmp://flash.cdn.deluxemusic.tv/deluxemusic.tv-live/web_850.stream";
		
	mp_ItemInfo.push_back(file);
}

void CMPBrowser::addFilmon1(void)
{
	//
	CFile file;
		
	file.Title = "Filmon 1";
	file.Info1 = "stream";
	file.Info2 = "Porno Sender";
	file.thumbnail = PLUGINDIR "/mediaportal/filmon.png";
	file.Name = "Filmon 1";
	file.Url = "rtmp://live190.la3.origin.filmon.com:8086/live/247.high.stream";
		
	mp_ItemInfo.push_back(file);
}

void CMPBrowser::addFilmon2(void)
{
	//
	CFile file;
		
	file.Title = "Filmon 2";
	file.Info1 = "stream";
	file.Info2 = "Porno Sender";
	file.thumbnail = PLUGINDIR "/mediaportal/filmon.png";
	file.Name = "Filmon 2";
	file.Url = "rtmp://live190.la3.origin.filmon.com:8086/live/245.high.stream";
		
	mp_ItemInfo.push_back(file);
}

void CMPBrowser::addFilmon3(void)
{
	//
	CFile file;
		
	file.Title = "Filmon 3";
	file.Info1 = "stream";
	file.Info2 = "Porno Sender";
	file.thumbnail = PLUGINDIR "/mediaportal/filmon.png";
	file.Name = "Filmon 3";
	file.Url = "rtmp://live190.la3.origin.filmon.com:8086/live/246.high.stream";
		
	mp_ItemInfo.push_back(file);
}

void CMPBrowser::loadItems(void)
{
	dprintf(DEBUG_NORMAL, "CMPBrowser::loadItems:\n");
	
	//first clear screen
	mp_Window->paintBackground();
	mp_Window->blit();
	
	// clear
	mp_ItemInfo.clear();
	
	// music deluxe
	addMusicDeluxe();
	addFilmon1();
	addFilmon2();
	addFilmon3();

	refreshBrowserList();	
	refreshItemInfo();	// is done by refreshBrowserList if needed
	
	mp_reload_items = false;
}

bool CMPBrowser::getItemInfoItem(CFile &item_info, MPB_INFO_ITEM item, std::string* item_string)
{
	#define MAX_STR_TMP 100
	bool result = true;
	*item_string = "";

	switch(item)
	{
		case MPB_INFO_FILENAME:
			*item_string = item_info.getFileName();
			break;
			
		case MPB_INFO_TITLE:
			*item_string = item_info.Title;
			
			if(item_info.Title.empty())
				result = false;
			break;
			
		case MPB_INFO_INFO1:
			*item_string = item_info.Info1;
			break;
			
		case MPB_INFO_MAX_NUMBER:
		default:
			*item_string = "";
			result = false;
			break;
	}
	
	return(result);
}

void CMPBrowser::showFileInfo()
{
	std::string buffer;
	
	// prepare print buffer  
	buffer = mp_itemSelectionHandler->Info1;
	buffer += "\n";
	buffer += mp_itemSelectionHandler->Info2;
	buffer += "\n";

	// thumbnail
	int pich = 246;	//FIXME
	int picw = 162; 	//FIXME
	int lx = g_settings.screen_StartX + 50 + g_settings.screen_EndX - g_settings.screen_StartX - 100 - (picw + 20);
	int ly = g_settings.screen_StartY + 50 + g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getHeight() + 20;
	
	std::string thumbnail = "";
	if(!access(mp_itemSelectionHandler->thumbnail.c_str(), F_OK))
		thumbnail = mp_itemSelectionHandler->thumbnail;
	
	int mode =  CInfoBox::SCROLL | CInfoBox::TITLE | CInfoBox::FOOT | CInfoBox::BORDER;// | //CInfoBox::NO_AUTO_LINEBREAK | //CInfoBox::CENTER | //CInfoBox::AUTO_WIDTH | //CInfoBox::AUTO_HIGH;
	CBox position(g_settings.screen_StartX + 50, g_settings.screen_StartY + 50, g_settings.screen_EndX - g_settings.screen_StartX - 100, g_settings.screen_EndY - g_settings.screen_StartY - 100); 
	
	CInfoBox * infoBox = new CInfoBox(mp_itemSelectionHandler->Title.c_str(), g_Font[SNeutrinoSettings::FONT_TYPE_MENU], mode, &position, mp_itemSelectionHandler->Title.c_str(), g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE], NULL);
	infoBox->setText(&buffer, thumbnail, lx, ly, picw, pich);
	infoBox->exec();
	delete infoBox;
}

//plugin API
void plugin_init(void)
{
}

void plugin_del(void)
{
}

void plugin_exec(void)
{
	CMPBrowser * mpHandler = new CMPBrowser();
	
BROWSER:
	if (mpHandler->exec()) 
	{
		// get the current file name
		CFile * file;

		if ((file = mpHandler->getSelectedFile()) != NULL) 
		{
			dprintf(DEBUG_NORMAL, "CMPBrowser::exec_plugin: %s\n", file->Name.c_str());
			
			moviePlayerGui->filename = file->Url.c_str();
				
			// movieinfos
			moviePlayerGui->Title = file->Title;
			moviePlayerGui->Info1 = file->Info1;
			moviePlayerGui->Info2 = file->Info2;
			moviePlayerGui->thumbnail = file->thumbnail;
				
			// play
			moviePlayerGui->exec(NULL, "urlplayback");
		}
		
		neutrino_msg_t msg;
		neutrino_msg_data_t data;

		g_RCInput->getMsg_ms(&msg, &data, 10); // 1 sec
		
		if (msg != CRCInput::RC_home) 
		{
			goto BROWSER;
		}
	}
	
	delete mpHandler;
}


