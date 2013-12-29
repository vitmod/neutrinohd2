#include <plugin.h>


extern "C" int plugin_exec(void);
  
int plugin_exec(void)
{
	printf("Plugins: starting youtube player\n");
	
	// start netzkino
	moviePlayerGui->exec(NULL, "ytplayback");
	
	return 0;
}


