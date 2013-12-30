#include <plugin.h>


extern "C" int plugin_exec(void);

int plugin_exec(void)
{
	printf("Plugins: starting Movieplayer\n");
	
	CMenuWidget * moviePlayerMenu = new CMenuWidget("Movie player", NEUTRINO_ICON_SETTINGS);
	
	// ts player
	moviePlayerMenu->addItem(new CMenuForwarderNonLocalized("ts player", true, NULL, moviePlayerGui, "tsmoviebrowser", CRCInput::convertDigitToKey(1)));
	
	// movie player
	moviePlayerMenu->addItem(new CMenuForwarderNonLocalized("movies player", true, NULL, moviePlayerGui, "moviebrowser", CRCInput::convertDigitToKey(2)));

	// file player
	moviePlayerMenu->addItem(new CMenuForwarderNonLocalized("file player", true, NULL, moviePlayerGui, "fileplayback", CRCInput::convertDigitToKey(3)));	
	
	// vlc client player
	moviePlayerMenu->addItem(new CMenuForwarderNonLocalized("vlc player", true, NULL, moviePlayerGui, "vlcplayback", CRCInput::convertDigitToKey(4)));
	
	moviePlayerMenu->exec(NULL, "");
	moviePlayerMenu->hide();
	
	delete moviePlayerMenu;
	
	return 0;
}


