/*
	Neutrino-GUI  -   DBoxII-Project

	Copyright (C) 2004 Zwen
	Copyright (C) 2013 martii

	ffmpeg audio decoder layer
	Homepage: http://forum.tuxbox.org/forum

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

#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <sstream>

#include <driver/audioplay.h>
#include <sectionsd/edvbstring.h> // UTF8
#include <ffmpegdec.h>

//#include <OpenThreads/ScopedLock>

#include <driver/netfile.h>
#include <system/helpers.h>

#ifdef AV_NOPTS_VALUE 
#undef AV_NOPTS_VALUE
#endif
#define AV_NOPTS_VALUE          ((int64_t)UINT64_C(0x8000000000000000))

#define ProgName "FfmpegDec"

#define COVERDIR "/tmp/cover"

//static OpenThreads::Mutex mutex;

static int cover_count = 0;

static void log_callback(void *, int, const char *format, va_list ap)
{
	vfprintf(stderr, format, ap);
}

CFfmpegDec::CFfmpegDec()
{
	av_log_set_callback(log_callback);
	//meta_data_valid = false;
	buffer_size = 0x1000;
	buffer = NULL;
	avc = NULL;
	avcodec_register_all();
	av_register_all();
}

CFfmpegDec::~CFfmpegDec()
{
	DeInit();
}

int CFfmpegDec::Read(void *buf, size_t buf_size)
{
	return (int) fread(buf, 1, buf_size, (FILE *) in);
}

static int read_packet(void *opaque, uint8_t *buf, int buf_size)
{
	return ((CFfmpegDec *) opaque)->Read(buf, (size_t) buf_size);
}

int64_t CFfmpegDec::Seek(int64_t offset, int whence)
{
	if (whence == AVSEEK_SIZE)
		return (int64_t) -1;

	fseek((FILE *) in, (long) offset, whence);
	return (int64_t) ftell((FILE *) in);
}

static int64_t seek_packet(void *opaque, int64_t offset, int whence)
{
	return ((CFfmpegDec *) opaque)->Seek(offset, whence);
}

bool CFfmpegDec::Init(void *_in, const CFile::FileType /* ft */)
{
        title = "";
        artist = "";
        date = "";
        album = "";
        genre = "";
        type_info = "";
	total_time = 0;
	bitrate = 0;

#ifdef FFDEC_DEBUG
	av_log_set_level(AV_LOG_DEBUG);
#endif

	AVIOContext *avioc = NULL;
	in = _in;
	is_stream = fseek((FILE *)in, 0, SEEK_SET);
	buffer = (unsigned char *) av_malloc(buffer_size);
	if (!buffer)
		return false;
	avc = avformat_alloc_context();
	if (!avc) {
		av_freep(&buffer);
		return false;
	}

	if (is_stream)
		avc->probesize = 128 * 1024;

	//av_opt_set_int(avc, "analyzeduration", 1 * AV_TIME_BASE, 0);

	avioc = avio_alloc_context (buffer, buffer_size, 0, this, read_packet, NULL, seek_packet);
	if (!avioc) {
		av_freep(&buffer);
		avformat_free_context(avc);
		return false;
	}
	avc->pb = avioc;
	avc->flags |= AVFMT_FLAG_CUSTOM_IO|AVFMT_FLAG_KEEP_SIDE_DATA;

	AVInputFormat *input_format = NULL;

	int r = avformat_open_input(&avc, "", input_format, NULL);
	if (r) {
		char buf[200]; av_strerror(r, buf, sizeof(buf));
		fprintf(stderr, "%d %s %d: %s\n", __LINE__, __func__,r,buf);
		if (avioc)
			av_freep(avioc);
		if (avc) {
			//avformat_close_input(&avc);
			avformat_free_context(avc);
			avc = NULL;
		}
		return false;
	}
	return true;
}

void CFfmpegDec::DeInit(void)
{
	if (avc) {
		//avformat_close_input(&avc);

		avformat_free_context(avc);
		avc = NULL;
	}

	in = NULL;
}

bool CFfmpegDec::GetMetaData(FILE *_in, const bool /*nice*/, CAudioMetaData* m)
{
	return SetMetaData(_in, m);
}

CFfmpegDec* CFfmpegDec::getInstance()
{
	static CFfmpegDec* FfmpegDec = NULL;
	if(FfmpegDec == NULL)
	{
		FfmpegDec = new CFfmpegDec();
	}
	return FfmpegDec;
}

