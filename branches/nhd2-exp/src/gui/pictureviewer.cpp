/*
	Neutrino-GUI  -   DBoxII-Project

	MP3Player by Dirch
	
	Homepage: http://dbox.cyberphoria.org/

	Kommentar:

	Diese GUI wurde von Grund auf neu programmiert und sollte nun vom
	Aufbau und auch den Ausbaumoeglichkeiten gut aussehen. Neutrino basiert
	auf der Client-Server Idee, diese GUI ist also von der direkten DBox-
	Steuerung getrennt. Diese wird dann von Daemons uebernommen.


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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gui/pictureviewer.h>

#include <global.h>
#include <neutrino.h>

#include <daemonc/remotecontrol.h>

#include <driver/fontrenderer.h>
#include <driver/rcinput.h>

#include <gui/nfs.h>

#include <gui/widget/buttons.h>
#include <gui/widget/icons.h>
#include <gui/widget/menue.h>
#include <gui/widget/messagebox.h>

// remove this
#include <gui/widget/hintbox.h>

#include <gui/widget/helpbox.h>
#include <gui/widget/stringinput.h>

#include <system/settings.h>
#include <system/debug.h>

#include <algorithm>
#include <sys/stat.h>
#include <sys/time.h>

#include <gui/webtv.h>


extern CPictureViewer * g_PicViewer;
extern CWebTV * webtv;

bool comparePictureByDate (const CPicture& a, const CPicture& b)
{
	return a.Date < b.Date ;
}

extern bool comparetolower(const char a, const char b); /* filebrowser.cpp */

bool comparePictureByFilename (const CPicture& a, const CPicture& b)
{
	return std::lexicographical_compare(a.Filename.begin(), a.Filename.end(), b.Filename.begin(), b.Filename.end(), comparetolower);
}

CPictureViewerGui::CPictureViewerGui()
{
	frameBuffer = CFrameBuffer::getInstance();

	visible = false;
	selected = 0;
	m_sort = FILENAME;
	isURL = false;

	if(strlen(g_settings.network_nfs_picturedir) != 0)
		Path = g_settings.network_nfs_picturedir;
	else
		Path = "/";

	picture_filter.addFilter("png");
	picture_filter.addFilter("bmp");
	picture_filter.addFilter("jpg");
	picture_filter.addFilter("jpeg");
}

CPictureViewerGui::~CPictureViewerGui()
{
	playlist.clear();
}

