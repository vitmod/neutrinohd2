#include <plugin.h>


extern "C" int plugin_exec(void);
  
int plugin_exec(void)
{
	printf("Plugins: starting upnp browser\n");
	
	CUpnpBrowserGui tmpUpnpBrowserGui;
	tmpUpnpBrowserGui.exec(NULL, "");
	
	return 0;
}


