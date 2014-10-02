/*
  neutrinoHD2 project
  
  https://code.google.com/p/neutrinohd2/
  
  $Id: mediaportal.cpp 2014/03/09 mohousch Exp $

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

#include <plugin.h>


extern "C" void plugin_exec(void);

//
#define MIN_MPBROWSER_FRAME_HEIGHT 	100
#define MAX_MPBROWSER_FRAME_HEIGHT 	400
#define MPB_MAX_ROWS 			3

typedef enum
{
	MPB_INFO_FILENAME 		= 0,
	MPB_INFO_TITLE 			= 1,
	MPB_INFO_INFO1 			= 2,
	MPB_INFO_RECORDDATE 		= 3,
	MPB_INFO_MAX_NUMBER		= 4
}MPB_INFO_ITEM;

typedef enum
{
	MPB_FOCUS_BROWSER = 0,
	MPB_FOCUS_MOVIE_INFO = 1,
	MPB_FOCUS_MAX_NUMBER = 2
}MPB_FOCUS;

typedef enum
{
	MPB_GUI_BROWSER_ONLY = 0,
	MPB_GUI_MOVIE_INFO = 1,
	MPB_GUI_MAX_NUMBER = 2
}MPB_GUI;

// settings
typedef struct
{
	MPB_GUI gui;
	
	// these variables are used for the listframes
	int browserFrameHeight;
	int browserRowNr;
	MPB_INFO_ITEM browserRowItem[MPB_MAX_ROWS];
	int browserRowWidth[MPB_MAX_ROWS];
}MPB_SETTINGS;

//
class CMPBrowser : public CMenuTarget
{
	private:
		CFrameBuffer * mp_Window;
		
		// mp icon
		int icon_head_w;
		int icon_head_h;
		
		//
		CListFrame * mp_Browser;
		CTextBox * mp_Info;
		
		CBox mp_BoxFrame;
		CBox mp_BoxFrameBrowserList;
		CBox mp_BoxFrameFootRel;
		CBox mp_BoxFrameTitleRel;
		CBox mp_BoxFrameInfo;
		
		LF_LINES mp_browserListLines;
		
		CFileList mp_ItemInfo;
		CFileList mp_vHandleBrowserList;
		
		unsigned int mp_currentBrowserSelection;
 		unsigned int mp_prevBrowserSelection;
		
		bool mp_showBrowserFiles;
		bool mp_showItemInfo;
		
		CFile * mp_itemSelectionHandler;
		
		MPB_FOCUS mp_windowFocus;
		
		static CFont * mp_FontFoot;
		static CFont * mp_FontTitle;
		
		MPB_SETTINGS mp_settings;
		
		CFile filelist;
		
        public:
		CMPBrowser();
		~CMPBrowser();
		
		int exec();
		int exec(CMenuTarget* parent, const std::string & actionKey);
		
		CFile * getSelectedFile(void); 
		
	private:
		// browser init
		void init(void); 
		void initGlobalSettings(void); 
		void initFrames(void);
		
		// browser main window
		int paint(void); 
		void refresh(void);
        	void hide(void); 
		void refreshBrowserList(void);
		void refreshItemInfo(void);
		//void refreshFoot(void);
		void refreshTitle(void);
		void refreshInfo(void);
		void refreshLCD(void);
		
		// event
		bool onButtonPress(neutrino_msg_t msg); 
		bool onButtonPressMainFrame(neutrino_msg_t msg);
		bool onButtonPressBrowserList(neutrino_msg_t msg);
		bool onButtonPressItemInfoList(neutrino_msg_t msg);
		
		void onSetFocus(MPB_FOCUS new_focus);
		void onSetFocusNext(void);
		
		void onSetGUIWindow(MPB_GUI gui);
		
		void loadItems(void);
		
		// misc
		void updateItemSelection(void);
		bool getItemInfoItem(CFileList& item_info, MPB_INFO_ITEM item, std::string* item_string);
};

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
		
	//m_reload_movies = true;

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

void CMPBrowser::initGlobalSettings(void)
{
	dprintf(DEBUG_NORMAL, "CMPBrowser::initGlobalSettings\n");
	
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
	mp_BoxFrameTitleRel.iHeight = 		/*mp_FontTitle->getHeight()*/g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getHeight()*2 + 20 + g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL]->getHeight() + 5;

	mp_BoxFrameBrowserList.iX = 		mp_BoxFrame.iX;
	mp_BoxFrameBrowserList.iY = 		mp_BoxFrame.iY + mp_BoxFrameTitleRel.iHeight + INTER_FRAME_SPACE;
	mp_BoxFrameBrowserList.iWidth = 	mp_BoxFrame.iWidth;
	mp_BoxFrameBrowserList.iHeight = 	mp_settings.browserFrameHeight;

	mp_BoxFrameFootRel.iX = 		0;
	mp_BoxFrameFootRel.iY = 		mp_BoxFrame.iHeight - mp_FontFoot->getHeight();
	mp_BoxFrameFootRel.iWidth = 		mp_BoxFrameBrowserList.iWidth;
	mp_BoxFrameFootRel.iHeight = 		mp_FontFoot->getHeight();
	
	mp_BoxFrameInfo.iX = 			mp_BoxFrameBrowserList.iX;
	mp_BoxFrameInfo.iY = 			mp_BoxFrameBrowserList.iY + mp_BoxFrameBrowserList.iHeight + INTER_FRAME_SPACE;
	mp_BoxFrameInfo.iWidth = 		mp_BoxFrameBrowserList.iWidth;
	mp_BoxFrameInfo.iHeight = 		mp_BoxFrame.iHeight - mp_BoxFrameBrowserList.iHeight - INTER_FRAME_SPACE - mp_BoxFrameFootRel.iHeight - mp_BoxFrameTitleRel.iHeight;
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
	//m_vHandleBrowserList.clear();
	
	//m_movieSelectionHandler = NULL;

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
	/*
	if(m_reload_movies == true)
	{
		dprintf(DEBUG_NORMAL, "CYTBrowser::exec\n");
		loadMovies();
	}
	else
	  */
	{
		// since we cleared everything above, we have to refresh the list now.
		refreshBrowserList();	
	}

	// get old movie selection and set position in windows	
	//mp_currentBrowserSelection = m_prevBrowserSelection;

	//mp_Browser->setSelectedLine(mp_currentBrowserSelection);

	// update movie selection
	//updateMovieSelection();

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
				//if(m_movieSelectionHandler != NULL)
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
	//m_prevBrowserSelection = m_currentBrowserSelection;
	
	return (res);
}