int CPictureViewerGui::exec(CMenuTarget* parent, const std::string &actionKey)
{
	selected = 0;
	
	width = frameBuffer->getScreenWidth(true) - 10; 
	height = frameBuffer->getScreenHeight(true) - 10;

	if((g_settings.screen_EndX- g_settings.screen_StartX) < width)
		width = (g_settings.screen_EndX- g_settings.screen_StartX);
	if((g_settings.screen_EndY- g_settings.screen_StartY) < height)
		height = (g_settings.screen_EndY- g_settings.screen_StartY);

	sheight      = g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL]->getHeight();
	
	frameBuffer->getIconSize(NEUTRINO_ICON_BUTTON_OKAY, &icon_foot_w, &icon_foot_h);
	buttonHeight = std::max(g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL]->getHeight(), icon_foot_h) + 6;
	
	// head
	frameBuffer->getIconSize(NEUTRINO_ICON_PICTURE, &icon_head_w, &icon_head_h);
	theight = std::max(g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE]->getHeight(), icon_head_h) + 6;
	
	// item
	fheight = g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getHeight();
	listmaxshow  = (height - theight - 2*buttonHeight)/(fheight);
	
	// recalculate height
	height = theight + 2*buttonHeight + listmaxshow*fheight;	// recalc height

	x = (((g_settings.screen_EndX- g_settings.screen_StartX)-width) / 2) + g_settings.screen_StartX;
	y = (((g_settings.screen_EndY- g_settings.screen_StartY)-height)/ 2) + g_settings.screen_StartY;

	g_PicViewer->SetScaling( (CFrameBuffer::ScalingMode)g_settings.picviewer_scaling);
	g_PicViewer->SetVisible(g_settings.screen_StartX, g_settings.screen_EndX, g_settings.screen_StartY, g_settings.screen_EndY);

	if(g_settings.video_Ratio == 1)
		g_PicViewer->SetAspectRatio(16.0/9);
	else
		g_PicViewer->SetAspectRatio(4.0/3);
	
	if(actionKey == "urlplayback")
	{
		isURL = true;
	}

	if(parent)
		parent->hide();

	// tell neutrino we're in pic_mode
	CNeutrinoApp::getInstance()->handleMsg( NeutrinoMessages::CHANGEMODE , NeutrinoMessages::mode_pic );
	
	// remember last mode
	m_LastMode=(CNeutrinoApp::getInstance()->getLastMode() | NeutrinoMessages::norezap);
	
	// Save and Clear background
	bool usedBackground = frameBuffer->getuseBackground();
	if (usedBackground) 
	{
		frameBuffer->saveBackgroundImage();
		frameBuffer->ClearFrameBuffer();
		frameBuffer->blit();
	}
	
	CNeutrinoApp::getInstance()->StopSubtitles();
	
	//
	if(CNeutrinoApp::getInstance()->getLastMode() == NeutrinoMessages::mode_iptv)
	{
		if(webtv)
			webtv->stopPlayBack();
	}
	else
	{
		// stop playback
		g_Zapit->lockPlayBack();
			
		// Stop Sectionsd
		g_Sectionsd->setPauseScanning(true);
	}

	show();

	// free picviewer mem
	g_PicViewer->Cleanup();

	// Restore previous background	
	if (usedBackground) 
	{
		frameBuffer->restoreBackgroundImage();
		frameBuffer->useBackground(true);
		frameBuffer->paintBackground();
		frameBuffer->blit();	
	}
	
	//
	if(CNeutrinoApp::getInstance()->getLastMode() == NeutrinoMessages::mode_iptv)
	{
		if(webtv)
			webtv->startPlayBack(webtv->getTunedChannel());
	}
	else
	{
		// start playback
		g_Zapit->unlockPlayBack();

		// Start Sectionsd
		g_Sectionsd->setPauseScanning(false);
	}
	
	CNeutrinoApp::getInstance()->StartSubtitles();

	// Restore last mode
	CNeutrinoApp::getInstance()->handleMsg( NeutrinoMessages::CHANGEMODE , m_LastMode );

	// always exit all	
	return menu_return::RETURN_REPAINT;
}

