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
        public:
		//void ORF(void);
                int exec(CMenuTarget *parent,  const std::string &actionkey);
};

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

	printf("CMediaPortal::exec: %s\n", actionKey.c_str());
	
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
	*/
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
	/*
	else if(actionKey == "orf")
	{
		ORF();
	}
	*/

	return ret;
}

void plugin_exec(void)
{
	printf("Plugins: starting Media Portal\n");
	
	CMenuWidget * mediaPortal = new CMenuWidget("Media Portal", NEUTRINO_ICON_STREAMING);
	CMediaPortal * mpHandler = new CMediaPortal();
	
	//mediaPortal->addItem(new CMenuForwarderItemMenuIconNonLocalized("Youtube", true, NULL, mpHandler, "youtube", NULL, NULL, NEUTRINO_ICON_MENUITEM_YT));
	
	// netzkino
	//mediaPortal->addItem(new CMenuForwarderItemMenuIconNonLocalized("Netzkino", true, NULL, mpHandler, "netzkino", NULL, NULL, PLUGINDIR "/netzkino.png"));
	
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
	
	mediaPortal->exec(NULL, "");
	mediaPortal->hide();
	
	delete mpHandler;
	delete mediaPortal;
}


