/*
  neutrinoHD2 project
  
  https://code.google.com/p/neutrinohd2/
  
  $Id: test.cpp 2014/01/22 mohousch Exp $

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

class CTestMenu : CMenuTarget
{
	private:
		void testCStringInput();
		void testCStringInputSMS();
		void testCPINInput();
		void testCIPInput();
		void testCDateInput();
		void testCMACInput();
		void testCTimeInput();
		void testCIntInput();
		void testCInfoBox();
		void testCInfoBoxShowMsg();
		void testCInfoBoxInfoBox();
		void testCMessageBox();
		void testCMessageBoxInfoMsg();
		void testCMessageBoxErrorMsg();
		void testCHintBox();
		void testCHintBoxInfo();
		void testCHintBoxExt();
		void testCHelpBox();
		void testCTextBox();
		void testCListFrameBox();
		void testCListBox();
		void testCListBoxDetails();
		void testCListBoxDetailsTitleInfo();
		//
		void testCallAudioPlayer();
		void testCallInternetRadio();
		void testCallTSMovieBrowser();
		void testCallMovieBrowser();
		void testCallFilePlayBack();
		void testCallPictureViewer();
		void testCallUPNPBrowser();
		//
		void testPlayMovieURL();
		void testPlayAudioURL();
		void testShowPictureURL();
		void testPlayAudioFolder();
		void testShowPictureFolder();
		//
		void testStartPlugin();
		//
		void testShowActuellEPG();
	public:
		CTestMenu();
		~CTestMenu();
		int exec(CMenuTarget* parent, const std::string& actionKey);
		void hide();
		void showTestMenu();
};

CTestMenu::CTestMenu()
{
}

CTestMenu::~CTestMenu()
{
}

void CTestMenu::hide()
{
	CFrameBuffer::getInstance()->paintBackground();
	CFrameBuffer::getInstance()->blit();
}

void CTestMenu::testCStringInput()
{
	std::string value;
	CStringInput * stringInput = new CStringInput("CStringInput", (char *)value.c_str());
	
	stringInput->exec(NULL, "");
	stringInput->hide();
	delete stringInput;
	stringInput = NULL;
}
void CTestMenu::testCStringInputSMS()
{
	std::string value;
	CStringInputSMS * stringInputSMS = new CStringInputSMS("CStringInputSMS", (char *)value.c_str());
	
	stringInputSMS->exec(NULL, "");
	stringInputSMS->hide();
	delete stringInputSMS;
}

void CTestMenu::testCPINInput()
{
	std::string value;
	CPINInput * pinInput = new CPINInput("CPINInput", (char *)value.c_str());
	
	pinInput->exec(NULL, "");
	pinInput->hide();
	delete pinInput;
}

void CTestMenu::testCIPInput()
{
	std::string value;
	CIPInput * ipInput = new CIPInput(LOCALE_STREAMINGMENU_SERVER_IP, value);
	
	ipInput->exec(NULL, "");
	ipInput->hide();
	delete ipInput;
}

void CTestMenu::testCMACInput()
{
	std::string value;
	CMACInput * macInput = new CMACInput(LOCALE_RECORDINGMENU_SERVER_MAC, (char *)value.c_str());
	
	macInput->exec(NULL, "");
	macInput->hide();
	delete macInput;
}

void CTestMenu::testCDateInput()
{
	/*
	time_t* value;
	CDateInput * dateInput = new CDateInput(LOCALE_FILEBROWSER_SORT_DATE, value);
	
	dateInput->exec(NULL, "");
	dateInput->hide();
	delete dateInput;
	*/
}

void CTestMenu::testCTimeInput()
{
	std::string value;
	CTimeInput * timeInput = new CTimeInput(LOCALE_FILEBROWSER_SORT_DATE, (char *)value.c_str());
	
	timeInput->exec(NULL, "");
	timeInput->hide();
	delete timeInput;
}

void CTestMenu::testCIntInput()
{
	/*
	int* value;
	CIntInput * intInput = new CIntInput(LOCALE_FILEBROWSER_SORT_DATE, value);
	
	intInput->exec(NULL, "");
	intInput->hide();
	delete intInput;
	*/
}

void CTestMenu::testCInfoBox()
{
	int mode =  CInfoBox::SCROLL | CInfoBox::TITLE | CInfoBox::FOOT | CInfoBox::BORDER;// | //CInfoBox::NO_AUTO_LINEBREAK | //CInfoBox::CENTER | //CInfoBox::AUTO_WIDTH | //CInfoBox::AUTO_HIGH;
	CBox position(g_settings.screen_StartX + 50, g_settings.screen_StartY + 50, g_settings.screen_EndX - g_settings.screen_StartX - 100, g_settings.screen_EndY - g_settings.screen_StartY - 100); 
	
	CInfoBox * infoBox = new CInfoBox("CInfoBox", g_Font[SNeutrinoSettings::FONT_TYPE_MENU], mode, &position, "infobox.cpp", g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE], NULL);
	
	infoBox->exec();
	infoBox->hide();
	delete infoBox;
}