int CPictureViewerGui::show()
{
	neutrino_msg_t      msg;
	neutrino_msg_data_t data;

	int res = -1;

	CVFD::getInstance()->setMode(CVFD::MODE_PIC, g_Locale->getText(LOCALE_PICTUREVIEWER_HEAD));

	m_state = MENU;

	int timeout;

	bool loop = true;
	bool update = true;
	
	if(isURL)
	{
		visible = false;
		if(playlist.size() > 1)
			m_state = SLIDESHOW;
		else
			m_state = VIEW;
		
		if (!playlist.empty())
			view(selected);
	}
	
	while(loop)
	{
		if(update && !isURL)
		{
			hide();
			update = false;
			paint();
		}
		
		if(m_state != SLIDESHOW)
			timeout = 10; 
		else
		{
			timeout = (m_time+atoi(g_settings.picviewer_slide_time) - (long)time(NULL))*10;
			if(timeout < 0 )
				timeout = 1;
		}

		g_RCInput->getMsg(&msg, &data, timeout);

		if( msg == CRCInput::RC_home)
		{
			if(isURL)
			{
				visible = true;
				loop = false;
			}
			
			//Exit after cancel key
			if(m_state != MENU)
			{
				endView();
				update = true;
			}
			else
				loop = false;
		}
		else if (msg == CRCInput::RC_timeout)
		{
			if(m_state == SLIDESHOW)
			{
				m_time = (long)time(NULL);
				unsigned int next = selected + 1;
				if (next >= playlist.size())
					next = 0;
				
				view(next);
			}
		}
		else if (msg == CRCInput::RC_left)
		{
			if (!playlist.empty())
			{
				if (m_state == MENU)
				{
					if (selected < listmaxshow)
						selected=playlist.size()-1;
					else
						selected -= listmaxshow;
					liststart = (selected/listmaxshow)*listmaxshow;
					paint();
				}
				else
				{
					view((selected == 0) ? (playlist.size() - 1) : (selected - 1));
				}
			}
		}
		else if (msg == CRCInput::RC_right)
		{
			if (!playlist.empty())
			{
				if (m_state == MENU)
				{
					selected += listmaxshow;
					if (selected >= playlist.size()) 
					{
						if (((playlist.size() / listmaxshow) + 1) * listmaxshow == playlist.size() + listmaxshow)
							selected=0;
						else
							selected = selected < (((playlist.size() / listmaxshow) + 1) * listmaxshow) ? (playlist.size() - 1) : 0;
					}
					liststart = (selected/listmaxshow)*listmaxshow;
					paint();
				}
				else
				{
					unsigned int next = selected + 1;
					if (next >= playlist.size())
						next = 0;
					view(next);
				}
			}
		}
		else if (msg == CRCInput::RC_up)
		{
			if ((m_state == MENU) && (!playlist.empty()))
			{
				int prevselected=selected;
				if(selected == 0)
				{
					selected = playlist.size()-1;
				}
				else
					selected--;
				paintItem(prevselected - liststart);
				unsigned int oldliststart = liststart;
				liststart = (selected/listmaxshow)*listmaxshow;
				if(oldliststart!=liststart)
				{
					update = true;
				}
				else
				{
					paintItem(selected - liststart);
				}
			}
		}
		else if (msg == CRCInput::RC_down)
		{
			if ((m_state == MENU) && (!playlist.empty()))
			{
				int prevselected=selected;
				selected = (selected+1)%playlist.size();
				paintItem(prevselected - liststart);
				unsigned int oldliststart = liststart;
				liststart = (selected/listmaxshow)*listmaxshow;
				if(oldliststart!=liststart)
				{
					update=true;
				}
				else
				{
					paintItem(selected - liststart);
				}
			}
		}
		else if (msg == CRCInput::RC_ok)
		{
			if (!playlist.empty())
				view(selected);
		}
		else if (msg == CRCInput::RC_red)
		{
			if (m_state == MENU)
			{
				if (!playlist.empty())
				{
					CViewList::iterator p = playlist.begin()+selected;
					playlist.erase(p);
					if (selected >= playlist.size())
						selected = playlist.size()-1;
					update = true;
				}
			}
		}
		else if(msg == CRCInput::RC_green)
		{
			if (m_state == MENU)
			{
				CFileBrowser filebrowser((g_settings.filebrowser_denydirectoryleave) ? g_settings.network_nfs_picturedir : "");

				filebrowser.Multi_Select    = true;
				filebrowser.Dirs_Selectable = true;
				filebrowser.Filter          = &picture_filter;

				hide();

				if (filebrowser.exec(Path.c_str()))
				{
					Path = filebrowser.getCurrentDir();
					CFileList::const_iterator files = filebrowser.getSelectedFiles().begin();
					for(; files != filebrowser.getSelectedFiles().end();files++)
					{
						if(files->getType() == CFile::FILE_PICTURE)
						{
							CPicture pic;
							pic.Filename = files->Name;
							std::string tmp = files->Name.substr(files->Name.rfind('/') + 1);
							pic.Name = tmp.substr(0, tmp.rfind('.'));
							pic.Type = tmp.substr(tmp.rfind('.') + 1);
							struct stat statbuf;
							if(stat(pic.Filename.c_str(),&statbuf) != 0)
								printf("stat error");
							pic.Date = statbuf.st_mtime;
							
							addToPlaylist(pic);
						}
					}
				}
				update = true;
			}
		}
		else if(msg == CRCInput::RC_yellow)
		{
			if (m_state == MENU && !playlist.empty())
			{
				playlist.clear();
				selected = 0;
				update = true;
			}
		}
		else if(msg == CRCInput::RC_blue)
		{
			if ((m_state == MENU) && (!playlist.empty()))
			{
				m_time = (long)time(NULL);
				view(selected);
				m_state = SLIDESHOW;
			} 
			else 
			{
				if(m_state == SLIDESHOW)
					m_state = VIEW;
				else
					m_state = SLIDESHOW;
			}
		}
		else if(msg == CRCInput::RC_info )
		{
			if (m_state == MENU)
			{
				showHelp();
				paint();
			}
		}
		else if( msg == CRCInput::RC_1 )
		{ 
			if (m_state != MENU)
			{
				g_PicViewer->Zoom(2.0/3);
			}

		}
		else if( msg == CRCInput::RC_2 )
		{ 
			if (m_state != MENU)
			{
				g_PicViewer->Move(0, -50);
			}
		}
		else if( msg == CRCInput::RC_3 )
		{ 
			if (m_state != MENU)
			{
				g_PicViewer->Zoom(1.5);
			}

		}
		else if( msg == CRCInput::RC_4 )
		{ 
			if (m_state != MENU)
			{
				g_PicViewer->Move(-50, 0);
			}
		}
		else if ( msg == CRCInput::RC_5 )
		{
			if(m_state == MENU) 
			{
				if (!playlist.empty())
				{
					if(m_sort == FILENAME)
					{
						m_sort = DATE;
						std::sort(playlist.begin(),playlist.end(),comparePictureByDate);
					}
					else if(m_sort == DATE)
					{
						m_sort = FILENAME;
						std::sort(playlist.begin(),playlist.end(),comparePictureByFilename);
					}
					update=true;
				}
			}
		}
		else if( msg == CRCInput::RC_6 )
		{ 
			if (m_state != MENU && playlist.empty())
			{
				g_PicViewer->Move(50, 0);
			}
		}
		else if( msg == CRCInput::RC_8 )
		{ 
			if (m_state != MENU && playlist.empty())
			{
				g_PicViewer->Move(0, 50);
			}
		}
		else if(msg == CRCInput::RC_0)
		{
			if (!playlist.empty())
				view(selected, true);
		}
		else if(msg == CRCInput::RC_setup)
		{
			if(m_state == MENU)
			{
				CNFSSmallMenu nfsMenu;
				nfsMenu.exec(this, "");
				update = true;

				CVFD::getInstance()->setMode(CVFD::MODE_MENU_UTF8, g_Locale->getText(LOCALE_PICTUREVIEWER_HEAD));				
			}
		}
		else if(msg == NeutrinoMessages::CHANGEMODE)
		{
			if((data & NeutrinoMessages::mode_mask) !=NeutrinoMessages::mode_pic)
			{
				loop = false;
				m_LastMode=data;
			}
		}
		else if(msg == NeutrinoMessages::RECORD_START ||
				  msg == NeutrinoMessages::ZAPTO ||
				  msg == NeutrinoMessages::STANDBY_ON ||
				  msg == NeutrinoMessages::SHUTDOWN ||
				  msg == NeutrinoMessages::SLEEPTIMER)
		{
			// Exit for Record/Zapto Timers
			if (m_state != MENU)
				endView();
	
			loop = false;
			g_RCInput->postMsg(msg, data);
		}
		else
		{
			if( CNeutrinoApp::getInstance()->handleMsg( msg, data ) & messages_return::cancel_all )
			{
				loop = false;
			}
		}

		frameBuffer->blit();	
	}

	hide();

	return(res);
}

