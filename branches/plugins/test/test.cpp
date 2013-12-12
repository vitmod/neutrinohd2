#include <plugin.h>
#include <test.h>


extern "C" int plugin_exec(void);
  
int plugin_exec(void)
{
	printf("Plugins: starting test\n");
	
	ShowMsg2UTF("neutrino", "to be continued", CMsgBox::mbrBack, CMsgBox::mbBack);
}

