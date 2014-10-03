/*
  neutrinoHD2 project
  
  https://code.google.com/p/neutrinohd2/
  
  $Id: mediaportal.h 2014/10/03 mohousch Exp $

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

#ifndef __MP__
#define __MP__

#include <plugin.h>


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
		bool mp_reload_items;
		
		CFile * mp_itemSelectionHandler;
		
		MPB_FOCUS mp_windowFocus;
		
		static CFont * mp_FontFoot;
		static CFont * mp_FontTitle;
		
		MPB_SETTINGS mp_settings;
		
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
		void refreshFoot(void);
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
		
		void addMusicDeluxe(void);
		void addFilmon1(void);
		void addFilmon2(void);
		void addFilmon3(void);
		void addNetzKino(void);
		void addYouTube(void);
		void loadItems(void);
		
		// misc
		void updateItemSelection(void);
		bool getItemInfoItem(CFile &item_info, MPB_INFO_ITEM item, std::string* item_string);
};

#endif //__MP__