void CPictureViewerGui::hide()
{
	if(visible) 
	{
		frameBuffer->paintBackground();
		frameBuffer->blit();

		visible = false;
	}
}

void CPictureViewerGui::paintItem(int pos)
{
	//printf("paintItem{\n");
	
	int ypos = y + theight + pos*fheight;

	uint8_t    color;
	fb_pixel_t bgcolor;

	if ((liststart+pos < playlist.size()) && (pos & 1) )
	{
		color   = COL_MENUCONTENTDARK;
		bgcolor = COL_MENUCONTENTDARK_PLUS_0;
	}
	else
	{
		color	= COL_MENUCONTENT;
		bgcolor = COL_MENUCONTENT_PLUS_0;
	}

	if (liststart+pos == selected)
	{
		frameBuffer->paintBoxRel(x,ypos, width-15, fheight, bgcolor);
		color   = COL_MENUCONTENTSELECTED;
		bgcolor = COL_MENUCONTENTSELECTED_PLUS_0;
	}

	frameBuffer->paintBoxRel(x,ypos, width-15, fheight, bgcolor);

	if(liststart + pos < playlist.size())
	{
		std::string tmp = playlist[liststart+pos].Name;
		tmp += " (";
		tmp += playlist[liststart+pos].Type;
		tmp += ')';
		char timestring[18];
		strftime(timestring, 18, "%d-%m-%Y %H:%M", gmtime(&playlist[liststart+pos].Date));
		int w = g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getRenderWidth(timestring);
		
		g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->RenderString(x+10,ypos+fheight, width-30 - w, tmp, color, fheight, true);
		g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->RenderString(x+width-20-w,ypos+fheight, w, timestring, color, fheight);

	}

	//printf("paintItem}\n");
}

