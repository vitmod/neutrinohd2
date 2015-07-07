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
extern "C" void plugin_init(void);
extern "C" void plugin_del(void);


static int skt = -1;

//
#define STREAMTYPE_DVD	1
#define STREAMTYPE_SVCD	2
#define STREAMTYPE_FILE	3

#define NEUTRINO_ICON_VLC_SMALL			PLUGINDIR "/vlcplayer/vlc_small.png"

VLC_SETTINGS m_settings;

CVLCPlayer::CVLCPlayer(): configfile ('\t')
{
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
}

CVLCPlayer::~CVLCPlayer()
{
	if(filebrowser)
		delete filebrowser;
}

// vlc
size_t CVLCPlayer::CurlWriteToString(void *ptr, size_t size, size_t nmemb, void *data)
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
	curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, &CVLCPlayer::CurlWriteToString);
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
	baseurl += m_settings.streaming_server_ip;
	baseurl += ':';
	baseurl += m_settings.streaming_server_port;
	baseurl += '/';

	// add sout (URL encoded)
	// Example(mit transcode zu mpeg1): ?sout=#transcode{vcodec=mpgv,vb=2000,acodec=mpga,ab=192,channels=2}:duplicate{dst=std{access=http,mux=ts,url=:8080/dboxstream}}
	// Example(ohne transcode zu mpeg1): ?sout=#duplicate{dst=std{access=http,mux=ts,url=:8080/dboxstream}}
	//TODO make this nicer :-)
	std::string souturl;

	//Resolve Resolution from Settings...
	const char * res_horiz;
	const char * res_vert;
	
	switch(m_settings.streaming_resolution)
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
			//souturl += m_settings.streaming_videorate;
			
			if (m_settings.streaming_vlc10 != 0)
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
			//souturl += m_settings.streaming_audiorate;
			souturl += ",channels=2";
		}
		souturl += "}:";
	}
	souturl += "std{access=http,mux=ts,dst=";
	souturl += ':';
	//souturl += m_settings.streaming_server_port;
	souturl += "/dboxstream}";
	
	char *tmp = curl_escape (souturl.c_str (), 0);

	std::string url = baseurl;
	url += "requests/status.xml?command=in_play&input=";
	url += mrl;	
	
	if (m_settings.streaming_vlc10 > 1)
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
	positionurl += m_settings.streaming_server_ip;
	positionurl += ':';
	positionurl += m_settings.streaming_server_port;
	positionurl += "/requests/status.xml";
	
	dprintf(DEBUG_NORMAL, "CVLCPlayer::VlcGetStreamTime: positionurl=%s\n",positionurl.c_str());
	
	std::string response = "";
	CURLcode httpres = sendGetRequest(positionurl, response);
	
	dprintf(DEBUG_NORMAL, "CVLCPlayer::VlcGetStreamTime: httpres=%d, response.length()=%d, stream_length = %s\n",httpres,response.length(),response.c_str());
	
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
	positionurl += m_settings.streaming_server_ip;
	positionurl += ':';
	positionurl += m_settings.streaming_server_port;
	positionurl += "/requests/status.xml";
	
	dprintf(DEBUG_NORMAL, "CVLCPlayer::VlcGetStreamLength: positionurl=%s\n",positionurl.c_str());
	
	std::string response = "";
	CURLcode httpres = sendGetRequest(positionurl, response);
	
	dprintf(DEBUG_NORMAL, "CVLCPlayer::VlcGetStreamLength: httpres=%d, response.length()=%d, stream_length = %s\n",httpres,response.length(),response.c_str());
	
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
	dprintf(DEBUG_NORMAL, "CVLCPlayer::VlcReceiveStreamStart: ReceiveStream started\n");

	// Get Server and Port from Config
	std::string response;

	baseurl += m_settings.streaming_server_ip;
	baseurl += ':';
	baseurl += m_settings.streaming_server_port;
	baseurl += '/';
	baseurl += "requests/status.xml";
	
	CURLcode httpres = sendGetRequest(baseurl, response);
	
	if(httpres != 0)
	{
		MessageBox(LOCALE_MESSAGEBOX_ERROR, g_Locale->getText(LOCALE_MOVIEPLAYER_NOSTREAMINGSERVER), CMessageBox::mbrCancel, CMessageBox::mbCancel, NEUTRINO_ICON_ERROR);
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
		if(m_settings.streaming_force_transcode_video)
			transcodeVideo = m_settings.streaming_transcode_video_codec + 1;
		else
			transcodeVideo=0;
		transcodeAudio = m_settings.streaming_transcode_audio;
	}
	else
	{
		transcodeVideo = m_settings.streaming_transcode_video_codec + 1;
		
		if((!memcmp((char*)mrl, "dvd", 3) && !m_settings.streaming_transcode_audio) ||
			(!strcasecmp(sMRL.substr(sMRL.length()-3).c_str(), "vob") && !m_settings.streaming_transcode_audio) ||
			(!strcasecmp(sMRL.substr(sMRL.length()-3).c_str(), "ac3") && !m_settings.streaming_transcode_audio) ||
			m_settings.streaming_force_avi_rawaudio)
			transcodeAudio = 0;
		else
			transcodeAudio = 1;
	}
	
	VlcRequestStream((char*)mrl, transcodeVideo, transcodeAudio);

	const char * server = m_settings.streaming_server_ip.c_str();
	int port;
	sscanf(m_settings.streaming_server_port, "%d", &port);

	struct sockaddr_in servAddr;
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons (port);
	servAddr.sin_addr.s_addr = inet_addr (server);

	int len;

	while(true)
	{
		//dprintf(DEBUG_NORMAL, "CVLCPlayer::VlcReceiveStreamStart: Trying to call socket\n");
		
		if(skt < 0)
		{
			skt = socket (AF_INET, SOCK_STREAM, 0);

			//dprintf(DEBUG_NORMAL, "CVLCPlayer::VlcReceiveStreamStart: Trying to connect socket\n");
			
			if(connect(skt, (struct sockaddr *) &servAddr, sizeof (servAddr)) < 0)
			{
				perror ("SOCKET");
				//playstate = CMoviePlayerGui::STOPPED;
				return false;
			}
			fcntl (skt, O_NONBLOCK);
		}
		
		//dprintf(DEBUG_NORMAL, "CVLCPlayer::VlcReceiveStreamStart: Socket OK\n");
       
		// Skip HTTP header
		const char * msg = "GET /dboxstream HTTP/1.0\r\n\r\n";
		int msglen = strlen (msg);
		if(send (skt, msg, msglen, 0) == -1)
		{
			perror ("send()");
			//playstate = CMoviePlayerGui::STOPPED;
			
			return false;
		}

		//dprintf(DEBUG_NORMAL, "CVLCPlayer::VlcReceiveStreamStart: GET Sent\n");
		
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
				//dprintf(DEBUG_NORMAL, "CVLCPlayer::VlcReceiveStreamStart: VLC still does not send. Exiting...\n");
				
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
	dprintf(DEBUG_NORMAL, "CVLCPlayer::VlcReceiveStreamStart: Now VLC is sending. Read sockets created\n");
	
	return true;
}

