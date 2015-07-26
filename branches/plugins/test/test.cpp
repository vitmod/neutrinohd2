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
		void testCHelpBox();
		void testCTextBox();
		void testCListFrameBox();
		void testCListBox();
		void testCListBoxDetails();
		void testCListBoxDetailsTitleInfo();
		void testCProgressBar();
		void testCProgressWindow();
		void testCButtons();
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
		void testCallChannel();
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
	value.clear();
}
void CTestMenu::testCStringInputSMS()
{
	std::string value;
	CStringInputSMS * stringInputSMS = new CStringInputSMS("CStringInputSMS", (char *)value.c_str());
	
	stringInputSMS->exec(NULL, "");
	stringInputSMS->hide();
	delete stringInputSMS;
	value.clear();
}

void CTestMenu::testCPINInput()
{
	std::string value;
	CPINInput * pinInput = new CPINInput("CPINInput", (char *)value.c_str());
	
	pinInput->exec(NULL, "");
	pinInput->hide();
	delete pinInput;
	value.clear();
}

void CTestMenu::testCIPInput()
{
	std::string value;
	CIPInput * ipInput = new CIPInput(LOCALE_STREAMINGMENU_SERVER_IP, value);
	
	ipInput->exec(NULL, "");
	ipInput->hide();
	delete ipInput;
	value.clear();
}

void CTestMenu::testCMACInput()
{
	std::string value;
	CMACInput * macInput = new CMACInput(LOCALE_RECORDINGMENU_SERVER_MAC, (char *)value.c_str());
	
	macInput->exec(NULL, "");
	macInput->hide();
	delete macInput;
	value.clear();
}

void CTestMenu::testCDateInput()
{
	time_t value;
	CDateInput * dateInput = new CDateInput(LOCALE_FILEBROWSER_SORT_DATE, &value);
	
	dateInput->exec(NULL, "");
	dateInput->hide();
	delete dateInput;
}

void CTestMenu::testCTimeInput()
{
	std::string value;
	CTimeInput * timeInput = new CTimeInput(LOCALE_FILEBROWSER_SORT_DATE, (char *)value.c_str());
	
	timeInput->exec(NULL, "");
	timeInput->hide();
	delete timeInput;
	value.clear();
}

void CTestMenu::testCIntInput()
{
	int value;
	CIntInput * intInput = new CIntInput(LOCALE_FILEBROWSER_SORT_DATE, value);
	
	intInput->exec(NULL, "");
	intInput->hide();
	delete intInput;
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
	
	CInfoBox * msgBox = new CInfoBox("CInfoBox", g_Font[SNeutrinoSettings::FONT_TYPE_MENU], mode, &position, "CInfoBoxInfoBox", g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE], NEUTRINO_ICON_BUTTON_SETUP, CInfoBox::mbCancel, CInfoBox::mbrCancel);
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

void CTestMenu::testCProgressBar()
{
	CProgressBar *timescale = NULL;
	
	CBox Box;
	
	Box.iX = g_settings.screen_StartX + 10;
	Box.iY = g_settings.screen_StartY + 10;
	Box.iWidth = (g_settings.screen_EndX - g_settings.screen_StartX - 20);
	Box.iHeight = (g_settings.screen_EndY - g_settings.screen_StartY - 20)/20;
	
	timescale = new CProgressBar(Box.iWidth, Box.iHeight, 30, 100, 70, true);
	timescale->reset();
	
	timescale->paint(Box.iX, Box.iY, 10);
	usleep(1000000);
	timescale->paint(Box.iX, Box.iY, 20);
	usleep(1000000);
	timescale->paint(Box.iX, Box.iY, 30);
	usleep(1000000);
	timescale->paint(Box.iX, Box.iY, 40);
	usleep(1000000);
	timescale->paint(Box.iX, Box.iY, 50);
	usleep(1000000);
	timescale->paint(Box.iX, Box.iY, 60);
	usleep(1000000);
	timescale->paint(Box.iX, Box.iY, 70);
	usleep(1000000);
	timescale->paint(Box.iX, Box.iY, 80);
	usleep(1000000);
	timescale->paint(Box.iX, Box.iY, 90);
	usleep(1000000);
	timescale->paint(Box.iX, Box.iY, 100);
	
	delete timescale;
	timescale = NULL;
	//
	hide();
}

