#include <plugin.h>

 
// plugins api
extern "C" int plugin_exec(void);

int plugin_exec(void)
{
	printf("Plugins: starting restart enigma\n");
	
	system("rm -f /etc/.nhd2");
	system("touch /etc/.e2");
	
	g_RCInput->postMsg( NeutrinoMessages::RESTART, 0 );
	ShowHintUTF(LOCALE_MESSAGEBOX_INFO, "enigma2 wird gestartet !", 450, 2 );
	
	return 0;
}


