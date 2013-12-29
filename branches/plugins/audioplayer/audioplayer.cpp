#include <plugin.h>


extern "C" int plugin_exec(void);

int plugin_exec(void)
{
	printf("Plugins: starting audioplayer\n");
	
	CAudioPlayerGui tmpAudioPlayerGui;
	tmpAudioPlayerGui.exec(NULL, "");
	
	return 0;
}


