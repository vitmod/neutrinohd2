#include <plugin.h>

#include <netzkino.h>

#include <config.h>
#include <global.h>
#include <driver/framebuffer.h>
#include <gui/color.h>
#include <gui/filebrowser.h>
#include <gui/movieplayer.h>


extern CMoviePlayerGui * moviePlayerGui;	// defined in neutrino.cpp

#ifdef __cplusplus
extern "C" {
void plugin_exec(PluginParam *par)
{
	printf("Plugins: starting netzkino.de player\n");
	
	CFrameBuffer::getInstance()->paintBackground();
	
	// start netzkino
	moviePlayerGui->exec(NULL, "netzkinoplayback");
}
}
#endif
