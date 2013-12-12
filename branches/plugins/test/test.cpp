#include <plugin.h>
#include <test.h>


extern "C" int plugin_exec(void);
  
int plugin_exec(void)
{
	printf("Plugins: starting test\n");
	
	//ShowMsg2UTF("neutrino", "to be continued", CMsgBox::mbrBack, CMsgBox::mbBack);
	CFileBrowser filebrowser;
	filebrowser.use_filter = false;	
	if(filebrowser.exec("/media/hdd"))
	{
		g_settings.streaming_server_url = filebrowser.getSelectedFile()->Name.c_str();
		moviePlayerGui->exec(NULL, "urlplayback");
	}
	
	return 0;
}