bool CFfmpegDec::SetMetaData(FILE *_in, CAudioMetaData* m, bool save_cover)
{
	if (!Init(_in, (const CFile::FileType) m->type))
		return false;

	int ret = avformat_find_stream_info(avc, NULL);
	if (ret < 0) 
	{
		DeInit();
		printf("avformat_find_stream_info error %d\n", ret);
		return false;
	}
		
	if (!is_stream) 
	{
		GetMeta(avc->metadata);
		for(unsigned int i = 0; i < avc->nb_streams; i++) 
		{
			if (avc->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
				GetMeta(avc->streams[i]->metadata);
		}
	}

	//fseek((FILE *) in, 0, SEEK_SET);

	codec = NULL;
	best_stream = av_find_best_stream(avc, AVMEDIA_TYPE_AUDIO, -1, -1, &codec, 0);

	if (best_stream < 0) 
	{
		DeInit();
		return false;
	}

	if (!codec)
		codec = avcodec_find_decoder(avc->streams[best_stream]->codec->codec_id);
		
	samplerate = avc->streams[best_stream]->codec->sample_rate;
	mChannels = av_get_channel_layout_nb_channels(avc->streams[best_stream]->codec->channel_layout);

	std::stringstream ss;

	if (codec && codec->long_name != NULL)
		type_info = codec->long_name;
	else if(codec && codec->name != NULL)
		type_info = codec->name;
	else
		type_info = "unknown";
	ss << " / " << mChannels << " channel" << ( mChannels > 1 ? "s" : "");
	type_info += ss.str();

	bitrate = 0;
	total_time = 0;

	if (avc->duration != static_cast<int64_t>(AV_NOPTS_VALUE))
		total_time = avc->duration / static_cast<int64_t>(AV_TIME_BASE);
		
	printf("CFfmpegDec: format %s (%s) duration %ld\n", avc->iformat->name, type_info.c_str(), total_time);

	// cover
	#if 0
	for(unsigned int i = 0; i < avc->nb_streams; i++) 
	{
		if (avc->streams[i]->codec->bit_rate > 0)
			bitrate += avc->streams[i]->codec->bit_rate;
		if (save_cover && (avc->streams[i]->disposition & AV_DISPOSITION_ATTACHED_PIC) ) 
		{
			mkdir(COVERDIR, 0755);
			std::string cover(COVERDIR);
			cover += "/" + to_string(cover_count++) + ".jpg";
			FILE *f = fopen(cover.c_str(), "wb");
			if (f) 
			{
				AVPacket *pkt = &avc->streams[i]->attached_pic;
				fwrite(pkt->data, pkt->size, 1, f);
				fclose(f);
				m->cover = cover;
				//m->cover_temporary = true;
			}
		}
	}
	#endif
		
	//
	if(!total_time && m->filesize && bitrate)
		total_time = 8 * m->filesize / bitrate;

	//meta_data_valid = true;
	m->changed = true;
	
	if (!is_stream) 
	{
		m->title = title;
		m->artist = artist;
		m->date = date;
		m->album = album;
		m->genre = genre;
		m->total_time = total_time;
	}
	m->type_info = type_info;
	// make sure bitrate is set to prevent refresh metadata from gui, its a flag
	m->bitrate = bitrate ? bitrate : 1; 
	m->samplerate = samplerate;

	return true;
}

#if LIBAVCODEC_VERSION_MAJOR < 54
void CFfmpegDec::GetMeta(AVMetadata* metadata)
#else
void CFfmpegDec::GetMeta(AVDictionary * metadata)
#endif
{
#if LIBAVCODEC_VERSION_MAJOR < 54
	AVMetadataTag *tag = NULL;
#else  
	AVDictionaryEntry *tag = NULL;
#endif	
	
#if LIBAVCODEC_VERSION_MAJOR < 54
	while ((tag = av_metadata_get(metadata, "", tag, AV_METADATA_IGNORE_SUFFIX)))
#else	
	while ((tag = av_dict_get(metadata, "", tag, AV_DICT_IGNORE_SUFFIX))) 
#endif	  
	{
		if(!strcasecmp(tag->key,"Title")) 
		{
			if (title.empty()) 
			{
				title = isUTF8(tag->value) ? tag->value : convertLatin1UTF8(tag->value);
				title = trim(title);
			}
			continue;
		}
		if(!strcasecmp(tag->key,"Artist")) 
		{
			if (artist.empty()) 
			{
				artist = isUTF8(tag->value) ? tag->value : convertLatin1UTF8(tag->value);
				artist = trim(artist);
			}
			continue;
		}
		if(!strcasecmp(tag->key,"Year")) 
		{
			if (date.empty()) 
			{
				date = isUTF8(tag->value) ? tag->value : convertLatin1UTF8(tag->value);
				date = trim(date);
			}
			continue;
		}
		if(!strcasecmp(tag->key,"Album")) 
		{
			if (album.empty()) 
			{
				album = isUTF8(tag->value) ? tag->value : convertLatin1UTF8(tag->value);
				album = trim(album);
			}
			continue;
		}
		if(!strcasecmp(tag->key,"Genre")) 
		{
			if (genre.empty()) 
			{
				genre = isUTF8(tag->value) ? tag->value : convertLatin1UTF8(tag->value);
				genre = trim(genre);
			}
			continue;
		}
	}
}
