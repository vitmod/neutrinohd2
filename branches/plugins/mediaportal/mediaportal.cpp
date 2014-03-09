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


extern "C" int plugin_exec(void);
  
int plugin_exec(void)
{
	printf("Plugins: starting Media Portal\n");
	
	CMenuWidget * mediaPortal = new CMenuWidget("Media Portal", NEUTRINO_ICON_STREAMING);
	
	// yt
	mediaPortal->addItem(new CMenuForwarderItemMenuIconNonLocalized("YouTube", true, NULL, moviePlayerGui, "ytplayback", CRCInput::convertDigitToKey(1)));
	
	// netzkino
	mediaPortal->addItem(new CMenuForwarderItemMenuIconNonLocalized("NetzKino.de", true, NULL, moviePlayerGui, "netzkinoplayback", CRCInput::convertDigitToKey(2)));	
	
	// deluxe music
	moviePlayerGui->filename = "rtmp://flash.cdn.deluxemusic.tv/deluxemusic.tv-live/web_850.stream";	
	mediaPortal->addItem(new CMenuForwarderItemMenuIconNonLocalized("Music Deluxe", true, NULL, moviePlayerGui, "urlplayback", CRCInput::convertDigitToKey(3)));	
	
	// orf1
	moviePlayerGui->filename = "rtsp://apasfwl.apa.at:80/orf1_q6a/orf.sdp";	
	mediaPortal->addItem(new CMenuForwarderItemMenuIconNonLocalized("ORF 1", true, NULL, moviePlayerGui, "urlplayback", CRCInput::convertDigitToKey(4)));
	
	// orf2
	moviePlayerGui->filename = "rtsp://apasfwl.apa.at:80/orf2_q6a/orf.sdp";	
	mediaPortal->addItem(new CMenuForwarderItemMenuIconNonLocalized("ORF 2", true, NULL, moviePlayerGui, "urlplayback", CRCInput::convertDigitToKey(3)));
	
	// orf3
	moviePlayerGui->filename = "rtsp://apasfwl.apa.at:80/orf3_q6a/orf.sdp";	
	mediaPortal->addItem(new CMenuForwarderItemMenuIconNonLocalized("ORF 3", true, NULL, moviePlayerGui, "urlplayback", CRCInput::convertDigitToKey(3)));
	
	// orf sport hq
	moviePlayerGui->filename = "rtsp://apasfwl.apa.at/orfs_q6a/orf.sdp";	
	mediaPortal->addItem(new CMenuForwarderItemMenuIconNonLocalized("ORF Sport HQ", true, NULL, moviePlayerGui, "urlplayback", CRCInput::convertDigitToKey(3)));
	
	mediaPortal->exec(NULL, "");
	mediaPortal->hide();
	
	delete mediaPortal;
	
	return 0;
}


