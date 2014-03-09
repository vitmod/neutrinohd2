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

#include <mediaplayer.h>


extern "C" int plugin_exec(void);

// 
CMediaPlayer::CMediaPlayer()
{
	speed = 1;
	//slow = 0;
	selected = 0;
	ac3state = CInfoViewer::NO_AC3;
	playstate = STOPPED;
	
	position = 0;
	duration = 0;
	file_prozent = 0;
	
	// global flags
	update_lcd = false;
	open_filebrowser = true;	//always default true (true valeue is needed for file/moviebrowser)
	start_play = false;
	exit = false;
	was_file = false;
	m_loop = false;
	
	filelist.clear();
}

CMediaPlayer::~CMediaPlayer()
{
	if(!filelist.empty())
		filelist.clear();
}

int CMediaPlayer::exec(CMenuTarget *parent, const std::string & /*actionKey*/)
{
	if(parent)
		parent->hide();
	
	return menu_return::RETURN_REPAINT;
}

extern CVideoSetupNotifier * videoSetupNotifier;	/* defined neutrino.cpp */
// aspect ratio
#if defined (__sh__)
#define VIDEOMENU_VIDEORATIO_OPTION_COUNT 2
const CMenuOptionChooser::keyval VIDEOMENU_VIDEORATIO_OPTIONS[VIDEOMENU_VIDEORATIO_OPTION_COUNT] =
{
	{ ASPECTRATIO_43, LOCALE_VIDEOMENU_VIDEORATIO_43, NULL },
	{ ASPECTRATIO_169, LOCALE_VIDEOMENU_VIDEORATIO_169, NULL }
};
#else
#define VIDEOMENU_VIDEORATIO_OPTION_COUNT 3
const CMenuOptionChooser::keyval VIDEOMENU_VIDEORATIO_OPTIONS[VIDEOMENU_VIDEORATIO_OPTION_COUNT] =
{
	{ ASPECTRATIO_43, LOCALE_VIDEOMENU_VIDEORATIO_43, NULL },
	{ ASPECTRATIO_169, LOCALE_VIDEOMENU_VIDEORATIO_169, NULL },
	{ ASPECTRATIO_AUTO, NONEXISTANT_LOCALE, "Auto" }
};
#endif

// policy
#if defined (__sh__)
/*
letterbox 
panscan 
non 
bestfit
*/
#define VIDEOMENU_VIDEOFORMAT_OPTION_COUNT 4
const CMenuOptionChooser::keyval VIDEOMENU_VIDEOFORMAT_OPTIONS[VIDEOMENU_VIDEOFORMAT_OPTION_COUNT] = 
{
	{ VIDEOFORMAT_LETTERBOX, LOCALE_VIDEOMENU_LETTERBOX, NULL },
	{ VIDEOFORMAT_PANSCAN, LOCALE_VIDEOMENU_PANSCAN, NULL },
	{ VIDEOFORMAT_FULLSCREEN, LOCALE_VIDEOMENU_FULLSCREEN, NULL },
	{ VIDEOFORMAT_PANSCAN2, LOCALE_VIDEOMENU_PANSCAN2, NULL }
};
#else
// giga/generic
/*
letterbox 
panscan 
bestfit 
nonlinear
*/
#define VIDEOMENU_VIDEOFORMAT_OPTION_COUNT 4
const CMenuOptionChooser::keyval VIDEOMENU_VIDEOFORMAT_OPTIONS[VIDEOMENU_VIDEOFORMAT_OPTION_COUNT] = 
{
	{ VIDEOFORMAT_LETTERBOX, LOCALE_VIDEOMENU_LETTERBOX, NULL },
	{ VIDEOFORMAT_PANSCAN, LOCALE_VIDEOMENU_PANSCAN, NULL },
	{ VIDEOFORMAT_PANSCAN2, LOCALE_VIDEOMENU_PANSCAN2, NULL },
	{ VIDEOFORMAT_FULLSCREEN, LOCALE_VIDEOMENU_FULLSCREEN, NULL }
};
#endif

// ac3
extern CAudioSetupNotifier * audioSetupNotifier;	/* defined neutrino.cpp */

#if !defined (PLATFORM_COOLSTREAM)
#define AC3_OPTION_COUNT 2
const CMenuOptionChooser::keyval AC3_OPTIONS[AC3_OPTION_COUNT] =
{
	{ AC3_PASSTHROUGH, NONEXISTANT_LOCALE, "passthrough" },
	{ AC3_DOWNMIX, NONEXISTANT_LOCALE, "downmix" }
};
#endif

