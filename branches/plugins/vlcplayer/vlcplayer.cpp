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

#include <vlcplayer.h>


extern "C" void plugin_exec(void);

static int skt = -1;

//
#define STREAMTYPE_DVD	1
#define STREAMTYPE_SVCD	2
#define STREAMTYPE_FILE	3

CVLCPlayer::CVLCPlayer()
{
	// vlc path
	Path_vlc  = "vlc://";
	if ((g_settings.streaming_vlc10 < 2) || (strcmp(g_settings.streaming_server_startdir, "/") != 0))
		Path_vlc += g_settings.streaming_server_startdir;
	Path_vlc_settings = g_settings.streaming_server_startdir;
	
	// filebrowser
	filebrowser = new CFileBrowser(Path_vlc.c_str());
	
	filebrowser->Dirs_Selectable = false;
	
	// vlcfilefilter
	vlcfilefilter.addFilter ("ts");
	vlcfilefilter.addFilter ("mpg");
	vlcfilefilter.addFilter ("mpeg");
	vlcfilefilter.addFilter ("divx");
	vlcfilefilter.addFilter ("avi");
	vlcfilefilter.addFilter ("mkv");
	vlcfilefilter.addFilter ("asf");
	vlcfilefilter.addFilter ("m2p");
	vlcfilefilter.addFilter ("mpv");
	vlcfilefilter.addFilter ("m2ts");
	vlcfilefilter.addFilter ("vob");
	vlcfilefilter.addFilter ("mp4");
	vlcfilefilter.addFilter ("mov");
	vlcfilefilter.addFilter ("flv");
	vlcfilefilter.addFilter ("m2v");
	vlcfilefilter.addFilter ("wmv");
	
	selected = 0;
	
	stream_url = "http://";
	stream_url += g_settings.streaming_server_ip;
	stream_url += ':';
	stream_url += g_settings.streaming_server_port;
	stream_url += "/dboxstream";
	
	moviePlayerGui->filename = stream_url.c_str();
}

CVLCPlayer::~CVLCPlayer()
{
	if(filebrowser)
		delete filebrowser;
}

// vlc
size_t CVLCPlayer::CurlDummyWrite (void *ptr, size_t size, size_t nmemb, void *data)
{
	std::string *pStr = (std::string *) data;
	*pStr += (char*) ptr;
	
	return size * nmemb;
}

CURLcode CVLCPlayer::sendGetRequest (const std::string & url, std::string & response) 
{
	CURL * curl;
	CURLcode httpres;

	curl = curl_easy_init();
	curl_easy_setopt (curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, &CVLCPlayer::CurlDummyWrite);
	curl_easy_setopt (curl, CURLOPT_FILE, (void *)&response);
	curl_easy_setopt (curl, CURLOPT_FAILONERROR, true);
	httpres = curl_easy_perform (curl);
	//printf ("[movieplayer.cpp] HTTP Result: %d\n", httpres);
	curl_easy_cleanup (curl);
	
	return httpres;
}

#define TRANSCODE_VIDEO_OFF 	0
#define TRANSCODE_VIDEO_MPEG1 	1
#define TRANSCODE_VIDEO_MPEG2 	2

