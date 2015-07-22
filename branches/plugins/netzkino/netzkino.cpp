/*
  neutrinoHD2 project
  
  https://code.google.com/p/neutrinohd2/
  
  $Id: netzkino.cpp 2014/10/03 mohousch Exp $

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

#include <netzkino.h>

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

#define NEUTRINO_ICON_NETZKINO_SMALL		PLUGINDIR "/netzkino/netzkino_small.png"

CFont* CNetzKinoBrowser::m_pcFontFoot = NULL;
CFont* CNetzKinoBrowser::m_pcFontTitle = NULL;

const neutrino_locale_t m_localizedItemName[NKB_INFO_MAX_NUMBER + 1] =
{
	LOCALE_MOVIEBROWSER_SHORT_FILENAME,
	LOCALE_MOVIEBROWSER_SHORT_TITLE ,
	LOCALE_MOVIEBROWSER_SHORT_INFO1,
	LOCALE_MOVIEBROWSER_SHORT_RECORDDATE,
	NONEXISTANT_LOCALE
};

// default row size in pixel for any element
#define	NKB_ROW_WIDTH_FILENAME 		150
#define	NKB_ROW_WIDTH_TITLE		750
#define	NKB_ROW_WIDTH_INFO1		200
#define	NKB_ROW_WIDTH_RECORDDATE 	120

const int m_defaultRowWidth[NKB_INFO_MAX_NUMBER + 1] = 
{
	NKB_ROW_WIDTH_FILENAME ,
	NKB_ROW_WIDTH_TITLE,
	NKB_ROW_WIDTH_INFO1,
	NKB_ROW_WIDTH_RECORDDATE ,
	0 //MB_ROW_WIDTH_MAX_NUMBER 
};
 
CNetzKinoBrowser::CNetzKinoBrowser()
{
	dprintf(DEBUG_NORMAL, "$Id: netzkino browser, v 0.0.1 2014/09/15 12:00:30 mohousch Exp $\n");
	init();
}

CNetzKinoBrowser::~CNetzKinoBrowser()
{
	dprintf(DEBUG_NORMAL, "CNetzKinoBrowser: del\n");
	
	m_vMovieInfo.clear();
	m_vHandleBrowserList.clear();

	m_movieSelectionHandler = NULL;

	for(int i = 0; i < LF_MAX_ROWS; i++)
	{
		m_browserListLines.lineArray[i].clear();
	}
}

void CNetzKinoBrowser::init(void)
{
	dprintf(DEBUG_NORMAL, "CNetzKinoBrowser::init\n");
	
	initGlobalSettings();
		
	m_reload_movies = true;

	m_pcWindow = CFrameBuffer::getInstance();
	
	m_pcBrowser = NULL;
	m_pcInfo = NULL;
	
	m_windowFocus = NKB_FOCUS_BROWSER;
	
	m_textTitle = g_Locale->getText(LOCALE_NETZKINO);
	
	m_movieSelectionHandler = NULL;
	m_currentBrowserSelection = 0;
 	m_prevBrowserSelection = 0;
	
	// browser
	if(m_settings.browserFrameHeight < MIN_NKBROWSER_FRAME_HEIGHT )
       		m_settings.browserFrameHeight = MIN_NKBROWSER_FRAME_HEIGHT;
	if(m_settings.browserFrameHeight > MAX_NKBROWSER_FRAME_HEIGHT)
        	m_settings.browserFrameHeight = MAX_NKBROWSER_FRAME_HEIGHT;
	
	// Browser List 
	if(m_settings.browserRowNr == 0)
	{
		dprintf(DEBUG_NORMAL, " row error\n");
		
		// init browser row elements if not configured correctly by neutrino.config
		m_settings.browserRowNr = 3;
		m_settings.browserRowItem[0] = NKB_INFO_TITLE;
		m_settings.browserRowItem[1] = NKB_INFO_INFO1;
		m_settings.browserRowItem[2] = NKB_INFO_RECORDDATE;
		m_settings.browserRowWidth[0] = m_defaultRowWidth[m_settings.browserRowItem[0]];		//300;
		m_settings.browserRowWidth[1] = m_defaultRowWidth[m_settings.browserRowItem[1]]; 		//100;
		m_settings.browserRowWidth[2] = m_defaultRowWidth[m_settings.browserRowItem[2]]; 		//80;
	}

	initFrames();
	
	refreshBrowserList();	
}

void CNetzKinoBrowser::initGlobalSettings(void)
{
	dprintf(DEBUG_NORMAL, "CNetzKinoBrowser::initGlobalSettings\n");
	
	m_settings.gui = NKB_GUI_MOVIE_INFO;

	// Browser List
	m_settings.browserFrameHeight = g_settings.screen_EndY - g_settings.screen_StartY - 20 - ((g_settings.screen_EndY - g_settings.screen_StartY - 20)>>1) - (INTER_FRAME_SPACE>>1);
	
	m_settings.browserRowNr = 3;
	m_settings.browserRowItem[0] = NKB_INFO_TITLE;
	m_settings.browserRowItem[1] = NKB_INFO_INFO1;
	m_settings.browserRowItem[2] = NKB_INFO_RECORDDATE;
	m_settings.browserRowWidth[0] = m_defaultRowWidth[m_settings.browserRowItem[0]];
	m_settings.browserRowWidth[1] = m_defaultRowWidth[m_settings.browserRowItem[1]];
	m_settings.browserRowWidth[2] = m_defaultRowWidth[m_settings.browserRowItem[2]];
	
	// netzkino
	m_settings.nkmode = cNKFeedParser::CATEGORY;
	m_settings.nkcategory = 8;	//8=Highlights, 81=neu bei Netzkino
	m_settings.nkcategoryname = "Highlights";
}

void CNetzKinoBrowser::initFrames(void)
{
	dprintf(DEBUG_NORMAL, "CNetzKinoBrowser::initFrames\n");
	
	m_pcFontFoot  = FOOT_FONT;
	m_pcFontTitle = TITLE_FONT;
	
	m_cBoxFrame.iX = 			g_settings.screen_StartX + 10;
	m_cBoxFrame.iY = 			g_settings.screen_StartY + 10;
	m_cBoxFrame.iWidth = 			g_settings.screen_EndX - g_settings.screen_StartX - 20;
	m_cBoxFrame.iHeight = 			g_settings.screen_EndY - g_settings.screen_StartY - 20;

	m_cBoxFrameTitleRel.iX =		0;
	m_cBoxFrameTitleRel.iY = 		0;
	m_cBoxFrameTitleRel.iWidth = 		m_cBoxFrame.iWidth;
	m_cBoxFrameTitleRel.iHeight = 		m_pcFontTitle->getHeight();

	m_cBoxFrameBrowserList.iX = 		m_cBoxFrame.iX;
	m_cBoxFrameBrowserList.iY = 		m_cBoxFrame.iY + m_cBoxFrameTitleRel.iHeight;
	m_cBoxFrameBrowserList.iWidth = 	m_cBoxFrame.iWidth;
	m_cBoxFrameBrowserList.iHeight = 	m_settings.browserFrameHeight;

	m_cBoxFrameFootRel.iX = 		0;
	m_cBoxFrameFootRel.iY = 		m_cBoxFrame.iHeight - m_pcFontFoot->getHeight()*2; //FIXME
	m_cBoxFrameFootRel.iWidth = 		m_cBoxFrameBrowserList.iWidth;
	m_cBoxFrameFootRel.iHeight = 		m_pcFontFoot->getHeight()*2; //FIXME
	
	m_cBoxFrameInfo.iX = 			m_cBoxFrameBrowserList.iX;
	m_cBoxFrameInfo.iY = 			m_cBoxFrameBrowserList.iY + m_cBoxFrameBrowserList.iHeight + INTER_FRAME_SPACE;
	m_cBoxFrameInfo.iWidth = 		m_cBoxFrameBrowserList.iWidth;
	m_cBoxFrameInfo.iHeight = 		m_cBoxFrame.iHeight - m_cBoxFrameBrowserList.iHeight - INTER_FRAME_SPACE - m_cBoxFrameFootRel.iHeight - m_cBoxFrameTitleRel.iHeight;
}

int CNetzKinoBrowser::exec(CMenuTarget * parent, const std::string & actionKey)
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

int CNetzKinoBrowser::exec()
{
	bool res = false;

	dprintf(DEBUG_NORMAL, "CNetzKinoBrowser::exec\n");
	
	int timeout = -1;
	int returnDefaultOnTimeout = true;
	neutrino_msg_t      msg;
	neutrino_msg_data_t data;

	CVFD::getInstance()->setMode(CVFD::MODE_MENU_UTF8, g_Locale->getText(LOCALE_NETZKINO));
	
	initGlobalSettings();
	
	// init frames
	initFrames();

	// Clear all, to avoid 'jump' in screen 
	m_vHandleBrowserList.clear();
	
	m_movieSelectionHandler = NULL;

	for(int i = 0; i < LF_MAX_ROWS; i++)
	{
		m_browserListLines.lineArray[i].clear();
	}

	// paint mb
	if(paint() == false)
		return res;// paint failed due to less memory , exit 

	if ( timeout == -1 )
		timeout = g_settings.timing[SNeutrinoSettings::TIMING_FILEBROWSER];

	unsigned long long timeoutEnd = CRCInput::calcTimeoutEnd( timeout );
	
	// reload movies
	if(m_reload_movies == true)
	{
		dprintf(DEBUG_NORMAL, "CNetzKinoBrowser::exec\n");
		loadMovies();
	}
	else
	{
		// since we cleared everything above, we have to refresh the list now.
		refreshBrowserList();	
	}

	// get old movie selection and set position in windows	
	m_currentBrowserSelection = m_prevBrowserSelection;
	m_pcBrowser->setSelectedLine(m_currentBrowserSelection);

	// update movie selection
	updateMovieSelection();

	// refresh title
	refreshTitle();
	
	// on set guiwindow
	onSetGUIWindow(m_settings.gui);
	
	// browser paint 
	m_pcBrowser->paint();
	m_pcWindow->blit();

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
				dprintf(DEBUG_NORMAL, "CNetzKinoBrowser::exec: Timerevent\n");
				
				loop = false;
			}
			else if(msg == CRCInput::RC_ok)
			{
				if(m_movieSelectionHandler != NULL)
				{
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
				dprintf(DEBUG_NORMAL, "CNetzKinoBrowser::exec: getInstance\n");
				
				loop = false;
			}
		}
		
		m_pcWindow->blit();	

		if ( msg <= CRCInput::RC_MaxRC )
			timeoutEnd = CRCInput::calcTimeoutEnd(timeout); // calcualate next timeout
	}
	
	hide();
	
	//
	m_prevBrowserSelection = m_currentBrowserSelection;
	
	return (res);
}

void CNetzKinoBrowser::hide(void)
{
	dprintf(DEBUG_NORMAL, "CNetzKinoBrowser::hide\n");
	
	m_pcWindow->paintBackground();
	m_pcWindow->blit();
	
	if (m_pcBrowser != NULL)
	{
		m_currentBrowserSelection = m_pcBrowser->getSelectedLine();
		delete m_pcBrowser;
		m_pcBrowser = NULL;
	}
	
	if (m_pcInfo != NULL) 
	{
		delete m_pcInfo;
		m_pcInfo = NULL;
	}
}

int CNetzKinoBrowser::paint(void)
{
	dprintf(DEBUG_NORMAL, "CNetzKinoBrowser::paint\n");

	CVFD::getInstance()->setMode(CVFD::MODE_MENU_UTF8, g_Locale->getText(LOCALE_NETZKINO));	

	m_pcBrowser = new CListFrame(&m_browserListLines, NULL, CListFrame::SCROLL, &m_cBoxFrameBrowserList);
	m_pcInfo = new CTextBox(" ", NULL, CTextBox::SCROLL, &m_cBoxFrameInfo);	

	if(m_pcBrowser == NULL || m_pcInfo == NULL )
	{
		if (m_pcBrowser != NULL)
			delete m_pcBrowser;

		if (m_pcInfo != NULL) 
			delete m_pcInfo;

		m_pcInfo = NULL;
		m_pcBrowser = NULL;

		return (false);
	} 
	
	NKStart = 0;
	NKEnd = m_pcBrowser->getLinesPerPage();
	
	return (true);
}

void CNetzKinoBrowser::refresh(void)
{
	dprintf(DEBUG_NORMAL, "CNetzKinoBrowser::refresh\n");
	
	refreshTitle();

	if (m_pcBrowser != NULL && m_showBrowserFiles == true )
		 m_pcBrowser->refresh();
	
	if (m_pcInfo != NULL && m_showMovieInfo == true) 
	{
		refreshMovieInfo();
		m_pcInfo->refresh();
	}
		
	refreshFoot();
	refreshLCD();
}

CFile * CNetzKinoBrowser::getSelectedFile(void)
{
	dprintf(DEBUG_INFO, "CNetzKinoBrowser::getSelectedFile: %s\n", m_movieSelectionHandler->file.Name.c_str());

	if(m_movieSelectionHandler != NULL)
		return(&m_movieSelectionHandler->file);
	else
		return(NULL);
}

void CNetzKinoBrowser::refreshMovieInfo(void)
{
	dprintf(DEBUG_INFO, "CNetzKinoBrowser::refreshMovieInfo: m_vMovieInfo.size %d\n", m_vMovieInfo.size());
	
	std::string emptytext = " ";
	
	if(m_vMovieInfo.size() <= 0) 
	{
		if(m_pcInfo != NULL)
			m_pcInfo->setText(&emptytext);
		return;
	}
	
	if (m_movieSelectionHandler == NULL)
	{
		// There is no selected element, clear LCD
		m_pcInfo->setText(&emptytext);
	}
	else
	{
		int pich = 0;
		int picw = 0;
		int lx = 0;
		int ly = 0;
		
		if(!access(m_movieSelectionHandler->tfile.c_str(), F_OK))
		{
			pich = m_cBoxFrameInfo.iHeight - 20;
			picw = pich * (4.0 / 3);		// 4/3 format pics
		
			// netzkino
			picw /= 2;
			
			lx = m_cBoxFrameInfo.iX + m_cBoxFrameInfo.iWidth - (picw + SCROLLBAR_WIDTH + 10);
			ly = m_cBoxFrameInfo.iY + (m_cBoxFrameInfo.iHeight - pich)/2;
		}
		
		m_pcInfo->setText(&m_movieSelectionHandler->epgInfo2, m_movieSelectionHandler->tfile, lx, ly, picw, pich);
	}
	
	m_pcWindow->blit();
}

void CNetzKinoBrowser::refreshLCD(void)
{
	if(m_vMovieInfo.size() <= 0) 
		return;

	//CVFD * lcd = CVFD::getInstance();
	if(m_movieSelectionHandler == NULL)
	{
		// There is no selected element, clear LCD
		//lcd->showMenuText(0, " ", -1, true); // UTF-8
		//lcd->showMenuText(1, " ", -1, true); // UTF-8
	}
	else
	{
		CVFD::getInstance()->showMenuText(0, m_movieSelectionHandler->epgTitle.c_str(), -1, true); // UTF-8
	} 	
}

void CNetzKinoBrowser::refreshBrowserList(void) //P1
{
	dprintf(DEBUG_INFO, "CNetzKinoBrowser::refreshBrowserList\n");
	
	std::string string_item;

	// Initialise and clear list array
	m_browserListLines.rows = m_settings.browserRowNr;
	for(int row = 0; row < m_settings.browserRowNr; row++)
	{
		m_browserListLines.lineArray[row].clear();
		m_browserListLines.rowWidth[row] = m_settings.browserRowWidth[row];
		m_browserListLines.lineHeader[row] = g_Locale->getText(m_localizedItemName[m_settings.browserRowItem[row]]);
	}
	m_vHandleBrowserList.clear();
	
	if(m_vMovieInfo.size() <= 0) 
	{
		m_currentBrowserSelection = 0;
		m_movieSelectionHandler = NULL;
		if(m_pcBrowser != NULL)
			m_pcBrowser->setLines(&m_browserListLines);//FIXME last delete test
		return; // exit here if nothing else is to do
	}
	
	MI_MOVIE_INFO* movie_handle;
	
	// prepare Browser list for sorting and filtering
	for(unsigned int file = 0; file < m_vMovieInfo.size(); file++)
	{
		movie_handle = &(m_vMovieInfo[file]);
		m_vHandleBrowserList.push_back(movie_handle);
	}

	for(unsigned int handle = 0; handle < m_vHandleBrowserList.size() ;handle++)
	{
		for(int row = 0; row < m_settings.browserRowNr ;row++)
		{
			if ( getMovieInfoItem(*m_vHandleBrowserList[handle], m_settings.browserRowItem[row], &string_item) == false)
			{
				string_item = "n/a";
				if(m_settings.browserRowItem[row] == NKB_INFO_TITLE)
					getMovieInfoItem(*m_vHandleBrowserList[handle], NKB_INFO_FILENAME, &string_item);
			}
			m_browserListLines.lineArray[row].push_back(string_item);
		}
	}
	m_pcBrowser->setLines(&m_browserListLines);

	m_currentBrowserSelection = m_pcBrowser->getSelectedLine();
	
	// update selected movie if browser is in the focus
	if (m_windowFocus == NKB_FOCUS_BROWSER)
	{
		updateMovieSelection();	
	}
}

void CNetzKinoBrowser::refreshTitle(void) 
{
	//Paint Text Background
	dprintf(DEBUG_INFO, "CNetzKinoBrowser::refreshTitle\n");
	
	// title
	std::string title;
	std::string mb_icon;
	
	title = g_Locale->getText(LOCALE_NETZKINO);
	if (m_settings.nkmode == cNKFeedParser::SEARCH) 
	{
		title += ": ";
		title += g_Locale->getText(LOCALE_YT_SEARCH);
		title += " \"" + m_settings.nksearch + "\"";
	} 
	else if (m_settings.nkmode == cNKFeedParser::CATEGORY) 
	{
		title += ": ";
		title += m_settings.nkcategoryname;
	}
		
	mb_icon = NEUTRINO_ICON_NETZKINO_SMALL;
	//

	// head box
	m_pcWindow->paintBoxRel(m_cBoxFrame.iX + m_cBoxFrameTitleRel.iX, m_cBoxFrame.iY + m_cBoxFrameTitleRel.iY, m_cBoxFrameTitleRel.iWidth, m_cBoxFrameTitleRel.iHeight, TITLE_BACKGROUND_COLOR, RADIUS_MID, CORNER_TOP);
	
	// movie icon
	int icon_w, icon_h;
	m_pcWindow->getIconSize(mb_icon.c_str(), &icon_w, &icon_h);
	m_pcWindow->paintIcon(mb_icon, m_cBoxFrame.iX + m_cBoxFrameTitleRel.iX + 10, m_cBoxFrame.iY + m_cBoxFrameTitleRel.iY + (m_cBoxFrameTitleRel.iHeight - icon_h)/2);

	// setup icon
	m_pcWindow->getIconSize(NEUTRINO_ICON_BUTTON_SETUP, &icon_w, &icon_h);
	int xpos1 = m_cBoxFrame.iX + m_cBoxFrameTitleRel.iX + m_cBoxFrameTitleRel.iWidth - 10;
	int ypos = m_cBoxFrame.iY + m_cBoxFrameTitleRel.iY + (m_cBoxFrameTitleRel.iHeight - icon_w)/2;

	m_pcWindow->paintIcon(NEUTRINO_ICON_BUTTON_SETUP, xpos1 - icon_w, ypos);

	// help icon
	int icon_h_w, icon_h_h;
	m_pcWindow->getIconSize(NEUTRINO_ICON_BUTTON_SETUP, &icon_h_w, &icon_h_h);
	m_pcWindow->paintIcon(NEUTRINO_ICON_BUTTON_HELP, xpos1 - icon_w - 2 - icon_h_w, ypos);
	
	// head title
	m_pcFontTitle->RenderString(m_cBoxFrame.iX + m_cBoxFrameTitleRel.iX + TEXT_BORDER_WIDTH + icon_w + 10, m_cBoxFrame.iY+m_cBoxFrameTitleRel.iY + m_cBoxFrameTitleRel.iHeight, m_cBoxFrameTitleRel.iWidth - (TEXT_BORDER_WIDTH << 1) - 2*icon_w - 10 - icon_h_w, title.c_str(), TITLE_FONT_COLOR, 0, true); // UTF-8
}

void CNetzKinoBrowser::refreshFoot(void) 
{
	dprintf(DEBUG_INFO, "CNetzKinoBrowser::refreshFoot\n");
	
	uint8_t color = COL_MENUHEAD;
	
	// ok (play)
	std::string next_text = g_Locale->getText(LOCALE_MOVIEBROWSER_NEXT_FOCUS);
	
	// draw the background first
	m_pcWindow->paintBoxRel(m_cBoxFrame.iX + m_cBoxFrameFootRel.iX, m_cBoxFrame.iY + m_cBoxFrameFootRel.iY, m_cBoxFrameFootRel.iWidth, m_cBoxFrameFootRel.iHeight + 6, COL_MENUHEAD_PLUS_0, RADIUS_MID, CORNER_BOTTOM);

	int width = m_cBoxFrameFootRel.iWidth>>2;
	int xpos1 = m_cBoxFrameFootRel.iX + 10;
	int xpos2 = xpos1 + width;
	int xpos3 = xpos2 + width;
	int xpos4 = xpos3 + width;
	
	int icon_w = 0;
	int icon_h = 0;
	
	m_pcWindow->getIconSize(NEUTRINO_ICON_BUTTON_RED, &icon_w, &icon_h);

	// red
	m_pcWindow->getIconSize(NEUTRINO_ICON_BUTTON_RED, &icon_w, &icon_h);
	m_pcWindow->paintIcon(NEUTRINO_ICON_BUTTON_RED, m_cBoxFrame.iX + xpos1, m_cBoxFrame.iY + m_cBoxFrameFootRel.iY + (m_cBoxFrameFootRel.iHeight + 6 - icon_h)/2 );

	m_pcFontFoot->RenderString(m_cBoxFrame.iX + xpos1 + 5 + icon_w, m_cBoxFrame.iY + m_cBoxFrameFootRel.iY + (m_cBoxFrameFootRel.iHeight + m_pcFontFoot->getHeight())/2, width-30, g_Locale->getText(LOCALE_YT_PREV_RESULTS), color, 0, true); // UTF-8

	// green
	m_pcWindow->getIconSize(NEUTRINO_ICON_BUTTON_GREEN, &icon_w, &icon_h);
	m_pcWindow->paintIcon(NEUTRINO_ICON_BUTTON_GREEN, m_cBoxFrame.iX + xpos2, m_cBoxFrame.iY + m_cBoxFrameFootRel.iY + (m_cBoxFrameFootRel.iHeight + 6 - icon_h)/2 );

	m_pcFontFoot->RenderString(m_cBoxFrame.iX + xpos2 + 5 + icon_w, m_cBoxFrame.iY + m_cBoxFrameFootRel.iY + (m_cBoxFrameFootRel.iHeight + m_pcFontFoot->getHeight())/2, width -30, g_Locale->getText(LOCALE_YT_NEXT_RESULTS), color, 0, true); // UTF-8

	// yellow
	next_text = g_Locale->getText(LOCALE_MOVIEBROWSER_NEXT_FOCUS);

	m_pcWindow->getIconSize(NEUTRINO_ICON_BUTTON_YELLOW, &icon_w, &icon_h);
	m_pcWindow->paintIcon(NEUTRINO_ICON_BUTTON_YELLOW, m_cBoxFrame.iX + xpos3, m_cBoxFrame.iY + m_cBoxFrameFootRel.iY + (m_cBoxFrameFootRel.iHeight + 6 - icon_h)/2);

	m_pcFontFoot->RenderString(m_cBoxFrame.iX + xpos3 + 5 + icon_w, m_cBoxFrame.iY + m_cBoxFrameFootRel.iY + (m_cBoxFrameFootRel.iHeight + m_pcFontFoot->getHeight())/2, width-30, next_text.c_str(), color, 0, true); // UTF-8

	// blue
	m_pcWindow->getIconSize(NEUTRINO_ICON_BUTTON_BLUE, &icon_w, &icon_h);
	m_pcWindow->paintIcon(NEUTRINO_ICON_BUTTON_BLUE, m_cBoxFrame.iX + xpos4, m_cBoxFrame.iY + m_cBoxFrameFootRel.iY + (m_cBoxFrameFootRel.iHeight + 6 - icon_h)/2);

	m_pcFontFoot->RenderString(m_cBoxFrame.iX + xpos4 + 5 + icon_w, m_cBoxFrame.iY + m_cBoxFrameFootRel.iY + (m_cBoxFrameFootRel.iHeight + m_pcFontFoot->getHeight())/2, width-30, g_Locale->getText(LOCALE_MOVIEBROWSER_SCAN_FOR_MOVIES), color, 0, true); // UTF-8
}

bool CNetzKinoBrowser::onButtonPress(neutrino_msg_t msg)
{
	dprintf(DEBUG_INFO, "CNetzKinoBrowser::onButtonPress %d\n", msg);
	
	bool result = false;
	
	result = onButtonPressMainFrame(msg);

	if(result == false)
	{
		// if Main Frame didnot process the button, the focused window may do
		switch(m_windowFocus)
		{
			case NKB_FOCUS_BROWSER:
			 	result = onButtonPressBrowserList(msg);		
				break;
				
			case NKB_FOCUS_MOVIE_INFO:
			 	result = onButtonPressMovieInfoList(msg);		
				break;
				
			default:
				break;
		}
	}
	
	return (result);
}

bool CNetzKinoBrowser::onButtonPressMainFrame(neutrino_msg_t msg)
{
	dprintf(DEBUG_INFO, "CNetzKinoBrowser::onButtonPressMainFrame: %d\n", msg);
	
	bool result = true;

	if (msg == CRCInput::RC_home)
	{
		result = false;
	}
	else if (msg == CRCInput::RC_red ) 
	{	
		NKStart -= m_pcBrowser->getLinesPerPage();
			
		if(NKStart >= 0)
		{
			NKEnd -= m_pcBrowser->getLinesPerPage();
			printf("[2], NKStart:%d NKEnd:%d\n", NKStart, NKEnd);
				
			m_pcWindow->paintBackground();
					
			//
			CHintBox loadBox(LOCALE_NETZKINO, g_Locale->getText(LOCALE_MOVIEBROWSER_SCAN_FOR_MOVIES));
			loadBox.paint();
				
			nkparser.Cleanup();
			loadNKTitles(m_settings.nkmode, m_settings.nksearch, m_settings.nkcategory, NKStart, NKEnd);
				
			loadBox.hide();
					
			refreshBrowserList();
			refresh();
		}
		else
			NKStart += m_pcBrowser->getLinesPerPage();
			
		printf("[3] NKStart:%d NKEnd:%d\n", NKStart, NKEnd);
	}
	else if (msg == CRCInput::RC_green) 
	{
		NKEnd += m_pcBrowser->getLinesPerPage();
			
		if(NKEnd <= videoListsize)
		{
			NKStart += m_pcBrowser->getLinesPerPage();
				
			m_pcWindow->paintBackground();
					
			//
			CHintBox loadBox(LOCALE_NETZKINO, g_Locale->getText(LOCALE_MOVIEBROWSER_SCAN_FOR_MOVIES));
			loadBox.paint();
				
			nkparser.Cleanup();
			loadNKTitles(m_settings.nkmode, m_settings.nksearch, m_settings.nkcategory, NKStart, NKEnd);
				
			loadBox.hide();
					
			refreshBrowserList();
			refresh();
		}
		else
			NKEnd -= m_pcBrowser->getLinesPerPage();
			
		printf("getLinesPerPages:%d, NKStart:%d NKEnd:%d\n", m_pcBrowser->getLinesPerPage(), NKStart, NKEnd);			
	}
	else if (msg == CRCInput::RC_yellow) 
	{
		onSetFocusNext();
	}
	else if (msg == CRCInput::RC_blue) 
	{
		nkparser.Cleanup();	
		loadMovies();
		refresh();
	}
	else if ( msg == CRCInput::RC_info) 
	{
		if(m_movieSelectionHandler != NULL)
		{
			m_pcWindow->paintBackground();
			m_pcWindow->blit();
	  
			m_movieInfo.showMovieInfo(*m_movieSelectionHandler);
			
			refresh();
		}
	}
	else if (msg == CRCInput::RC_setup) 
	{
		showNKMenu();
	}
	else if (msg == CRCInput::RC_spkr)
	{
		nkparser.downloadMovie(m_movieSelectionHandler->file.Name, m_movieSelectionHandler->file.Url);
	}
	else
	{
		dprintf(DEBUG_INFO, "CNetzKinoBrowser::onButtonPressMainFrame: none\n");
		
		result = false;
	}

	return (result);
}

bool CNetzKinoBrowser::onButtonPressBrowserList(neutrino_msg_t msg) 
{
	dprintf(DEBUG_INFO, "CNetzKinoBrowser::onButtonPressBrowserList %d\n", msg);
	
	bool result = true;
	
	if(msg == CRCInput::RC_up)
	{
		m_pcBrowser->scrollLineUp(1);
	}
	else if (msg == CRCInput::RC_down)
	{
		m_pcBrowser->scrollLineDown(1);
	}
	else if (msg == CRCInput::RC_page_up)
	{
		m_pcBrowser->scrollPageUp(1);
	}
	else if (msg == CRCInput::RC_page_down)
	{
		m_pcBrowser->scrollPageDown(1);
	}
	else if (msg == CRCInput::RC_left)
	{
		m_pcBrowser->scrollPageUp(1);
	}
	else if (msg == CRCInput::RC_right)
	{
		m_pcBrowser->scrollPageDown(1);
	}
	else
	{
		// default
		result = false;
	}
	
	if(result == true)
		updateMovieSelection();

	return (result);
}

bool CNetzKinoBrowser::onButtonPressMovieInfoList(neutrino_msg_t msg) 
{
	dprintf(DEBUG_INFO, "CNetzKinoBrowser::onButtonPressMovieInfoList: %d\n", msg);
	
	bool result = true;
	
	if(msg == CRCInput::RC_up)
	{
		m_pcInfo->scrollPageUp(1);
	}
	else if (msg == CRCInput::RC_down)
	{
		m_pcInfo->scrollPageDown(1);
	}
	else
	{
		// default
		result = false;
	}	

	return (result);
}

void CNetzKinoBrowser::onSetGUIWindow(NKB_GUI gui)
{
	m_settings.gui = gui;
	
	if(gui == NKB_GUI_MOVIE_INFO)
	{
		dprintf(DEBUG_NORMAL, "CNetzKinoBrowser::onSetGUIWindow:\n");
		
		// Paint these frames ...
		m_showMovieInfo = true;
		m_showBrowserFiles = true;
		
		m_pcBrowser->paint();
		onSetFocus(NKB_FOCUS_BROWSER);
		m_pcInfo->paint();
		refreshMovieInfo();
	}
}

void CNetzKinoBrowser::onSetFocus(NKB_FOCUS new_focus)
{
	dprintf(DEBUG_INFO, "CNetzKinoBrowser::onSetFocus: %d\n", new_focus);
	
	m_windowFocus = new_focus;
	
	if(m_windowFocus == NKB_FOCUS_BROWSER)
	{
		m_pcBrowser->showSelection(true);
	}
	else if(m_windowFocus == NKB_FOCUS_MOVIE_INFO)
	{
		m_pcBrowser->showSelection(false);
	}
	
	updateMovieSelection();
	refreshFoot();	
}

void CNetzKinoBrowser::onSetFocusNext(void) 
{
	dprintf(DEBUG_INFO, "CNetzKinoBrowser::onSetFocusNext:\n");
	
	if(m_settings.gui == NKB_GUI_MOVIE_INFO)
	{
		if(m_windowFocus == NKB_FOCUS_BROWSER)
		{
			dprintf(DEBUG_NORMAL, "CNetzKinoBrowser::onSetFocusNext: NKB_FOCUS_MOVIE_INFO\n");
			
			onSetFocus(NKB_FOCUS_MOVIE_INFO);
			m_windowFocus = NKB_FOCUS_MOVIE_INFO;
		}
		else
		{
			dprintf(DEBUG_NORMAL, "CNetzKinoBrowser::onSetFocusNext: NKB_FOCUS_BROWSER\n");
			onSetFocus(NKB_FOCUS_BROWSER);
		}
	}
}

void CNetzKinoBrowser::updateMovieSelection(void)
{
	dprintf(DEBUG_INFO, "CNetzKinoBrowser::updateMovieSelection: %d\n", m_windowFocus);
	
	if (m_vMovieInfo.size() == 0) 
		return;
	
	bool new_selection = false;
	 
	unsigned int old_movie_selection;
	
	if(m_windowFocus == NKB_FOCUS_BROWSER)
	{
		if(m_vHandleBrowserList.size() == 0)
		{
			// There are no elements in the Filebrowser, clear all handles
			m_currentBrowserSelection = 0;
			m_movieSelectionHandler = NULL;
			new_selection = true;
		}
		else
		{
			old_movie_selection = m_currentBrowserSelection;
			m_currentBrowserSelection = m_pcBrowser->getSelectedLine();
			
			if(m_currentBrowserSelection != old_movie_selection)
				new_selection = true;
			
			if(m_currentBrowserSelection < m_vHandleBrowserList.size())
				m_movieSelectionHandler = m_vHandleBrowserList[m_currentBrowserSelection];
		}
	}
	
	if(new_selection == true)
	{
		refreshMovieInfo();
		refreshLCD();
	}
}

void CNetzKinoBrowser::loadMovies(void)
{
	dprintf(DEBUG_NORMAL, "CNetzKinoBrowser::loadMovies:\n");
	
	//first clear screen */
	m_pcWindow->paintBackground();
	m_pcWindow->blit();	

	CHintBox loadBox(LOCALE_NETZKINO, g_Locale->getText(LOCALE_MOVIEBROWSER_SCAN_FOR_MOVIES));
	
	loadBox.paint();

	loadNKTitles(m_settings.nkmode, m_settings.nksearch, m_settings.nkcategory, 0, m_pcBrowser->getLinesPerPage());
	
	loadBox.hide();

	refreshBrowserList();	
	refreshMovieInfo();	// is done by refreshBrowserList if needed
	
	m_reload_movies = false;
}

