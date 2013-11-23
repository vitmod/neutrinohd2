#include <plugin.h>

#include <test.h>

#include <config.h>
#include <global.h>
#include <driver/framebuffer.h>
#include <gui/color.h>
#include <gui/filebrowser.h>
#include <gui/movieplayer.h>
#include <gui/widget/messagebox.h>
#include <gui/widget/hintbox.h>


#ifdef __cplusplus
extern "C" {
void plugin_exec(PluginParam *par)
{
	printf("Plugins: starting test\n");
	
	CFrameBuffer::getInstance()->paintBackground();
	
	ShowHintUTF(LOCALE_MESSAGEBOX_INFO, (const char *) "this neutrino new plugins interface\n"); // UTF-8
}
}
#endif