void CTestMenu::testCInfoBoxShowMsg()
{
	InfoBox("InfoBox", "infobox.cpp", CInfoBox::mbrBack, CInfoBox::mbBack);	// UTF-8
}

void CTestMenu::testCInfoBoxInfoBox()
{
	std::string buffer;
	
	// prepare print buffer  
	buffer = "CInfoBox";
	buffer += "\n";
	buffer += "CInfoBoxInfoBox";
	buffer += "\n";

	// thumbnail
	int pich = 246;	//FIXME
	int picw = 162; 	//FIXME
	int lx = g_settings.screen_StartX + 50 + g_settings.screen_EndX - g_settings.screen_StartX - 100 - (picw + 20);
	int ly = g_settings.screen_StartY + 50 + g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getHeight() + 20;
	
	std::string thumbnail = PLUGINDIR "/netzkino/netzkino.png";
	if(access(thumbnail.c_str(), F_OK))
		thumbnail = "";
	
	int mode =  CInfoBox::SCROLL | CInfoBox::TITLE | CInfoBox::FOOT | CInfoBox::BORDER;// | //CInfoBox::NO_AUTO_LINEBREAK | //CInfoBox::CENTER | //CInfoBox::AUTO_WIDTH | //CInfoBox::AUTO_HIGH;
	CBox position(g_settings.screen_StartX + 50, g_settings.screen_StartY + 50, g_settings.screen_EndX - g_settings.screen_StartX - 100, g_settings.screen_EndY - g_settings.screen_StartY - 100); 
	
	CInfoBox * msgBox = new CInfoBox("CInfoBox", g_Font[SNeutrinoSettings::FONT_TYPE_MENU], mode, &position, "CInfoBoxInfoBox", g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE], NEUTRINO_ICON_BUTTON_SETUP, CInfoBox::mbAll, CInfoBox::mbrCancel);
	msgBox->setText(&buffer, thumbnail, lx, ly, picw, pich);
	msgBox->exec();
	delete msgBox;
}

void CTestMenu::testCMessageBox()
{
	CMessageBox * messageBox = new CMessageBox(LOCALE_MESSAGEBOX_ERROR, "CMessagebox.cpp");
	
	messageBox->exec();
	messageBox->hide();
	delete messageBox;
}

void CTestMenu::testCMessageBoxInfoMsg()
{
	MessageBox(LOCALE_MESSAGEBOX_INFO, "InfoMessage", CMessageBox::mbrBack, CMessageBox::mbBack, NEUTRINO_ICON_INFO);
}

void CTestMenu::testCMessageBoxErrorMsg()
{
	MessageBox(LOCALE_MESSAGEBOX_ERROR, "ErrorMessage", CMessageBox::mbrCancel, CMessageBox::mbCancel, NEUTRINO_ICON_ERROR);
}

void CTestMenu::testCHintBox()
{
	CHintBox * hintBox = new CHintBox(LOCALE_MESSAGEBOX_INFO, "HintBox");
	
	hintBox->paint();
	sleep(3);
	hintBox->hide();
	delete hintBox;
}

void CTestMenu::testCHintBoxInfo()
{
	HintBox(LOCALE_MESSAGEBOX_INFO, "HintBox");
}

void CTestMenu::testCHintBoxExt()
{
	CHintBoxExt * hintBoxExt = new CHintBoxExt(LOCALE_MESSAGEBOX_INFO, "HintBoxExt");
	
	hintBoxExt->paint();
	sleep(3);
	hintBoxExt->hide();
	delete hintBoxExt;
}

void CTestMenu::testCHelpBox()
{
	Helpbox * helpBox = new Helpbox();
	
	helpBox->addLine(NEUTRINO_ICON_BUTTON_RED, "helpBox");
	helpBox->addLine("HELPBOX");
	helpBox->addLine("");
	helpBox->addPagebreak();
	helpBox->show(LOCALE_MESSAGEBOX_INFO);
	
	delete helpBox;
}

