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
				bool loop = true;
					
				g_PicViewer->SetScaling((CFrameBuffer::ScalingMode)g_settings.picviewer_scaling);
				g_PicViewer->SetVisible(g_settings.screen_StartX, g_settings.screen_EndX, g_settings.screen_StartY, g_settings.screen_EndY);

				if(g_settings.video_Ratio == 1)
					g_PicViewer->SetAspectRatio(16.0/9);
				else
					g_PicViewer->SetAspectRatio(4.0/3);


				g_PicViewer->ShowImage(file->Name);
					
				while (loop)
				{
					g_RCInput->getMsg(&msg, &data, 10); // 1 sec

					if( msg == CRCInput::RC_home)
						loop = false;
				}
						
				CFrameBuffer::getInstance()->ClearFrameBuffer();
				CFrameBuffer::getInstance()->blit();	
			}
			else if(file->getType() == CFile::FILE_TEXT || file->getType() == CFile::FILE_XML)
			{
				std::string buffer;
				
				/*
				FILE* pFile;
				pFile = fopen(file->Name.c_str(), "r");
				if(pFile)
				{
					fgets((char *)buffer.c_str(), 256, pFile); 
				}
				fclose(pFile);
				*/

				int mode =  CMsgBox::SCROLL | CMsgBox::TITLE | CMsgBox::FOOT | CMsgBox::BORDER;// | //CMsgBox::NO_AUTO_LINEBREAK | //CMsgBox::CENTER | //CMsgBox::AUTO_WIDTH | //CMsgBox::AUTO_HIGH;
				CBox position(g_settings.screen_StartX + 50, g_settings.screen_StartY + 50, g_settings.screen_EndX - g_settings.screen_StartX - 100, g_settings.screen_EndY - g_settings.screen_StartY - 100); 
					
				CMsgBox * msgBox = new CMsgBox(file->getFileName().c_str(), g_Font[SNeutrinoSettings::FONT_TYPE_MENU], mode, &position, file->getFileName().c_str(), g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE], NULL);
				msgBox->setText(&buffer);
				msgBox->exec();
				delete msgBox;
				buffer.clear();
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
				bool usedBackground = CFrameBuffer::getInstance()->getuseBackground();
				if (usedBackground)
					CFrameBuffer::getInstance()->saveBackgroundImage();
				
				//show audio background pic	
				CFrameBuffer::getInstance()->loadBackgroundPic("mp3.jpg");
				CFrameBuffer::getInstance()->blit();	
		
				// stop playback
				if(CNeutrinoApp::getInstance()->getMode() == NeutrinoMessages::mode_iptv)
				{
					if(webtv)
						webtv->stopPlayBack();
				}
				else
				{
					// stop/lock live playback	
					g_Zapit->lockPlayBack();
					
					//pause epg scanning
					g_Sectionsd->setPauseScanning(true);
				}	
		
				CAudiofile mp3(file->Name.c_str(), file->getExtension());
				
				printf("\ngetMetaData\n");
				// get metainfo
				CAudioPlayer::getInstance()->readMetaData(&mp3, false);
				
				printf("\npaintMetaData\n");
				// metainfobox
				CBox Box;
		
				Box.iX = g_settings.screen_StartX + 10;
				Box.iY = g_settings.screen_StartY + 10;
				Box.iWidth = g_settings.screen_EndX - g_settings.screen_StartX - 20;
				Box.iHeight = 50;
		
				CFrameBuffer::getInstance()->paintBoxRel(Box.iX, Box.iY, Box.iWidth, Box.iHeight, COL_MENUCONTENT_PLUS_6 );
				
				// infobox refresh
				CFrameBuffer::getInstance()->paintBoxRel(Box.iX + 2, Box.iY + 2 , Box.iWidth - 4, Box.iHeight - 4, COL_MENUCONTENTSELECTED_PLUS_0);

				std::string tmp;
				
				char sNr[20];
				sprintf(sNr, ": %2d", 1);
				tmp = g_Locale->getText(LOCALE_AUDIOPLAYER_PLAYING);
				tmp += sNr ;

				// first line
				int w = g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getRenderWidth(tmp, true); // UTF-8
				int xstart = (Box.iWidth - w) / 2;
				if(xstart < 10)
					xstart = 10;
				g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->RenderString(Box.iX + xstart, Box.iY + 4 + g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getHeight(), Box.iWidth - 20, tmp, COL_MENUCONTENTSELECTED, 0, true); // UTF-8
				
				tmp = mp3.MetaData.title;
				tmp += " / ";
				tmp += mp3.MetaData.artist;
				
				w = g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getRenderWidth(tmp, true); // UTF-8
				xstart = (Box.iWidth - w)/2;
				if(xstart < 10)
					xstart = 10;
				
				g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->RenderString(Box.iX + xstart, Box.iY + 4 + 2*g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getHeight(), Box.iWidth - 20, tmp, COL_MENUCONTENTSELECTED, 0, true); // UTF-8		
				
				// cover
				if (!mp3.MetaData.cover.empty())
				{
					if(!access("/tmp/cover.jpg", F_OK))
						g_PicViewer->DisplayImage("/tmp/cover.jpg", Box.iX + 2, Box.iY + 2, Box.iHeight - 4, Box.iHeight - 4);		
				}

				printf("\nPlay\n");
				// play
				CAudioPlayer::getInstance()->play(&mp3, g_settings.audioplayer_highprio == 1);
				
				printf("\nloop\n");
				bool loop = true;
				while (loop)
				{
					g_RCInput->getMsg(&msg, &data, 10); // 1 sec
					
					if( (msg == CRCInput::RC_home || msg == CRCInput::RC_stop) || CAudioPlayer::getInstance()->getState() == CBaseDec::STOP)
					{
						CAudioPlayer::getInstance()->stop();
						loop = false;
					}
				}
			
				printf("\nstop\n");
				// start playback
				if(CNeutrinoApp::getInstance()->getMode() == NeutrinoMessages::mode_iptv)
				{
					if(webtv)
						webtv->startPlayBack(webtv->getTunedChannel());
				}
				else
				{
					// unlock playback	
					g_Zapit->unlockPlayBack();	
					
					//start epg scanning
					g_Sectionsd->setPauseScanning(false);
				}
				
				CNeutrinoApp::getInstance()->StartSubtitles();
				
				//restore previous background
				if (usedBackground)
					CFrameBuffer::getInstance()->restoreBackgroundImage();
				
				CFrameBuffer::getInstance()->useBackground(usedBackground);
					
				CFrameBuffer::getInstance()->paintBackground();
				CFrameBuffer::getInstance()->blit();
			}
		}

		g_RCInput->getMsg_ms(&msg, &data, 10);
		
		if (msg != CRCInput::RC_home) 
		{
			goto BROWSER;
		}
	}
}