bool CNetzKinoBrowser::getMovieInfoItem(MI_MOVIE_INFO& movie_info, NKB_INFO_ITEM item, std::string* item_string)
{
	#define MAX_STR_TMP 100
	char str_tmp[MAX_STR_TMP];
	bool result = true;
	*item_string = "";

	switch(item)
	{
		case NKB_INFO_FILENAME: 
			*item_string = movie_info.file.getFileName();
			break;
			
		case NKB_INFO_TITLE:
			*item_string = movie_info.epgTitle;
			if(strcmp("not available", movie_info.epgTitle.c_str()) == 0)
				result = false;
			if(movie_info.epgTitle.empty())
				result = false;
			break;
			
		case NKB_INFO_INFO1:
			*item_string = movie_info.epgInfo1;
			break;
			
		case NKB_INFO_RECORDDATE:
			// YYYY-MM-DD hh:mm:ss
			int day, month, year;
			if (3 == sscanf(movie_info.ytdate.c_str(), "%d-%d-%d", &day, &month, &year)) 
			{
				snprintf(str_tmp,MAX_STR_TMP,"%02d.%02d.%02d", day, month, year);
				*item_string = str_tmp;
			}		
			break;
			
		case NKB_INFO_MAX_NUMBER:
		default:
			*item_string = "";
			result = false;
			break;
	}
	
	return(result);
}