void CTestMenu::testCTextBox()
{
	CBox Box;
	
	Box.iX = g_settings.screen_StartX + 10;
	Box.iY = g_settings.screen_StartY + 10;
	Box.iWidth = g_settings.screen_EndX - g_settings.screen_StartX - 20;
	Box.iHeight = (g_settings.screen_EndY - g_settings.screen_StartY - 20)/2;
	
	CTextBox * textBox = new CTextBox(" ", NULL, CTextBox::SCROLL, &Box);
	
	std::string text = "neutrinoHD2 is cool :-)";
		
	int pich = 246;	//FIXME
	int picw = 162; 	//FIXME
	int lx = Box.iX + Box.iWidth - (picw + 20);
	int ly = Box.iY + 20;
		
	std::string fname;

	fname = PLUGINDIR "/netzkino/netzkino.png";
		
	if(access(fname.c_str(), F_OK))
		fname = "";
	
	textBox->setText(&text, fname, lx, ly, picw, pich);
	
	textBox->paint();
	
	sleep(3);
	
	delete textBox;
	textBox = NULL;
}

void CTestMenu::testCListFrameBox()
{
	CBox Box1;
	
	Box1.iX = g_settings.screen_StartX + 10;
	Box1.iY = g_settings.screen_StartY + 10;
	Box1.iWidth = (g_settings.screen_EndX - g_settings.screen_StartX - 20)/4;
	Box1.iHeight = (g_settings.screen_EndY - g_settings.screen_StartY - 20)/4;
	
	LF_LINES listFrameLines;
	CListFrame * listFrame1 = new CListFrame(&listFrameLines, NULL, CListFrame::TITLE | CListFrame::HEADER_LINE, &Box1);
	
	CBox Box2;
	
	Box2.iX = Box1.iX + Box1.iWidth + 10;
	Box2.iY = g_settings.screen_StartY + 10;
	Box2.iWidth = 3*Box1.iWidth;
	Box2.iHeight = Box1.iHeight;
	
	CListFrame * listFrame2 = new CListFrame(&listFrameLines, NULL, CListFrame::TITLE | CListFrame::SCROLL | CListFrame::HEADER_LINE, &Box2);
	
	std::string testIcon1 = PLUGINDIR "/youtube/youtube_small.png";
	listFrame1->setTitle("listFrameBox1(mainMenu)", testIcon1);
	
	listFrame2->setTitle("listFrameBox2(subMenu)", testIcon1);
	
	listFrame1->paint();
	listFrame2->paint();
	
	sleep(3);
	
	delete listFrame1;
	listFrame1 = NULL;
	delete listFrame2;
	listFrame2 = NULL;
}

void CTestMenu::testCListBox()
{
	CListBox * listBox = new CListBox("listBox", MENU_WIDTH, MENU_HEIGHT, false, false, true);
	
	listBox->exec(NULL, "");
	delete listBox;
}

void CTestMenu::testCListBoxDetails()
{
	CListBox * listBox = new CListBox("listBoxInfoDetails", w_max ( (CFrameBuffer::getInstance()->getScreenWidth() / 20 * 17), (CFrameBuffer::getInstance()->getScreenWidth() / 20 )), h_max ( (CFrameBuffer::getInstance()->getScreenHeight() / 20 * 16), (CFrameBuffer::getInstance()->getScreenHeight() / 20)), true, false, true);
	
	listBox->exec(NULL, "");
	delete listBox;
}

void CTestMenu::testCListBoxDetailsTitleInfo()
{
	CListBox * listBox = new CListBox("listBoxDetailsTitleInfo", w_max ( (CFrameBuffer::getInstance()->getScreenWidth() / 20 * 17), (CFrameBuffer::getInstance()->getScreenWidth() / 20 )), h_max ( (CFrameBuffer::getInstance()->getScreenHeight() / 20 * 16), (CFrameBuffer::getInstance()->getScreenHeight() / 20)), true, true, true);
	
	listBox->exec(NULL, "");
	delete listBox;
}

void CTestMenu::testCallAudioPlayer()
{
	CAudioPlayerGui tmpAudioPlayerGui;
	tmpAudioPlayerGui.exec(NULL, "");
}

void CTestMenu::testCallInternetRadio()
{
	CAudioPlayerGui tmpAudioPlayerGui(true);
	tmpAudioPlayerGui.exec(NULL, "");
}

void CTestMenu::testCallTSMovieBrowser()
{
	moviePlayerGui->exec(NULL, "tsmoviebrowser");
}

void CTestMenu::testCallMovieBrowser()
{
	moviePlayerGui->exec(NULL, "moviebrowser");
}

void CTestMenu::testCallFilePlayBack()
{
	moviePlayerGui->exec(NULL, "fileplayback");
}

void CTestMenu::testCallPictureViewer()
{
	CPictureViewerGui tmpPictureViewerGui;
	tmpPictureViewerGui.exec(NULL, "");
}

void CTestMenu::testCallUPNPBrowser()
{
	CUpnpBrowserGui tmpUPNPBrowserGui;
	tmpUPNPBrowserGui.exec(NULL, "");
}

