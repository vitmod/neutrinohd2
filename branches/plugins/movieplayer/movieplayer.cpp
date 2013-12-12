#include <plugin.h>


extern "C" int plugin_exec(void);

int plugin_exec(void)
{
	printf("Plugins: starting Movieplayer\n");
	
	CMenuWidget *moviePlayerMenu = new CMenuWidget(LOCALE_MAINMENU_MEDIAPLAYER, NEUTRINO_ICON_SETTINGS);
	
	// ts player
	moviePlayerMenu->addItem(new CMenuForwarderItemMenuIcon(LOCALE_MOVIEPLAYER_RECORDS, true, NULL, moviePlayerGui, "tsmoviebrowser", CRCInput::convertDigitToKey(1), NULL, NEUTRINO_ICON_MOVIEPLAYER, LOCALE_HELPTEXT_TSMOVIEBROWSER ));
	
	// movie player
	moviePlayerMenu->addItem(new CMenuForwarderItemMenuIcon(LOCALE_MOVIEPLAYER_MOVIES, true, NULL, moviePlayerGui, "moviebrowser", CRCInput::convertDigitToKey(2), NULL, NEUTRINO_ICON_MOVIEPLAYER, LOCALE_HELPTEXT_TSMOVIEBROWSER ));
	
	moviePlayerMenu->addItem( new CMenuSeparatorItemMenuIcon(CMenuSeparatorItemMenuIcon::LINE) );

	// file player
	moviePlayerMenu->addItem(new CMenuForwarderItemMenuIcon(LOCALE_MOVIEPLAYER_FILEPLAYBACK, true, NULL, moviePlayerGui, "fileplayback", CRCInput::convertDigitToKey(3), NULL, NEUTRINO_ICON_MOVIEPLAYER, LOCALE_HELPTEXT_FILEPLAYBACK ));	
	
	// vlc client player
	moviePlayerMenu->addItem(new CMenuForwarderItemMenuIcon(LOCALE_MOVIEPLAYER_VLCPLAYBACK, true, NULL, moviePlayerGui, "vlcplayback", CRCInput::convertDigitToKey(4), NULL, NEUTRINO_ICON_VLC, LOCALE_HELPTEXT_NETSTREAM ));
	
	// youtube player
	moviePlayerMenu->addItem(new CMenuForwarderItemMenuIcon(LOCALE_MOVIEPLAYER_YTPLAYBACK, true, NULL, moviePlayerGui, "ytplayback", CRCInput::convertDigitToKey(5), NULL, NEUTRINO_ICON_YT, LOCALE_HELPTEXT_NETSTREAM ));
	
	// netzkino
	moviePlayerMenu->addItem(new CMenuForwarderItemMenuIcon(LOCALE_WEBTV_NETZKINO, true, NULL, moviePlayerGui, "netzkinoplayback", CRCInput::convertDigitToKey(6), NULL, NEUTRINO_ICON_NETZKINO, LOCALE_HELPTEXT_NETSTREAM ));	
	
	moviePlayerMenu->exec(NULL, "");
	moviePlayerMenu->hide();
	
	return 0;
}