//netzkino
void CNetzKinoBrowser::loadNKTitles(int mode, std::string search, int id, unsigned int start, unsigned int end)
{
	//
	if (nkparser.ParseFeed((cNKFeedParser::nk_feed_mode_t)mode, search, id)) 
	{
		nkparser.DownloadThumbnails(start, end);
	} 
	else 
	{
		//FIXME show error
		MessageBox(LOCALE_MESSAGEBOX_ERROR, g_Locale->getText(LOCALE_NK_ERROR), CMessageBox::mbrCancel, CMessageBox::mbCancel, NEUTRINO_ICON_ERROR);
		m_reload_movies = true;
		return;
	}
	
	m_vMovieInfo.clear();
	nk_video_list_t &ylist = nkparser.GetVideoList();
	
	//
	videoListsize = ylist.size();
	
	for (unsigned int i = start; i < end && end <= ylist.size(); i++) 
	{
		MI_MOVIE_INFO movieInfo;
		m_movieInfo.clearMovieInfo(&movieInfo); // refresh structure
		
		movieInfo.epgTitle = ylist[i].title;
		movieInfo.epgInfo1 = m_settings.nkcategoryname;
		movieInfo.epgInfo2 = ylist[i].description;
		movieInfo.tfile = ylist[i].tfile;
		movieInfo.ytdate = ylist[i].published;
		movieInfo.ytid = ylist[i].id;
		movieInfo.file.Name = ylist[i].title;
		movieInfo.file.Url = ylist[i].url;
		
		m_vMovieInfo.push_back(movieInfo);
	}
	
	m_currentBrowserSelection = 0;
	m_pcBrowser->setSelectedLine(m_currentBrowserSelection);
}

