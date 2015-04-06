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

void plugin_exec(void)
{
	// CStringInput
	/*
	std::string value;
	CStringInput * stringInput = new CStringInput("CStringInput", (char *)value.c_str());
	
	stringInput->exec(NULL, "");
	strinInput->hide();
	delete stringInput;
	*/
	
	// CStringinputSMS
	/*
	std::string value;
	CStringInputSMS * stringInputSMS = new CStringInputSMS("CStringInputSMS", (char *)value.c_str());
	
	stringInputSMS->exec(NULL, "");
	stringInputSMS->hide();
	delete stringInputSMS;
	*/
	
	// CPINInput
	/*
	std::string value;
	CPINInput * pinInput = new CPINInput("CPINInput", (char *)value.c_str());
	
	pinInput->exec(NULL, "");
	pinInput->hide();
	delete pinInput;
	*/
	
	// msgbox.cpp
	/*
	int mode =  CMsgBox::SCROLL | CMsgBox::TITLE | CMsgBox::FOOT | CMsgBox::BORDER;// | //CMsgBox::NO_AUTO_LINEBREAK | //CMsgBox::CENTER | //CMsgBox::AUTO_WIDTH | //CMsgBox::AUTO_HIGH;
	CBox position(g_settings.screen_StartX + 50, g_settings.screen_StartY + 50, g_settings.screen_EndX - g_settings.screen_StartX - 100, g_settings.screen_EndY - g_settings.screen_StartY - 100); 
	
	CMsgBox * msgBox = new CMsgBox("ShowMsg2UTF", g_Font[SNeutrinoSettings::FONT_TYPE_MENU], mode, &position, "msgbox.cpp", g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE], NULL);
	//CMsgBox * msgBox = new CMsgBox("CMsgBox");
	
	msgBox->exec(3, true);
	msgBox->hide();
	delete msgBox;
	*/
	
	//ShowMsg2UTF("ShowMsg2UTF", "msgbox.cpp", CMsgBox::mbrBack, CMsgBox::mbBack);	// UTF-8
	
	// messagebox.cpp
	/*
	CMessageBox * messageBox = new CMessageBox(LOCALE_MESSAGEBOX_ERROR, "CMessagebox.cpp");
	
	messageBox->exec(3);
	messageBox->hide();
	delete messageBox;
	*/
	// infomsg
	//DisplayInfoMessage("InfoMessage");
	// errormsg
	//DisplayErrorMessage("ErrorMessage");
	
	// hintbox.cpp
	/*
	CHintBox * hintBox = new CHintBox(LOCALE_MESSAGEBOX_INFO, "HintBox");
	
	hintBox->paint();
	sleep(3);
	hintBox->hide();
	delete hintBox;
	*/
	
	// helpbox.cpp
	/*
	Helpbox * helpBox = new Helpbox();
	
	helpBox->addLine(NEUTRINO_ICON_BUTTON_RED, "helpBox");
	helpBox->addLine("HELPBOX");
	helpBox->addLine("");
	helpBox->addPagebreak();
	helpBox->show(LOCALE_MESSAGEBOX_INFO);
	
	delete helpBox;
	*/
	
	// lisbox.cpp
	CListBox * listBox = new CListBox("listBox)");
	
	listBox->exec(NULL, "");
	delete listBox;
	
	// textbox.cpp
	/*
	CBox Box;
	
	Box.iX = g_settings.screen_StartX + 10;
	Box.iY = g_settings.screen_StartY + 10;
	Box.iWidth = g_settings.screen_EndX - g_settings.screen_StartX - 20;
	Box.iHeight = g_settings.screen_EndY - g_settings.screen_StartY - 20;
	
	CTextBox * textBox = new CTextBox(" ", NULL, CTextBox::SCROLL, &Box);
	
	std::string text = "qqqqqqqqqqqqqqqqwwwwwwwww";
	
	bool logo_ok = false;
		
	int pich, picw, lx, ly;
		
	std::string fname;

	fname = DATADIR "/neutrino/icons/pictureviewer.png";
		
	logo_ok = !access(fname.c_str(), F_OK);
		
	// display screenshot if exists
	if(logo_ok) 
	{
		pich = 320;
		picw = pich * (4.0 / 3);		// 4/3 format pics
		lx = Box.iX + Box.iWidth - picw - 10;
		ly = Box.iY + (Box.iHeight - pich)/2;
	}
	
	textBox->setText(&text, Box.iWidth - picw - 20, fname, lx, ly, picw, pich);
	
	textBox->paint();
	
	sleep(3);
	
	delete textBox;
	textBox = NULL;
	*/
	
	// listframe.cpp
	/*
	LF_LINES listFrameLines;
	CListFrame * listFrame = new CListFrame(&listFrameLines, NULL, CListFrame::SCROLL | CListFrame::HEADER_LINE, &Box);
	
	listFrame->paint();
	sleep(3);
	delete listFrame;
	listFrame = NULL;
	*/
	
	// menue.cpp
	/*
	CMenuWidget * testMenu = new CMenuWidget("testMenu",NEUTRINO_ICON_BUTTON_SETUP);
	
	testMenu->addItem(new CMenuForwarderNonLocalized("CMenuForwarderNonLocalized", true, NULL, NULL, NULL));
	testMenu->addItem(new CMenuForwarderNonLocalized("CMenuForwarderNonLocalized", true, NULL, NULL, NULL));
	testMenu->addItem(new CMenuForwarderNonLocalized("CMenuForwarderNonLocalized", true, NULL, NULL, NULL));
	testMenu->addItem(new CMenuForwarderNonLocalized("CMenuForwarderNonLocalized", true, NULL, NULL, NULL));
	
	testMenu->exec(NULL, "");
	testMenu->hide();
	delete testMenu;
	*/
}