void CTestMenu::testPlayMovieURL()
{
	neutrino_msg_t msg;
	neutrino_msg_data_t data;
		
	CFileBrowser * fileBrowser;
	
	fileBrowser = new CFileBrowser();
	
	CFileFilter fileFilter;
	
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

	fileBrowser->Multi_Select    = false;
	fileBrowser->Dirs_Selectable = false;
	fileBrowser->Filter = &fileFilter;
	
	std::string Path_local = g_settings.network_nfs_moviedir;

BROWSER:
	if (fileBrowser->exec(Path_local.c_str()))
	{
		Path_local = fileBrowser->getCurrentDir();
		
		CFile * file;
		
		if ((file = fileBrowser->getSelectedFile()) != NULL) 
		{
		
			moviePlayerGui->filename = file->Name.c_str();
				
			// movieinfos
			moviePlayerGui->Title = file->Title;
			moviePlayerGui->Info1 = file->Info1;
			moviePlayerGui->Info2 = file->Info2;
			moviePlayerGui->thumbnail = file->thumbnail;
				
			// play
			moviePlayerGui->exec(NULL, "urlplayback");
		}

		g_RCInput->getMsg_ms(&msg, &data, 10); // 1 sec
		
		if (msg != CRCInput::RC_home) 
		{
			goto BROWSER;
		}
	}
	
	delete fileBrowser;
}

void CTestMenu::testPlayAudioURL()
{
	neutrino_msg_t msg;
	neutrino_msg_data_t data;
		
	CFileBrowser * fileBrowser;
	
	fileBrowser = new CFileBrowser();
	
	CFileFilter fileFilter;
	
	fileFilter.addFilter("cdr");
	fileFilter.addFilter("mp3");
	fileFilter.addFilter("m2a");
	fileFilter.addFilter("mpa");
	fileFilter.addFilter("mp2");
	fileFilter.addFilter("m3u");
	fileFilter.addFilter("ogg");
	fileFilter.addFilter("wav");
	fileFilter.addFilter("flac");
	fileFilter.addFilter("aac");
	fileFilter.addFilter("dts");
	fileFilter.addFilter("m4a");
	
	fileBrowser->Multi_Select = false;
	fileBrowser->Dirs_Selectable = false;
	fileBrowser->Filter = &fileFilter;
	
	std::string Path_local = g_settings.network_nfs_audioplayerdir;

BROWSER:
	if (fileBrowser->exec(Path_local.c_str()))
	{
		Path_local = fileBrowser->getCurrentDir();
		
		CFile * file;
		
		if ((file = fileBrowser->getSelectedFile()) != NULL) 
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
			
			// second line
			tmp = mp3.MetaData.title;
			tmp += " / ";
			tmp += mp3.MetaData.artist;
			
			w = g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getRenderWidth(tmp, true); // UTF-8
			xstart = (Box.iWidth - w)/2;
			if(xstart < 10)
				xstart = 10;
			
			g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->RenderString(Box.iX + xstart, Box.iY + 2*g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getHeight(), Box.iWidth - 20, tmp, COL_MENUCONTENTSELECTED, 0, true); // UTF-8		
			
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

		g_RCInput->getMsg_ms(&msg, &data, 10); // 1 sec
		
		if (msg != CRCInput::RC_home) 
		{
			goto BROWSER;
		}
	}
	
	delete fileBrowser;
}

void CTestMenu::testShowPictureURL()
{
	neutrino_msg_t msg;
	neutrino_msg_data_t data;

	CFileBrowser * fileBrowser;
	
	fileBrowser = new CFileBrowser();
	
	CFileFilter fileFilter;
	
	fileFilter.addFilter("png");
	fileFilter.addFilter("bmp");
	fileFilter.addFilter("jpg");
	fileFilter.addFilter("jpeg");
	
	fileBrowser->Multi_Select    = false;
	fileBrowser->Dirs_Selectable = false;
	fileBrowser->Filter = &fileFilter;
	
	std::string Path_local = g_settings.network_nfs_audioplayerdir;

BROWSER:
	if (fileBrowser->exec(Path_local.c_str()))
	{
		Path_local = fileBrowser->getCurrentDir();
		
		CFile * file;
		
		if ((file = fileBrowser->getSelectedFile()) != NULL) 
		{	
			g_PicViewer->SetScaling((CFrameBuffer::ScalingMode)g_settings.picviewer_scaling);
			g_PicViewer->SetVisible(g_settings.screen_StartX, g_settings.screen_EndX, g_settings.screen_StartY, g_settings.screen_EndY);

			if(g_settings.video_Ratio == 1)
				g_PicViewer->SetAspectRatio(16.0/9);
			else
				g_PicViewer->SetAspectRatio(4.0/3);


			g_PicViewer->ShowImage(file->Name);
			
			bool loop = true;
			while (loop)
			{
				g_RCInput->getMsg(&msg, &data, 10); // 1 sec

				if( msg == CRCInput::RC_home)
					loop = false;
			}
						
			CFrameBuffer::getInstance()->ClearFrameBuffer();
			CFrameBuffer::getInstance()->blit();	
		}

		g_RCInput->getMsg_ms(&msg, &data, 10); // 1 sec
		
		if (msg != CRCInput::RC_home) 
		{
			goto BROWSER;
		}
	}
	
	delete fileBrowser;
}

