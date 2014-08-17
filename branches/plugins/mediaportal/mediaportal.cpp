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
                int exec(CMenuTarget *parent,  const std::string &actionkey);
};

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
	mediaPortal->addItem(new CMenuForwarderItemMenuIconNonLocalized("ORF 1", true, NULL, mpHandler, "orf1", NULL, NULL, NEUTRINO_ICON_MENUITEM_WEBTV));
	mediaPortal->addItem(new CMenuForwarderItemMenuIconNonLocalized("ORF 2", true, NULL, mpHandler, "orf2", NULL, NULL, NEUTRINO_ICON_MENUITEM_WEBTV));
	mediaPortal->addItem(new CMenuForwarderItemMenuIconNonLocalized("ORF 3", true, NULL, mpHandler, "orf3", NULL, NULL, NEUTRINO_ICON_MENUITEM_WEBTV));
	mediaPortal->addItem(new CMenuForwarderItemMenuIconNonLocalized("ORF Sport HQ", true, NULL, mpHandler, "orfsport", NULL, NULL, NEUTRINO_ICON_MENUITEM_WEBTV));
	
	mediaPortal->exec(NULL, "");
	mediaPortal->hide();
	
	delete mpHandler;
	delete mediaPortal;
}