#define VLC_URI "vlc://"
bool CVLCPlayer::readDir_vlc(const std::string& dirname, CFileList* flist)
{
	std::string answer = "";
	char *dir_escaped = curl_escape(dirname.substr(strlen(VLC_URI)).c_str(), 0);
	std::string url = "http://" + m_settings.streaming_server_ip + ':' + m_settings.streaming_server_port + "/requests/browse.xml?dir=";
	url += dir_escaped;
	curl_free(dir_escaped);
	
	dprintf(DEBUG_NORMAL, "\nURL:%s\n", url.c_str());

	CURL *curl_handle;
	CURLcode httpres;
	
	/* init the curl session */
	curl_handle = curl_easy_init();
	/* timeout. 15 seconds should be enough */
	curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT, 15);
	/* specify URL to get */
	curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
	/* send all data to this function  */
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, CurlWriteToString);
	/* we pass our 'chunk' struct to the callback function */
	curl_easy_setopt(curl_handle, CURLOPT_FILE, (void *)&answer);
	/* Generate error if http error >= 400 occurs */
	curl_easy_setopt(curl_handle, CURLOPT_FAILONERROR, 1);
	
	
	
	/* error handling */
	char error[CURL_ERROR_SIZE];
	curl_easy_setopt(curl_handle, CURLOPT_ERRORBUFFER, error);
	/* get it! */
	httpres = curl_easy_perform(curl_handle);
	/* cleanup curl stuff */
	curl_easy_cleanup(curl_handle);
	
	std::string name = dirname;
	std::replace(name.begin(), name.end(), '\\', '/');
	
	if (!answer.empty() && httpres == 0)
	{
		xmlDocPtr answer_parser = parseXml(answer.c_str());

		if (answer_parser != NULL) 
		{
			xmlNodePtr element = xmlDocGetRootElement(answer_parser);
			element = element->xmlChildrenNode;
			char *ptr;
			if (element == NULL) 
			{
				dprintf(DEBUG_NORMAL, "CVLCPlayer::readDir_vlcDrive is not readable. Possibly no disc inserted\n");
				
				CFile file;
				file.Mode = S_IFDIR + 0777 ;
				file.Name = name + "..";
				file.Size = 0;
				file.Time = 0;
				
				flist->push_back(file);
			} 
			else 
			{
				while (element) 
				{
					CFile file;
					ptr = xmlGetAttribute(element, "type");
					if (strcmp(ptr, "directory") == 0)
						file.Mode = S_IFDIR + 0777 ;
					else
						file.Mode = S_IFREG + 0777 ;

					file.Name = name + xmlGetAttribute(element, "name");
					ptr = xmlGetAttribute(element, "size");
					if (ptr) 
						file.Size = atoi(ptr);
					else 
						file.Size = 0;
					file.Time = 0;

					element = element->xmlNextNode;
					flist->push_back(file);
				}
			}
			xmlFreeDoc(answer_parser);
			
			return true;
		}
	}
	
	/* since all CURL error messages use only US-ASCII characters, when can safely print them as if they were UTF-8 encoded */
	if (httpres == 22) 
	{
	    strcat(error, "\nProbably wrong vlc version\nPlease use vlc 0.8.5 or higher");
	}
	
	//DisplayErrorMessage(error); // UTF-8
	MessageBox(LOCALE_MESSAGEBOX_ERROR, error, CMessageBox::mbrCancel, CMessageBox::mbCancel, NEUTRINO_ICON_ERROR);
	
	CFile file;

	file.Name = name + "..";
	file.Mode = S_IFDIR + 0777;
	file.Size = 0;
	file.Time = 0;
	flist->push_back(file);

	return false;
}

