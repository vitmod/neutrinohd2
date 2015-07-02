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
		void testCMsgBox();
		void testCMsgBoxShowMsg();
		void testCMsgBoxInfoBox();
		void testCMessageBox();
		void testCMessageBoxInfoMsg();
		void testCMessageBoxErrorMsg();
		void testCHintBox();
		void testCHintBoxExt();
		void testCHelpBox();
		void testCTextBox();
		void testCListFrameBox();
		void testCListBox();
		void testCListBoxDetails();
		void testCListBoxHeadInfo();
		void testCallAudioPlayer();
		void testCallInternetRadio();
		void testCallTSMovieBrowser();
		void testCallMovieBrowser();
		void testCallFilePlayBack();
		void testCallPictureViewer();
		void testCallUPNPBrowser();
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

void CTestMenu::testCMsgBox()
{
	int mode =  CMsgBox::SCROLL | CMsgBox::TITLE | CMsgBox::FOOT | CMsgBox::BORDER;// | //CMsgBox::NO_AUTO_LINEBREAK | //CMsgBox::CENTER | //CMsgBox::AUTO_WIDTH | //CMsgBox::AUTO_HIGH;
	CBox position(g_settings.screen_StartX + 50, g_settings.screen_StartY + 50, g_settings.screen_EndX - g_settings.screen_StartX - 100, g_settings.screen_EndY - g_settings.screen_StartY - 100); 
	
	CMsgBox * msgBox = new CMsgBox("ShowMsg2UTF", g_Font[SNeutrinoSettings::FONT_TYPE_MENU], mode, &position, "msgbox.cpp", g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE], NULL);
	
	msgBox->exec();
	msgBox->hide();
	delete msgBox;
}

void CTestMenu::testCMsgBoxShowMsg()
{
	ShowMsg2UTF("ShowMsg2UTF", "msgbox.cpp", CMsgBox::mbrBack, CMsgBox::mbBack);	// UTF-8
}

void CTestMenu::testCMsgBoxInfoBox()
{
	std::string buffer;
	
	// prepare print buffer  
	buffer = "CMsgBox";
	buffer += "\n";
	buffer += "CMsgBoxInfoBox";
	buffer += "\n";

	// thumbnail
	int pich = 246;	//FIXME
	int picw = 162; 	//FIXME
	int lx = g_settings.screen_StartX + 50 + g_settings.screen_EndX - g_settings.screen_StartX - 100 - (picw + 20);
	int ly = g_settings.screen_StartY + 50 + g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getHeight() + 20;
	
	std::string thumbnail = PLUGINDIR "/netzkino/netzkino.png";
	if(access(thumbnail.c_str(), F_OK))
		thumbnail = "";
	
	int mode =  CMsgBox::SCROLL | CMsgBox::TITLE | CMsgBox::FOOT | CMsgBox::BORDER;// | //CMsgBox::NO_AUTO_LINEBREAK | //CMsgBox::CENTER | //CMsgBox::AUTO_WIDTH | //CMsgBox::AUTO_HIGH;
	CBox position(g_settings.screen_StartX + 50, g_settings.screen_StartY + 50, g_settings.screen_EndX - g_settings.screen_StartX - 100, g_settings.screen_EndY - g_settings.screen_StartY - 100); 
	
	CMsgBox * msgBox = new CMsgBox("CMsgBox", g_Font[SNeutrinoSettings::FONT_TYPE_MENU], mode, &position, "CMsgBoxInfoBox", g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE], NULL);
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
	DisplayInfoMessage("InfoMessage");
}

void CTestMenu::testCMessageBoxErrorMsg()
{
	DisplayErrorMessage("ErrorMessage");
}

void CTestMenu::testCHintBox()
{
	CHintBox * hintBox = new CHintBox(LOCALE_MESSAGEBOX_INFO, "HintBox");
	
	hintBox->paint();
	sleep(3);
	hintBox->hide();
	delete hintBox;
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
	Box.iHeight = g_settings.screen_EndY - g_settings.screen_StartY - 20;
	
	CTextBox * textBox = new CTextBox(" ", NULL, CTextBox::SCROLL, &Box);
	
	std::string text = "qqqqqqqqqqqqqqqqwwwwwwwww";
		
	int pich, picw, lx, ly;
		
	std::string fname;

	fname = PLUGINDIR "/netzkino/netzkino.png";
		
	if(access(fname.c_str(), F_OK))
		fname = "";
		
	// display screenshot if exist
	pich = 320;
	picw = pich * (4.0 / 3);		// 4/3 format pics
	lx = Box.iX + Box.iWidth - picw - 10;
	ly = Box.iY + (Box.iHeight - pich)/2;
	
	textBox->setText(&text, fname, lx, ly, picw, pich);
	
	textBox->paint();
	
	sleep(3);
	
	delete textBox;
	textBox = NULL;
}

void CTestMenu::testCListFrameBox()
{
	CBox Box;
	
	Box.iX = g_settings.screen_StartX + 10;
	Box.iY = g_settings.screen_StartY + 10;
	Box.iWidth = g_settings.screen_EndX - g_settings.screen_StartX - 20;
	Box.iHeight = g_settings.screen_EndY - g_settings.screen_StartY - 20;
	
	LF_LINES listFrameLines;
	CListFrame * listFrame = new CListFrame(&listFrameLines, NULL, CListFrame::SCROLL | CListFrame::HEADER_LINE, &Box);
	
	listFrame->paint();
	sleep(3);
	delete listFrame;
	listFrame = NULL;
}