void CMPBrowser::hide(void)
{
	dprintf(DEBUG_NORMAL, "CYTBrowser::hide\n");
	
	mp_Window->paintBackground();
	mp_Window->blit();
	
	if (mp_Browser != NULL)
	{
		//m_currentBrowserSelection = m_pcBrowser->getSelectedLine();
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
		
	//refreshFoot();
	refreshLCD();
}

CFile * CMPBrowser::getSelectedFile(void)
{
	//dprintf(DEBUG_INFO, "CMPBrowser::getSelectedFile: %s\n", mp_itemSelectionHandler->file.Name.c_str());

	/*
	if(mp_itemSelectionHandler != NULL)
		return(&mp_itemSelectionHandler->file);
	else
	  */
		return(NULL);
}

void CMPBrowser::refreshItemInfo(void)
{
	//dprintf(DEBUG_INFO, "CMPBrowser::refreshMovieInfo: m_vMovieInfo.size %d\n", m_vMovieInfo.size());
	
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
	//m_vHandleBrowserList.clear();
	
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
		//mp_vHandleBrowserList.push_back(item_handle);
	}

	for(unsigned int handle = 0; handle < mp_vHandleBrowserList.size(); handle++)
	{
		for(int row = 0; row < mp_settings.browserRowNr ;row++)
		{
			/*
			if ( getItemInfoItem(*mp_vHandleBrowserList[handle], mp_settings.browserRowItem[row], &string_item) == false)
			{
				string_item = "n/a";
				if(mp_settings.browserRowItem[row] == MPB_INFO_TITLE)
					getItemInfoItem(*mp_vHandleBrowserList[handle], MPB_INFO_FILENAME, &string_item);
			}
			mp_browserListLines.lineArray[row].push_back(string_item);
			*/
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
	
	//mp_frameBuffer->paintBoxRel(mp_x, mp_y, mp_width, mp_title_height - 10, COL_MENUCONTENT_PLUS_6 );
	mp_Window->paintBoxRel(mp_BoxFrame.iX + mp_BoxFrameTitleRel.iX, mp_BoxFrame.iY + mp_BoxFrameTitleRel.iY, mp_BoxFrameTitleRel.iWidth, mp_BoxFrameTitleRel.iHeight, COL_MENUCONTENT_PLUS_6 );
	//mp_frameBuffer->paintBoxRel(mp_x + 2, mp_y + 2 , mp_width - 4, mp_title_height - 14, COL_MENUCONTENTSELECTED_PLUS_0);
	mp_Window->paintBoxRel(mp_BoxFrame.iX + mp_BoxFrameTitleRel.iX + 2, mp_BoxFrame.iY + mp_BoxFrameTitleRel.iY + 2 , mp_BoxFrameTitleRel.iWidth - 4, mp_BoxFrameTitleRel.iHeight - 14, COL_MENUCONTENTSELECTED_PLUS_0);
	
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
	mp_Window->paintIcon(icon_head.c_str(), mp_BoxFrame.iX + mp_BoxFrameTitleRel.iX + 10, mp_BoxFrame.iY + mp_BoxFrameTitleRel.iY + (mp_BoxFrameTitleRel.iHeight - 10 - icon_head_h)/2);
	
	
	
	
	
	
	/*
	
	// title
	std::string title;
	std::string mb_icon;
	
	title = g_Locale->getText(LOCALE_MOVIEPLAYER_YTPLAYBACK);
	title += " : ";
		
	neutrino_locale_t loc = getFeedLocale();
	title += g_Locale->getText(loc);
	if (loc == LOCALE_MOVIEBROWSER_YT_RELATED || loc == LOCALE_MOVIEBROWSER_YT_SEARCH)
		title += " \"" + m_settings.ytsearch + "\"";
		
	mb_icon = NEUTRINO_ICON_YT_SMALL;
	//

	// head box
	m_pcWindow->paintBoxRel(m_cBoxFrame.iX + m_cBoxFrameTitleRel.iX, m_cBoxFrame.iY + m_cBoxFrameTitleRel.iY, m_cBoxFrameTitleRel.iWidth, m_cBoxFrameTitleRel.iHeight, TITLE_BACKGROUND_COLOR, RADIUS_MID, CORNER_TOP, CFrameBuffer::PAINT_SHADING, 2);
	
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
	*/
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
		//ytparser.Cleanup();
		//loadMovies();
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
	//refreshFoot();	
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
			
			//if(mp_currentBrowserSelection < mp_vHandleBrowserList.size())
			//	mp_itemSelectionHandler = mp_vHandleBrowserList[mp_currentBrowserSelection];
		}
	}
	
	if(new_selection == true)
	{
		refreshItemInfo();
		refreshLCD();
	}
}