bool CVLCPlayer::loadSettings(VLC_SETTINGS *settings)
{
	bool result = true;
	
	dprintf(DEBUG_NORMAL, "CVLCPlayer::loadSettings\r\n");
	
	if(configfile.loadConfig(VLC_SETTINGS_FILE))
	{
		settings->streaming_server_ip = configfile.getString("streaming_server_ip", "10.10.10.10");
		strcpy( settings->streaming_server_port, configfile.getString( "streaming_server_port", "8080").c_str() );
		strcpy( settings->streaming_server_cddrive, configfile.getString("streaming_server_cddrive", "D:").c_str() );
		strcpy( settings->streaming_videorate,  configfile.getString("streaming_videorate", "1000").c_str() );
		strcpy( settings->streaming_audiorate, configfile.getString("streaming_audiorate", "192").c_str() );
		strcpy( settings->streaming_server_startdir, configfile.getString("streaming_server_startdir", "C:/Movies").c_str() );
		settings->streaming_transcode_audio = configfile.getInt32( "streaming_transcode_audio", 0 );
		settings->streaming_force_transcode_video = configfile.getInt32( "streaming_force_transcode_video", 0 );
		settings->streaming_transcode_video_codec = configfile.getInt32( "streaming_transcode_video_codec", 0 );
		settings->streaming_force_avi_rawaudio = configfile.getInt32( "streaming_force_avi_rawaudio", 0 );
		settings->streaming_resolution = configfile.getInt32( "streaming_resolution", 0 );
	}
	else
	{
		dprintf(DEBUG_NORMAL, "CVLCPlayer::loadSettings failed\r\n"); 
		configfile.clear();
		result = false;
	}
	
	return (result);
}

