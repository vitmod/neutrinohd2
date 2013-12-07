#include <plugin.h>
#include <test.h>


#ifdef __cplusplus
extern "C" {
  
void plugin_exec(void)
{
	printf("Plugins: starting test\n");
	
	//ShowHintUTF(LOCALE_MESSAGEBOX_INFO, (const char *) "this neutrino new plugins interface\n"); // UTF-8
	
	ShowMsg2UTF("neutrino", "to be continued", CMsgBox::mbrBack, CMsgBox::mbBack);
}

}
#endif
