#include <plugin.h>
#include <netzkino.h>


#ifdef __cplusplus
extern "C" {
  
void plugin_exec(void)
{
	printf("Plugins: starting netzkino.de player\n");
	
	// start netzkino
	moviePlayerGui->exec(NULL, "netzkinoplayback");
}

}
#endif