void CTestMenu::testPlayAudioFolder()
{
	neutrino_msg_t msg;
	neutrino_msg_data_t data;
		
	CFileBrowser * fileBrowser;

	CFileFilter fileFilter;
	
	CFileList filelist;
	int selected = 0;
	
	fileFilter.addFilter("cdr");
	fileFilter.addFilter("mp3");
	fileFilter.addFilter("m2a");
	fileFilter.addFilter("mpa");
	fileFilter.addFilter("mp2");
	fileFilter.addFilter("m3u");
	fileFilter.addFilter("ogg");
	fileFilter.addFilter("wav");
	fileFilter.addFilter("flac");
	fileFilter.addFilter("aac");
	fileFilter.addFilter("dts");
	fileFilter.addFilter("m4a");
	
	fileBrowser = new CFileBrowser();
	fileBrowser->Multi_Select = true;
	fileBrowser->Dirs_Selectable = false;
	fileBrowser->Filter = &fileFilter;
	
	std::string Path_local = g_settings.network_nfs_audioplayerdir;

BROWSER:
	if (fileBrowser->exec(Path_local.c_str()))
	{
		Path_local = fileBrowser->getCurrentDir();
		filelist = fileBrowser->getSelectedFiles();

		if (!filelist.empty()) 
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
PLAY:
			CAudiofile mp3(filelist[selected].Name.c_str(), filelist[selected].getExtension());
			
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
	
			//
			CFrameBuffer::getInstance()->paintBackgroundBoxRel(Box.iX, Box.iY, Box.iWidth, Box.iHeight);
			
			//
			CFrameBuffer::getInstance()->paintBoxRel(Box.iX, Box.iY, Box.iWidth, Box.iHeight, COL_MENUCONTENT_PLUS_6 );
			
			// infobox refresh
			CFrameBuffer::getInstance()->paintBoxRel(Box.iX + 2, Box.iY + 2 , Box.iWidth - 4, Box.iHeight - 4, COL_MENUCONTENTSELECTED_PLUS_0);

			std::string tmp;
			
			tmp.clear();
			
			char sNr[20];
			sprintf(sNr, ": %2d", (selected + 1));
			tmp = g_Locale->getText(LOCALE_AUDIOPLAYER_PLAYING);
			tmp += sNr ;

			// first line
			int w = g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getRenderWidth(tmp, true); // UTF-8
			int xstart = (Box.iWidth - w) / 2;
			if(xstart < 10)
				xstart = 10;
			g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->RenderString(Box.iX + xstart, Box.iY + 4 + g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getHeight(), Box.iWidth - 20, tmp, COL_MENUCONTENTSELECTED, 0, true); // UTF-8
			
			// second line
			tmp = mp3.MetaData.title;
			tmp += " / ";
			tmp += mp3.MetaData.artist;
			
			w = g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getRenderWidth(tmp, true); // UTF-8
			xstart = (Box.iWidth - w)/2;
			if(xstart < 10)
				xstart = 10;
			
			g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->RenderString(Box.iX + xstart, Box.iY + 2*g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getHeight(), Box.iWidth - 20, tmp, COL_MENUCONTENTSELECTED, 0, true); // UTF-8		
			
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
				
				if((msg == CRCInput::RC_right || msg == CRCInput::RC_up) || CAudioPlayer::getInstance()->getState() == CBaseDec::STOP)
				{
					if(filelist.size() > 1 && selected < filelist.size())
					{
						loop = false;
						selected++;
						CAudioPlayer::getInstance()->stop();
						mp3.clear();
						goto PLAY;
					}
				}
				else if(msg == CRCInput::RC_left || msg == CRCInput::RC_down)
				{
					if(filelist.size() > 1 && selected > 0)
					{
						loop = false;
						selected--;
						CAudioPlayer::getInstance()->stop();
						mp3.clear();
						goto PLAY;
					}
				}
				else if(msg == CRCInput::RC_home || msg == CRCInput::RC_stop)
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

		g_RCInput->getMsg_ms(&msg, &data, 10); // 1 sec
		
		if (msg != CRCInput::RC_home) 
		{
			goto BROWSER;
		}
	}
	
	delete fileBrowser;
}