void CMPBrowser::loadItems(void)
{
	dprintf(DEBUG_NORMAL, "CMPBrowser::loadItems:\n");
	
	//first clear screen
	mp_Window->paintBackground();
	mp_Window->blit();	

	refreshBrowserList();	
	refreshItemInfo();	// is done by refreshBrowserList if needed
	
	//m_reload_movies = false;
}

bool CMPBrowser::getItemInfoItem(CFileList& item_info, MPB_INFO_ITEM item, std::string* item_string)
{
	#define MAX_STR_TMP 100
	bool result = true;
	*item_string = "";

	switch(item)
	{
		case MPB_INFO_FILENAME:
			//*item_string = item_info.file.getFileName();
			break;
			
		case MPB_INFO_TITLE:
			//*item_string = item_info.Title;
			
			//if(item_info.Title.empty())
			//	result = false;
			break;
			
		case MPB_INFO_INFO1:
			//*item_string = item_info.Info1;
			break;
			
		case MPB_INFO_RECORDDATE:
			//*item_string = item_info.date;	
			break;
			
		case MPB_INFO_MAX_NUMBER:
		default:
			//*item_string = "";
			result = false;
			break;
	}
	
	return(result);
}

/*
void CMediaPortal::ORF(void)
{
	static int old_select = 0;
	char cnt[5];
	
	CMenuWidget InputSelector(LOCALE_WEBTV_HEAD, NEUTRINO_ICON_WEBTV_SMALL);
	int count = 0;
	int select = -1;
					
	CMenuSelectorTarget *ORFInputChanger = new CMenuSelectorTarget(&select);
			
	// orf1
	sprintf(cnt, "%d", count);
	InputSelector.addItem(new CMenuForwarderNonLocalized("ORF 1", true, NULL, ORFInputChanger, cnt, CRCInput::convertDigitToKey(count + 1)), old_select == count);
	
	// orf 2
	sprintf(cnt, "%d", ++count);
	InputSelector.addItem(new CMenuForwarderNonLocalized("ORF 2", true, NULL, ORFInputChanger, cnt, CRCInput::convertDigitToKey(count + 1)), old_select == count);

	// orf 3
	sprintf(cnt, "%d", ++count);
	InputSelector.addItem(new CMenuForwarderNonLocalized("ORF 3", true, NULL, ORFInputChanger, cnt, CRCInput::convertDigitToKey(count + 1)), old_select == count);
	
	// orf sport
	sprintf(cnt, "%d", ++count);
	InputSelector.addItem(new CMenuForwarderNonLocalized("ORF Sport", true, NULL, ORFInputChanger, cnt, CRCInput::convertDigitToKey(count + 1)), old_select == count);
	
	hide();
	InputSelector.exec(NULL, "");
	delete ORFInputChanger;
					
	if(select >= 0)
	{
		old_select = select;
					
		switch (select) 
		{
			case 0:
				moviePlayerGui->filename = "rtsp://apasfwl.apa.at:80/orf1_q6a/orf.sdp";
				moviePlayerGui->Title = "ORF 1";
				moviePlayerGui->Info1 = "Stream";
				
				moviePlayerGui->exec(NULL, "urlplayback");
				break;
						
			case 1:
				moviePlayerGui->filename = "rtsp://apasfwl.apa.at:80/orf2_q6a/orf.sdp";
				moviePlayerGui->Title = "ORF 2";
				moviePlayerGui->Info1 = "Stream";
				moviePlayerGui->exec(NULL, "urlplayback");
				break;
				
			case 2:
				moviePlayerGui->filename = "rtsp://apasfwl.apa.at:80/orf3_q6a/orf.sdp";	
				moviePlayerGui->Title = "ORF 3";
				moviePlayerGui->Info1 = "Stream";
				moviePlayerGui->exec(NULL, "urlplayback");
				break;
			case 3:
				moviePlayerGui->filename = "rtsp://apasfwl.apa.at/orfs_q6a/orf.sdp";
				moviePlayerGui->Title = "ORF Sport";
				moviePlayerGui->Info1 = "Stream";
				moviePlayerGui->exec(NULL, "urlplayback");
				break;
						
			default: break;
		}
	}
}
*/

