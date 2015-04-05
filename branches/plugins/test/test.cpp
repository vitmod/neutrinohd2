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
	dprintf(DEBUG_NORMAL, "Plugins: starting testMenu\n");
	
	// CStringInput
	std::string value;
	CStringInput * stringInput = new CStringInput("CStringInput", (char *)value.c_str());
	
	//stringInput->exec(NULL, "");
	//strinInput->hide();
	//delete stringInput;
	
	// CStringinputSMS
	//std::string value;
	CStringInputSMS * stringInputSMS = new CStringInputSMS("CStringInputSMS", (char *)value.c_str());
	
	//stringInputSMS->exec(NULL, "");
	//stringInputSMS->hide();
	//delete stringInputSMS;
	
	// CPINInput
	//std::string value;
	CPINInput * pinInput = new CPINInput("CPINInput", (char *)value.c_str());
	
	//pinInput->exec(NULL, "");
	//pinInput->hide();
	//delete pinInput;
	
	// msgbox.cpp
	CMsgBox * msgBox = new CMsgBox("CMsgBox");
	
	msgBox->exec(5, true);
	msgBox->hide();
	delete msgBox;
	
	ShowMsg2UTF("ShowMsg2UTF", "msgbox.cpp", CMsgBox::mbrBack, CMsgBox::mbBack);	// UTF-8
	
	// messagebox.cpp
	CMessageBox * messageBox = new CMessageBox(LOCALE_MESSAGEBOX_ERROR, "CMessagebox.cpp");
	
	messageBox->exec(5);
	messageBox->hide();
	delete messageBox;
	// infomsg
	DisplayInfoMessage("InfoMessage");
	// errormsg
	DisplayErrorMessage("ErrorMessage");
	
	// hintbox.cpp
	CHintBox * hintBox = new CHintBox(LOCALE_MESSAGEBOX_INFO, "HintBox");
	
	hintBox->paint();
	sleep(5);
	hintBox->hide();
	delete hintBox;
	
	// helpbox.cpp
	
	Helpbox * helpBox = new Helpbox();
	
	helpBox->addLine(NEUTRINO_ICON_BUTTON_RED, "helpBox");
	helpBox->addLine("HELPBOX");
	helpBox->addLine("");
	helpBox->addPagebreak();
	helpBox->show(LOCALE_MESSAGEBOX_INFO);
	
	delete helpBox;
	
	// lisbox.cpp
	CListBox * listBox = new CListBox("listBox)");
	
	//listBox->exec(NULL, "");
	//delete listBox;
	
	// textbox.cpp
	
	// listframe.cpp
	
	// testmenu
	CMenuWidget * testMenu = new CMenuWidget("testMenu",NEUTRINO_ICON_BUTTON_SETUP);
	
	testMenu->addItem(new CMenuForwarderNonLocalized("stringInput", true, NULL, stringInput, NULL));
	testMenu->addItem(new CMenuForwarderNonLocalized("stringInputSMS", true, NULL, stringInputSMS, NULL));
	testMenu->addItem(new CMenuForwarderNonLocalized("PINInput", true, NULL, pinInput, NULL));
	testMenu->addItem(new CMenuForwarderNonLocalized("listBox", true, NULL, listBox, NULL));
	
	testMenu->exec(NULL, "");
	testMenu->hide();
	
	delete stringInput;
	delete stringInputSMS;
	delete pinInput;
	delete listBox;
	
	delete testMenu;
}


