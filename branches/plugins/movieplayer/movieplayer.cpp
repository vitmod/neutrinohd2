#include <plugin.h>


extern "C" int plugin_exec(void);

int plugin_exec(void)
{
	printf("Plugins: starting Movieplayer\n");
	
	CMenuWidget *moviePlayerMenu = new CMenuWidget(LOCALE_MAINMENU_MEDIAPLAYER, NEUTRINO_ICON_SETTINGS);
	
	// ts player
	moviePlayerMenu->addItem(new CMenuForwarderItemMenuIconNonLocalized("ts player", true, NULL, moviePlayerGui, "tsmoviebrowser", CRCInput::convertDigitToKey(1), NULL, NEUTRINO_ICON_MOVIEPLAYER));
	
	// movie player
	moviePlayerMenu->addItem(new CMenuForwarderItemMenuIconNonLocalized("movies player", true, NULL, moviePlayerGui, "moviebrowser", CRCInput::convertDigitToKey(2), NULL, NEUTRINO_ICON_MOVIEPLAYER));

	// file player
	moviePlayerMenu->addItem(new CMenuForwarderItemMenuIconNonLocalized("file player", true, NULL, moviePlayerGui, "fileplayback", CRCInput::convertDigitToKey(3), NULL, NEUTRINO_ICON_MOVIEPLAYER));	
	
	// vlc client player
	moviePlayerMenu->addItem(new CMenuForwarderItemMenuIconNonLocalized("vlc player", true, NULL, moviePlayerGui, "vlcplayback", CRCInput::convertDigitToKey(4), NULL, NEUTRINO_ICON_VLC));
	
	// youtube player
	moviePlayerMenu->addItem(new CMenuForwarderItemMenuIconNonLocalized("youtube player", true, NULL, moviePlayerGui, "ytplayback", CRCInput::convertDigitToKey(5), NULL, NEUTRINO_ICON_YT));
	
	// netzkino
	moviePlayerMenu->addItem(new CMenuForwarderItemMenuIconNonLocalized("netzkino.de player", true, NULL, moviePlayerGui, "netzkinoplayback", CRCInput::convertDigitToKey(6), NULL, NEUTRINO_ICON_NETZKINO));	
	
	moviePlayerMenu->exec(NULL, "");
	moviePlayerMenu->hide();
	
	return 0;
}