class CNKCategoriesMenu : public CMenuTarget
{
	private:
		int *nkmode;
		int *nkcategory;
		std::string *nkcategoryname;
		cNKFeedParser *nkparser;
	public:
		CNKCategoriesMenu(int &_nkmode, int &_nkcategory, std::string &_nkcategoryname, cNKFeedParser &_nkparser);
		int exec(CMenuTarget* parent, const std::string & actionKey);
};

CNKCategoriesMenu::CNKCategoriesMenu(int &_nkmode, int &_nkcategory, std::string &_nkcategoryname, cNKFeedParser &_nkparser)
{
	nkmode = &_nkmode;
	nkcategory = &_nkcategory;
	nkcategoryname = &_nkcategoryname;
	nkparser = &_nkparser;
}

int CNKCategoriesMenu::exec(CMenuTarget *parent, const std::string &actionKey)
{
	nk_category_list_t cats = nkparser->GetCategoryList();
	
	if (!cats.size())
		return menu_return::RETURN_NONE;
	
	if (actionKey != "") 
	{
		unsigned int i = atoi(actionKey);
		if (i < cats.size()) 
		{
			*nkmode = cNKFeedParser::CATEGORY;
			*nkcategory = cats[i].id;
			*nkcategoryname = cats[i].title;
		}
		g_RCInput->postMsg(CRCInput::RC_home, 0);
		return menu_return::RETURN_EXIT;
	}

	if(parent)
		parent->hide();

	CMenuWidget m(LOCALE_NK_CATEGORIES, NEUTRINO_ICON_NETZKINO_SMALL);

	for (unsigned i = 0; i < cats.size(); i++)
	{
		m.addItem(new CMenuForwarder(cats[i].title.c_str(), true, /*("(" + to_string(cats[i].post_count) + ")").c_str()*/NULL, this, to_string(i).c_str(), CRCInput::convertDigitToKey(i + 1)), cats[i].id == *nkcategory);
	}

	m.exec(NULL, "");

	return menu_return::RETURN_REPAINT;
}
  
