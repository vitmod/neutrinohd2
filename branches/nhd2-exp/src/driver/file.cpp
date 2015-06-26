/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: non-nil; c-basic-offset: 4 -*- */
/*
	Neutrino-GUI  -   DBoxII-Project
	
	$Id: file.cpp 2013/10/12 mohousch Exp $

	Copyright (C) 2001 Steffen Hehn 'McClean'
	Homepage: http://dbox.cyberphoria.org/

	Kommentar:

	Diese GUI wurde von Grund auf neu programmiert und sollte nun vom
	Aufbau und auch den Ausbaumoeglichkeiten gut aussehen. Neutrino basiert
	auf der Client-Server Idee, diese GUI ist also von der direkten DBox-
	Steuerung getrennt. Diese wird dann von Daemons uebernommen.


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
#endif /* HAVE_CONFIG_H */

#include <stdio.h>

#include <sys/stat.h>
#include <driver/file.h>
#include <cstring>
#include <cstdlib>


/* ATTENTION: the array file_extension_list MUST BE SORTED ASCENDING (cf. sort, man bsearch) - otherwise bsearch will not work correctly! */
const char * const file_extension_list[] =
{
	"aac",
	"aiff",
	"asf",   
	"avi",  
	"bmp",  
	"cdr",  
	"crw",
	"dat",
	"divx",
	"dts",
	"flac",
	"flv",
	"gif",  
	"imu",  
	"jpeg", 
	"jpg",
	"m2a",
	"m2p",
	"m2ts",
	"m3u", 
	"m4a",
	"mkv",
	"mov",
	"mp2",  
	"mp3",
	"mp4",
	"mpa",
	"mpeg",
	"mpg",
	"mpv",
	"mts",
	"ogg",  
	"png",
	"pls",
	"sh",
	"trp"
	"ts",
	"txt",	
	"url",
	"vdr",
	"vob",  
	"wav", 
	"wmv",
	"xml"
};

/* ATTENTION: the array file_extension_list MUST BE SORTED ASCENDING (cf. sort, man bsearch) - otherwise bsearch will not work correctly! */
const CFile::FileType file_type_list[] =
{
	CFile::FILE_AAC,
	CFile::FILE_AIFF,
	CFile::FILE_ASF, 
	CFile::FILE_AVI, 
	CFile::FILE_PICTURE, 
	CFile::FILE_CDR,
	CFile::FILE_PICTURE,
	CFile::FILE_DAT,
	CFile::FILE_DIVX,
	CFile::FILE_DTS,
	CFile::FILE_FLAC,
	CFile::FILE_MPG,
	CFile::FILE_PICTURE, 
	CFile::FILE_IMU, 
	CFile::FILE_PICTURE, 
	CFile::FILE_PICTURE, 
	CFile::FILE_MP3, 
	CFile::FILE_M2P,
	CFile::FILE_M2TS,
	CFile::FILE_PLAYLIST, 
	CFile::FILE_PLAYLIST,
	CFile::FILE_MKV,
	CFile::FILE_MOV,
	CFile::FILE_MP3, 
	CFile::FILE_MP3, 
	CFile::FILE_MPG, 
	CFile::FILE_MP3, 
	CFile::FILE_MPG,
	CFile::FILE_MPG,
	CFile::FILE_MPV,
	CFile::FILE_MTS,
	CFile::FILE_OGG, 
	CFile::FILE_PICTURE, 
	CFile::FILE_PLAYLIST,
	CFile::FILE_TEXT,
	CFile::FILE_TRP,
	CFile::FILE_TS,
	CFile::FILE_TEXT, 
	CFile::FILE_URL, 
	CFile::FILE_VDR,
	CFile::FILE_VOB, 
	CFile::FILE_WAV, 
	CFile::FILE_WMV,
	CFile::FILE_XML
};

int mycasecmp(const void * a, const void * b)
{
	return strcasecmp(*(const char * *)a, *(const char * *)b);
}

CFile::CFile()
  : Size( 0 ), Mode( 0 ), Marked( false ), Time( 0 )
{
}

CFile::FileType CFile::getType(void) const
{
	if(S_ISDIR(Mode))
		return FILE_DIR;

	std::string::size_type ext_pos = Name.rfind('.');

	if (ext_pos != std::string::npos)
	{
		const char * key = &(Name.c_str()[ext_pos + 1]);

		void * result = ::bsearch(&key, file_extension_list, sizeof(file_extension_list) / sizeof(const char *), sizeof(const char *), mycasecmp);
		
		if (result != NULL)
			return file_type_list[(const char * *)result - (const char * *)&file_extension_list];
	}
	
	return FILE_UNKNOWN;
}

std::string CFile::getFileName(void) const  // return name.extension or folder name without trailing /
{
	std::string::size_type namepos = Name.rfind('/');

	return (namepos == std::string::npos) ? Name : Name.substr(namepos + 1);
}

std::string CFile::getPath(void) const      // return complete path including trailing /
{
	int pos = 0;

	return ((pos = Name.rfind('/')) > 1) ? Name.substr(0, pos + 1) : "/";
}