void CTestMenu::testShowPictureFolder()
{
	neutrino_msg_t msg;
	neutrino_msg_data_t data;

	CFileBrowser * fileBrowser;
	
	CFileFilter fileFilter;
	
	CFileList filelist;
	int selected = 0;
	
	fileFilter.addFilter("png");
	fileFilter.addFilter("bmp");
	fileFilter.addFilter("jpg");
	fileFilter.addFilter("jpeg");
	
	fileBrowser = new CFileBrowser();
	fileBrowser->Multi_Select    = true;
	fileBrowser->Dirs_Selectable = true;
	fileBrowser->Filter = &fileFilter;
	
	std::string Path_local = g_settings.network_nfs_audioplayerdir;

BROWSER:
	if (fileBrowser->exec(Path_local.c_str()))
	{
		Path_local = fileBrowser->getCurrentDir();
		
		filelist = fileBrowser->getSelectedFiles();
		
		if (!filelist.empty()) 
		{	
			g_PicViewer->SetScaling((CFrameBuffer::ScalingMode)g_settings.picviewer_scaling);
			g_PicViewer->SetVisible(g_settings.screen_StartX, g_settings.screen_EndX, g_settings.screen_StartY, g_settings.screen_EndY);

			if(g_settings.video_Ratio == 1)
				g_PicViewer->SetAspectRatio(16.0/9);
			else
				g_PicViewer->SetAspectRatio(4.0/3);

VIEWPIC:
			g_PicViewer->ShowImage(filelist[selected].Name);
			
			bool loop = true;
			while (loop)
			{
				g_RCInput->getMsg(&msg, &data, 10); // 1 sec
				
				if(msg != CRCInput::RC_home && selected < filelist.size())
				{
					loop = false;
					usleep(10000);
					selected++;
					CFrameBuffer::getInstance()->ClearFrameBuffer();
					CFrameBuffer::getInstance()->blit();	
					goto VIEWPIC;
				}
				else if( msg == CRCInput::RC_home)
				{
					loop = false;
				}
			}
						
			CFrameBuffer::getInstance()->ClearFrameBuffer();
			CFrameBuffer::getInstance()->blit();	
		}

		g_RCInput->getMsg_ms(&msg, &data, 10); // 1 sec
		
		if (msg != CRCInput::RC_home) 
		{
			goto BROWSER;
		}
	}
	
	delete fileBrowser;
}

void CTestMenu::testStartPlugin()
{
	g_PluginList->startPlugin("youtube");
}

void CTestMenu::testShowActuellEPG()
{
	CEPGData epgData;
	event_id_t epgid = 0;
			
	if(sectionsd_getActualEPGServiceKey(live_channel_id&0xFFFFFFFFFFFFULL, &epgData))
		epgid = epgData.eventID;

	if(epgid != 0) 
	{
		CShortEPGData epgdata;
				
		if(sectionsd_getEPGidShort(epgid, &epgdata)) 
		{
			//InfoBox
			std::string title;
			title = g_Zapit->getChannelName(live_channel_id);
			title += ":";
			title += epgdata.title;
			std::string buffer;
			buffer = epgdata.info1;
			buffer += "\n";
			buffer += epgdata.info2;
			
			InfoBox(title.c_str(), buffer.c_str(), CInfoBox::mbrBack, CInfoBox::mbBack);	// UTF-8
			
		}
	}
	else
		MessageBox(LOCALE_MESSAGEBOX_ERROR, "No EPG found!", CMessageBox::mbrCancel, CMessageBox::mbCancel, NEUTRINO_ICON_ERROR);
}