// paint head
void CPictureViewerGui::paintHead()
{
	//printf("paintHead{\n");
	
	std::string strCaption = g_Locale->getText(LOCALE_PICTUREVIEWER_HEAD);
	
	//head box
	frameBuffer->paintBoxRel(x, y, width, theight, COL_MENUHEAD_PLUS_0, RADIUS_MID, CORNER_TOP);
	
	// head icon
	//frameBuffer->getIconSize(NEUTRINO_ICON_PICTURE, &icon_head_w, &icon_head_h);
	frameBuffer->paintIcon(NEUTRINO_ICON_PICTURE, x + BORDER_LEFT, y + (theight - icon_head_h)/2);
	
	//head title
	g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE]->RenderString(x + BORDER_LEFT + icon_head_w + 5, y + theight, width - (BORDER_LEFT + BORDER_RIGHT + icon_head_w + 10) , strCaption, COL_MENUHEAD, 0, true); // UTF-8
	
	// icon help
	int icon_w, icon_h;
	frameBuffer->getIconSize(NEUTRINO_ICON_BUTTON_HELP, &icon_w, &icon_h);
	frameBuffer->paintIcon(NEUTRINO_ICON_BUTTON_HELP, x + width - BORDER_RIGHT - 2*icon_w - 5, y + (theight - icon_h)/2);
	
	// icon setup
	frameBuffer->getIconSize(NEUTRINO_ICON_BUTTON_HELP, &icon_w, &icon_h);
	frameBuffer->paintIcon(NEUTRINO_ICON_BUTTON_SETUP, x + width - BORDER_RIGHT - icon_w, y + (theight - icon_h)/2);
}

const struct button_label PictureViewerButtons[4] =
{
	{ NEUTRINO_ICON_BUTTON_RED   , LOCALE_AUDIOPLAYER_DELETE        },
	{ NEUTRINO_ICON_BUTTON_GREEN , LOCALE_AUDIOPLAYER_ADD           },
	{ NEUTRINO_ICON_BUTTON_YELLOW, LOCALE_AUDIOPLAYER_DELETEALL     },
	{ NEUTRINO_ICON_BUTTON_BLUE  , LOCALE_PICTUREVIEWER_SLIDESHOW 	}
};

