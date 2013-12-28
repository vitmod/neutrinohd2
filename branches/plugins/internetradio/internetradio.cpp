#include <plugin.h>


extern "C" int plugin_exec(void);

int plugin_exec(void)
{
	printf("Plugins: starting internetradio\n");
	
	CAudioPlayerGui tmpAudioPlayerGui(true);
	tmpAudioPlayerGui.exec(NULL, "");
	
	return 0;
}