int CTestMenu::exec(CMenuTarget* parent, const std::string& actionKey)
{
	int res = menu_return::RETURN_REPAINT;
	dprintf(DEBUG_NORMAL, "\nCTestMenu::exec: actionKey:%s\n", actionKey.c_str());
	
	if(parent)
		hide();
	
	if(actionKey == "stringinput")
	{
		testCStringInput();
		return res;
	}
	else if(actionKey == "stringinputsms")
	{
		testCStringInputSMS();
		return res;
	}
	else if(actionKey == "pininput")
	{
		testCPINInput();
		return res;
	}
	else if(actionKey == "ipinput")
	{
		testCIPInput();
		return res;
	}
	else if(actionKey == "macinput")
	{
		testCMACInput();
		return res;
	}
	else if(actionKey == "dateinput")
	{
		testCDateInput();
		return res;
	}
	else if(actionKey == "timeinput")
	{
		testCTimeInput();
		return res;
	}
	else if(actionKey == "intinput")
	{
		testCIntInput();
		return res;
	}
	else if(actionKey == "infobox")
	{
		testCInfoBox();
		return res;
	}
	else if(actionKey == "infoboxshowmsg")
	{
		testCInfoBoxShowMsg();
		return res;
	}
	else if(actionKey == "infoboxinfobox")
	{
		testCInfoBoxInfoBox();
		return res;
	}
	else if(actionKey == "messagebox")
	{
		testCMessageBox();
		return res;
	}
	else if(actionKey == "messageboxinfomsg")
	{
		testCMessageBoxInfoMsg();
		return res;
	}
	else if(actionKey == "messageboxerrormsg")
	{
		testCMessageBoxErrorMsg();
		return res;
	}
	else if(actionKey == "hintbox")
	{
		testCHintBox();
		return res;
	}
	else if(actionKey == "hintboxinfo")
	{
		testCHintBoxInfo();
		return res;
	}
	else if(actionKey == "hintboxext")
	{
		testCHintBoxExt();
		return res;
	}
	else if(actionKey == "helpbox")
	{
		testCHelpBox();
		return res;
	}
	else if(actionKey == "textbox")
	{
		testCTextBox();
		return res;
	}
	else if(actionKey == "listframebox")
	{
		testCListFrameBox();
		return res;
	}
	else if(actionKey == "listbox")
	{
		testCListBox();
		return res;
	}
	else if(actionKey == "listboxdetails")
	{
		testCListBoxDetails();
		return res;
	}
	else if(actionKey == "listboxdetailstitleinfo")
	{
		testCListBoxDetailsTitleInfo();
		return res;
	}
	else if(actionKey == "audioplayer")
	{
		testCallAudioPlayer();
		return res;
	}
	else if(actionKey == "internetradio")
	{
		testCallInternetRadio();
		return res;
	}
	else if(actionKey == "tsmoviebrowser")
	{
		testCallTSMovieBrowser();
		return res;
	}
	else if(actionKey == "moviebrowser")
	{
		testCallMovieBrowser();
		return res;
	}
	else if(actionKey == "fileplayback")
	{
		testCallFilePlayBack();
		return res;
	}
	else if(actionKey == "pictureviewer")
	{
		testCallPictureViewer();
		return res;
	}
	else if(actionKey == "upnpbrowser")
	{
		testCallUPNPBrowser();
		return res;
	}
	else if(actionKey == "playmovieurl")
	{
		testPlayMovieURL();
		return res;
	}
	else if(actionKey == "playaudiourl")
	{
		testPlayAudioURL();
		return res;
	}
	else if(actionKey == "showpictureurl")
	{
		testShowPictureURL();
		return res;
	}
	else if(actionKey == "playaudiofolder")
	{
		testPlayAudioFolder();
		return res;
	}
	else if(actionKey == "showpicturefolder")
	{
		testShowPictureFolder();
		return res;
	}
	else if(actionKey == "startplugin")
	{
		testStartPlugin();
		return res;
	}
	else if(actionKey == "showepg")
	{
		testShowActuellEPG();
		return res;
	}
	
	showTestMenu();
	
	return res;
}

