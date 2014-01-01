#include <plugin.h>


class CFBCallMonitor : public CMenuTarget
{
        public:
		void ReadSettings();
		bool SaveSettings();
		
		int exec(CMenuTarget *parent,  const std::string &actionkey);
};

//int FB_AUTOSTART;
std::string FB_IP_STRG = "";
std::string FB_PORT_STRG = "";
std::string FB_ZIEL1_STRG = "";
std::string FB_ZIEL1N_STRG = "";
std::string FB_ZIEL2_STRG = "";
std::string FB_ZIEL2N_STRG = "";
std::string FB_ZIEL3_STRG = "";
std::string FB_ZIEL3N_STRG = "";
std::string FB_BOXIP_STRG = "";
std::string FB_BOXUSERNAME_STRG = "";
std::string FB_BOXPASSWORD_STRG = "";
int FB_DEBUG;
int FB_ALLE;
int FB_MONRING;
int FB_MONDISCONNECT;
int FB_MUTERING;
int FB_POPUP;
int FB_INVERS;

// option off0_on1
#define OPTIONS_OFF0_ON1_OPTION_COUNT 2
const CMenuOptionChooser::keyval OPTIONS_OFF0_ON1_OPTIONS[OPTIONS_OFF0_ON1_OPTION_COUNT] =
{
        { 0, LOCALE_OPTIONS_OFF, NULL },
        { 1, LOCALE_OPTIONS_ON, NULL }
};


void CFBCallMonitor::ReadSettings() 
{
	CConfigFile *bpfbconfig = new CConfigFile(',');
	bpfbconfig->clear();
	bpfbconfig->loadConfig(PLUGINDIR "/fb.conf");
	
	FB_IP_STRG = bpfbconfig->getString("FRITZBOXIP", "fritz.box");
	FB_PORT_STRG = bpfbconfig->getString("TELDPORT", "1012");
	FB_ZIEL1_STRG = bpfbconfig->getString("Ziel_1", "01234567890");
	FB_ZIEL1N_STRG = bpfbconfig->getString("Ziel_1_name", "Nummer_1");
	FB_ZIEL2_STRG = bpfbconfig->getString("Ziel_2", "01234567890");
	FB_ZIEL2N_STRG = bpfbconfig->getString("Ziel_2_name", "Nummer_2");
	FB_ZIEL3_STRG = bpfbconfig->getString("Ziel_3", "01234567890");
	FB_ZIEL3N_STRG = bpfbconfig->getString("Ziel_3_name", "Nummer_3");
	FB_BOXIP_STRG = bpfbconfig->getString("ip", "127.0.0.1");
	FB_BOXUSERNAME_STRG = bpfbconfig->getString("loginname", "root");
	FB_BOXPASSWORD_STRG = bpfbconfig->getString("passwort", "root");
	FB_DEBUG = bpfbconfig->getInt32("debug", 0);
	FB_ALLE = bpfbconfig->getInt32("Alle", 0);
	FB_MONRING = bpfbconfig->getInt32("monRing", 1);
	FB_MONDISCONNECT = bpfbconfig->getInt32("monDisconnect", 1);
	FB_MUTERING = bpfbconfig->getInt32("muteRing", 1);
	FB_POPUP = bpfbconfig->getInt32("popup", 0);
	FB_INVERS = bpfbconfig->getInt32("invers", 1);
}

bool CFBCallMonitor::SaveSettings() 
{
	CConfigFile *bpfbconfig = new CConfigFile(',');
	bpfbconfig->setString("FRITZBOXIP", FB_IP_STRG);
	bpfbconfig->setString("TELDPORT", FB_PORT_STRG);
	bpfbconfig->setString("Ziel_1", FB_ZIEL1_STRG);
	bpfbconfig->setString("Ziel_1_name", FB_ZIEL1N_STRG);
	bpfbconfig->setString("Ziel_2", FB_ZIEL2_STRG);
	bpfbconfig->setString("Ziel_2_name", FB_ZIEL2N_STRG);
	bpfbconfig->setString("Ziel_3", FB_ZIEL3_STRG);
	bpfbconfig->setString("Ziel_3_name", FB_ZIEL3N_STRG);
	bpfbconfig->setString("ip", FB_BOXIP_STRG);
	bpfbconfig->setString("loginname", FB_BOXUSERNAME_STRG);
	bpfbconfig->setString("passwort", FB_BOXPASSWORD_STRG);
	bpfbconfig->setInt32("debug", FB_DEBUG);
	bpfbconfig->setInt32("Alle", FB_ALLE);
	bpfbconfig->setInt32("monRing", FB_MONRING);
	bpfbconfig->setInt32("monDisconnect", FB_MONDISCONNECT);
	bpfbconfig->setInt32("muteRing", FB_MUTERING);
	bpfbconfig->setInt32("popup", FB_POPUP);
	bpfbconfig->setInt32("invers", FB_INVERS);
	bpfbconfig->saveConfig(PLUGINDIR "/fb.conf");
	
	return true;
}

