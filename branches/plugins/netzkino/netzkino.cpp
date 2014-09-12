/*
  neutrinoHD2 project
  
  https://code.google.com/p/neutrinohd2/
  
  $Id: netzkino.cpp 2014/01/22 mohousch Exp $

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
	printf("Plugins: starting netzkino.de player\n");
	
	// start netzkino
	//moviePlayerGui->exec(NULL, "netzkinoplayback");
	
	
	CMovieBrowser * moviebrowser;
	MI_MOVIE_INFO * p_movie_info;
	
	moviebrowser = new CMovieBrowser();
	std::string Path_local = "/";
	
	moviebrowser->setMode(MB_SHOW_NETZKINO);
	
	if (moviebrowser->exec(Path_local.c_str())) 
	{
		// get the current path and file name
		Path_local = moviebrowser->getCurrentDir();
		CFile * file;

		if ((file = moviebrowser->getSelectedFile()) != NULL) 
		{
			moviePlayerGui->filename = file->Url.c_str();
			
			// movieinfos
			p_movie_info = moviebrowser->getCurrentMovieInfo();
			
			moviePlayerGui->Title = p_movie_info->epgTitle;
			moviePlayerGui->Info1 = p_movie_info->epgInfo1;
			moviePlayerGui->Info2 = p_movie_info->epgInfo2;
			moviePlayerGui->Channel = p_movie_info->epgChannel;
			
			// play
			moviePlayerGui->exec(NULL, "urlplayback");
		}
	}
						
	delete moviebrowser;				
}