bool CNetzKinoBrowser::showNKMenu()
{
	m_pcWindow->paintBackground();

	CMenuWidget mainMenu(LOCALE_NETZKINO, NEUTRINO_ICON_NETZKINO_SMALL);

	int select = -1;
	CMenuSelectorTarget * selector = new CMenuSelectorTarget(&select);

	CNKCategoriesMenu nkCategoriesMenu(m_settings.nkmode, m_settings.nkcategory, m_settings.nkcategoryname, nkparser);
	
	mainMenu.addItem(new CMenuForwarder(LOCALE_NK_CATEGORIES, true, m_settings.nkcategoryname, &nkCategoriesMenu));
	mainMenu.addItem(new CMenuSeparator(CMenuSeparator::LINE));

	std::string search = m_settings.nksearch;
	CStringInputSMS stringInput(LOCALE_YT_SEARCH, &search);
	mainMenu.addItem(new CMenuForwarder(LOCALE_YT_SEARCH, true, search, &stringInput, NULL, CRCInput::RC_nokey, ""));

	mainMenu.addItem(new CMenuForwarder(LOCALE_EVENTFINDER_START_SEARCH, true, NULL, selector, to_string(cNKFeedParser::SEARCH).c_str(), CRCInput::RC_red, NEUTRINO_ICON_BUTTON_RED));

	int oldcat = m_settings.nkcategory;
	int oldmode = m_settings.nkmode;

	mainMenu.exec(NULL, "");

	delete selector;

	bool reload = false;
	
	dprintf(DEBUG_NORMAL, "select:%d\n", select);
	
	if (select == cNKFeedParser::SEARCH) 
	{
		dprintf(DEBUG_NORMAL, "search for: %s\n", search.c_str());
		
		if (!search.empty()) 
		{
			reload = true;
			m_settings.nksearch = search;
			m_settings.nkmode = cNKFeedParser::SEARCH;
		}
	}
	else if (oldmode != m_settings.nkmode || oldcat != m_settings.nkcategory) 
	{
		reload = true;
	}
	
	if (reload) 
	{
		CHintBox loadBox(LOCALE_NETZKINO, g_Locale->getText(LOCALE_MOVIEBROWSER_SCAN_FOR_MOVIES));
		loadBox.paint();
		nkparser.Cleanup();
		loadNKTitles(m_settings.nkmode, m_settings.nksearch, m_settings.nkcategory, 0, m_pcBrowser->getLinesPerPage());
		loadBox.hide();
	}
	
	refreshBrowserList();
	refresh();
	
	return true;
}




