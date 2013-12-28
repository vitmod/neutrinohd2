#include <plugin.h>


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
	
	if(actionKey == "vfd") 
	{
		CVFD::getInstance()->Clear();
		int icon = 0x00040000;
		while(icon > 0x2) 
		{
			CVFD::getInstance()->ShowIcon((vfd_icon) icon, true);
			icon /= 2;
		}
		
		for(int i = 0x01000001; i <= 0x0C000001; i+= 0x01000000) 
		{
			CVFD::getInstance()->ShowIcon((vfd_icon) i, true);
		}
		
		CVFD::getInstance()->ShowIcon((vfd_icon) 0x09000002, true);
		CVFD::getInstance()->ShowIcon((vfd_icon) 0x0B000002, true);
		char text[255];
		char buf[XML_UTF8_ENCODE_MAX];
		int ch = 0x2588;
		int len = XmlUtf8Encode(ch, buf);

		for(int i = 0; i < 12; i++) 
		{
			memcpy(&text[i*len], buf, len);
		}
		text[12*len] = 0;

		CVFD::getInstance()->ShowText(text);
		
		ShowMsgUTF(LOCALE_MESSAGEBOX_INFO, "VFD test, Press OK to return", CMessageBox::mbrBack, CMessageBox::mbBack, "info");
		
		CVFD::getInstance()->Clear();
	}
	else if(actionKey == "network") 
	{
		int fd, ret;
		struct ifreq ifr;
		char * ip = NULL, str[255];
		struct sockaddr_in *addrp=NULL;

		fd = socket(AF_INET, SOCK_DGRAM, 0);

		ifr.ifr_addr.sa_family = AF_INET;
		strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);

		ret = ioctl(fd, SIOCGIFHWADDR, &ifr);
		if(ret < 0)
			perror("SIOCGIFHWADDR");

		ret = ioctl(fd, SIOCGIFADDR, &ifr );
		if(ret < 0)
			perror("SIOCGIFADDR");
		else {
			addrp = (struct sockaddr_in *)&(ifr.ifr_addr);
			ip = inet_ntoa(addrp->sin_addr);
		}

		sprintf(str, "MAC: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\nIP: %s",
				(unsigned char)ifr.ifr_hwaddr.sa_data[0],
				(unsigned char)ifr.ifr_hwaddr.sa_data[1],
				(unsigned char)ifr.ifr_hwaddr.sa_data[2],
				(unsigned char)ifr.ifr_hwaddr.sa_data[3],
				(unsigned char)ifr.ifr_hwaddr.sa_data[4],
				(unsigned char)ifr.ifr_hwaddr.sa_data[5], ip == NULL ? "Unknown" : ip);

		close(fd);
		ShowMsgUTF(LOCALE_MESSAGEBOX_INFO, str, CMessageBox::mbrBack, CMessageBox::mbBack, "info");
	}
	else if(actionKey == "card") {
	}
	else if(actionKey == "hdd") {
		char buffer[255];
		FILE *f = fopen("/proc/mounts", "r");
		bool mounted = false;
		if(f != NULL) {
			while (fgets (buffer, 255, f) != NULL) {
				if(strstr(buffer, "/dev/sda1")) {
					mounted = true;
					break;
				}
			}
			fclose(f);
		}
		sprintf(buffer, "HDD: /dev/sda1 is %s", mounted ? "mounted" : "NOT mounted");
		printf("%s\n", buffer);
		ShowMsgUTF(LOCALE_MESSAGEBOX_INFO, buffer, CMessageBox::mbrBack, CMessageBox::mbBack, "info");
	}
	else if(actionKey == "buttons") {
		neutrino_msg_t      msg;
		neutrino_msg_data_t data;
		CHintBox * khintBox = NULL;
		CHintBox * hintBox = new CHintBox(LOCALE_MESSAGEBOX_INFO, "Press button, or press EXIT to return");
		hintBox->paint();
		while(1) {
			g_RCInput->getMsg(&msg, &data, 100);
			if(msg == CRCInput::RC_home)
				break;

			if (msg != CRCInput::RC_timeout && msg <= CRCInput::RC_MaxRC) {
				char keyname[50];
				sprintf(keyname, "Button [%s] pressed (EXIT to return)", g_RCInput->getKeyName(msg).c_str());
				if(khintBox) {
					delete khintBox;
				}
				khintBox = new CHintBox(LOCALE_MESSAGEBOX_INFO, keyname);
				hintBox->hide();
				khintBox->paint();
			}
		}
		if(khintBox)
			delete khintBox;
		delete hintBox;
	}
	/*
	else if(actionKey == "22kon" || actionKey == "22koff") {
		CScanTs * scanTs = new CScanTs();

		int freq = (actionKey == "22kon") ? 12000*1000: 11000*1000;

                sprintf(get_set.TP_freq, "%d", freq);
#if 0 // not needed ?
                switch(frontend->getInfo()->type) {
                        case FE_QPSK:
                                sprintf(get_set.TP_rate, "%d", tmpI->second.feparams.u.qpsk.symbol_rate);
                                get_set.TP_fec = tmpI->second.feparams.u.qpsk.fec_inner;
                                get_set.TP_pol = tmpI->second.polarization;
                                break;
                        case FE_QAM:
                                sprintf(get_set.TP_rate, "%d", tmpI->second.feparams.u.qam.symbol_rate);
                                get_set.TP_fec = tmpI->second.feparams.u.qam.fec_inner;
                                get_set.TP_mod = tmpI->second.feparams.u.qam.modulation;
                                break;
		}
#endif
		scanTs->exec(NULL, "test");
		delete scanTs;
	}
	*/
	/*
        else if(actionKey == "scan") {
                CScanTs * scanTs = new CScanTs();

                int freq = 12538000;
                sprintf(get_set.TP_freq, "%d", freq);
                switch(frontend->getInfo()->type) {
                        case FE_QPSK:
                                sprintf(get_set.TP_rate, "%d", 41250*1000);
                                get_set.TP_fec = 1;
                                get_set.TP_pol = 1;
                                break;
                        case FE_QAM:
#if 0
                                sprintf(get_set.TP_rate, "%d", tmpI->second.feparams.u.qam.symbol_rate);
                                get_set.TP_fec = tmpI->second.feparams.u.qam.fec_inner;
                                get_set.TP_mod = tmpI->second.feparams.u.qam.modulation;
#endif
                                break;
			case FE_OFDM:
			case FE_ATSC:
				break;
                }
                scanTs->exec(NULL, "manual");
                delete scanTs;
        }
        */

	return menu_return::RETURN_REPAINT;
}

extern "C" int plugin_exec(void);

int plugin_exec(void)
{
	printf("Plugins: starting testMenu\n");
	
	CMenuWidget * TestMenu = new CMenuWidget("Test menu");
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
	
	return 0;
}