bool CVLCPlayer::VlcRequestStream(char * mrl, int  transcodeVideo, int transcodeAudio)
{
	CURLcode httpres;
	std::string baseurl = "http://";
	baseurl += g_settings.streaming_server_ip;
	baseurl += ':';
	baseurl += g_settings.streaming_server_port;
	baseurl += '/';

	// add sout (URL encoded)
	// Example(mit transcode zu mpeg1): ?sout=#transcode{vcodec=mpgv,vb=2000,acodec=mpga,ab=192,channels=2}:duplicate{dst=std{access=http,mux=ts,url=:8080/dboxstream}}
	// Example(ohne transcode zu mpeg1): ?sout=#duplicate{dst=std{access=http,mux=ts,url=:8080/dboxstream}}
	//TODO make this nicer :-)
	std::string souturl;

	//Resolve Resolution from Settings...
	const char * res_horiz;
	const char * res_vert;
	
	switch(g_settings.streaming_resolution)
	{
		case 0:
			res_horiz = "352";
			res_vert = "288";
			break;
		case 1:
			res_horiz = "352";
			res_vert = "576";
			break;
		case 2:
			res_horiz = "480";
			res_vert = "576";
			break;
		case 3:
			res_horiz = "704";
			res_vert = "576";
			break;
		case 4:
			res_horiz = "704";
			res_vert = "288";
			break;
		default:
			res_horiz = "352";
			res_vert = "288";
	} //switch
	
	souturl = "#";
	if(transcodeVideo != TRANSCODE_VIDEO_OFF || transcodeAudio != 0)
	{
		souturl += "transcode{";
		if(transcodeVideo != TRANSCODE_VIDEO_OFF)
		{
			souturl += "vcodec=";
			souturl += (transcodeVideo == TRANSCODE_VIDEO_MPEG1) ? "mpgv" : "mp2v";
			souturl += ",vb=";
			souturl += g_settings.streaming_videorate;
			if (g_settings.streaming_vlc10 != 0)
			{
				souturl += ",scale=1,vfilter=canvas{padd,width=";
				souturl += res_horiz;
				souturl += ",height=";
				souturl += res_vert;
				souturl += ",aspect=4:3}";
			}
			else
			{
				souturl += ",width=";
				souturl += res_horiz;
				souturl += ",height=";
				souturl += res_vert;
			}
			souturl += ",fps=25";
		}
		
		if(transcodeAudio != 0)
		{
			if(transcodeVideo!=TRANSCODE_VIDEO_OFF)
				souturl += ",";
			souturl += "acodec=mpga,ab=";
			souturl += g_settings.streaming_audiorate;
			souturl += ",channels=2";
		}
		souturl += "}:";
	}
	souturl += "std{access=http,mux=ts,dst=";
	//souturl += g_settings.streaming_server_ip;
	souturl += ':';
	souturl += g_settings.streaming_server_port;
	souturl += "/dboxstream}";
	
	char *tmp = curl_escape (souturl.c_str (), 0);

	std::string url = baseurl;
	url += "requests/status.xml?command=in_play&input=";
	url += mrl;	
	
	if (g_settings.streaming_vlc10 > 1)
		url += "&option=";
	else
		url += "%20";
	url += "%3Asout%3D";
	url += tmp;
	curl_free(tmp);
	printf("[movieplayer.cpp] URL(enc) : %s\n", url.c_str());
	std::string response;
	httpres = sendGetRequest(url, response);

	return true; // TODO error checking
}

int CVLCPlayer::VlcGetStreamTime()
{
	// TODO calculate REAL position as position returned by VLC does not take the ringbuffer into consideration
	std::string positionurl = "http://";
	positionurl += g_settings.streaming_server_ip;
	positionurl += ':';
	positionurl += g_settings.streaming_server_port;
	positionurl += "/requests/status.xml";
	//printf("[movieplayer.cpp] positionurl=%s\n",positionurl.c_str());
	std::string response = "";
	CURLcode httpres = sendGetRequest(positionurl, response);
	//printf("[movieplayer.cpp] httpres=%d, response.length()=%d, stream_length = %s\n",httpres,response.length(),response.c_str());
	if(httpres == 0 && response.length() > 0) 
	{
		xmlDocPtr answer_parser = parseXml(response.c_str());
		if (answer_parser != NULL) 
		{
			xmlNodePtr element = xmlDocGetRootElement(answer_parser);
			element = element->xmlChildrenNode;
			while (element) 
			{
				char* tmp = xmlGetName(element);
				if (strcmp(tmp, "time") == 0) 
				{
					return atoi(xmlGetData(element));
				}
				element = element->xmlNextNode;
			}
		}
		return -1;
	}
	else
		return -1;
}

