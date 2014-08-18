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

class CMediaPortal : public CMenuTarget
{
        public:
		void ORF(void);
                int exec(CMenuTarget *parent,  const std::string &actionkey);
};

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
				moviePlayerGui->g_file_epg = "ORF 1";
				moviePlayerGui->g_file_epg1 = "Stream";
				
				moviePlayerGui->exec(NULL, "urlplayback");
				break;
						
			case 1:
				moviePlayerGui->filename = "rtsp://apasfwl.apa.at:80/orf2_q6a/orf.sdp";
				moviePlayerGui->g_file_epg = "ORF 2";
				moviePlayerGui->g_file_epg1 = "Stream";
				moviePlayerGui->exec(NULL, "urlplayback");
				break;
				
			case 2:
				moviePlayerGui->filename = "rtsp://apasfwl.apa.at:80/orf3_q6a/orf.sdp";	
				moviePlayerGui->g_file_epg = "ORF 3";
				moviePlayerGui->g_file_epg1 = "Stream";
				moviePlayerGui->exec(NULL, "urlplayback");
				break;
			case 3:
				moviePlayerGui->filename = "rtsp://apasfwl.apa.at/orfs_q6a/orf.sdp";
				moviePlayerGui->g_file_epg = "ORF Sport";
				moviePlayerGui->g_file_epg1 = "Stream";
				moviePlayerGui->exec(NULL, "urlplayback");
				break;
						
			default: break;
		}
	}
}

int CMediaPortal::exec(CMenuTarget *parent, const std::string &actionKey)
{
	if(parent)
		parent->hide();

	printf("CMediaPortal::exec: %s\n", actionKey.c_str());
	
	if(actionKey == "youtube") 
	{
		moviePlayerGui->exec(NULL, "ytplayback");
	}
	else if(actionKey == "netzkino") 
	{
		moviePlayerGui->exec(NULL, "netzkinoplayback");
	}
	else if(actionKey == "musicdeluxe")
	{
		moviePlayerGui->filename = "rtmp://flash.cdn.deluxemusic.tv/deluxemusic.tv-live/web_850.stream";
		moviePlayerGui->g_file_epg = "Music Deluxe";
		moviePlayerGui->g_file_epg1 = "Stream";
		moviePlayerGui->exec(NULL, "urlplayback");
	}
	else if(actionKey == "orf1")
	{
		moviePlayerGui->filename = "rtsp://apasfwl.apa.at:80/orf1_q6a/orf.sdp";	
		moviePlayerGui->exec(NULL, "urlplayback");
	}
	else if(actionKey == "orf2")
	{
		moviePlayerGui->filename = "rtsp://apasfwl.apa.at:80/orf2_q6a/orf.sdp";	
		moviePlayerGui->exec(NULL, "urlplayback");
	}
	else if(actionKey == "orf3")
	{
		moviePlayerGui->filename = "rtsp://apasfwl.apa.at:80/orf3_q6a/orf.sdp";	
		moviePlayerGui->exec(NULL, "urlplayback");
	}
	else if(actionKey == "orfsport")
	{
		moviePlayerGui->filename = "rtsp://apasfwl.apa.at/orfs_q6a/orf.sdp";	
		moviePlayerGui->exec(NULL, "urlplayback");
	}
	else if(actionKey == "ardmt")
	{
		ShowHintUTF(LOCALE_MESSAGEBOX_INFO, "not yet", 450, 2 );
	}
	else if(actionKey == "zdfmt")
	{
		ShowHintUTF(LOCALE_MESSAGEBOX_INFO, "not yet", 450, 2 );
	}
	else if(actionKey == "orfmt")
	{
		ShowHintUTF(LOCALE_MESSAGEBOX_INFO, "not yet", 450, 2 );
	}
	else if(actionKey == "artemt")
	{
		ShowHintUTF(LOCALE_MESSAGEBOX_INFO, "not yet", 450, 2 );
	}
	else if(actionKey == "orf")
	{
		ORF();
	}

	return menu_return::RETURN_REPAINT;
}

void plugin_exec(void)
{
	printf("Plugins: starting Media Portal\n");
	
	CMenuWidget * mediaPortal = new CMenuWidget("Media Portal", NEUTRINO_ICON_STREAMING);
	CMediaPortal * mpHandler = new CMediaPortal();
	
	mediaPortal->addItem(new CMenuForwarderItemMenuIconNonLocalized("Youtube", true, NULL, mpHandler, "youtube", NULL, NULL, NEUTRINO_ICON_MENUITEM_YT));
	
	// netzkino
	mediaPortal->addItem(new CMenuForwarderItemMenuIconNonLocalized("Netzkino", true, NULL, mpHandler, "netzkino", NULL, NULL, NEUTRINO_ICON_MENUITEM_NETZKINO));
	
	mediaPortal->addItem(new CMenuForwarderItemMenuIconNonLocalized("Music Deluxe", true, NULL, mpHandler, "musicdeluxe", NULL, NULL, NEUTRINO_ICON_MENUITEM_WEBTV));
	
	// orf stream
	/*
	mediaPortal->addItem(new CMenuForwarderItemMenuIconNonLocalized("ORF 1", true, NULL, mpHandler, "orf1", NULL, NULL, NEUTRINO_ICON_MENUITEM_WEBTV));
	mediaPortal->addItem(new CMenuForwarderItemMenuIconNonLocalized("ORF 2", true, NULL, mpHandler, "orf2", NULL, NULL, NEUTRINO_ICON_MENUITEM_WEBTV));
	mediaPortal->addItem(new CMenuForwarderItemMenuIconNonLocalized("ORF 3", true, NULL, mpHandler, "orf3", NULL, NULL, NEUTRINO_ICON_MENUITEM_WEBTV));
	mediaPortal->addItem(new CMenuForwarderItemMenuIconNonLocalized("ORF Sport HQ", true, NULL, mpHandler, "orfsport", NULL, NULL, NEUTRINO_ICON_MENUITEM_WEBTV));
	*/
	mediaPortal->addItem(new CMenuForwarderItemMenuIconNonLocalized("ORF Streams", true, NULL, mpHandler, "orf", NULL, NULL, NEUTRINO_ICON_MENUITEM_WEBTV));
	
	// dummies
	mediaPortal->addItem(new CMenuForwarderItemMenuIconNonLocalized("dummy 1", true, NULL, mpHandler, "ardmt", NULL, NULL, NEUTRINO_ICON_MENUITEM_WEBTV));
	mediaPortal->addItem(new CMenuForwarderItemMenuIconNonLocalized("dummy 2", true, NULL, mpHandler, "zdfmt", NULL, NULL, NEUTRINO_ICON_MENUITEM_WEBTV));
	mediaPortal->addItem(new CMenuForwarderItemMenuIconNonLocalized("dummy 3", true, NULL, mpHandler, "orfmt", NULL, NULL, NEUTRINO_ICON_MENUITEM_WEBTV));
	mediaPortal->addItem(new CMenuForwarderItemMenuIconNonLocalized("dummy 4", true, NULL, mpHandler, "artemt", NULL, NULL, NEUTRINO_ICON_MENUITEM_WEBTV));
	
	mediaPortal->exec(NULL, "");
	mediaPortal->hide();
	
	delete mpHandler;
	delete mediaPortal;
}


