#include <plugin.h>

#include <movieplayer.h>

#include <config.h>
#include <global.h>
#include <driver/framebuffer.h>
#include <gui/color.h>
#include <gui/filebrowser.h>
#include <gui/movieplayer.h>


#ifdef __cplusplus
extern "C" 
{
#endif

void plugin_exec(void)
{
	printf("Plugins: starting Movieplayer\n");
	
	CFrameBuffer::getInstance()->paintBackground();
	
	CMenuWidget *testMenu = new CMenuWidget(LOCALE_MAINMENU_MEDIAPLAYER, NEUTRINO_ICON_SETTINGS);
	
	// ts player
	testMenu->addItem(new CMenuForwarderItemMenuIcon(LOCALE_MOVIEPLAYER_RECORDS, true, NULL, moviePlayerGui, "tsmoviebrowser", CRCInput::convertDigitToKey(1), NULL, NEUTRINO_ICON_MOVIEPLAYER, LOCALE_HELPTEXT_TSMOVIEBROWSER ));
	
	// movie player
	testMenu->addItem(new CMenuForwarderItemMenuIcon(LOCALE_MOVIEPLAYER_MOVIES, true, NULL, moviePlayerGui, "moviebrowser", CRCInput::convertDigitToKey(2), NULL, NEUTRINO_ICON_MOVIEPLAYER, LOCALE_HELPTEXT_TSMOVIEBROWSER ));
	
	testMenu->addItem( new CMenuSeparatorItemMenuIcon(CMenuSeparatorItemMenuIcon::LINE) );

	// file player
	testMenu->addItem(new CMenuForwarderItemMenuIcon(LOCALE_MOVIEPLAYER_FILEPLAYBACK, true, NULL, moviePlayerGui, "fileplayback", CRCInput::convertDigitToKey(3), NULL, NEUTRINO_ICON_MOVIEPLAYER, LOCALE_HELPTEXT_FILEPLAYBACK ));	
	
	// vlc client player
	testMenu->addItem(new CMenuForwarderItemMenuIcon(LOCALE_MOVIEPLAYER_VLCPLAYBACK, true, NULL, moviePlayerGui, "vlcplayback", CRCInput::convertDigitToKey(4), NULL, NEUTRINO_ICON_VLC, LOCALE_HELPTEXT_NETSTREAM ));
	
	// youtube player
	testMenu->addItem(new CMenuForwarderItemMenuIcon(LOCALE_MOVIEPLAYER_YTPLAYBACK, true, NULL, moviePlayerGui, "ytplayback", CRCInput::convertDigitToKey(5), NULL, NEUTRINO_ICON_YT, LOCALE_HELPTEXT_NETSTREAM ));
	
	// netzkino
	testMenu->addItem(new CMenuForwarderItemMenuIcon(LOCALE_WEBTV_NETZKINO, true, NULL, moviePlayerGui, "netzkinoplayback", CRCInput::convertDigitToKey(6), NULL, NEUTRINO_ICON_NETZKINO, LOCALE_HELPTEXT_NETSTREAM ));	
	
	testMenu->exec(NULL, "");
	testMenu->hide();
}

#ifdef __cplusplus
}
#endif
