/*
  neutrinoHD2 project
  
  https://code.google.com/p/neutrinohd2/
  
  $Id: vlcplayer.h 2014/01/22 mohousch Exp $

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

void plugin_exec(void)
{
	printf("Plugins: starting VLCPlayer\n");
	
	dprintf(DEBUG_INFO, "Plugins: starting VLCPlayer\n");
	
	CMenuWidget * vlcPlayerMenu = new CMenuWidget("VLC Client player", NEUTRINO_ICON_STREAMING);
	
	// file
	vlcPlayerMenu->addItem(new CMenuForwarderNonLocalized("vlc player", true, NULL, moviePlayerGui, "vlcplayback", CRCInput::convertDigitToKey(1)));
	
	// dvd
	vlcPlayerMenu->addItem(new CMenuForwarderNonLocalized("DVD", true, NULL, moviePlayerGui, "vlcdvdplayback", CRCInput::convertDigitToKey(2)));

	// (s)vcd
	vlcPlayerMenu->addItem(new CMenuForwarderNonLocalized("(S)VCD", true, NULL, moviePlayerGui, "vlcsvcdplayback", CRCInput::convertDigitToKey(3)));	
	
	vlcPlayerMenu->exec(NULL, "");
	vlcPlayerMenu->hide();
	
	delete vlcPlayerMenu;
}