bool CVLCPlayer::saveSettings(VLC_SETTINGS *settings)
{
	bool result = true;
	dprintf(DEBUG_NORMAL, "CVLCPlayer::saveSettings\r\n");

	configfile.setString( "streaming_server_ip", settings->streaming_server_ip );
	configfile.setString( "streaming_server_port", settings->streaming_server_port );
	configfile.setString( "streaming_server_cddrive", settings->streaming_server_cddrive );
	configfile.setString( "streaming_videorate", settings->streaming_videorate );
	configfile.setString( "streaming_audiorate", settings->streaming_audiorate );
	configfile.setString( "streaming_server_startdir", settings->streaming_server_startdir );
	configfile.setInt32 ( "streaming_transcode_audio", settings->streaming_transcode_audio );
	configfile.setInt32 ( "streaming_force_avi_rawaudio", settings->streaming_force_avi_rawaudio );
	configfile.setInt32 ( "streaming_force_transcode_video", settings->streaming_force_transcode_video );
	configfile.setInt32 ( "streaming_transcode_video_codec", settings->streaming_transcode_video_codec );
	configfile.setInt32 ( "streaming_resolution", settings->streaming_resolution );
 
 	if (configfile.getModifiedFlag())
		configfile.saveConfig(VLC_SETTINGS_FILE);
	
	return (result);
}

#define MESSAGEBOX_NO_YES_OPTION_COUNT 2
const CMenuOptionChooser::keyval MESSAGEBOX_NO_YES_OPTIONS[MESSAGEBOX_NO_YES_OPTION_COUNT] =
{
	{ 0, LOCALE_MESSAGEBOX_NO, NULL },
	{ 1, LOCALE_MESSAGEBOX_YES, NULL }
};

#define STREAMINGMENU_STREAMING_TRANSCODE_VIDEO_CODEC_OPTION_COUNT 2
const CMenuOptionChooser::keyval STREAMINGMENU_STREAMING_TRANSCODE_VIDEO_CODEC_OPTIONS[STREAMINGMENU_STREAMING_TRANSCODE_VIDEO_CODEC_OPTION_COUNT] =
{
	{ 0, LOCALE_STREAMINGMENU_MPEG1, NULL },
	{ 1, LOCALE_STREAMINGMENU_MPEG2, NULL }
};

#define STREAMINGMENU_STREAMING_RESOLUTION_OPTION_COUNT 4
const CMenuOptionChooser::keyval STREAMINGMENU_STREAMING_RESOLUTION_OPTIONS[STREAMINGMENU_STREAMING_RESOLUTION_OPTION_COUNT] =
{
	{ 0, LOCALE_STREAMINGMENU_352X288, NULL },
	{ 1, LOCALE_STREAMINGMENU_352X576, NULL },
	{ 2, LOCALE_STREAMINGMENU_480X576, NULL },
	{ 3, LOCALE_STREAMINGMENU_704X576, NULL }
};

