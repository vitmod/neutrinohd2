/*
  neutrinoHD2 project
  
  https://code.google.com/p/neutrinohd2/
  
  $Id: mediaplayer.cpp 2014/01/25 mohousch Exp $

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
	printf("Plugins: starting mediaplayer\n");
	
	//moviePlayerGui->exec(NULL, "fileplayback");
	CFileBrowser filebrowser;
	CFileFilter fileFilter;
	CFileList filelist;
	
	int selected = 0;
	
	fileFilter.addFilter("ts");
	fileFilter.addFilter("mpg");
	fileFilter.addFilter("mpeg");
	fileFilter.addFilter("divx");
	fileFilter.addFilter("avi");
	fileFilter.addFilter("mkv");
	fileFilter.addFilter("asf");
	fileFilter.addFilter("aiff");
	fileFilter.addFilter("m2p");
	fileFilter.addFilter("mpv");
	fileFilter.addFilter("m2ts");
	fileFilter.addFilter("vob");
	fileFilter.addFilter("mp4");
	fileFilter.addFilter("mov");	
	fileFilter.addFilter("flv");	
	fileFilter.addFilter("dat");
	fileFilter.addFilter("trp");
	fileFilter.addFilter("vdr");
	fileFilter.addFilter("mts");
	fileFilter.addFilter("wmv");
	fileFilter.addFilter("wav");
	fileFilter.addFilter("flac");
	fileFilter.addFilter("mp3");
	fileFilter.addFilter("wma");
	fileFilter.addFilter("ogg");

	//filebrowser.Multi_Select    = true;
	//filebrowser.Dirs_Selectable = true;
	filebrowser.Filter = &fileFilter;

BROWSER:
	if (filebrowser.exec(g_settings.network_nfs_moviedir))
	{
		filelist = filebrowser.getSelectedFiles();
		
		CFile *file = filebrowser.getSelectedFile();
		filelist.clear();
		filelist.push_back(*file);
		
		if(!filelist.empty())
		{
			moviePlayerGui->filename = filelist[selected].Name.c_str();	
			moviePlayerGui->Title = filelist[selected].getFileName();
			moviePlayerGui->Info1 = filelist[selected].getFileName();
			moviePlayerGui->Info2 = filelist[selected].getFileName();
			
			//moviePlayerGui->was_file = true;
			//moviePlayerGui->filelist = filelist;

			// play
			moviePlayerGui->exec(NULL, "urlplayback");
		}
		
		neutrino_msg_t msg;
		neutrino_msg_data_t data;

		g_RCInput->getMsg_ms(&msg, &data, 40);
		
		if (msg != CRCInput::RC_home) 
		{
			goto BROWSER;
		}
	}
}
