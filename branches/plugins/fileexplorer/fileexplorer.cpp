/*
  neutrinoHD2 project
  
  https://code.google.com/p/neutrinohd2/
  
  $Id: fileexplorer.cpp 2015/06/26 mohousch Exp $

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
extern "C" void plugin_init(void);
extern "C" void plugin_del(void);

void plugin_init(void)
{
}

void plugin_del(void)
{
}

void plugin_exec(void)
{
	neutrino_msg_t msg;
	neutrino_msg_data_t data;
		
	CFileBrowser filebrowser;	
	filebrowser.use_filter = false;	
	
	std::string Path_local = "/media/hdd";
	
	
BROWSER:	
	if (filebrowser.exec(Path_local.c_str())) 
	{
		Path_local = filebrowser.getCurrentDir(); // remark path
		
		// get the current file name
		CFile * file;

		if ((file = filebrowser.getSelectedFile()) != NULL) 
		{
			// parse file extension
			if(file->getType() == CFile::FILE_PICTURE)
			{
				CPictureViewerGui tmpPictureViewerGui;
				CPicture pic;
				struct stat statbuf;
				
				pic.Filename = file->Name;
				std::string tmp = file->Name.substr(file->Name.rfind('/') + 1);
				pic.Name = tmp.substr(0, tmp.rfind('.'));
				pic.Type = tmp.substr(tmp.rfind('.') + 1);
				
				if(stat(pic.Filename.c_str(), &statbuf) != 0)
					printf("stat error");
				pic.Date = statbuf.st_mtime;
								
				tmpPictureViewerGui.addToPlaylist(pic);
				tmpPictureViewerGui.exec(NULL, "urlplayback");
			}
			else if(file->getType() == CFile::FILE_TEXT || file->getType() == CFile::FILE_XML)
			{
				std::string buffer;
				buffer.clear();
				
				char buf[256];
				FILE* pFile;
				pFile = fopen(file->Name.c_str(), "r");
				if(pFile != NULL)
				{
					fgets(buf, sizeof(buf), pFile);
				}
				fclose(pFile);

				buffer = buf;
				int mode =  CInfoBox::SCROLL | CInfoBox::TITLE | CInfoBox::FOOT | CInfoBox::BORDER;// | //CInfoBox::NO_AUTO_LINEBREAK | //CInfoBox::CENTER | //CInfoBox::AUTO_WIDTH | //CInfoBox::AUTO_HIGH;
				CBox position(g_settings.screen_StartX + 50, g_settings.screen_StartY + 50, g_settings.screen_EndX - g_settings.screen_StartX - 100, g_settings.screen_EndY - g_settings.screen_StartY - 100); 
					
				CInfoBox * infoBox = new CInfoBox(file->getFileName().c_str(), g_Font[SNeutrinoSettings::FONT_TYPE_MENU], mode, &position, file->getFileName().c_str(), g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE], NULL);
				infoBox->setText(&buffer);
				infoBox->exec();
				delete infoBox;
			}
			else if(file->getType() == CFile::FILE_VIDEO)
			{
				moviePlayerGui->filename = file->Name.c_str();
				
				moviePlayerGui->Title = file->Title;
				moviePlayerGui->Info1 = file->Info1;
				moviePlayerGui->Info2 = file->Info2;
				
				// play
				moviePlayerGui->exec(NULL, "urlplayback");
			}
			else if(file->getType() == CFile::FILE_AUDIO)
			{
				CAudioPlayerGui tmpAudioPlayerGui;
			
				CAudiofileExt audiofile(file->Name, file->getExtension());
				tmpAudioPlayerGui.addToPlaylist(audiofile);
				tmpAudioPlayerGui.exec(NULL, "urlplayback");
			}
		}

		g_RCInput->getMsg_ms(&msg, &data, 10);
		
		if (msg != CRCInput::RC_home) 
		{
			goto BROWSER;
		}
	}
}
