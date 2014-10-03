/*
  neutrinoHD2 project
  
  https://code.google.com/p/neutrinohd2/
  
  $Id: ard.cpp 2014/03/09 mohousch Exp $

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

#include <ard.h>


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

CFont* CARDBrowser::mp_FontFoot = NULL;
CFont* CARDBrowser::mp_FontTitle = NULL;

const neutrino_locale_t mp_localizedItemName[MPB_INFO_MAX_NUMBER + 1] =
{
	LOCALE_MOVIEBROWSER_SHORT_FILENAME,
	LOCALE_MOVIEBROWSER_SHORT_TITLE ,
	LOCALE_MOVIEBROWSER_SHORT_INFO1,
	LOCALE_MOVIEBROWSER_SHORT_RECORDDATE,
	NONEXISTANT_LOCALE
};

// default row size in pixel for any element
#define	MPB_ROW_WIDTH_FILENAME 		150
#define	MPB_ROW_WIDTH_TITLE		750
#define	MPB_ROW_WIDTH_INFO1		200
#define	MPB_ROW_WIDTH_RECORDDATE 	120

const int mp_defaultRowWidth[MPB_INFO_MAX_NUMBER + 1] = 
{
	MPB_ROW_WIDTH_FILENAME ,
	MPB_ROW_WIDTH_TITLE,
	MPB_ROW_WIDTH_INFO1,
	MPB_ROW_WIDTH_RECORDDATE ,
	0 //MPB_ROW_WIDTH_MAX_NUMBER 
};

CARDBrowser::CARDBrowser()
{
	dprintf(DEBUG_NORMAL, "$Id: ard browser, v 0.0.1 2014/10/02 17:52:30 mohousch Exp $\n");
	init();
}

CARDBrowser::~CARDBrowser()
{
	dprintf(DEBUG_NORMAL, "CARDBrowser: del\n");
	
	mp_ItemInfo.clear();
	mp_vHandleBrowserList.clear();

	mp_itemSelectionHandler = NULL;

	for(int i = 0; i < LF_MAX_ROWS; i++)
	{
		mp_browserListLines.lineArray[i].clear();
	}
}

void CARDBrowser::init(void)
{
	dprintf(DEBUG_NORMAL, "CARDBrowser::init\n");
	
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
		mp_settings.browserRowNr = 3;
		mp_settings.browserRowItem[0] = MPB_INFO_TITLE;
		mp_settings.browserRowItem[1] = MPB_INFO_INFO1;
		mp_settings.browserRowItem[2] = MPB_INFO_RECORDDATE;
		mp_settings.browserRowWidth[0] = mp_defaultRowWidth[mp_settings.browserRowItem[0]];		//300;
		mp_settings.browserRowWidth[1] = mp_defaultRowWidth[mp_settings.browserRowItem[1]]; 		//100;
		mp_settings.browserRowWidth[2] = mp_defaultRowWidth[mp_settings.browserRowItem[2]]; 		//80;
	}

	initFrames();
	
	refreshBrowserList();	
}

void CARDBrowser::initGlobalSettings(void)
{
	dprintf(DEBUG_NORMAL, "CARDBrowser::initGlobalSettings\n");
	
	mp_settings.gui = MPB_GUI_MOVIE_INFO;

	// Browser List
	mp_settings.browserFrameHeight = g_settings.screen_EndY - g_settings.screen_StartY - 20 - ((g_settings.screen_EndY - g_settings.screen_StartY - 20)>>1) - (INTER_FRAME_SPACE>>1);
	
	mp_settings.browserRowNr = 3;
	mp_settings.browserRowItem[0] = MPB_INFO_TITLE;
	mp_settings.browserRowItem[1] = MPB_INFO_INFO1;
	mp_settings.browserRowItem[2] = MPB_INFO_RECORDDATE;
	mp_settings.browserRowWidth[0] = mp_defaultRowWidth[mp_settings.browserRowItem[0]];
	mp_settings.browserRowWidth[1] = mp_defaultRowWidth[mp_settings.browserRowItem[1]];
	mp_settings.browserRowWidth[2] = mp_defaultRowWidth[mp_settings.browserRowItem[2]];
}

void CARDBrowser::initFrames(void)
{
	dprintf(DEBUG_NORMAL, "CARDBrowser::initFrames\n");
	
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

int CARDBrowser::exec(CMenuTarget * parent, const std::string & actionKey)
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

int CARDBrowser::exec()
{
	bool res = false;

	dprintf(DEBUG_NORMAL, "CARDBrowser::exec\n");
	
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
				dprintf(DEBUG_NORMAL, "CARDBrowser::exec: Timerevent\n");
				
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

void CARDBrowser::hide(void)
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

int CARDBrowser::paint(void)
{
	dprintf(DEBUG_NORMAL, "CARDBrowser::paint\n");

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

void CARDBrowser::refresh(void)
{
	dprintf(DEBUG_NORMAL, "CARDBrowser::refresh\n");
	
	refreshTitle();

	if (mp_Browser != NULL && mp_showBrowserFiles == true )
		 mp_Browser->refresh();
	
	if (mp_Info != NULL && mp_showItemInfo == true) 
		refreshItemInfo();
		
	refreshFoot();
	refreshLCD();
}

CFile * CARDBrowser::getSelectedFile(void)
{
	dprintf(DEBUG_INFO, "CARDBrowser::getSelectedFile: %s\n", mp_itemSelectionHandler->Name.c_str());

	if(mp_itemSelectionHandler != NULL)
		return(mp_itemSelectionHandler);
	else
		return(NULL);
}

void CARDBrowser::refreshItemInfo(void)
{
	dprintf(DEBUG_INFO, "CARDBrowser::refreshItemInfo: mp_ItemInfo.size %d\n", mp_ItemInfo.size());
	
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
		bool logo_ok = false;
		
		int pich = mp_BoxFrameInfo.iHeight - 10;
		int picw = pich * (4.0 / 3);		// 4/3 format pics
		
		int lx, ly;
		
		// youtube
		std::string fname;

		fname = mp_itemSelectionHandler->thumbnail;
		
		logo_ok = !access(fname.c_str(), F_OK);
		
		// display screenshot if exists
		if(logo_ok) 
		{
			lx = mp_BoxFrameInfo.iX + mp_BoxFrameInfo.iWidth - picw - 10;
			ly = mp_BoxFrameInfo.iY + (mp_BoxFrameInfo.iHeight - pich)/2;
			
			mp_Info->setText(&mp_itemSelectionHandler->Info2, mp_BoxFrameInfo.iWidth - picw - 20, fname, lx, ly, picw, pich);
		}
		else
			mp_Info->setText(&mp_itemSelectionHandler->Info2);
	}
	
	mp_Window->blit();
}

void CARDBrowser::refreshLCD(void)
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

void CARDBrowser::refreshBrowserList(void) 
{
	dprintf(DEBUG_INFO, "CARDBrowser::refreshBrowserList\n");
	
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

void CARDBrowser::refreshTitle(void) 
{
	dprintf(DEBUG_INFO, "CARDBrowser::refreshTitle\n");
	
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

void CARDBrowser::refreshFoot(void) 
{
	dprintf(DEBUG_INFO, "CARDBrowser::refreshFoot\n");
	
	mp_Window->paintBoxRel(mp_BoxFrame.iX + mp_BoxFrameFootRel.iX, mp_BoxFrame.iY + mp_BoxFrameFootRel.iY, mp_BoxFrameFootRel.iWidth, mp_BoxFrameFootRel.iHeight, COL_MENUCONTENT_PLUS_6 );
	mp_Window->paintBoxRel(mp_BoxFrame.iX + mp_BoxFrameFootRel.iX + 2, mp_BoxFrame.iY + mp_BoxFrameFootRel.iY + 2 , mp_BoxFrameFootRel.iWidth - 4, mp_BoxFrameFootRel.iHeight - 4, COL_MENUCONTENTSELECTED_PLUS_0);
}

bool CARDBrowser::onButtonPress(neutrino_msg_t msg)
{
	dprintf(DEBUG_INFO, "CARDBrowser::onButtonPress %d\n", msg);
	
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

bool CARDBrowser::onButtonPressMainFrame(neutrino_msg_t msg)
{
	dprintf(DEBUG_INFO, "CARDBrowser::onButtonPressMainFrame: %d\n", msg);
	
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
		//ytparser.Cleanup();
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
		dprintf(DEBUG_INFO, "CARDBrowser::onButtonPressMainFrame: none\r\n");
		
		result = false;
	}

	return (result);
}

bool CARDBrowser::onButtonPressBrowserList(neutrino_msg_t msg) 
{
	dprintf(DEBUG_INFO, "CARDBrowser::onButtonPressBrowserList %d\n", msg);
	
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

bool CARDBrowser::onButtonPressItemInfoList(neutrino_msg_t msg) 
{
	dprintf(DEBUG_INFO, "CARDBrowser::onButtonPressItemInfoList: %d\n", msg);
	
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

void CARDBrowser::onSetGUIWindow(MPB_GUI gui)
{
	mp_settings.gui = gui;
	
	if(gui == MPB_GUI_MOVIE_INFO)
	{
		dprintf(DEBUG_NORMAL, "CARDBrowser::onSetGUIWindow:\n");
		
		// Paint these frames ...
		mp_showItemInfo = true;
		mp_showBrowserFiles = true;
		
		mp_Browser->paint();
		onSetFocus(MPB_FOCUS_BROWSER);
		mp_Info->paint();
		refreshItemInfo();
	}
}

void CARDBrowser::onSetFocus(MPB_FOCUS new_focus)
{
	dprintf(DEBUG_INFO, "CARDBrowser::onSetFocus: %d\n", new_focus);
	
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

void CARDBrowser::onSetFocusNext(void) 
{
	dprintf(DEBUG_INFO, "CARDBrowser::onSetFocusNext:\n");
	
	if(mp_settings.gui == MPB_GUI_MOVIE_INFO)
	{
		if(mp_windowFocus == MPB_FOCUS_BROWSER)
		{
			dprintf(DEBUG_NORMAL, "CARDBrowser::onSetFocusNext: MPB_FOCUS_MOVIE_INFO\r\n");
			
			onSetFocus(MPB_FOCUS_MOVIE_INFO);
			mp_windowFocus = MPB_FOCUS_MOVIE_INFO;
		}
		else
		{
			dprintf(DEBUG_NORMAL, "CARDBrowser::onSetFocusNext: MPB_FOCUS_BROWSER\r\n");
			onSetFocus(MPB_FOCUS_BROWSER);
		}
	}
}

void CARDBrowser::updateItemSelection(void)
{
	dprintf(DEBUG_INFO, "CARDBrowser::updateItemSelection: %d\n", mp_windowFocus);
	
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

void CARDBrowser::loadItems(void)
{
	dprintf(DEBUG_NORMAL, "CARDBrowser::loadItems:\n");
	
	//first clear screen
	mp_Window->paintBackground();
	mp_Window->blit();
	
	// music deluxe
	addMusicDeluxe();
	addFilmon1();
	addFilmon2();
	addFilmon3();
	addNetzKino();
	addYouTube();

	refreshBrowserList();	
	refreshItemInfo();	// is done by refreshBrowserList if needed
	
	mp_reload_items = false;
}

bool CARDBrowser::getItemInfoItem(CFile &item_info, MPB_INFO_ITEM item, std::string* item_string)
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
			
		case MPB_INFO_RECORDDATE:
			//*item_string = item_info.date;	
			break;
			
		case MPB_INFO_MAX_NUMBER:
		default:
			*item_string = "";
			result = false;
			break;
	}
	
	return(result);
}

