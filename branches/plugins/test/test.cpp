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
	
	// menue.cpp
	/*
	CMenuWidget * TestMenu = new CMenuWidget("menue.cpp",NEUTRINO_ICON_BUTTON_SETUP);
	
	TestMenu->addItem(new CMenuForwarderNonLocalized("VFD", true, NULL, NULL, "vfd"));
	TestMenu->addItem(new CMenuForwarderNonLocalized("Network", true, NULL, NULL, "network"));
	TestMenu->addItem(new CMenuForwarderNonLocalized("Smartcard", true, NULL, NULL, "card"));
	TestMenu->addItem(new CMenuForwarderNonLocalized("HDD", true, NULL, NULL, "hdd"));
	TestMenu->addItem(new CMenuForwarderNonLocalized("Buttons", true, NULL, NULL, "buttons"));
	TestMenu->addItem(new CMenuForwarderNonLocalized("Scan 12538000", true, NULL, NULL, "scan"));
	TestMenu->addItem(new CMenuForwarderNonLocalized("22 Khz ON", true, NULL, NULL, "22kon"));
	TestMenu->addItem(new CMenuForwarderNonLocalized("22 Khz OFF", true, NULL, NULL, "22koff"));
	
	TestMenu->exec(NULL, "");
	TestMenu->hide();
	delete TestMenu;
	*/
	
	// CStringInput
	/*
	std::string value;
	CStringInput * testMenu = new CStringInput("CStringInput", (char *)value.c_str());
	
	testMenu->exec(NULL, "");
	testMenu->hide();
	delete testMenu;
	*/
	
	// CStringinputSMS
	/*
	std::string value;
	CStringInputSMS * testMenu = new CStringInputSMS("CStringInputSMS", (char *)value.c_str());
	
	testMenu->exec(NULL, "");
	testMenu->hide();
	delete testMenu;
	*/
	
	// CPINInput
	/*
	std::string value;
	CPINInput * testMenu = new CPINInput("CPINInput", (char *)value.c_str());
	
	testMenu->exec(NULL, "");
	testMenu->hide();
	delete testMenu;
	*/
	
	// msgbox.cpp
	/*
	CMsgBox * testMenu = new CMsgBox("CMsgBox");
	
	testMenu->exec(5, true);
	testMenu->hide();
	delete testMenu;
	*/
	//ShowMsg2UTF("ShowMsg2UTF", "msgbox.cpp", CMsgBox::mbrBack, CMsgBox::mbBack);	// UTF-8
	
	// messagebox.cpp
	/*
	CMessageBox * testMenu = new CMessageBox(LOCALE_MESSAGEBOX_ERROR, "CMessagebox.cpp");
	
	testMenu->exec(5);
	testMenu->hide();
	delete testMenu;
	*/
	
	// infomsg
	//DisplayInfoMessage("InfoMessage");
	
	// errormsg
	//DisplayErrorMessage("ErrorMessage");
	
	// hintbox.cpp
	/*
	CHintBox * testMenu = new CHintBox(LOCALE_MESSAGEBOX_INFO, "HintBox");
	
	testMenu->paint();
	sleep(5);
	testMenu->hide();
	delete testMenu;
	*/
	
	// helpbox.cpp
	/*
	Helpbox testMenu;
	testMenu.addLine(NEUTRINO_ICON_BUTTON_RED, "helpBox");
	testMenu.addLine("HELPBOX");
	testMenu.addLine("");
	testMenu.addPagebreak();
	testMenu.show(LOCALE_MESSAGEBOX_INFO);
	*/
	
	// lisbox.cpp
	CListBox * testMenu = new CListBox("listBox)");
	
	testMenu->exec(NULL, "");
	delete testMenu;
	
	// textbox.cpp
	
	// listframe.cpp
}


