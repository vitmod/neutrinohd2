/*
  neutrinoHD2 project
  
  https://code.google.com/p/neutrinohd2/
  
  $Id: mediaplayer.h 2014/01/25 mohousch Exp $

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

#ifndef __mediaplayer_h__
#define __mediaplayer_h__

#include <plugin.h>


class CMediaPlayer : public CMenuTarget
{
	public:
		enum state
		{
			STOPPED     =  0,
			PLAY        =  1,
			PAUSE       =  2,
			FF          =  3,
			REW         =  4,
			SLOW        =  5,
			SOFTRESET   = 99
		};
		
		int playstate;
		
		unsigned short apids[10];
		unsigned short ac3flags[10];
		unsigned short numpida;
		unsigned short vpid;
		unsigned short vtype;
		std::string language[10];

		unsigned int currentapid;
		unsigned int currentac3;
		unsigned int apidchanged;

		int ac3state;
		
		std::string filename;
		int m_LastMode;
		int speed;
		int64_t duration;
		int64_t position;
		int file_prozent;
		
		int selected;
		CFileList filelist;
		
		std::string title;
		std::string info1;
		std::string info2;
		
		// timeosd
		CTimeOSD FileTime;
		
		// global flags
		bool update_lcd;
		bool open_filebrowser;	//always default true (true valeue is needed for file/moviebrowser)
		bool start_play;
		bool exit;
		bool was_file;
		bool m_loop;

	private:
		
	public:
		CMediaPlayer();
		~CMediaPlayer();
		
		void cutNeutrino();
		void restoreNeutrino();
		void showAudioDialog();
		void updateLCD();
		
		void playFile(/*const char *file*/void);
		void showFileInfo();
		void showHelpTS();
		
		int exec(CMenuTarget *parent,  const std::string &actionkey);
};

class CMediaPlayerAPIDSelectExec : public CMediaPlayer
{
	public:
		int exec(CMenuTarget * parent, const std::string & actionKey);
};

#endif