int CVLCPlayer::VlcGetStreamLength()
{
	// TODO calculate REAL position as position returned by VLC does not take the ringbuffer into consideration
	std::string positionurl = "http://";
	positionurl += g_settings.streaming_server_ip;
	positionurl += ':';
	positionurl += g_settings.streaming_server_port;
	positionurl += "/requests/status.xml";
	//printf("[movieplayer.cpp] positionurl=%s\n",positionurl.c_str());
	std::string response = "";
	CURLcode httpres = sendGetRequest(positionurl, response);
	//printf("[movieplayer.cpp] httpres=%d, response.length()=%d, stream_length = %s\n",httpres,response.length(),response.c_str());
	if(httpres == 0 && response.length() > 0) 
	{
		xmlDocPtr answer_parser = parseXml(response.c_str());
		if (answer_parser != NULL) 
		{
			xmlNodePtr element = xmlDocGetRootElement(answer_parser);
			element = element->xmlChildrenNode;
			while (element) 
			{
				char* tmp = xmlGetName(element);
				if (strcmp(tmp, "length") == 0) 
				{
					return atoi(xmlGetData(element));
				}
				element = element->xmlNextNode;
			}
		}
		return -1;
	}
	else
		return -1;
}

bool CVLCPlayer::VlcReceiveStreamStart(void * mrl)
{
	dprintf(DEBUG_NORMAL, "[movieplayer.cpp] ReceiveStream started\n");

	// Get Server and Port from Config
	std::string response;
	std::string baseurl = "http://";
	baseurl += g_settings.streaming_server_ip;
	baseurl += ':';
	baseurl += g_settings.streaming_server_port;
	baseurl += '/';
	baseurl += "requests/status.xml";
	CURLcode httpres = sendGetRequest(baseurl, response);
	
	if(httpres != 0)
	{
		DisplayErrorMessage(g_Locale->getText(LOCALE_MOVIEPLAYER_NOSTREAMINGSERVER));	// UTF-8
		//playstate = CMoviePlayerGui::STOPPED;
		return false;
		// Assume safely that all succeeding HTTP requests are successful
	}


	int transcodeVideo, transcodeAudio;
	std::string sMRL = (char*)mrl;
	//Menu Option Force Transcode: Transcode all Files, including mpegs.
	if((!memcmp((char*)mrl, "vcd:", 4) ||
		 !strcasecmp(sMRL.substr(sMRL.length()-3).c_str(), "mpg") ||
		 !strcasecmp(sMRL.substr(sMRL.length()-4).c_str(), "mpeg") ||
		 !strcasecmp(sMRL.substr(sMRL.length()-3).c_str(), "m2p")))
	{
		if(g_settings.streaming_force_transcode_video)
			transcodeVideo=g_settings.streaming_transcode_video_codec+1;
		else
			transcodeVideo=0;
		transcodeAudio=g_settings.streaming_transcode_audio;
	}
	else
	{
		transcodeVideo = g_settings.streaming_transcode_video_codec + 1;
		if((!memcmp((char*)mrl, "dvd", 3) && !g_settings.streaming_transcode_audio) ||
			(!strcasecmp(sMRL.substr(sMRL.length()-3).c_str(), "vob") && !g_settings.streaming_transcode_audio) ||
			(!strcasecmp(sMRL.substr(sMRL.length()-3).c_str(), "ac3") && !g_settings.streaming_transcode_audio) ||
			g_settings.streaming_force_avi_rawaudio)
			transcodeAudio = 0;
		else
			transcodeAudio = 1;
	}
	
	VlcRequestStream((char*)mrl, transcodeVideo, transcodeAudio);

	const char * server = g_settings.streaming_server_ip.c_str();
	int port;
	sscanf(g_settings.streaming_server_port, "%d", &port);

	struct sockaddr_in servAddr;
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons (port);
	servAddr.sin_addr.s_addr = inet_addr (server);

	dprintf(DEBUG_NORMAL, "[movieplayer.cpp] Server: %s\n", server);
	dprintf(DEBUG_NORMAL, "[movieplayer.cpp] Port: %d\n", port);
	int len;

	while(true)
	{
		//printf ("[movieplayer.cpp] Trying to call socket\n");
		
		if(skt < 0)
		{
			skt = socket (AF_INET, SOCK_STREAM, 0);

			dprintf(DEBUG_NORMAL, "[movieplayer.cpp] Trying to connect socket\n");
			
			if(connect(skt, (struct sockaddr *) &servAddr, sizeof (servAddr)) < 0)
			{
				perror ("SOCKET");
				//playstate = CMoviePlayerGui::STOPPED;
				return false;
			}
			fcntl (skt, O_NONBLOCK);
		}
		
		dprintf(DEBUG_NORMAL, "[movieplayer.cpp] Socket OK\n");
       
		// Skip HTTP header
		const char * msg = "GET /dboxstream HTTP/1.0\r\n\r\n";
		int msglen = strlen (msg);
		if(send (skt, msg, msglen, 0) == -1)
		{
			perror ("send()");
			//playstate = CMoviePlayerGui::STOPPED;
			
			return false;
		}

		dprintf(DEBUG_NORMAL, "[movieplayer.cpp] GET Sent\n");
		
		usleep(1000);

		// Skip HTTP Header
		int found = 0;
		char buf[2];
		char line[200];
		buf[0] = buf[1] = '\0';
		strcpy (line, "");
		
		while(true)
		{
			len = recv(skt, buf, 1, 0);
			strncat (line, buf, 1);
			
			if(strcmp (line, "HTTP/1.0 404") == 0)
			{
				dprintf(DEBUG_NORMAL, "[movieplayer.cpp] VLC still does not send. Exiting...\n");
				
				if(skt > 0)
				{
					close(skt);
					skt = -1;
				}
		
				//break;
				// only once
				return false;
			}
			
			if((((found & (~2)) == 0) && (buf[0] == '\r')) || (((found & (~2)) == 1) && (buf[0] == '\n')))  	// found == 0 || found == 2 *//*   found == 1 || found == 3
			{
				if(found == 3)
					goto vlc_is_sending;
				else
					found++;
			}
			else
			{
				found = 0;
			}
		}
		
		/*
		if(playstate == CMoviePlayerGui::STOPPED)
		{
			if(skt > 0 )
			{
				close(skt);
				skt = -1;
			}
			return false;
		}
		*/
	}
	
vlc_is_sending:
	dprintf(DEBUG_NORMAL, "[movieplayer.cpp] Now VLC is sending. Read sockets created\n");
	
	return true;
}

