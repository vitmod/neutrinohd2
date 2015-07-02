/*
  neutrinoHD2 project
  
  https://code.google.com/p/neutrinohd2/
  
  $Id: netzkino.h 2014/10/03 mohousch Exp $

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

#ifndef __NK__
#define __NK__

#include <plugin.h>
#include <nkparser.h>


#define MIN_NKBROWSER_FRAME_HEIGHT 	100
#define MAX_NKBROWSER_FRAME_HEIGHT 	400
#define NKB_MAX_ROWS 			3

typedef enum
{
	NKB_INFO_FILENAME 		= 0,
	NKB_INFO_TITLE 			= 1,
	NKB_INFO_INFO1 			= 2,
	NKB_INFO_RECORDDATE 		= 3,
	NKB_INFO_MAX_NUMBER		= 4
}NKB_INFO_ITEM;

typedef enum
{
	NKB_FOCUS_BROWSER = 0,
	NKB_FOCUS_MOVIE_INFO = 1,
	NKB_FOCUS_MAX_NUMBER = 2
}NKB_FOCUS;

typedef enum
{
	NKB_GUI_BROWSER_ONLY = 0,
	NKB_GUI_MOVIE_INFO = 1,
	NKB_GUI_MAX_NUMBER = 2
}NKB_GUI;

typedef struct
{
	NKB_GUI gui;
	
	// these variables are used for the listframes
	int browserFrameHeight;
	int browserRowNr;
	NKB_INFO_ITEM browserRowItem[NKB_MAX_ROWS];
	int browserRowWidth[NKB_MAX_ROWS];
	
	// netzkino	
	int nkmode;
	int nkcategory;
	std::string nkcategoryname;
	std::string nksearch;
}NKB_SETTINGS;

class CNetzKinoBrowser : public CMenuTarget
{
	private:
		CFrameBuffer * m_pcWindow;
		CListFrame * m_pcBrowser;
		
		CTextBox * m_pcInfo;
		
		CBox m_cBoxFrame;
		CBox m_cBoxFrameBrowserList;
		CBox m_cBoxFrameInfo;
		CBox m_cBoxFrameFootRel;
		CBox m_cBoxFrameTitleRel;
		
		LF_LINES m_browserListLines;
		
		std::vector<MI_MOVIE_INFO> m_vMovieInfo;
		std::vector<MI_MOVIE_INFO*> m_vHandleBrowserList;
		
		unsigned int m_currentBrowserSelection;
 		unsigned int m_prevBrowserSelection;
		
		bool m_showBrowserFiles;
		bool m_showMovieInfo;
		
		MI_MOVIE_INFO * m_movieSelectionHandler;
		
		NKB_FOCUS m_windowFocus;
		
		bool m_reload_movies;
		
		static CFont * m_pcFontFoot;
		static CFont * m_pcFontTitle;
		
		std::string m_textTitle;
		
		NKB_SETTINGS m_settings;
		
		CMovieInfo m_movieInfo;
		
		// netzkino		
		cNKFeedParser nkparser;
		std::string nkcategory_name;
		
		void loadNKTitles(int mode, std::string search, int id, unsigned int start, unsigned int end);
		bool showNKMenu();
		int videoListsize;
		
		int NKStart, NKEnd;
		
	public:
		CNetzKinoBrowser();
		~CNetzKinoBrowser();
		
		int exec();
		int exec(CMenuTarget* parent, const std::string& actionKey);
		
		CFile * getSelectedFile(void); 
		MI_MOVIE_INFO* getCurrentMovieInfo(void){return(m_movieSelectionHandler);};
		
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
		void refreshMovieInfo(void);
		void refreshFoot(void);
		void refreshTitle(void);
		void refreshInfo(void);
		void refreshLCD(void);
		
		// event
		bool onButtonPress(neutrino_msg_t msg); 
		bool onButtonPressMainFrame(neutrino_msg_t msg);
		bool onButtonPressBrowserList(neutrino_msg_t msg);
		bool onButtonPressMovieInfoList(neutrino_msg_t msg);
		
		void onSetFocus(NKB_FOCUS new_focus);
		void onSetFocusNext(void);
		
		void onSetGUIWindow(NKB_GUI gui);
		
		void loadMovies();
		
		// misc
		void updateMovieSelection(void);
		bool getMovieInfoItem(MI_MOVIE_INFO& movie_info, NKB_INFO_ITEM item, std::string* item_string);
};

#endif //__NK__

