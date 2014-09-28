/*
  neutrinoHD2 project
  
  https://code.google.com/p/neutrinohd2/
  
  $Id: mediaplayer.cpp 2014/01/25 mohousch Exp $

  License: GPL

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <plugin.h>


extern "C" void plugin_exec(void);

#if defined (__USE_FILE_OFFSET64) || defined (_DARWIN_USE_64_BIT_INODE)
typedef struct dirent64 dirent_struct;
#define my_alphasort alphasort64
#define my_scandir scandir64
typedef struct stat64 stat_struct;
#define my_stat stat64
#define my_lstat lstat64
#else
typedef struct dirent dirent_struct;
#define my_alphasort alphasort
#define my_scandir scandir
typedef struct stat stat_struct;
#define my_stat stat
#define my_lstat lstat
#error not using 64 bit file offsets
#endif

void plugin_exec(void)
{
	printf("Plugins: starting mediaplayer\n");
	
	//moviePlayerGui->exec(NULL, "fileplayback");
	
	CFileBrowser * fileBrowser;
	
	fileBrowser = new CFileBrowser();
	
	CFileFilter fileFilter;
	CFileList filelist;
	
	int selected = 0;
	
	fileFilter.addFilter("ts");
	fileFilter.addFilter("mpg");
	fileFilter.addFilter("mpeg");
	fileFilter.addFilter("divx");
	fileFilter.addFilter("avi");
	fileFilter.addFilter("mkv");
	fileFilter.addFilter("asf");
	fileFilter.addFilter("aiff");
	fileFilter.addFilter("m2p");
	fileFilter.addFilter("mpv");
	fileFilter.addFilter("m2ts");
	fileFilter.addFilter("vob");
	fileFilter.addFilter("mp4");
	fileFilter.addFilter("mov");	
	fileFilter.addFilter("flv");	
	fileFilter.addFilter("dat");
	fileFilter.addFilter("trp");
	fileFilter.addFilter("vdr");
	fileFilter.addFilter("mts");
	fileFilter.addFilter("wmv");
	fileFilter.addFilter("wav");
	fileFilter.addFilter("flac");
	fileFilter.addFilter("mp3");
	fileFilter.addFilter("wma");
	fileFilter.addFilter("ogg");

	//fileBrowser->Multi_Select    = true;
	//fileBrowser->Dirs_Selectable = true;
	fileBrowser->Filter = &fileFilter;
	
	#if 0
	CFileList _filelist;
	
	stat_struct statbuf;
	dirent_struct **namelist;
	int n;
	std::string dirname;
	
	dirname = g_settings.network_nfs_moviedir;

	n = my_scandir(dirname.c_str(), &namelist, 0, my_alphasort);
	if (n < 0)
	{
		//perror(("mediaplayer scandir: " + dirname).c_str());
		//return false;
	}
	
	for(int i = 0; i < n;i++)
	{
		CFile file;
		if(strcmp(namelist[i]->d_name, ".") != 0)
		{
			file.Name = dirname + namelist[i]->d_name;

			if(my_stat((file.Name).c_str(),&statbuf) != 0)
				perror("stat error");
			else
			{
				file.Mode = statbuf.st_mode;
				file.Size = statbuf.st_size;
				file.Time = statbuf.st_mtime;
				_filelist.push_back(file);
			}
		}
		free(namelist[i]);
	}

	free(namelist);
	
	fileBrowser->filelist = _filelist;
	#endif

BROWSER:
	if (fileBrowser->exec(g_settings.network_nfs_moviedir))
	{
		filelist = fileBrowser->getSelectedFiles();
		
		CFile *file = fileBrowser->getSelectedFile();
		filelist.clear();
		filelist.push_back(*file);
		
		if(!filelist.empty())
		{
			moviePlayerGui->filename = filelist[selected].Name.c_str();	
			moviePlayerGui->Title = filelist[selected].getFileName();
			moviePlayerGui->Info1 = filelist[selected].getFileName();
			moviePlayerGui->Info2 = filelist[selected].getFileName();
			
			//moviePlayerGui->was_file = true;
			//moviePlayerGui->filelist = filelist;

			// play
			moviePlayerGui->exec(NULL, "urlplayback");
		}
		
		neutrino_msg_t msg;
		neutrino_msg_data_t data;

		g_RCInput->getMsg_ms(&msg, &data, 40);
		
		if (msg != CRCInput::RC_home) 
		{
			goto BROWSER;
		}
	}
	
	delete fileBrowser;
}