int CMediaPlayerAPIDSelectExec::exec(CMenuTarget */*parent*/, const std::string & actionKey)
{
	apidchanged = 0;
	unsigned int sel = atoi(actionKey.c_str());

	if (currentapid != apids[sel - 1]) 
	{
		currentapid = apids[sel - 1];
		currentac3 = ac3flags[sel - 1];
		apidchanged = 1;
		
		dprintf(DEBUG_NORMAL, "[fileplayer] apid changed to %d\n", apids[sel - 1]);
	}

	return menu_return::RETURN_EXIT;
}

void CMediaPlayer::showAudioDialog(void)
{
	CMenuWidget APIDSelector("Audio select", NEUTRINO_ICON_AUDIO);

	// g_apids will be rewritten for mb
	playback->FindAllPids(apids, ac3flags, &numpida, language);
			
	if (numpida > 0) 
	{
		CMediaPlayerAPIDSelectExec * APIDChanger = new CMediaPlayerAPIDSelectExec;
		bool enabled;
		bool defpid;

		for (unsigned int count = 0; count < numpida; count++) 
		{
			bool name_ok = false;
			char apidnumber[10];
			sprintf(apidnumber, "%d %X", count + 1, apids[count]);
			enabled = true;
			defpid = currentapid ? (currentapid == apids[count]) : (count == 0);
			std::string apidtitle = "Stream ";

			// language
			if (!language[count].empty())
			{
				apidtitle = language[count];
				name_ok = true;
			}

			if (!name_ok)
			{
				apidtitle = "Stream ";
				name_ok = true;
			}

			switch(ac3flags[count])
			{
				case 1: /*AC3,EAC3*/
					if (apidtitle.find("AC3") <= 0)
					{
						apidtitle.append(" (AC3)");
								
						// ac3 state
						ac3state = CInfoViewer::AC3_AVAILABLE;
					}
					break;

				case 2: /*teletext*/
					apidtitle.append(" (Teletext)");
					enabled = false;
					break;

				case 3: /*MP2*/
					apidtitle.append(" (MP2)");
					break;

				case 4: /*MP3*/
					apidtitle.append(" (MP3)");
					break;

				case 5: /*AAC*/
					apidtitle.append(" (AAC)");
					break;

				case 6: /*DTS*/
					apidtitle.append(" (DTS)");
					break;

				case 7: /*MLP*/
					apidtitle.append(" (MLP)");
					break;

				default:
					break;
			}

			if (!name_ok)
				apidtitle.append(apidnumber);

			APIDSelector.addItem(new CMenuForwarderNonLocalized( apidtitle.c_str(), enabled, NULL, APIDChanger, apidnumber, CRCInput::convertDigitToKey(count + 1)), defpid);
		}
				
		// ac3				
		APIDSelector.addItem(GenericMenuSeparatorLine);
		APIDSelector.addItem(new CMenuOptionChooser("Dolby Digital", &g_settings.hdmi_dd, AC3_OPTIONS, AC3_OPTION_COUNT, true, audioSetupNotifier, CRCInput::RC_red, NEUTRINO_ICON_BUTTON_RED ));	
				
		// policy/aspect ratio
		APIDSelector.addItem(GenericMenuSeparatorLine);
				
		// video aspect ratio 4:3/16:9
		APIDSelector.addItem(new CMenuOptionChooser("video ratio", &g_settings.video_Ratio, VIDEOMENU_VIDEORATIO_OPTIONS, VIDEOMENU_VIDEORATIO_OPTION_COUNT, true, videoSetupNotifier, CRCInput::RC_green, NEUTRINO_ICON_BUTTON_GREEN, true ));
	
		// video format bestfit/letterbox/panscan/non
		APIDSelector.addItem(new CMenuOptionChooser("video format", &g_settings.video_Format, VIDEOMENU_VIDEOFORMAT_OPTIONS, VIDEOMENU_VIDEOFORMAT_OPTION_COUNT, true, videoSetupNotifier, CRCInput::RC_yellow, NEUTRINO_ICON_BUTTON_YELLOW, true ));

		apidchanged = 0;
		APIDSelector.exec(NULL, "");

		if (apidchanged) 
		{
			if (currentapid == 0) 
			{
				currentapid = apids[0];
				currentac3 = ac3flags[0];

				if(currentac3)
					ac3state = CInfoViewer::AC3_ACTIVE;
			}
					
			playback->SetAPid(currentapid);					
			apidchanged = 0;
		}
				
		delete APIDChanger;
				
		//CVFD::getInstance()->setMode(CVFD::MODE_MENU_UTF8);

	} 
	else 
	{
		DisplayErrorMessage("no audio tracks found!"); // UTF-8
	}
}