// plugin API
void plugin_init(void)
{
}

void plugin_del(void)
{
}

void plugin_exec(void)
{
	CNetzKinoBrowser * moviebrowser;
	MI_MOVIE_INFO * p_movie_info;
	
	moviebrowser = new CNetzKinoBrowser();
	
BROWSER:
	if (moviebrowser->exec()) 
	{
		// get the current file name
		CFile * file;

		if ((file = moviebrowser->getSelectedFile()) != NULL) 
		{
			moviePlayerGui->filename = file->Url.c_str();
			
			// movieinfos
			p_movie_info = moviebrowser->getCurrentMovieInfo();
			
			moviePlayerGui->Title = p_movie_info->epgTitle;
			moviePlayerGui->Info1 = p_movie_info->epgInfo1;
			moviePlayerGui->Info2 = p_movie_info->epgInfo2;
			moviePlayerGui->thumbnail = p_movie_info->tfile;
			
			// play
			moviePlayerGui->exec(NULL, "urlplayback");
		}
		
		neutrino_msg_t msg;
		neutrino_msg_data_t data;

		g_RCInput->getMsg_ms(&msg, &data, 0); // 1 sec
		
		if (msg != CRCInput::RC_home) 
		{
			goto BROWSER;
		}
	}
						
	delete moviebrowser;				
}


