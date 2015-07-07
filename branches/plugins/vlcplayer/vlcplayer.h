/*
  neutrinoHD2 project
  
  https://code.google.com/p/neutrinohd2/
  
  $Id: vlcplayer.h 2014/01/22 mohousch Exp $

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

#ifndef __vlcplayer__
#define __vlcplayer__


#include <plugin.h>


#define VLC_SETTINGS_FILE          PLUGINDIR "/vlcplayer/vlc.conf"


// settings
typedef struct
{
	std::string streaming_server_ip;
	char streaming_server_port[10];
	char streaming_server_cddrive[21];
	
	int streaming_vlc10;
	
	char streaming_videorate[6];
	char streaming_audiorate[6];
	char streaming_server_startdir[40];
	int streaming_transcode_audio;
	int streaming_force_avi_rawaudio;
	int streaming_force_transcode_video;
	int streaming_transcode_video_codec;
	int streaming_resolution;
}VLC_SETTINGS;

class CVLCPlayer : public CMenuTarget
{
	public:
		CConfigFile configfile;
		
		//
		int streamtype;
		
		//
		std::string Path_vlc;
		std::string Path_vlc_settings;

		//
		CFileBrowser * filebrowser;
		CFileFilter vlcfilefilter;
		
		// vlc
		std::string stream_url;
		char _mrl[200];
		std::string filename;
		CFileList _filelist;
		unsigned int selected;
		std::string title;
		std::string baseurl;
		std::string positionurl;
		std::string m_baseurl;
		
		// vlc
		static size_t CurlWriteToString (void *ptr, size_t size, size_t nmemb, void *data);
		CURLcode sendGetRequest (const std::string & url, std::string & response) ;
		bool VlcRequestStream(char *_mrl, int  transcodeVideo, int transcodeAudio);
		int VlcGetStreamTime();
		int VlcGetStreamLength();
		bool VlcReceiveStreamStart(void * mrl);
		
		bool loadSettings(VLC_SETTINGS* settings); 
		bool saveSettings(VLC_SETTINGS* settings);
		
		void VLCPlayerSetup();
		void vlcPlayerMenu();
		
		bool readDir_vlc(const std::string & dirname, CFileList* flist);
		
	public:
		CVLCPlayer();
		~CVLCPlayer();
		int exec();
		int exec(CMenuTarget* parent, const std::string & actionKey);
};

#endif
 