int CFBCallMonitor::exec(CMenuTarget* parent, const std::string &actionKey)
{
	if(parent)
		parent->hide();
	
	if(actionKey == "save")
	{
		//SaveSettings();
		if(this->SaveSettings())
		 	ShowHintUTF(LOCALE_MESSAGEBOX_INFO, "Einstellungen werden gespeichert !", 450, 2 );
		else
		 	ShowHintUTF(LOCALE_MESSAGEBOX_INFO, "Einstellungen NICHT gespeichert !", 450, 2 );
	}
	
	return menu_return::RETURN_REPAINT;
}

extern "C" int plugin_exec(void);

int plugin_exec(void)
{
	// read settings
	// menuhandler
	CFBCallMonitor * FBCallMonitorHandler = new CFBCallMonitor();
	FBCallMonitorHandler->ReadSettings();
	
	// create menu
	CMenuWidget * FritzBoxCallSettingsMenu = new CMenuWidget("FritzBoxCallMonitor", NEUTRINO_ICON_SETTINGS);

	//FritzBoxCallSettingsMenu->addItem(GenericMenuSeparator);
	FritzBoxCallSettingsMenu->addItem(GenericMenuBack);
	FritzBoxCallSettingsMenu->addItem(GenericMenuSeparatorLine);
	FritzBoxCallSettingsMenu->addItem(new CMenuForwarderNonLocalized("Einstellungen speichern", true, NULL, FBCallMonitorHandler, "save", CRCInput::RC_red, NEUTRINO_ICON_BUTTON_RED));
	FritzBoxCallSettingsMenu->addItem(GenericMenuSeparatorLine);
	
	// autostart
	//FritzBoxCallSettingsMenu->addItem(new CMenuOptionChooser("Autostart", &FB_AUTOSTART, OPTIONS_OFF0_ON1_OPTIONS, OPTIONS_OFF0_ON1_OPTION_COUNT, true, /*SaveSettingsNowDestinationChanger*/NULL));
	
	// fb ip
	CStringInputSMS * FB_IP = new CStringInputSMS((char *)"IP der Fritzbox", (char *)FB_IP_STRG.c_str());
	FritzBoxCallSettingsMenu->addItem(new CMenuForwarderNonLocalized("IP der Fritzbox", true, FB_IP_STRG, FB_IP, NULL));
	
	// fb port
	CStringInputSMS * FB_PORT = new CStringInputSMS((char *)"Port der Fritzbox", (char *)FB_PORT_STRG.c_str());
	FritzBoxCallSettingsMenu->addItem(new CMenuForwarderNonLocalized("PORT der Fritzbox", true, FB_PORT_STRG, FB_PORT, NULL));
	
	// debug
	FritzBoxCallSettingsMenu->addItem(new CMenuOptionChooser("Debug (nur in Telnet!)", &FB_DEBUG, OPTIONS_OFF0_ON1_OPTIONS, OPTIONS_OFF0_ON1_OPTION_COUNT, true, /*SaveSettingsNowDestinationChanger*/NULL));
	
	// ziel1
	CStringInputSMS * FB_ZIEL1 = new CStringInputSMS((char *)"Rufnummer 1", (char *)FB_ZIEL1_STRG.c_str());
	FritzBoxCallSettingsMenu->addItem(new CMenuForwarderNonLocalized("Rufnummer 1", true, FB_ZIEL1_STRG, FB_ZIEL1, NULL));
	
	CStringInputSMS * FB_ZIEL1N = new CStringInputSMS((char *)"Rufnummer 1 Name", (char *)FB_ZIEL1N_STRG.c_str());
	FritzBoxCallSettingsMenu->addItem(new CMenuForwarderNonLocalized("Rufnummer 1 Name", true, FB_ZIEL1N_STRG, FB_ZIEL1N, NULL));
	
	CStringInputSMS * FB_ZIEL2 = new CStringInputSMS((char *)"Rufnummer 2", (char *)FB_ZIEL2_STRG.c_str());
	FritzBoxCallSettingsMenu->addItem(new CMenuForwarderNonLocalized("Rufnummer 2", true, FB_ZIEL2_STRG, FB_ZIEL2, NULL));
	
	CStringInputSMS * FB_ZIEL2N = new CStringInputSMS((char *)"Rufnummer 2 Name", (char *)FB_ZIEL2N_STRG.c_str());
	FritzBoxCallSettingsMenu->addItem(new CMenuForwarderNonLocalized("Rufnummer 2 Name", true, FB_ZIEL2N_STRG, FB_ZIEL2N, NULL));
	
	CStringInputSMS * FB_ZIEL3 = new CStringInputSMS((char *)"Rufnummer 3", (char *)FB_ZIEL3_STRG.c_str());
	FritzBoxCallSettingsMenu->addItem(new CMenuForwarderNonLocalized("Rufnummer 3", true, FB_ZIEL3_STRG, FB_ZIEL3, NULL));
	
	CStringInputSMS * FB_ZIEL3N = new CStringInputSMS((char *)"Rufnummer 3 Name", (char *)FB_ZIEL3N_STRG.c_str());
	FritzBoxCallSettingsMenu->addItem(new CMenuForwarderNonLocalized("Rufnummer 3 Name", true, FB_ZIEL3N_STRG, FB_ZIEL3N, NULL));
	
	FritzBoxCallSettingsMenu->addItem(new CMenuOptionChooser("alle Rufnummern ueberwachen", &FB_ALLE, OPTIONS_OFF0_ON1_OPTIONS, OPTIONS_OFF0_ON1_OPTION_COUNT, true, /*SaveSettingsNowDestinationChanger*/NULL));
	FritzBoxCallSettingsMenu->addItem(new CMenuOptionChooser("eingehende Anrufe anzeigen", &FB_MONRING, OPTIONS_OFF0_ON1_OPTIONS, OPTIONS_OFF0_ON1_OPTION_COUNT, true, /*SaveSettingsNowDestinationChanger*/NULL));
	FritzBoxCallSettingsMenu->addItem(new CMenuOptionChooser("Ende+Dauer des Anrufs anzeigen", &FB_MONDISCONNECT, OPTIONS_OFF0_ON1_OPTIONS, OPTIONS_OFF0_ON1_OPTION_COUNT, true, /*SaveSettingsNowDestinationChanger*/NULL));
	FritzBoxCallSettingsMenu->addItem(new CMenuOptionChooser("Ton aus bei Anruf", &FB_MUTERING, OPTIONS_OFF0_ON1_OPTIONS, OPTIONS_OFF0_ON1_OPTION_COUNT, true, /*SaveSettingsNowDestinationChanger*/NULL));
	FritzBoxCallSettingsMenu->addItem(new CMenuOptionChooser("Popup statt normale Meldung", &FB_POPUP, OPTIONS_OFF0_ON1_OPTIONS, OPTIONS_OFF0_ON1_OPTION_COUNT, true, /*SaveSettingsNowDestinationChanger*/NULL));
	FritzBoxCallSettingsMenu->addItem(new CMenuOptionChooser("Inverssuche (GoYellow)", &FB_INVERS, OPTIONS_OFF0_ON1_OPTIONS, OPTIONS_OFF0_ON1_OPTION_COUNT, true, /*SaveSettingsNowDestinationChanger*/NULL));
	
	CStringInputSMS * FB_BOXIP = new CStringInputSMS((char *)"Box IP", (char *)FB_BOXIP_STRG.c_str());
	FritzBoxCallSettingsMenu->addItem(new CMenuForwarderNonLocalized("Box IP", true, FB_BOXIP_STRG, FB_BOXIP, NULL));
	
	CStringInputSMS * FB_BOXUSERNAME = new CStringInputSMS((char *)"Box Username", (char *)FB_BOXUSERNAME_STRG.c_str());
	FritzBoxCallSettingsMenu->addItem(new CMenuForwarderNonLocalized("Box Username", true, FB_BOXUSERNAME_STRG, FB_BOXUSERNAME, NULL));
	
	CStringInputSMS * FB_BOXPASSWORD = new CStringInputSMS((char *)"Box Passwort", (char *)FB_BOXPASSWORD_STRG.c_str());
	FritzBoxCallSettingsMenu->addItem(new CMenuForwarderNonLocalized("Box Passwort", true, FB_BOXPASSWORD_STRG, FB_BOXPASSWORD, NULL));

	FritzBoxCallSettingsMenu->exec(NULL, "");
	FritzBoxCallSettingsMenu->hide();
	
	delete FBCallMonitorHandler;
	
	return 0;
}

