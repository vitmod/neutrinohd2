#include <plugin.h>
#include <netzkino.h>


extern "C" int plugin_exec(void);
  
int plugin_exec(void)
{
	printf("Plugins: starting netzkino.de player\n");
	
	// start netzkino
	moviePlayerGui->exec(NULL, "netzkinoplayback");
	
	return 0;
}