void CMediaPlayer::cutNeutrino()
{
	// tell neutrino we are in ts mode
	CNeutrinoApp::getInstance()->handleMsg(NeutrinoMessages::CHANGEMODE, NeutrinoMessages::mode_ts);
	
	// save (remeber) last mode
	m_LastMode = (CNeutrinoApp::getInstance()->getLastMode() | NeutrinoMessages::norezap);
	
	if(CNeutrinoApp::getInstance()->getLastMode() == NeutrinoMessages::mode_iptv)
	{
		if(webtv)
			webtv->stopPlayBack();
	}
	else
	{
		// pause epg scanning
		g_Sectionsd->setPauseScanning(true);
			
		// lock playback
		g_Zapit->lockPlayBack();
	}
}

void CMediaPlayer::restoreNeutrino()
{
	if(CNeutrinoApp::getInstance()->getLastMode() == NeutrinoMessages::mode_iptv)
	{
		if(webtv)
			webtv->startPlayBack(webtv->getTunedChannel());
	}
	else
	{
		// unlock playback
		g_Zapit->unlockPlayBack();
			
		// start epg scanning
		g_Sectionsd->setPauseScanning(false);
	}
}

void CMediaPlayer::playFile(/*const char *file*/)
{	
	cutNeutrino();
			
	playback->Close();
		
	// init player			
	playback->Open();		

	// PlayBack Start			  
	playback->Start((char *)filename.c_str());
		
	playstate = PLAY;
			
	g_InfoViewer->showMovieInfo(filename.c_str(), filename.c_str(), 0, duration, CInfoViewer::NO_AC3, speed, playstate);
			
	bool loop = true;
		
	// playfile
	while(loop)
	{
		//get position/duration/speed during playing
		if ( playstate >= PLAY )
		{
			if( !playback->GetPosition(position, duration) )								
				g_RCInput->postMsg((neutrino_msg_t) g_settings.mpkey_stop, 0);
		}
				  
		neutrino_msg_t      msg;
		neutrino_msg_data_t data;
				
		// loop msg
		g_RCInput->getMsg(&msg, &data, 10);	// 1 secs
				
		if (msg == (neutrino_msg_t) g_settings.mpkey_stop) 
		{
			playstate = STOPPED;
			loop = false;
		}
		else if (msg == (neutrino_msg_t) g_settings.mpkey_play) 
		{
			if (playstate >= PLAY) 
			{
				playstate = PLAY;
						
				speed = 1;
				playback->SetSpeed(speed);
			} 
		}
		else if ( msg == (neutrino_msg_t) g_settings.mpkey_pause) 
		{
			if (playstate == PAUSE) 
			{
				playstate = PLAY;
						
				speed = 1;
				playback->SetSpeed(speed);
			} 
			else 
			{
				playstate = PAUSE;
						
				speed = 0;
				playback->SetSpeed(speed);
			}
		}
		else if (msg == (neutrino_msg_t) g_settings.mpkey_rewind) 
		{
			// backward
			speed = (speed >= 0) ? -1 : speed - 1;
								
			if(speed < -15)
				speed = -15;			

			playback->SetSpeed(speed);
			playstate = REW;
		}
		else if (msg == (neutrino_msg_t) g_settings.mpkey_forward) 
		{	// fast-forward
			speed = (speed <= 0) ? 2 : speed + 1;
								
			if(speed > 15)
				speed = 15;			
			
			playback->SetSpeed(speed);

			playstate = FF;
		}
		else if (msg == CRCInput::RC_info) 
		{	
			if( !g_InfoViewer->m_visible )
				g_InfoViewer->showMovieInfo(filename.c_str(), filename.c_str(), 0, duration, CInfoViewer::NO_AC3, speed, playstate);
		}
		else if( ( msg == CRCInput::RC_green) || ( msg == CRCInput::RC_audio) )
		{
			showAudioDialog();
		}
		else if( msg == CRCInput::RC_red )
		{
			showFileInfo();
		}
		else if( msg == CRCInput::RC_yellow )
		{
			showHelpTS();
		}
		else 
		{
			if (CNeutrinoApp::getInstance()->handleMsg(msg, data) & messages_return::cancel_all)
				loop = false;
		}
			
	}
		
	// stop playing
	playback->Close();
			
	restoreNeutrino();
}

void CMediaPlayer::showFileInfo()
{
	ShowMsg2UTF(filename.c_str(), filename.c_str(), CMsgBox::mbrBack, CMsgBox::mbBack);	// UTF-8*/ 
}