void CVLCPlayer::VLCPlayerSetup()
{
	CMenuWidget* vlc_setup = new CMenuWidget( LOCALE_MAINSETTINGS_STREAMING, NEUTRINO_ICON_SETTINGS);

	// intros
	vlc_setup->addItem(GenericMenuSeparator);
	vlc_setup->addItem(GenericMenuBack);
	vlc_setup->addItem(GenericMenuSeparatorLine);
	vlc_setup->addItem(new CMenuForwarder(LOCALE_MAINSETTINGS_SAVESETTINGSNOW, true, NULL, this, "Save", CRCInput::RC_red, NEUTRINO_ICON_BUTTON_RED));
	vlc_setup->addItem(GenericMenuSeparatorLine);

	// server ip
	CIPInput * vlc_setup_server_ip = new CIPInput(LOCALE_STREAMINGMENU_SERVER_IP, m_settings.streaming_server_ip, LOCALE_IPSETUP_HINT_1, LOCALE_IPSETUP_HINT_2);
	CStringInput * vlc_setup_server_port = new CStringInput(LOCALE_STREAMINGMENU_SERVER_PORT, m_settings.streaming_server_port);
	CStringInputSMS * cddriveInput = new CStringInputSMS(LOCALE_STREAMINGMENU_STREAMING_SERVER_CDDRIVE, m_settings.streaming_server_cddrive);
	CStringInput * vlc_setup_videorate = new CStringInput(LOCALE_STREAMINGMENU_STREAMING_VIDEORATE, m_settings.streaming_videorate);
	CStringInput * vlc_setup_audiorate = new CStringInput(LOCALE_STREAMINGMENU_STREAMING_AUDIORATE, m_settings.streaming_audiorate);
	CStringInputSMS * startdirInput = new CStringInputSMS(LOCALE_STREAMINGMENU_STREAMING_SERVER_STARTDIR, m_settings.streaming_server_startdir);

	CMenuForwarder* mf1 = new CMenuForwarder(LOCALE_STREAMINGMENU_SERVER_IP, true, m_settings.streaming_server_ip, vlc_setup_server_ip);
	CMenuForwarder* mf2 = new CMenuForwarder(LOCALE_STREAMINGMENU_SERVER_PORT, true, m_settings.streaming_server_port, vlc_setup_server_port);
	CMenuForwarder* mf3 = new CMenuForwarder(LOCALE_STREAMINGMENU_STREAMING_SERVER_CDDRIVE, true, m_settings.streaming_server_cddrive , cddriveInput);
	CMenuForwarder* mf4 = new CMenuForwarder(LOCALE_STREAMINGMENU_STREAMING_VIDEORATE, true, m_settings.streaming_videorate, vlc_setup_videorate);
	CMenuForwarder* mf5 = new CMenuForwarder(LOCALE_STREAMINGMENU_STREAMING_AUDIORATE, true, m_settings.streaming_audiorate, vlc_setup_audiorate);
	CMenuForwarder* mf6 = new CMenuForwarder(LOCALE_STREAMINGMENU_STREAMING_SERVER_STARTDIR, true, m_settings.streaming_server_startdir, startdirInput);
	CMenuOptionChooser* oj1 = new CMenuOptionChooser(LOCALE_STREAMINGMENU_STREAMING_TRANSCODE_AUDIO, &m_settings.streaming_transcode_audio, MESSAGEBOX_NO_YES_OPTIONS, MESSAGEBOX_NO_YES_OPTION_COUNT, true);
	CMenuOptionChooser* oj2 = new CMenuOptionChooser(LOCALE_STREAMINGMENU_STREAMING_FORCE_AVI_RAWAUDIO, &m_settings.streaming_force_avi_rawaudio, MESSAGEBOX_NO_YES_OPTIONS, MESSAGEBOX_NO_YES_OPTION_COUNT, true);
	CMenuOptionChooser* oj3 = new CMenuOptionChooser(LOCALE_STREAMINGMENU_STREAMING_FORCE_TRANSCODE_VIDEO, &m_settings.streaming_force_transcode_video, MESSAGEBOX_NO_YES_OPTIONS, MESSAGEBOX_NO_YES_OPTION_COUNT, true);
	CMenuOptionChooser* oj4 = new CMenuOptionChooser(LOCALE_STREAMINGMENU_STREAMING_TRANSCODE_VIDEO_CODEC, &m_settings.streaming_transcode_video_codec, STREAMINGMENU_STREAMING_TRANSCODE_VIDEO_CODEC_OPTIONS, STREAMINGMENU_STREAMING_TRANSCODE_VIDEO_CODEC_OPTION_COUNT, true);
	CMenuOptionChooser* oj5 = new CMenuOptionChooser(LOCALE_STREAMINGMENU_STREAMING_RESOLUTION, &m_settings.streaming_resolution, STREAMINGMENU_STREAMING_RESOLUTION_OPTIONS, STREAMINGMENU_STREAMING_RESOLUTION_OPTION_COUNT, true);

	vlc_setup->addItem( mf1);
	vlc_setup->addItem( mf2);
	vlc_setup->addItem( mf3);
	vlc_setup->addItem( mf6);
	vlc_setup->addItem(GenericMenuSeparatorLine);
	vlc_setup->addItem( mf4);
	vlc_setup->addItem( oj3);
	vlc_setup->addItem( oj4);
	vlc_setup->addItem( oj5);
	vlc_setup->addItem(GenericMenuSeparatorLine);
	vlc_setup->addItem( mf5);
	vlc_setup->addItem( oj1);
	vlc_setup->addItem( oj2);

	vlc_setup->exec (NULL, "");
	vlc_setup->hide ();
	delete vlc_setup;
}