void CTestMenu::testCListBox()
{
	CListBox * listBox = new CListBox("listBox", MENU_WIDTH, MENU_HEIGHT);
	
	listBox->exec(NULL, "");
	delete listBox;
}

void CTestMenu::testCListBoxDetails()
{
	CListBox * listBox = new CListBox("listBox", MENU_WIDTH, MENU_HEIGHT, true);
	
	listBox->exec(NULL, "");
	delete listBox;
}

void CTestMenu::testCListBoxHeadInfo()
{
	CListBox * listBox = new CListBox("listBox", MENU_WIDTH, MENU_HEIGHT, true, true);
	
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

	if( CNeutrinoApp::getInstance()->getMode() == NeutrinoMessages::mode_radio )
	{
		if (!g_settings.radiotext_enable)
		{
			CFrameBuffer::getInstance()->loadBackgroundPic("radiomode.jpg");
			CFrameBuffer::getInstance()->blit();	
		}
	}
}

void CTestMenu::testCallMovieBrowser()
{
	moviePlayerGui->exec(NULL, "moviebrowser");

	if( CNeutrinoApp::getInstance()->getMode() == NeutrinoMessages::mode_radio )
	{
		if (!g_settings.radiotext_enable)
		{
			CFrameBuffer::getInstance()->loadBackgroundPic("radiomode.jpg");
			CFrameBuffer::getInstance()->blit();	
		}
	}
}

void CTestMenu::testCallFilePlayBack()
{
	moviePlayerGui->exec(NULL, "fileplayback");

	if( CNeutrinoApp::getInstance()->getMode() == NeutrinoMessages::mode_radio )
	{
		if (!g_settings.radiotext_enable)
		{
			CFrameBuffer::getInstance()->loadBackgroundPic("radiomode.jpg");
			CFrameBuffer::getInstance()->blit();	
		}
	}
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

int CTestMenu::exec(CMenuTarget* parent, const std::string& actionKey)
{
	int res = menu_return::RETURN_REPAINT;
	dprintf(DEBUG_NORMAL, "CTestMenu::exec: actionKey:%s", actionKey.c_str());
	
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
	else if(actionKey == "msgbox")
	{
		testCMsgBox();
		return res;
	}
	else if(actionKey == "msgboxshowmsg")
	{
		testCMsgBoxShowMsg();
		return res;
	}
	else if(actionKey == "msgboxinfobox")
	{
		testCMsgBoxInfoBox();
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
	else if(actionKey == "listboxheadinfo")
	{
		testCListBoxHeadInfo();
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
	mainMenu->addItem(new CMenuForwarderNonLocalized("CMsgBox", true, NULL, this, "msgbox"));
	mainMenu->addItem(new CMenuForwarderNonLocalized("CMsgBoxShowMsg", true, NULL, this, "msgboxshowmsg"));
	mainMenu->addItem(new CMenuForwarderNonLocalized("CMsgBoxInfoBox", true, NULL, this, "msgboxinfobox"));
	mainMenu->addItem(new CMenuForwarderNonLocalized("CMessageBox", true, NULL, this, "messagebox"));
	mainMenu->addItem(new CMenuForwarderNonLocalized("CMessageBoxInfoMsg", true, NULL, this, "messageboxinfomsg"));
	mainMenu->addItem(new CMenuForwarderNonLocalized("CMessageBoxErrorMsg", true, NULL, this, "messageboxerrormsg"));
	mainMenu->addItem(new CMenuForwarderNonLocalized("CHintBox", true, NULL, this, "hintbox"));
	mainMenu->addItem(new CMenuForwarderNonLocalized("CHintBoxExt", true, NULL, this, "hintboxext"));
	mainMenu->addItem(new CMenuForwarderNonLocalized("CHelpBox", true, NULL, this, "helpbox"));
	mainMenu->addItem(new CMenuForwarderNonLocalized("CTextBox", true, NULL, this, "textbox"));
	mainMenu->addItem(new CMenuForwarderNonLocalized("CListFrameBox", true, NULL, this, "listframebox"));
	mainMenu->addItem(new CMenuForwarderNonLocalized("CListBox", true, NULL, this, "listbox"));
	mainMenu->addItem(new CMenuForwarderNonLocalized("CListBoxInfoDetails", true, NULL, this, "listboxdetails"));
	mainMenu->addItem(new CMenuForwarderNonLocalized("CListBoxHeadInfo", true, NULL, this, "listboxheadinfo"));
	mainMenu->addItem(new CMenuForwarderNonLocalized("AudioPlayer", true, NULL, this, "audioplayer"));
	mainMenu->addItem(new CMenuForwarderNonLocalized("InternetRadio", true, NULL, this, "internetradio"));
	mainMenu->addItem(new CMenuForwarderNonLocalized("TSMovieBrowser", true, NULL, this, "tsmoviebrowser"));
	mainMenu->addItem(new CMenuForwarderNonLocalized("MovieBrowser", true, NULL, this, "moviebrowser"));
	mainMenu->addItem(new CMenuForwarderNonLocalized("FilePlayBack", true, NULL, this, "fileplayback"));
	mainMenu->addItem(new CMenuForwarderNonLocalized("PictureViewer", true, NULL, this, "pictureviewer"));
	mainMenu->addItem(new CMenuForwarderNonLocalized("UPNPBrowser", true, NULL, this, "upnpbrowser"));
	
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


