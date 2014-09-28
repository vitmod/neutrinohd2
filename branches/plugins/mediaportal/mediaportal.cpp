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
class CMediaPortal : public CMenuTarget
{
	private:
		CFrameBuffer * mp_frameBuffer;
		int mp_title_height;
		int mp_width;
		int mp_height;
		int mp_x;
		int mp_y;	
		
		int icon_head_w;
		int icon_head_h;
		
		//void ORF(void);
		
        public:
		CMediaPortal();
		~CMediaPortal();
                int exec(CMenuTarget *parent,  const std::string &actionkey);
		
		void paintHead();
		void hide();
		void paintGrid();
};

CMediaPortal::CMediaPortal()
{
	mp_frameBuffer = CFrameBuffer::getInstance();
	
	mp_width = mp_frameBuffer->getScreenWidth(true) - 50; 
	mp_height = mp_frameBuffer->getScreenHeight(true) - 50;
	
	mp_x = ((g_settings.screen_EndX - g_settings.screen_StartX) - mp_width)/2 + g_settings.screen_StartX;
	mp_y = ((g_settings.screen_EndY - g_settings.screen_StartY) - mp_height)/ 2 + g_settings.screen_StartY;
	
	mp_title_height = g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getHeight()*2 + 20 + g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL]->getHeight(); + 5;
}

CMediaPortal::~CMediaPortal()
{
	hide();
}

void CMediaPortal::hide()
{
	mp_frameBuffer->paintBackgroundBoxRel(mp_x, mp_y, mp_width, mp_title_height);
}

void CMediaPortal::paintHead()
{
	mp_frameBuffer->paintBoxRel(mp_x, mp_y, mp_width, mp_title_height - 10, COL_MENUCONTENT_PLUS_6 );
	mp_frameBuffer->paintBoxRel(mp_x + 2, mp_y + 2 , mp_width - 4, mp_title_height - 14, COL_MENUCONTENTSELECTED_PLUS_0);
	
	// title
	std::string title = "neutrinoHD2 Media Portlal (C)";
	int tw = g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getRenderWidth(title, true); // UTF-8
	int xstart = (mp_width - tw) / 2;
	if(xstart < 10)
		xstart = 10;
	g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->RenderString(mp_x + xstart, mp_y + 4 + 1*g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getHeight(), mp_width - 20, title, COL_MENUCONTENTSELECTED, 0, true); // UTF-8
	
	// info
	std::string info = "Mediatheck Archiv";
	int iw = g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getRenderWidth(info, true); // UTF-8
	xstart = (mp_width - iw) / 2;
	if(xstart < 10)
		xstart = 10;
	g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->RenderString(mp_x + xstart, mp_y + 4 + 2*g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getHeight(), mp_width - 20, "Mediatheck Archiv", COL_MENUCONTENTSELECTED, 0, true); // UTF-8	
	
	// icon
	// head icon
	std::string icon_head = PLUGINDIR "/mediaportal/mp.png";
	mp_frameBuffer->getIconSize(icon_head.c_str(), &icon_head_w, &icon_head_h);
	mp_frameBuffer->paintIcon(icon_head.c_str(), mp_x + 10, mp_y + (mp_title_height - 10 - icon_head_h)/2);
}

void CMediaPortal::paintGrid()
{
	/*
	int COL_H = 44;
	int COL_W = 104;
	int COLS = (mp_height - mp_title_height)/COL_H;
	int COLX = mp_width/COL_W;
	int SPACE = 2;
	
	//for (int y = mp_y + mp_title_height + SPACE; y < COL_H*COLS; y++)
	{
		for (int x = mp_x; x < COLX*COL_W; x++)
		{
			mp_frameBuffer->paintBoxRel(x, mp_y + mp_title_height + SPACE, COL_W, COL_H, COL_MENUCONTENTSELECTED_PLUS_0 );
		}
	}
	*/
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

int CMediaPortal::exec(CMenuTarget *parent, const std::string &actionKey)
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

void plugin_exec(void)
{
	printf("Plugins: starting Media Portal\n");
	
	//CMenuWidget * mediaPortal = new CMenuWidget("Media Portal", NEUTRINO_ICON_STREAMING);
	CMediaPortal * mpHandler = new CMediaPortal();
	
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
	
	mpHandler->exec(NULL, "");
	
	delete mpHandler;
	//delete mediaPortal;
}