void CPictureViewerGui::paintFoot()
{
	int ButtonWidth = (width - 20) / 4;
	int ButtonWidth2 = (width - 50) / 2;
	
	int icon_w, icon_h;
	frameBuffer->getIconSize(NEUTRINO_ICON_BUTTON_RED, &icon_w, &icon_h);
	
	frameBuffer->paintBoxRel(x, y + height - 2*buttonHeight, width, 2*buttonHeight, COL_MENUHEAD_PLUS_0, RADIUS_MID, CORNER_BOTTOM);

	if (!playlist.empty())
	{
		//OK
		frameBuffer->paintIcon(NEUTRINO_ICON_BUTTON_OKAY, x + ButtonWidth2 + 25, y + (height - 2*buttonHeight) + buttonHeight + (buttonHeight - icon_foot_h)/2);
		g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL]->RenderString(x + ButtonWidth2 + 53 , y + (height - 2*buttonHeight) + buttonHeight + (buttonHeight - g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL]->getHeight())/2 + g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL]->getHeight(), ButtonWidth2 - 28, g_Locale->getText(LOCALE_PICTUREVIEWER_SHOW), COL_INFOBAR, 0, true); // UTF-8

		// 5
		frameBuffer->paintIcon(NEUTRINO_ICON_BUTTON_5, x + BORDER_LEFT, y + (height - 2*buttonHeight) + buttonHeight + (buttonHeight - icon_foot_h)/2);
		std::string tmp = g_Locale->getText(LOCALE_PICTUREVIEWER_SORTORDER);
		tmp += ' ';
		if(m_sort == FILENAME)
			tmp += g_Locale->getText(LOCALE_PICTUREVIEWER_SORTORDER_DATE);
		else if(m_sort == DATE)
			tmp += g_Locale->getText(LOCALE_PICTUREVIEWER_SORTORDER_FILENAME);
		
		g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL]->RenderString(x + BORDER_LEFT + icon_w + 5 , y + (height - 2*buttonHeight) + buttonHeight + (buttonHeight - g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL]->getHeight())/2 + g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL]->getHeight(), ButtonWidth2 - 28, tmp, COL_INFOBAR, 0, true); // UTF-8

		// foot buttons
		::paintButtons(frameBuffer, g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL], g_Locale, x + BORDER_LEFT, y + height - 2*buttonHeight, ButtonWidth, 4, PictureViewerButtons, buttonHeight);
	}
	else
		::paintButtons(frameBuffer, g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL], g_Locale, x + BORDER_LEFT + ButtonWidth, y + height - 2*buttonHeight, ButtonWidth, 1, &(PictureViewerButtons[1]), buttonHeight);
}

void CPictureViewerGui::paintInfo()
{
}

void CPictureViewerGui::paint()
{
	liststart = (selected/listmaxshow)*listmaxshow;

	paintHead();
	
	for(unsigned int count = 0; count < listmaxshow; count++)
	{
		paintItem(count);
	}

	int ypos = y + theight;
	int sb = fheight* listmaxshow;
	frameBuffer->paintBoxRel(x + width - 15, ypos, 15, sb,  COL_MENUCONTENT_PLUS_1);

	int sbc = ((playlist.size()- 1)/ listmaxshow)+ 1;
	float sbh = (sb- 4)/ sbc;
	int sbs = (selected/listmaxshow);

	frameBuffer->paintBoxRel(x + width - 13, ypos + 2 + int(sbs* sbh) , 11, int(sbh),  COL_MENUCONTENT_PLUS_3);

	paintFoot();
	paintInfo();
	
	frameBuffer->blit();
	
	visible = true;
}

void CPictureViewerGui::view(unsigned int index, bool unscaled)
{
	selected = index;
	
	if (CVFD::getInstance()->is4digits)
		CVFD::getInstance()->LCDshowText(selected + 1);
	else
		CVFD::getInstance()->showMenuText(0, playlist[index].Name.c_str());
	
	char timestring[19];
	strftime(timestring, 18, "%d-%m-%Y %H:%M", gmtime(&playlist[index].Date));
	
	if(unscaled)
		g_PicViewer->DecodeImage(playlist[index].Filename, true, unscaled);
	
	g_PicViewer->ShowImage(playlist[index].Filename, unscaled);

	//Decode next
	unsigned int next = selected + 1;
	if( next > playlist.size() - 1 )
		next = 0;
	
	if(m_state == MENU)
		m_state = VIEW;
	
	if(m_state == VIEW )
		g_PicViewer->DecodeImage(playlist[next].Filename, true);
	else
		g_PicViewer->DecodeImage(playlist[next].Filename, false);
}

void CPictureViewerGui::endView()
{
	if(m_state != MENU)
		m_state = MENU;
}