void CTestMenu::testCProgressWindow()
{
	CProgressWindow * progress;
	
	progress = new CProgressWindow();
	progress->setTitle("CProgressWindow");
	progress->exec(NULL, "");
	
	progress->showStatusMessageUTF("testing CProgressWindow");
	progress->showGlobalStatus(0);
	usleep(1000000);
	progress->showGlobalStatus(10);
	usleep(1000000);
	progress->showGlobalStatus(20);
	usleep(1000000);
	progress->showGlobalStatus(30);
	usleep(1000000);
	progress->showGlobalStatus(40);
	usleep(1000000);
	progress->showGlobalStatus(50);
	usleep(1000000);
	progress->showGlobalStatus(60);
	usleep(1000000);
	progress->showGlobalStatus(70);
	usleep(1000000);
	progress->showGlobalStatus(80);
	usleep(1000000);
	progress->showGlobalStatus(90);
	usleep(1000000);
	progress->showGlobalStatus(100);
	usleep(1000000);
	
	progress->hide();
	delete progress;
	progress = NULL;
        
}

const struct button_label Buttons[4] =
{
	{ NEUTRINO_ICON_BUTTON_RED, NONEXISTANT_LOCALE },
	{ NEUTRINO_ICON_BUTTON_GREEN, NONEXISTANT_LOCALE },
	{ NEUTRINO_ICON_BUTTON_YELLOW, NONEXISTANT_LOCALE },
	{ NEUTRINO_ICON_BUTTON_BLUE, NONEXISTANT_LOCALE },
	
};

void CTestMenu::testCButtons()
{
	::paintButtons(CFrameBuffer::getInstance(), g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL], g_Locale, g_settings.screen_StartX + 50 + BORDER_LEFT, g_settings.screen_StartY + 50, (g_settings.screen_EndX - g_settings.screen_StartX - 100)/4, 4, Buttons);
	usleep(1000000);
	hide();
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
	fileFilter.addFilter("mpa");
	fileFilter.addFilter("mp2");
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
			CAudioPlayerGui tmpAudioPlayerGui;
			
			if (file->getType() == CFile::FILE_AUDIO)
			{
				CAudiofileExt audiofile(file->Name, file->getExtension());
				tmpAudioPlayerGui.addToPlaylist(audiofile);
				tmpAudioPlayerGui.exec(NULL, "urlplayback");
			}
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
	
	fileFilter.addFilter("cdr");
	fileFilter.addFilter("mp3");
	fileFilter.addFilter("m2a");
	fileFilter.addFilter("mpa");
	fileFilter.addFilter("mp2");
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
		
		CAudioPlayerGui tmpAudioPlayerGui;
		CFileList::const_iterator files = fileBrowser->getSelectedFiles().begin();
		
		for(; files != fileBrowser->getSelectedFiles().end(); files++)
		{

			if ( (files->getExtension() == CFile::EXTENSION_CDR)
					||  (files->getExtension() == CFile::EXTENSION_MP3)
					||  (files->getExtension() == CFile::EXTENSION_WAV)
					||  (files->getExtension() == CFile::EXTENSION_FLAC)
			)
			{
				CAudiofileExt audiofile(files->Name, files->getExtension());
				tmpAudioPlayerGui.addToPlaylist(audiofile);
			}
		}
		
		tmpAudioPlayerGui.exec(NULL, "urlplayback");

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
		
		CPictureViewerGui tmpPictureViewerGui;
		CPicture pic;
		struct stat statbuf;
				
		CFileList::const_iterator files = fileBrowser->getSelectedFiles().begin();
		
		for(; files != fileBrowser->getSelectedFiles().end(); files++)
		{

			if (files->getType() == CFile::FILE_PICTURE)
			{
				pic.Filename = files->Name;
				std::string tmp = files->Name.substr(files->Name.rfind('/') + 1);
				pic.Name = tmp.substr(0, tmp.rfind('.'));
				pic.Type = tmp.substr(tmp.rfind('.') + 1);
			
				if(stat(pic.Filename.c_str(), &statbuf) != 0)
					printf("stat error");
				pic.Date = statbuf.st_mtime;
				
				tmpPictureViewerGui.addToPlaylist(pic);
			}
		}
		
		tmpPictureViewerGui.exec(NULL, "urlplayback");

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

void CTestMenu::testCallChannel()
{
	CSelectChannelWidget* CSelectChannelWidgetHandler = new CSelectChannelWidget();
	CSelectChannelWidgetHandler->exec(NULL, "tv");
		
	//CSelectChannelWidget_TVChanID;
	//CSelectChannelWidget_TVChanName.c_str();
		
	delete CSelectChannelWidgetHandler;
	CSelectChannelWidgetHandler = NULL;
		
	menu_return::RETURN_REPAINT;
}

