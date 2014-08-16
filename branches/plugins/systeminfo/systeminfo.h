/*
  neutrinoHD2 project
  
  https://code.google.com/p/neutrinohd2/
  
  $Id: systeminfo.h 2014/01/22 mohousch Exp $

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

#ifndef __sysinfo__
#define __sysinfo__

#include <plugin.h>

#define MAXLINES 256


typedef struct sfileline
{
	bool state;
	char *addr;
}sfileline;

typedef struct sreadline
{
	char line[256];
}sreadline;

class CSysInfoWidget : public CMenuTarget
{
	private:
		enum {
			SYSINFO = 1,
			DMESGINFO,
			CPUINFO,
			PSINFO
		};
		
		CFrameBuffer *frameBuffer;

		enum
		{
			beDefault,
			beMoving
		} state;

		unsigned int selected;
		unsigned int origPosition;
		unsigned int newPosition;

		unsigned int liststart;
		unsigned int listmaxshow;
		unsigned int numwidth;
		int fheight; // Fonthoehe Bouquetlist-Inhalt
		int theight; // Fonthoehe Bouquetlist-Titel
		
		int icon_head_w;
		int icon_head_h;
		
		int icon_foot_w;
		int icon_foot_h;

		int  ButtonHeight;

		bool syslistChanged;
		int width;
		int height;
		int x;
		int y;
		int mode;

		void paintItem(int pos);
		void paint();
		void paintHead();
		void paintFoot();
		//void hide();

		int sysinfo();
		int dmesg();
		int cpuinfo();
		int ps();
		int readList(struct sfileline *inbuffer);

	public:
		CSysInfoWidget(int m = SYSINFO);
		int exec(CMenuTarget *parent, const std::string &actionKey);
		void hide();
};

#endif