int CVLCPlayer::exec(CMenuTarget* parent, const std::string &actionKey)
{
	int   res = menu_return::RETURN_REPAINT;

	if (parent)
		parent->hide();
	
	if(actionKey == "setup")
	{
		VLCPlayerSetup();
		return res;
	}
	else if(actionKey == "Save")
	{
		saveSettings(&m_settings);
		return res;
	}
	else if(actionKey == "file")
	{
		Path_vlc  = "vlc://";
		Path_vlc += m_settings.streaming_server_startdir;
	
		CFileList allfiles;
		readDir_vlc(Path_vlc, &allfiles);
		
		// filebrowser
		/*
		filebrowser = new CFileBrowser();
	
		filebrowser->Dirs_Selectable = false;
		
		filebrowser->Filter = &vlcfilefilter;
				
		filebrowser->exec(Path_vlc.c_str());
		*/
		
		return res;
	}
	else if(actionKey == "vcd")
	{
		return res;
	}
	else if(actionKey == "dvd")
	{
		return res;
	}
	
	
	vlcPlayerMenu();
	
	return res;
}

void CVLCPlayer::vlcPlayerMenu()
{
	loadSettings(&m_settings);
	
	CMenuWidget * vlcMenu = new CMenuWidget("VLC Player", NEUTRINO_ICON_SETTINGS);
	
	vlcMenu->addItem(GenericMenuBack);
	vlcMenu->addItem(GenericMenuSeparatorLine);
	vlcMenu->addItem( new CMenuForwarderNonLocalized("File Player", true, NULL, this, "file"));
	vlcMenu->addItem( new CMenuForwarderNonLocalized("VCD Player", true, NULL, this, "vcd"));
	vlcMenu->addItem( new CMenuForwarderNonLocalized("DVD Player", true, NULL, this, "dvd"));
	vlcMenu->addItem(GenericMenuSeparatorLine);
	vlcMenu->addItem( new CMenuForwarderNonLocalized("Setup", true, NULL, this, "setup"));
	
	vlcMenu->exec(NULL, "");
	vlcMenu->hide();
	delete vlcMenu;
}

void plugin_init(void)
{
}

void plugin_del(void)
{
}

void plugin_exec(void)
{
#if 0
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
			
			/*
			if (m_settings.streaming_vlc10 > 1)
			{
				mrl_str += "file://";
				if (VLCPlayer->filename[namepos + 6] != '/')
					mrl_str += "/";
			}
			*/
						
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
			//VLCPlayer->stream_url += m_settings.streaming_server_ip;
			VLCPlayer->stream_url += ':';
			//VLCPlayer->stream_url += m_settings.streaming_server_port;
			VLCPlayer->stream_url += "/dboxstream";
			
			moviePlayerGui->filename = VLCPlayer->stream_url.c_str();
		}
		
		moviePlayerGui->exec(NULL, "urlplayback");
	}
	
	delete VLCPlayer;
#endif
	
	CVLCPlayer* vlcPlayerMenu = new CVLCPlayer();
	
	vlcPlayerMenu->vlcPlayerMenu();
	
	delete vlcPlayerMenu;
}
 
