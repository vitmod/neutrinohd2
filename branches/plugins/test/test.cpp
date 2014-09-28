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

class CTestMenu : public CMenuTarget
{
        public:
                int exec(CMenuTarget* parent,  const std::string &actionkey);
};

int CTestMenu::exec(CMenuTarget* parent, const std::string &actionKey)
{
	if(parent)
		parent->hide();

	printf("CTestMenu::exec: %s\n", actionKey.c_str());

	return menu_return::RETURN_REPAINT;
}

void plugin_exec(void)
{
	printf("Plugins: starting testMenu\n");
	
	/*
	
	CMenuWidget * TestMenu = new CMenuWidget("Test menu",NEUTRINO_ICON_BUTTON_SETUP);
	CTestMenu * testHandler = new CTestMenu();
	
	TestMenu->addItem(new CMenuForwarderNonLocalized("VFD", true, NULL, testHandler, "vfd"));
	TestMenu->addItem(new CMenuForwarderNonLocalized("Network", true, NULL, testHandler, "network"));
	TestMenu->addItem(new CMenuForwarderNonLocalized("Smartcard", true, NULL, testHandler, "card"));
	TestMenu->addItem(new CMenuForwarderNonLocalized("HDD", true, NULL, testHandler, "hdd"));
	TestMenu->addItem(new CMenuForwarderNonLocalized("Buttons", true, NULL, testHandler, "buttons"));
	//TestMenu->addItem(new CMenuForwarderNonLocalized("Scan 12538000", true, NULL, testHandler, "scan"));
	//TestMenu->addItem(new CMenuForwarderNonLocalized("22 Khz ON", true, NULL, testHandler, "22kon"));
	//TestMenu->addItem(new CMenuForwarderNonLocalized("22 Khz OFF", true, NULL, testHandler, "22koff"));
	
	TestMenu->exec(NULL, "");
	TestMenu->hide();
	
	delete testHandler;
	delete TestMenu;
	*/
	
	// test color map
	//CFrameBuffer::getInstance()->paintBackground();;

	
	int w = 50;
	int h = 50;
	
	/*
	for (uint8_t i = 0; i < 15; i++)
	{
		CFrameBuffer::getInstance()->paintBoxRel(g_settings.screen_StartX + 10 + w*i, g_settings.screen_StartY, w, h, CFrameBuffer::getInstance()->realcolor[i]);
	}
	
	CFrameBuffer::getInstance()->paintBoxRel(g_settings.screen_StartX + 10, g_settings.screen_StartY + h, w, h, CFrameBuffer::getInstance()->realcolor[0x0]);
	CFrameBuffer::getInstance()->paintBoxRel(g_settings.screen_StartX + 10 + w*1, g_settings.screen_StartY + h, w, h, CFrameBuffer::getInstance()->realcolor[0x1]);
	CFrameBuffer::getInstance()->paintBoxRel(g_settings.screen_StartX + 10 + w*2, g_settings.screen_StartY + h, w, h, CFrameBuffer::getInstance()->realcolor[0x2]);
	CFrameBuffer::getInstance()->paintBoxRel(g_settings.screen_StartX + 10 + w*3, g_settings.screen_StartY + h, w, h, CFrameBuffer::getInstance()->realcolor[0x3]);
	CFrameBuffer::getInstance()->paintBoxRel(g_settings.screen_StartX + 10 + w*4, g_settings.screen_StartY + h, w, h, CFrameBuffer::getInstance()->realcolor[0x4]);
	CFrameBuffer::getInstance()->paintBoxRel(g_settings.screen_StartX + 10 + w*5, g_settings.screen_StartY + h, w, h, CFrameBuffer::getInstance()->realcolor[0x5]);
	CFrameBuffer::getInstance()->paintBoxRel(g_settings.screen_StartX + 10 + w*6, g_settings.screen_StartY + h, w, h, CFrameBuffer::getInstance()->realcolor[0x6]);
	CFrameBuffer::getInstance()->paintBoxRel(g_settings.screen_StartX + 10 + w*7, g_settings.screen_StartY + h, w, h, CFrameBuffer::getInstance()->realcolor[0x7]);
	CFrameBuffer::getInstance()->paintBoxRel(g_settings.screen_StartX + 10 + w*8, g_settings.screen_StartY + h, w, h, CFrameBuffer::getInstance()->realcolor[0x8]);
	CFrameBuffer::getInstance()->paintBoxRel(g_settings.screen_StartX + 10 + w*9, g_settings.screen_StartY + h, w, h, CFrameBuffer::getInstance()->realcolor[0x9]);
	CFrameBuffer::getInstance()->paintBoxRel(g_settings.screen_StartX + 10 + w*10, g_settings.screen_StartY + h, w, h, CFrameBuffer::getInstance()->realcolor[0x10]);
	CFrameBuffer::getInstance()->paintBoxRel(g_settings.screen_StartX + 10 + w*11, g_settings.screen_StartY  + h, w, h, CFrameBuffer::getInstance()->realcolor[0x11]);
	CFrameBuffer::getInstance()->paintBoxRel(g_settings.screen_StartX + 10 + w*12, g_settings.screen_StartY + h, w, h, CFrameBuffer::getInstance()->realcolor[0xA]);
	CFrameBuffer::getInstance()->paintBoxRel(g_settings.screen_StartX + 10 + w*13, g_settings.screen_StartY + h, w, h, CFrameBuffer::getInstance()->realcolor[0xB]);
	CFrameBuffer::getInstance()->paintBoxRel(g_settings.screen_StartX + 10 + w*14, g_settings.screen_StartY + h, w, h, CFrameBuffer::getInstance()->realcolor[0xC]);
	CFrameBuffer::getInstance()->paintBoxRel(g_settings.screen_StartX + 10 + w*15, g_settings.screen_StartY + h, w, h, CFrameBuffer::getInstance()->realcolor[0xE]);
	CFrameBuffer::getInstance()->paintBoxRel(g_settings.screen_StartX + 10 + w*16, g_settings.screen_StartY + h, w, h, CFrameBuffer::getInstance()->realcolor[0xD]);
	CFrameBuffer::getInstance()->paintBoxRel(g_settings.screen_StartX + 10 + w*17, g_settings.screen_StartY + h, w, h, CFrameBuffer::getInstance()->realcolor[0xFF]);
	
	
	CFrameBuffer::getInstance()->blit();
	*/
	
	neutrino_msg_t msg;
	neutrino_msg_data_t data;
	
	uint8_t index = 0;
	bool loop = true;
	
	CFrameBuffer::getInstance()->paintBoxRel(g_settings.screen_StartX + 10, g_settings.screen_StartY + h, w, h, CFrameBuffer::getInstance()->realcolor[index]);
	CFrameBuffer::getInstance()->blit();
		
	while (loop)
	{
		g_RCInput->getMsg_ms(&msg, &data, 100);
		
		if(msg == CRCInput::RC_up)
		{
			index += 1;
			dprintf(DEBUG_NORMAL, "CTest: index:%d\n", index);
		}
		else if (msg == CRCInput::RC_down)
		{
			index -= 1;
			dprintf(DEBUG_NORMAL, "CTest: index:%d\n", index);
		}
		else if (msg == CRCInput::RC_home)
		{
			loop = false;
		}
		else if (CNeutrinoApp::getInstance()->handleMsg(msg, data) & messages_return::cancel_all)
		{
			dprintf(DEBUG_NORMAL, "CTest: forward events to neutrino\n");
				
			loop = false;
		}
		
		CFrameBuffer::getInstance()->paintBoxRel(g_settings.screen_StartX + 10, g_settings.screen_StartY + 10, w, h, CFrameBuffer::getInstance()->realcolor[index]);
		CFrameBuffer::getInstance()->blit();	
	}
}


