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


class CVLCPlayer
{
	public:
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
		
		// vlc
		static size_t CurlDummyWrite (void *ptr, size_t size, size_t nmemb, void *data);
		CURLcode sendGetRequest (const std::string & url, std::string & response) ;
		bool VlcRequestStream(char *_mrl, int  transcodeVideo, int transcodeAudio);
		int VlcGetStreamTime();
		int VlcGetStreamLength();
		bool VlcReceiveStreamStart(void * mrl);
		
	public:
		CVLCPlayer();
		~CVLCPlayer();
};

#endif
 