int CTestMenu::exec(CMenuTarget* parent, const std::string& actionKey)
{
	dprintf(DEBUG_NORMAL, "\nCTestMenu::exec: actionKey:%s\n", actionKey.c_str());
	
	if(parent)
		hide();
	
	if(actionKey == "stringinput")
	{
		testCStringInput();
	}
	else if(actionKey == "stringinputsms")
	{
		testCStringInputSMS();
	}
	else if(actionKey == "pininput")
	{
		testCPINInput();
	}
	else if(actionKey == "ipinput")
	{
		testCIPInput();
	}
	else if(actionKey == "macinput")
	{
		testCMACInput();
	}
	else if(actionKey == "dateinput")
	{
		testCDateInput();
	}
	else if(actionKey == "timeinput")
	{
		testCTimeInput();
	}
	else if(actionKey == "intinput")
	{
		testCIntInput();
	}
	else if(actionKey == "infobox")
	{
		testCInfoBox();
	}
	else if(actionKey == "infoboxshowmsg")
	{
		testCInfoBoxShowMsg();
	}
	else if(actionKey == "infoboxinfobox")
	{
		testCInfoBoxInfoBox();
	}
	else if(actionKey == "messagebox")
	{
		testCMessageBox();
	}
	else if(actionKey == "messageboxinfomsg")
	{
		testCMessageBoxInfoMsg();
	}
	else if(actionKey == "messageboxerrormsg")
	{
		testCMessageBoxErrorMsg();
	}
	else if(actionKey == "hintbox")
	{
		testCHintBox();
	}
	else if(actionKey == "hintboxinfo")
	{
		testCHintBoxInfo();
	}
	else if(actionKey == "helpbox")
	{
		testCHelpBox();
	}
	else if(actionKey == "textbox")
	{
		testCTextBox();
	}
	else if(actionKey == "listframebox")
	{
		testCListFrameBox();
	}
	else if(actionKey == "listbox")
	{
		testCListBox();
	}
	else if(actionKey == "listboxdetails")
	{
		testCListBoxDetails();
	}
	else if(actionKey == "listboxdetailstitleinfo")
	{
		testCListBoxDetailsTitleInfo();
	}
	else if(actionKey == "progressbar")
	{
		testCProgressBar();
	}
	else if(actionKey == "progresswindow")
	{
		testCProgressWindow();
	}
	else if(actionKey == "buttons")
	{
		testCButtons();
	}
	else if(actionKey == "audioplayer")
	{
		testCallAudioPlayer();
	}
	else if(actionKey == "internetradio")
	{
		testCallInternetRadio();
	}
	else if(actionKey == "tsmoviebrowser")
	{
		testCallTSMovieBrowser();
	}
	else if(actionKey == "moviebrowser")
	{
		testCallMovieBrowser();
	}
	else if(actionKey == "fileplayback")
	{
		testCallFilePlayBack();
	}
	else if(actionKey == "pictureviewer")
	{
		testCallPictureViewer();
	}
	else if(actionKey == "upnpbrowser")
	{
		testCallUPNPBrowser();
	}
	else if(actionKey == "playmovieurl")
	{
		testPlayMovieURL();
	}
	else if(actionKey == "playaudiourl")
	{
		testPlayAudioURL();
	}
	else if(actionKey == "showpictureurl")
	{
		testShowPictureURL();
	}
	else if(actionKey == "playaudiofolder")
	{
		testPlayAudioFolder();
	}
	else if(actionKey == "showpicturefolder")
	{
		testShowPictureFolder();
	}
	else if(actionKey == "startplugin")
	{
		testStartPlugin();
	}
	else if(actionKey == "showepg")
	{
		testShowActuellEPG();
	}
	else if(actionKey == "channelselect")
	{
		testCallChannel();
	}
	
	return menu_return::RETURN_REPAINT;
}