#if 0
int CMPBrowser::exec(CMenuTarget *parent, const std::string &actionKey)
{
	int ret = menu_return::RETURN_REPAINT;
	
	if(parent)
		parent->hide();
	
	/*
	if(actionKey == "youtube") 
	{
		//moviePlayerGui->exec(NULL, "ytplayback");
	
		CMovieBrowser moviebrowser;
		std::string Path_local = "/";
		MI_MOVIE_INFO * p_movie_info;
		
		moviebrowser.setMode(MB_SHOW_YT);
		
YT_BROWSER:	
		if (moviebrowser.exec(Path_local.c_str())) 
		{
			// get the current path and file name
			Path_local = moviebrowser.getCurrentDir();
			CFile * file;

			if ((file = moviebrowser.getSelectedFile()) != NULL) 
			{
				moviePlayerGui->filename = file->Url.c_str();
				
				// movieinfos
				p_movie_info = moviebrowser.getCurrentMovieInfo();
				
				moviePlayerGui->Title = p_movie_info->epgTitle;
				moviePlayerGui->Info1 = p_movie_info->epgInfo1;
				moviePlayerGui->Info2 = p_movie_info->epgInfo2;
				
				// play
				moviePlayerGui->exec(NULL, "urlplayback");
			}
			
			neutrino_msg_t msg;
			neutrino_msg_data_t data;

			g_RCInput->getMsg_ms(&msg, &data, 40);
			
			if (msg != CRCInput::RC_home) 
			{
				goto YT_BROWSER;
			}
		}
							
		return ret;	
	}
	else if(actionKey == "netzkino") 
	{
		//moviePlayerGui->exec(NULL, "netzkinoplayback");

		CNetzKinoBrowser nkBrowser;
		MI_MOVIE_INFO * p_movie_info;
		//std::string Path_local = "/";
		
NK_BROWSER:
		if (nkBrowser.exec()) 
		{
			// get the current file name
			CFile * file;

			if ((file = nkBrowser.getSelectedFile()) != NULL) 
			{
				moviePlayerGui->filename = file->Url.c_str();
				
				// movieinfos
				p_movie_info = nkBrowser.getCurrentMovieInfo();
				
				moviePlayerGui->Title = p_movie_info->epgTitle;
				moviePlayerGui->Info1 = p_movie_info->epgInfo1;
				moviePlayerGui->Info2 = p_movie_info->epgInfo2;
				
				// play
				moviePlayerGui->exec(NULL, "urlplayback");
			}
			
			neutrino_msg_t msg;
			neutrino_msg_data_t data;

			g_RCInput->getMsg_ms(&msg, &data, 40);
			
			if (msg != CRCInput::RC_home) 
			{
				goto NK_BROWSER;
			}
		}
		
		return ret;
	}
	else if(actionKey == "orf")
	{
		ORF();
	}
	*/
	
	#if 0
	if(actionKey == "musicdeluxe")
	{
		moviePlayerGui->filename = "rtmp://flash.cdn.deluxemusic.tv/deluxemusic.tv-live/web_850.stream";
		moviePlayerGui->Title = "Music Deluxe";
		moviePlayerGui->Info1 = "Stream";
		moviePlayerGui->exec(NULL, "urlplayback");
	}
	else if(actionKey == "orf1")
	{
		moviePlayerGui->filename = "rtsp://apasfwl.apa.at:80/orf1_q6a/orf.sdp";
		moviePlayerGui->Title = "ORF 1";
		moviePlayerGui->Info1 = "Stream";
		moviePlayerGui->exec(NULL, "urlplayback");
	}
	else if(actionKey == "orf2")
	{
		moviePlayerGui->filename = "rtsp://apasfwl.apa.at:80/orf2_q6a/orf.sdp";	
		moviePlayerGui->Title = "ORF 2";
		moviePlayerGui->Info1 = "Stream";
		moviePlayerGui->exec(NULL, "urlplayback");
	}
	else if(actionKey == "orf3")
	{
		moviePlayerGui->filename = "rtsp://apasfwl.apa.at:80/orf3_q6a/orf.sdp";	
		moviePlayerGui->Title = "ORF 3";
		moviePlayerGui->Info1 = "Stream";
		moviePlayerGui->exec(NULL, "urlplayback");
	}
	else if(actionKey == "orfsport")
	{
		moviePlayerGui->filename = "rtsp://apasfwl.apa.at/orfs_q6a/orf.sdp";
		moviePlayerGui->Title = "ORF Sport";
		moviePlayerGui->Info1 = "Stream";
		moviePlayerGui->exec(NULL, "urlplayback");
	}
	else if(actionKey == "filmonasia")
	{
		moviePlayerGui->filename = "rtmp://live190.la3.origin.filmon.com:8086/live/198.high.stream";
		moviePlayerGui->Title = "Filmon asia";
		moviePlayerGui->Info1 = "Stream";
		moviePlayerGui->exec(NULL, "urlplayback");
	}
	else if(actionKey == "filmonblack")
	{
		moviePlayerGui->filename = "rtmp://live190.la3.origin.filmon.com:8086/live/244.high.stream";
		moviePlayerGui->Title = "Filmon black";
		moviePlayerGui->Info1 = "Stream";
		moviePlayerGui->exec(NULL, "urlplayback");
	}
	else if(actionKey == "filmon1")
	{
		moviePlayerGui->filename = "rtmp://live190.la3.origin.filmon.com:8086/live/247.high.stream";
		moviePlayerGui->Title = "Filmon 1";
		moviePlayerGui->Info1 = "Stream";
		moviePlayerGui->exec(NULL, "urlplayback");
	}
	else if(actionKey == "filmon2")
	{
		moviePlayerGui->filename = "rtmp://live190.la3.origin.filmon.com:8086/live/245.high.stream";
		moviePlayerGui->Title = "Filmon 2";
		moviePlayerGui->Info1 = "Stream";
		moviePlayerGui->exec(NULL, "urlplayback");
	}
	else if(actionKey == "filmon3")
	{
		moviePlayerGui->filename = "rtmp://live190.la3.origin.filmon.com:8086/live/246.high.stream";
		moviePlayerGui->Title = "Filmon 3";
		moviePlayerGui->Info1 = "Stream";
		moviePlayerGui->exec(NULL, "urlplayback");
	}
	#endif
	
	paintHead();
	
	mp_frameBuffer->blit();
	
	neutrino_msg_t msg;
	neutrino_msg_data_t data;
	
	bool loop = true;
		
	while (loop)
	{
		g_RCInput->getMsg_ms(&msg, &data, 100);
		
		if(msg == CRCInput::RC_up)
		{
			loop = false;
		}
		else if (msg == CRCInput::RC_down)
		{
			loop = false;
		}
		else if (msg == CRCInput::RC_home)
		{
			loop = false;
		}
		if(msg == CRCInput::RC_1)
		{
			hide();
			moviePlayerGui->filename = "rtmp://flash.cdn.deluxemusic.tv/deluxemusic.tv-live/web_850.stream";
			moviePlayerGui->Title = "Music Deluxe";
			moviePlayerGui->Info1 = "Stream";
			moviePlayerGui->exec(NULL, "urlplayback");
			paintHead();
		}
		else if(msg == CRCInput::RC_2)
		{
			hide();
			moviePlayerGui->filename = "rtsp://apasfwl.apa.at:80/orf1_q6a/orf.sdp";
			moviePlayerGui->Title = "ORF 1";
			moviePlayerGui->Info1 = "Stream";
			moviePlayerGui->exec(NULL, "urlplayback");
			paintHead();
		}
		else if(msg == CRCInput::RC_3)
		{
			hide();
			moviePlayerGui->filename = "rtsp://apasfwl.apa.at:80/orf2_q6a/orf.sdp";	
			moviePlayerGui->Title = "ORF 2";
			moviePlayerGui->Info1 = "Stream";
			moviePlayerGui->exec(NULL, "urlplayback");
			paintHead();
		}
		else if(msg == CRCInput::RC_4)
		{
			hide();
			moviePlayerGui->filename = "rtsp://apasfwl.apa.at:80/orf3_q6a/orf.sdp";	
			moviePlayerGui->Title = "ORF 3";
			moviePlayerGui->Info1 = "Stream";
			moviePlayerGui->exec(NULL, "urlplayback");
			paintHead();
		}
		else if(msg == CRCInput::RC_5)
		{
			hide();
			moviePlayerGui->filename = "rtsp://apasfwl.apa.at/orfs_q6a/orf.sdp";
			moviePlayerGui->Title = "ORF Sport";
			moviePlayerGui->Info1 = "Stream";
			moviePlayerGui->exec(NULL, "urlplayback");
			paintHead();
		}
		else if(msg == CRCInput::RC_6)
		{
			hide();
			moviePlayerGui->filename = "rtmp://live190.la3.origin.filmon.com:8086/live/198.high.stream";
			moviePlayerGui->Title = "Filmon asia";
			moviePlayerGui->Info1 = "Stream";
			moviePlayerGui->exec(NULL, "urlplayback");
			paintHead();
		}
		else if(msg == CRCInput::RC_7)
		{
			hide();
			moviePlayerGui->filename = "rtmp://live190.la3.origin.filmon.com:8086/live/244.high.stream";
			moviePlayerGui->Title = "Filmon black";
			moviePlayerGui->Info1 = "Stream";
			moviePlayerGui->exec(NULL, "urlplayback");
			paintHead();
		}
		else if(msg == CRCInput::RC_8)
		{
			hide();
			moviePlayerGui->filename = "rtmp://live190.la3.origin.filmon.com:8086/live/247.high.stream";
			moviePlayerGui->Title = "Filmon 1";
			moviePlayerGui->Info1 = "Stream";
			moviePlayerGui->exec(NULL, "urlplayback");
			paintHead();
		}
		else if(msg == CRCInput::RC_9)
		{
			hide();
			moviePlayerGui->filename = "rtmp://live190.la3.origin.filmon.com:8086/live/245.high.stream";
			moviePlayerGui->Title = "Filmon 2";
			moviePlayerGui->Info1 = "Stream";
			moviePlayerGui->exec(NULL, "urlplayback");
			paintHead();
		}
		else if(msg == CRCInput::RC_0)
		{
			hide();
			moviePlayerGui->filename = "rtmp://live190.la3.origin.filmon.com:8086/live/246.high.stream";
			moviePlayerGui->Title = "Filmon 3";
			moviePlayerGui->Info1 = "Stream";
			moviePlayerGui->exec(NULL, "urlplayback");
			paintHead();
		}
		else if (CNeutrinoApp::getInstance()->handleMsg(msg, data) & messages_return::cancel_all)
		{
			dprintf(DEBUG_NORMAL, "CTest: forward events to neutrino\n");
				
			loop = false;
		}
		
		mp_frameBuffer->blit();	
	}

	return ret;
}
#endif