void plugin_exec(void)
{
	printf("Plugins: starting VLCPlayer\n");
	
	CVLCPlayer * VLCPlayer = new CVLCPlayer();
	
	//moviePlayerGui->filename = NULL;
	//filebrowser->Filter = &vlcfilefilter;
				
	if(VLCPlayer->filebrowser->exec(VLCPlayer->Path_vlc.c_str()))
	{
		VLCPlayer->Path_vlc = VLCPlayer->filebrowser->getCurrentDir();
					
		VLCPlayer->_filelist = VLCPlayer->filebrowser->getSelectedFiles();

		if(!VLCPlayer->_filelist.empty())
		{
			VLCPlayer->filename = VLCPlayer->_filelist[VLCPlayer->selected].Name.c_str();
			//sel_filename = _filelist[selected].getFileName();
						
			int namepos = VLCPlayer->_filelist[VLCPlayer->selected].Name.rfind("vlc://");
			std::string mrl_str = "";
						
			if (g_settings.streaming_vlc10 > 1)
			{
				mrl_str += "file://";
				if (VLCPlayer->filename[namepos + 6] != '/')
					mrl_str += "/";
			}
						
			mrl_str += VLCPlayer->_filelist[VLCPlayer->selected].Name.substr(namepos + 6);
			char * tmp = curl_escape(mrl_str.c_str (), 0);
			strncpy (VLCPlayer->_mrl, tmp, sizeof (VLCPlayer->_mrl) - 1);
			curl_free (tmp);
			
			dprintf(DEBUG_NORMAL, "[vlcplayer.cpp] Generated FILE MRL: %s\n", VLCPlayer->_mrl);

			VLCPlayer->selected = 0;
		}
		
		if( VLCPlayer->VlcReceiveStreamStart(VLCPlayer->_mrl) )
		{
			VLCPlayer->stream_url = "http://";
			VLCPlayer->stream_url += g_settings.streaming_server_ip;
			VLCPlayer->stream_url += ':';
			VLCPlayer->stream_url += g_settings.streaming_server_port;
			VLCPlayer->stream_url += "/dboxstream";
			
			moviePlayerGui->filename = VLCPlayer->stream_url.c_str();
		}
		
		moviePlayerGui->exec(NULL, "urlplayback");
	}
	
	delete VLCPlayer;
}
 