void CTestMenu::showTestMenu()
{
	/// menue.cpp
	CMenuWidget * mainMenu = new CMenuWidget("testMenu", NEUTRINO_ICON_BUTTON_SETUP);
	
	mainMenu->addItem(new CMenuForwarderNonLocalized("CStringInput", true, NULL, this, "stringinput"));
	mainMenu->addItem(new CMenuForwarderNonLocalized("CStringInputSMS", true, NULL, this, "stringinputsms"));
	mainMenu->addItem(new CMenuForwarderNonLocalized("CPINInput", true, NULL, this, "pininput"));
	mainMenu->addItem(new CMenuForwarderNonLocalized("CIPInput", true, NULL, this, "ipinput"));
	mainMenu->addItem(new CMenuForwarderNonLocalized("CMACInput", true, NULL, this, "macinput"));
	mainMenu->addItem(new CMenuForwarderNonLocalized("CDateInput", false, NULL, this, "dateinput"));
	mainMenu->addItem(new CMenuForwarderNonLocalized("CTimeInput", true, NULL, this, "timeinput"));
	mainMenu->addItem(new CMenuForwarderNonLocalized("CIntInput", false, NULL, this, "intinput"));
	mainMenu->addItem(new CMenuForwarderNonLocalized("CInfoBox", true, NULL, this, "infobox"));
	mainMenu->addItem(new CMenuForwarderNonLocalized("CInfoBoxShowMsg", true, NULL, this, "infoboxshowmsg"));
	mainMenu->addItem(new CMenuForwarderNonLocalized("CInfoBoxInfoBox", true, NULL, this, "infoboxinfobox"));
	mainMenu->addItem(new CMenuForwarderNonLocalized("CMessageBox", true, NULL, this, "messagebox"));
	mainMenu->addItem(new CMenuForwarderNonLocalized("CMessageBoxInfoMsg", true, NULL, this, "messageboxinfomsg"));
	mainMenu->addItem(new CMenuForwarderNonLocalized("CMessageBoxErrorMsg", true, NULL, this, "messageboxerrormsg"));
	mainMenu->addItem(new CMenuForwarderNonLocalized("CHintBox", true, NULL, this, "hintbox"));
	mainMenu->addItem(new CMenuForwarderNonLocalized("CHintBoxInfo", true, NULL, this, "hintboxinfo"));
	mainMenu->addItem(new CMenuForwarderNonLocalized("CHintBoxExt", false, NULL, this, "hintboxext"));
	mainMenu->addItem(new CMenuForwarderNonLocalized("CHelpBox", true, NULL, this, "helpbox"));
	mainMenu->addItem(new CMenuForwarderNonLocalized("CTextBox", true, NULL, this, "textbox"));
	mainMenu->addItem(new CMenuForwarderNonLocalized("CListFrameBox", true, NULL, this, "listframebox"));
	mainMenu->addItem(new CMenuForwarderNonLocalized("CListBox", true, NULL, this, "listbox"));
	mainMenu->addItem(new CMenuForwarderNonLocalized("CListBoxInfoDetails", true, NULL, this, "listboxdetails"));
	mainMenu->addItem(new CMenuForwarderNonLocalized("CListBoxDetailsTitleInfo", true, NULL, this, "listboxdetailstitleinfo"));
	mainMenu->addItem( new CMenuSeparator(CMenuSeparatorItemMenuIcon::LINE) );
	mainMenu->addItem(new CMenuForwarderNonLocalized("AudioPlayer", true, NULL, this, "audioplayer"));
	mainMenu->addItem(new CMenuForwarderNonLocalized("InternetRadio", true, NULL, this, "internetradio"));
	mainMenu->addItem(new CMenuForwarderNonLocalized("TSMovieBrowser", true, NULL, this, "tsmoviebrowser"));
	mainMenu->addItem(new CMenuForwarderNonLocalized("MovieBrowser", true, NULL, this, "moviebrowser"));
	mainMenu->addItem(new CMenuForwarderNonLocalized("FilePlayBack", true, NULL, this, "fileplayback"));
	mainMenu->addItem(new CMenuForwarderNonLocalized("PictureViewer", true, NULL, this, "pictureviewer"));
	mainMenu->addItem(new CMenuForwarderNonLocalized("UPNPBrowser", true, NULL, this, "upnpbrowser"));
	mainMenu->addItem( new CMenuSeparator(CMenuSeparatorItemMenuIcon::LINE) );
	mainMenu->addItem(new CMenuForwarderNonLocalized("PlayMovieURL", true, NULL, this, "playmovieurl"));
	mainMenu->addItem(new CMenuForwarderNonLocalized("PlayAudioURL", true, NULL, this, "playaudiourl"));
	mainMenu->addItem(new CMenuForwarderNonLocalized("ShowPictureURL", true, NULL, this, "showpictureurl"));
	mainMenu->addItem(new CMenuForwarderNonLocalized("PlayAudioFolder", true, NULL, this, "playaudiofolder"));
	mainMenu->addItem(new CMenuForwarderNonLocalized("ShowPictureFolder", true, NULL, this, "showpicturefolder"));
	mainMenu->addItem( new CMenuSeparator(CMenuSeparatorItemMenuIcon::LINE) );
	mainMenu->addItem(new CMenuForwarderNonLocalized("StartPlugin(e.g: youtube)", true, NULL, this, "startplugin"));
	mainMenu->addItem( new CMenuSeparator(CMenuSeparatorItemMenuIcon::LINE) );
	mainMenu->addItem(new CMenuForwarderNonLocalized("ShowActuellEPG", true, NULL, this, "showepg"));
	
	mainMenu->exec(NULL, "");
	mainMenu->hide();
	delete mainMenu;
}

void plugin_init(void)
{
	dprintf(DEBUG_NORMAL, "test: plugin_init\n");
}

void plugin_del(void)
{
	dprintf(DEBUG_NORMAL, "test: plugin_del\n");
}

void plugin_exec(void)
{
	CTestMenu* testMenu = new CTestMenu();
	
	testMenu->showTestMenu();
	
	delete testMenu;
}