void CMediaPlayer::showHelpTS()
{
	Helpbox helpbox;
	helpbox.addLine(NEUTRINO_ICON_BUTTON_RED, g_Locale->getText(LOCALE_MOVIEPLAYER_TSHELP1));
	helpbox.addLine(NEUTRINO_ICON_BUTTON_GREEN, g_Locale->getText(LOCALE_MOVIEPLAYER_TSHELP2));
	helpbox.addLine(NEUTRINO_ICON_BUTTON_YELLOW, g_Locale->getText(LOCALE_MOVIEPLAYER_TSHELP3));
	helpbox.addLine(NEUTRINO_ICON_BUTTON_BLUE, g_Locale->getText(LOCALE_MOVIEPLAYER_TSHELP4));
	helpbox.addLine(NEUTRINO_ICON_BUTTON_SETUP, g_Locale->getText(LOCALE_MOVIEPLAYER_TSHELP5));
	helpbox.addLine(NEUTRINO_ICON_BUTTON_HELP, g_Locale->getText(LOCALE_MOVIEPLAYER_TSHELP5));
	helpbox.addLine(NEUTRINO_ICON_BUTTON_1, g_Locale->getText(LOCALE_MOVIEPLAYER_TSHELP6));
	helpbox.addLine(NEUTRINO_ICON_BUTTON_2, g_Locale->getText(LOCALE_MOVIEPLAYER_TSHELP12) );
	helpbox.addLine(NEUTRINO_ICON_BUTTON_3, g_Locale->getText(LOCALE_MOVIEPLAYER_TSHELP7));
	helpbox.addLine(NEUTRINO_ICON_BUTTON_4, g_Locale->getText(LOCALE_MOVIEPLAYER_TSHELP8));
	helpbox.addLine(NEUTRINO_ICON_BUTTON_5, g_Locale->getText(LOCALE_MOVIEPLAYER_TSHELP13));
	helpbox.addLine(NEUTRINO_ICON_BUTTON_6, g_Locale->getText(LOCALE_MOVIEPLAYER_TSHELP9));
	helpbox.addLine(NEUTRINO_ICON_BUTTON_7, g_Locale->getText(LOCALE_MOVIEPLAYER_TSHELP10));
	helpbox.addLine(NEUTRINO_ICON_BUTTON_8, g_Locale->getText(LOCALE_MOVIEPLAYER_TSHELP14));
	helpbox.addLine(NEUTRINO_ICON_BUTTON_9, g_Locale->getText(LOCALE_MOVIEPLAYER_TSHELP11));
	helpbox.addLine("Version: $Revision: 0.0.1 $");
	helpbox.addLine("$id: neutrinoHD2 MediaPlayer plugin mohousch 2014");
	hide();
	helpbox.show(LOCALE_MESSAGEBOX_INFO);
}

int plugin_exec(void)
{
	printf("Plugins: starting mediaplayer\n");
	
	// class handler
	CMediaPlayer *MediaPlayer = new CMediaPlayer();
	
	CFileBrowser filebrowser;
	CFileFilter filefilter;
		
	filefilter.addFilter("ts");
	filefilter.addFilter("mpg");
	filefilter.addFilter("mpeg");
	filefilter.addFilter("divx");
	filefilter.addFilter("avi");
	filefilter.addFilter("mkv");
	filefilter.addFilter("asf");
	filefilter.addFilter("aiff");
	filefilter.addFilter("m2p");
	filefilter.addFilter("mpv");
	filefilter.addFilter("m2ts");
	filefilter.addFilter("vob");
	filefilter.addFilter("mp4");
	filefilter.addFilter("mov");	
	filefilter.addFilter("flv");	
	filefilter.addFilter("dat");
	filefilter.addFilter("trp");
	filefilter.addFilter("vdr");
	filefilter.addFilter("mts");
	filefilter.addFilter("wav");
	filefilter.addFilter("flac");
	filefilter.addFilter("mp3");
	filefilter.addFilter("wmv");
	filefilter.addFilter("wma");
	filefilter.addFilter("ogg");
	
	filebrowser.Filter = &filefilter;
	
	if(filebrowser.exec(g_settings.network_nfs_moviedir))
	{
		CFile *file = filebrowser.getSelectedFile();
		MediaPlayer->filelist.clear();
		MediaPlayer->filelist.push_back(*file);
		
		//if(!filebrowser.getSelectedFiles()->Name.empty())
		if(!MediaPlayer->filelist.empty())
		{
			#if 1
			MediaPlayer->filename = MediaPlayer->filelist[MediaPlayer->selected].Name.c_str();
			MediaPlayer->title = std::string(rindex(MediaPlayer->filelist[MediaPlayer->selected].getFileName().c_str(), '/') + 1);
						
			MediaPlayer->info1 = MediaPlayer->title;
			MediaPlayer->info2 = MediaPlayer->title;
						
			MediaPlayer->playFile(/*MediaPlayer->filename.c_str()*/);
			#else
			moviePlayerGui->filename = MediaPlayer->filelist[MediaPlayer->selected].Name.c_str();
			
			moviePlayerGui->exec(NULL, "urlplayback");
			#endif
		}
	}
	
	playback->Close();
	
	delete MediaPlayer;
	
	return 0;
}