void CPictureViewerGui::addToPlaylist(CPicture& file)
{	
	dprintf(DEBUG_NORMAL, "CPictureViewerGui::addToPlaylist: %s\n", file.Filename.c_str());
	
	playlist.push_back(file);
	
	if (m_sort == FILENAME)
		std::sort(playlist.begin(), playlist.end(), comparePictureByFilename);
	else if (m_sort == DATE)
		std::sort(playlist.begin(), playlist.end(), comparePictureByDate);
}

void CPictureViewerGui::showHelp()
{
	Helpbox helpbox;
	helpbox.addLine(g_Locale->getText(LOCALE_PICTUREVIEWER_HELP1));
	helpbox.addLine(NEUTRINO_ICON_BUTTON_OKAY, g_Locale->getText(LOCALE_PICTUREVIEWER_HELP2));
	helpbox.addLine(NEUTRINO_ICON_BUTTON_0, g_Locale->getText(LOCALE_PICTUREVIEWER_HELP4));
	helpbox.addLine(NEUTRINO_ICON_BUTTON_5, g_Locale->getText(LOCALE_PICTUREVIEWER_HELP3));
	helpbox.addPagebreak();
	helpbox.addLine(g_Locale->getText(LOCALE_PICTUREVIEWER_HELP5));
	helpbox.addLine(NEUTRINO_ICON_BUTTON_LEFT, g_Locale->getText(LOCALE_PICTUREVIEWER_HELP6));
	helpbox.addLine(NEUTRINO_ICON_BUTTON_RIGHT, g_Locale->getText(LOCALE_PICTUREVIEWER_HELP7));
	helpbox.addLine(NEUTRINO_ICON_BUTTON_5, g_Locale->getText(LOCALE_PICTUREVIEWER_HELP8));
	helpbox.addLine(NEUTRINO_ICON_BUTTON_HOME, g_Locale->getText(LOCALE_PICTUREVIEWER_HELP9));
	helpbox.addPagebreak();
	helpbox.addLine(g_Locale->getText(LOCALE_PICTUREVIEWER_HELP10));
	helpbox.addLine(NEUTRINO_ICON_BUTTON_OKAY, g_Locale->getText(LOCALE_PICTUREVIEWER_HELP11));
	helpbox.addLine(NEUTRINO_ICON_BUTTON_LEFT, g_Locale->getText(LOCALE_PICTUREVIEWER_HELP12));
	helpbox.addLine(NEUTRINO_ICON_BUTTON_RIGHT, g_Locale->getText(LOCALE_PICTUREVIEWER_HELP13));
	helpbox.addPagebreak();
	helpbox.addLine(NEUTRINO_ICON_BUTTON_0, g_Locale->getText(LOCALE_PICTUREVIEWER_HELP21));
	helpbox.addLine(NEUTRINO_ICON_BUTTON_1, g_Locale->getText(LOCALE_PICTUREVIEWER_HELP14));
	helpbox.addLine(NEUTRINO_ICON_BUTTON_2, g_Locale->getText(LOCALE_PICTUREVIEWER_HELP16));
	helpbox.addLine(NEUTRINO_ICON_BUTTON_3, g_Locale->getText(LOCALE_PICTUREVIEWER_HELP15));
	//helpbox.addPagebreak();
	helpbox.addLine(NEUTRINO_ICON_BUTTON_4, g_Locale->getText(LOCALE_PICTUREVIEWER_HELP17));
	helpbox.addLine(NEUTRINO_ICON_BUTTON_5, g_Locale->getText(LOCALE_PICTUREVIEWER_HELP20));
	helpbox.addLine(NEUTRINO_ICON_BUTTON_6, g_Locale->getText(LOCALE_PICTUREVIEWER_HELP18));
	helpbox.addLine(NEUTRINO_ICON_BUTTON_8, g_Locale->getText(LOCALE_PICTUREVIEWER_HELP19));
	helpbox.addLine(NEUTRINO_ICON_BUTTON_HOME, g_Locale->getText(LOCALE_PICTUREVIEWER_HELP22));
	helpbox.addPagebreak();

	helpbox.addLine("Version: $Revision: 1.57 $");
	hide();
	helpbox.show(LOCALE_MESSAGEBOX_INFO);
}
