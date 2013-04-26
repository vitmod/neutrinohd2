/*
	Neutrino-GUI  -   DBoxII-Project

	audioplayer
	Copyright (C) 2002 Bjoern Kalkbrenner <terminar@cyberphoria.org>
	Copyright (C) 2002,2003 Dirch
	Copyright (C) 2002,2003,2004 Zwen
	Homepage: http://www.dbox2.info/

	Kommentar:

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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "global.h"
#include <stdio.h>
#include <fcntl.h>
#include <sched.h>

#include <neutrino.h>
#include <driver/audioplay.h>
#include <driver/netfile.h>

#if ENABLE_GSTREAMER
#include <playback_cs.h>
extern cPlayback * playback;
#endif


void CAudioPlayer::stop()
{
	state = CBaseDec::STOP_REQ;
	
#if ENABLE_GSTREAMER
	playback->Stop();
#endif	
	if(thrPlay)
		pthread_join(thrPlay,NULL);
	thrPlay = 0;	
}

void CAudioPlayer::pause()
{
	if(state == CBaseDec::PLAY || state == CBaseDec::FF || state == CBaseDec::REV)
	{
		state = CBaseDec::PAUSE;
#if ENABLE_GSTREAMER
		playback->SetSpeed(0);
#endif
	}
	else if(state==CBaseDec::PAUSE)
	{
		state = CBaseDec::PLAY;
#if ENABLE_GSTREAMER
		playback->SetSpeed(1);
#endif		
	}
}

void CAudioPlayer::ff(unsigned int seconds)
{
	m_SecondsToSkip = seconds;

	if(state == CBaseDec::PLAY || state == CBaseDec::PAUSE || state == CBaseDec::REV)
	{
		state = CBaseDec::FF;
#if ENABLE_GSTREAMER
		playback->SetSpeed(2);
#endif
	}
	else if(state == CBaseDec::FF)
	{
		state = CBaseDec::PLAY;
		
#if ENABLE_GSTREAMER	
		playback->SetSpeed(1);
#endif
	}
}

void CAudioPlayer::rev(unsigned int seconds)
{
	m_SecondsToSkip = seconds;

	if(state == CBaseDec::PLAY || state == CBaseDec::PAUSE || state == CBaseDec::FF)
	{
		state = CBaseDec::REV;
#if ENABLE_GSTREAMER
		playback->SetSpeed(-2);
#endif
	}
	else if(state == CBaseDec::REV)
	{
		state = CBaseDec::PLAY;
		
#if ENABLE_GSTREAMER
		playback->SetSpeed(1);
#endif
	}
}

CAudioPlayer * CAudioPlayer::getInstance()
{
	static CAudioPlayer * AudioPlayer = NULL;
	
	if(AudioPlayer == NULL)
	{
		AudioPlayer = new CAudioPlayer();
	}
	
	return AudioPlayer;
}

void* CAudioPlayer::PlayThread( void * /*dummy*/ )
{
	int soundfd = -1;
	
#if ENABLE_GSTREAMER
	int position = 0;
	
	do {
		if(! playback->GetPosition(position))
		{
			getInstance()->state = CBaseDec::STOP;
			break;
		}
		getInstance()->m_played_time = position/1000;
	}while(getInstance()->state != CBaseDec::STOP_REQ);
#else
	// Decode stdin to stdout.
	CBaseDec::RetCode Status = CBaseDec::DecoderBase( &getInstance()->m_Audiofile, soundfd, &getInstance()->state, &getInstance()->m_played_time, &getInstance()->m_SecondsToSkip );

	if (Status != CBaseDec::OK)
	{
		fprintf( stderr, "Error during decoding: %s.\n",
				( Status == CBaseDec::READ_ERR ) ? "READ_ERR" :
				( Status == CBaseDec::WRITE_ERR ) ? "WRITE_ERR" :
				( Status == CBaseDec::DSPSET_ERR ) ? "DSPSET_ERR" :
				( Status == CBaseDec::DATA_ERR ) ? "DATA_ERR" :
				( Status == CBaseDec::INTERNAL_ERR ) ? "INTERNAL_ERR" :
				"unknown" );
	}	

	getInstance()->state = CBaseDec::STOP;	
#endif

	pthread_exit(0);

	return NULL;
}

bool CAudioPlayer::play(const CAudiofile* file, const bool highPrio)
{
	if (state != CBaseDec::STOP)
		stop();

	getInstance()->clearFileData();

	/* 
	transfer information from CAudiofile to member variable, so that it does not have to be gathered again
	this assignment is important, otherwise the player would crash if the file currently played was deleted from the playlist
	*/
	m_Audiofile = * file;

	state = CBaseDec::PLAY;

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	if(highPrio)
	{
		struct sched_param param;
		pthread_attr_setschedpolicy(&attr, SCHED_RR);
		param.sched_priority=1;
		pthread_attr_setschedparam(&attr, &param);
		usleep(100000); // give the event thread some time to handle his stuff
						// without this sleep there were duplicated events...
	}

	bool ret = true;
	
#if ENABLE_GSTREAMER
		playback->Close();
		
		playback->Open();
				
		if(!playback->Start( (char *)file->Filename.c_str() ))
			ret = false;
#endif	

#warning fixme: There must be a way to call the playing thread without arguments. (NULL did not work for me)
	if (pthread_create (&thrPlay, &attr, PlayThread, (void*)&ret) != 0 )
	{
		perror("audioplay: pthread_create(PlayThread)");
		ret = false;
	}

	pthread_attr_destroy(&attr);

	return ret;
}

CAudioPlayer::CAudioPlayer()
{
	init();
}

void CAudioPlayer::init()
{
	CBaseDec::Init();
	state = CBaseDec::STOP;
		
	thrPlay = 0;	
}

void CAudioPlayer::sc_callback(void *arg)
{
	bool changed=false;
	CSTATE *stat = (CSTATE*)arg;
	
	if(m_Audiofile.MetaData.artist != stat->artist)
	{
		m_Audiofile.MetaData.artist = stat->artist;
		changed = true;
	}
	
	if (m_Audiofile.MetaData.title != stat->title)
	{
		m_Audiofile.MetaData.title = stat->title;
		changed = true;
	}
	
	if (m_Audiofile.MetaData.sc_station != stat->station)
	{
		m_Audiofile.MetaData.sc_station = stat->station;
		changed = true;
	}
	
	if (m_Audiofile.MetaData.genre != stat->genre)
	{
		m_Audiofile.MetaData.genre = stat->genre;
		changed = true;
	}
	
	if(changed)
	{
		m_played_time = 0;
	}
	
	m_sc_buffered = stat->buffered;
	m_Audiofile.MetaData.changed = changed;
	
	//printf("Callback %s %s %s %d\n",stat->artist, stat->title, stat->station, stat->buffered);
}

void CAudioPlayer::clearFileData()
{
	m_Audiofile.clear();
	m_played_time=0;
	m_sc_buffered=0;
}

CAudioMetaData CAudioPlayer::getMetaData()
{
	CAudioMetaData m = m_Audiofile.MetaData;
	m_Audiofile.MetaData.changed = false;
	return m;
}

bool CAudioPlayer::hasMetaDataChanged()
{
	return m_Audiofile.MetaData.changed;
}

bool CAudioPlayer::readMetaData(CAudiofile* const file, const bool nice)
{
	return CBaseDec::GetMetaDataBase(file, nice);
}

bool CAudioPlayer::readCoverData(CAudiofile * const file, const bool nice)
{
	return CBaseDec::GetCoverDataBase(file, nice);
}



