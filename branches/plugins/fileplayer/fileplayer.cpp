#include <plugin.h>
#include <fileplayer.h>


extern "C" int plugin_exec(void);

int plugin_exec(void)
{
	printf("Plugins: starting fileplayer\n");
	
	//ShowMsg2UTF("neutrino", "to be continued", CMsgBox::mbrBack, CMsgBox::mbBack);
	
	CFileBrowser filebrowser;
	CFileFilter filefilter;
		
	filefilter.addFilter("ts");
	filefilter.addFilter("mpg");
	filefilter.addFilter("mpeg");
	filefilter.addFilter("divx");
	filefilter.addFilter("avi");
	filefilter.addFilter("mkv");
	filefilter.addFilter("asf");
	filefilter.addFilter("aiff");
	filefilter.addFilter("m2p");
	filefilter.addFilter("mpv");
	filefilter.addFilter("m2ts");
	filefilter.addFilter("vob");
	filefilter.addFilter("mp4");
	filefilter.addFilter("mov");	
	filefilter.addFilter("flv");	
	filefilter.addFilter("dat");
	filefilter.addFilter("trp");
	filefilter.addFilter("vdr");
	filefilter.addFilter("mts");
	filefilter.addFilter("wav");
	filefilter.addFilter("flac");
	filefilter.addFilter("mp3");
	filefilter.addFilter("wmv");
	filefilter.addFilter("wma");
	filefilter.addFilter("ogg");
	
	filebrowser.Filter = &filefilter;
	
	if(filebrowser.exec(recDir))
	{
		g_settings.streaming_server_url = filebrowser.getSelectedFile()->Name.c_str();
		moviePlayerGui->exec(NULL, "urlplayback");
	}
	
	return 0;
}