void CTestMenu::showTestMenu()
{
	/// menue.cpp
	CMenuWidget * mainMenu = new CMenuWidget("testMenu", NEUTRINO_ICON_BUTTON_SETUP);
	
	mainMenu->addItem(new CMenuForwarder("CStringInput", true, NULL, this, "stringinput"));
	mainMenu->addItem(new CMenuForwarder("CStringInputSMS", true, NULL, this, "stringinputsms"));
	mainMenu->addItem(new CMenuForwarder("CPINInput", true, NULL, this, "pininput"));
	mainMenu->addItem(new CMenuForwarder("CIPInput", true, NULL, this, "ipinput"));
	mainMenu->addItem(new CMenuForwarder("CMACInput", true, NULL, this, "macinput"));
	mainMenu->addItem(new CMenuForwarder("CDateInput", true, NULL, this, "dateinput"));
	mainMenu->addItem(new CMenuForwarder("CTimeInput", true, NULL, this, "timeinput"));
	mainMenu->addItem(new CMenuForwarder("CIntInput", true, NULL, this, "intinput"));
	mainMenu->addItem(new CMenuForwarder("CInfoBox", true, NULL, this, "infobox"));
	mainMenu->addItem(new CMenuForwarder("CInfoBoxShowMsg", true, NULL, this, "infoboxshowmsg"));
	mainMenu->addItem(new CMenuForwarder("CInfoBoxInfoBox", true, NULL, this, "infoboxinfobox"));
	mainMenu->addItem(new CMenuForwarder("CMessageBox", true, NULL, this, "messagebox"));
	mainMenu->addItem(new CMenuForwarder("CMessageBoxInfoMsg", true, NULL, this, "messageboxinfomsg"));
	mainMenu->addItem(new CMenuForwarder("CMessageBoxErrorMsg", true, NULL, this, "messageboxerrormsg"));
	mainMenu->addItem(new CMenuForwarder("CHintBox", true, NULL, this, "hintbox"));
	mainMenu->addItem(new CMenuForwarder("CHintBoxInfo", true, NULL, this, "hintboxinfo"));
	mainMenu->addItem(new CMenuForwarder("CHelpBox", true, NULL, this, "helpbox"));
	mainMenu->addItem(new CMenuForwarder("CTextBox", true, NULL, this, "textbox"));
	mainMenu->addItem(new CMenuForwarder("CListFrameBox", true, NULL, this, "listframebox"));
	mainMenu->addItem(new CMenuForwarder("CListBox", true, NULL, this, "listbox"));
	mainMenu->addItem(new CMenuForwarder("CListBoxInfoDetails", true, NULL, this, "listboxdetails"));
	mainMenu->addItem(new CMenuForwarder("CListBoxDetailsTitleInfo", true, NULL, this, "listboxdetailstitleinfo"));
	mainMenu->addItem(new CMenuForwarder("CProgressBar", true, NULL, this, "progressbar"));
	mainMenu->addItem(new CMenuForwarder("CProgressWindow", true, NULL, this, "progresswindow"));
	mainMenu->addItem(new CMenuForwarder("CButtons", true, NULL, this, "buttons"));
	mainMenu->addItem( new CMenuSeparator(CMenuSeparator::LINE) );
	mainMenu->addItem(new CMenuForwarder("AudioPlayer", true, NULL, this, "audioplayer"));
	mainMenu->addItem(new CMenuForwarder("InternetRadio", true, NULL, this, "internetradio"));
	mainMenu->addItem(new CMenuForwarder("TSMovieBrowser", true, NULL, this, "tsmoviebrowser"));
	mainMenu->addItem(new CMenuForwarder("MovieBrowser", true, NULL, this, "moviebrowser"));
	mainMenu->addItem(new CMenuForwarder("FilePlayBack", true, NULL, this, "fileplayback"));
	mainMenu->addItem(new CMenuForwarder("PictureViewer", true, NULL, this, "pictureviewer"));
	mainMenu->addItem(new CMenuForwarder("UPNPBrowser", true, NULL, this, "upnpbrowser"));
	mainMenu->addItem( new CMenuSeparator(CMenuSeparator::LINE) );
	mainMenu->addItem(new CMenuForwarder("PlayMovieURL", true, NULL, this, "playmovieurl"));
	mainMenu->addItem(new CMenuForwarder("PlayAudioURL", true, NULL, this, "playaudiourl"));
	mainMenu->addItem(new CMenuForwarder("ShowPictureURL", true, NULL, this, "showpictureurl"));
	mainMenu->addItem(new CMenuForwarder("PlayAudioFolder", true, NULL, this, "playaudiofolder"));
	mainMenu->addItem(new CMenuForwarder("ShowPictureFolder", true, NULL, this, "showpicturefolder"));
	mainMenu->addItem( new CMenuSeparator(CMenuSeparator::LINE) );
	mainMenu->addItem(new CMenuForwarder("StartPlugin(e.g: youtube)", true, NULL, this, "startplugin"));
	mainMenu->addItem( new CMenuSeparator(CMenuSeparator::LINE) );
	mainMenu->addItem(new CMenuForwarder("ShowActuellEPG", true, NULL, this, "showepg"));
	mainMenu->addItem(new CMenuForwarder("CallChannelSelectWidget", true, NULL, this, "channelselect"));
	
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