void plugin_exec(void)
{
	printf("Plugins: starting Media Portal\n");
	
	//CMenuWidget * mediaPortal = new CMenuWidget("Media Portal", NEUTRINO_ICON_STREAMING);
	CMPBrowser * mpHandler = new CMPBrowser();
	
	// youtube
	//mediaPortal->addItem(new CMenuForwarderItemMenuIconNonLocalized("Youtube", true, NULL, mpHandler, "youtube", NULL, NULL, NEUTRINO_ICON_MENUITEM_YT));
	
	// netzkino
	//mediaPortal->addItem(new CMenuForwarderItemMenuIconNonLocalized("Netzkino", true, NULL, mpHandler, "netzkino", NULL, NULL, PLUGINDIR "/netzkino.png"));
	
	#if 0
	mediaPortal->addItem(new CMenuForwarderItemMenuIconNonLocalized("Music Deluxe", true, NULL, mpHandler, "musicdeluxe", NULL, NULL, PLUGINDIR "/mediaportal/deluxemusic.png"));
	
	// orf
	mediaPortal->addItem(new CMenuForwarderItemMenuIconNonLocalized("ORF 1", true, NULL, mpHandler, "orf1", NULL, NULL, PLUGINDIR "/mediaportal/orf.png"));
	
	mediaPortal->addItem(new CMenuForwarderItemMenuIconNonLocalized("ORF 2", true, NULL, mpHandler, "orf2", NULL, NULL, PLUGINDIR "/mediaportal/orf.png"));
	
	mediaPortal->addItem(new CMenuForwarderItemMenuIconNonLocalized("ORF 3", true, NULL, mpHandler, "orf3", NULL, NULL, PLUGINDIR "/mediaportal/orf.png"));
	
	mediaPortal->addItem(new CMenuForwarderItemMenuIconNonLocalized("ORF Sport", true, NULL, mpHandler, "orfsport", NULL, NULL, PLUGINDIR "/mediaportal/orf.png"));
	
	// filmon
	mediaPortal->addItem(new CMenuForwarderItemMenuIconNonLocalized("Filmon asia", true, NULL, mpHandler, "filmonasia", NULL, NULL, PLUGINDIR "/mediaportal/filmon.png"));
	mediaPortal->addItem(new CMenuForwarderItemMenuIconNonLocalized("Filmon black", true, NULL, mpHandler, "filmonblack", NULL, NULL, PLUGINDIR "/mediaportal/filmon.png"));
	mediaPortal->addItem(new CMenuForwarderItemMenuIconNonLocalized("Filmon 1", true, NULL, mpHandler, "filmon1", NULL, NULL, PLUGINDIR "/mediaportal/filmon.png"));
	mediaPortal->addItem(new CMenuForwarderItemMenuIconNonLocalized("Filmon 2", true, NULL, mpHandler, "filmon2", NULL, NULL, PLUGINDIR "/mediaportal/filmon.png"));
	mediaPortal->addItem(new CMenuForwarderItemMenuIconNonLocalized("Filmon 3", true, NULL, mpHandler, "filmon3", NULL, NULL, PLUGINDIR "/mediaportal/filmon.png"));
	#endif
	
	//mediaPortal->exec(NULL, "");
	//mediaPortal->hide();
	
	//mpHandler->paintHead();
	//mpHandler->paintGrid();
	
	mpHandler->exec();
	
	delete mpHandler;
	//delete mediaPortal;
}


