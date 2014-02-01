/*
  neutrinoHD2 project
  
  https://code.google.com/p/neutrinohd2/
  
  $Id: dvdplayer.h 2014/02/01 mohousch Exp $

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
	dprintf(DEBUG_INFO, "Plugins: starting DVDPlayer\n");
	
	CMenuWidget * dvdPlayerMenu = new CMenuWidget("DVD/Blueray Player", NEUTRINO_ICON_STREAMING);
	
	// dvd
	dvdPlayerMenu->addItem(new CMenuForwarderNonLocalized("DVD player", true, NULL, moviePlayerGui, "dvdplayback", CRCInput::convertDigitToKey(1)));
	
	// blueray
	dvdPlayerMenu->addItem(new CMenuForwarderNonLocalized("Blueray Player", true, NULL, moviePlayerGui, "bluerayplayback", CRCInput::convertDigitToKey(2)));	
	
	dvdPlayerMenu->exec(NULL, "");
	dvdPlayerMenu->hide();
	
	delete dvdPlayerMenu;
	
	return 0;
}
