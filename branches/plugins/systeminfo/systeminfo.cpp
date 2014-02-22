/*
  neutrinoHD2 project
  
  https://code.google.com/p/neutrinohd2/
  
  $Id: systeminfo.cpp 2014/01/22 mohousch Exp $

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

#include <systeminfo.h>


extern "C" int plugin_exec(void);

sfileline sinbuffer[3*MAXLINES];
sreadline sysbuffer[(3*MAXLINES)];

int slinecount, syscount;
bool refreshIt = true;

// construktor
CBESysInfoWidget::CBESysInfoWidget(int m)
{
	frameBuffer = CFrameBuffer::getInstance();
	
	selected = 0;
	
	// windows size
	width  = w_max ( (frameBuffer->getScreenWidth() / 20 * 17), (frameBuffer->getScreenWidth() / 20 ));
	height = h_max ( (frameBuffer->getScreenHeight() / 20 * 16), (frameBuffer->getScreenHeight() / 20));
	
	//head height
	frameBuffer->getIconSize(NEUTRINO_ICON_SETTINGS, &icon_head_w, &icon_head_h);
	theight = std::max(g_Font[SNeutrinoSettings::FONT_TYPE_CHANNELLIST]->getHeight(), icon_head_h) + 6;
       
	//foot height
	frameBuffer->getIconSize(NEUTRINO_ICON_BUTTON_RED, &icon_foot_w, &icon_foot_h);
	ButtonHeight = std::max(g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL]->getHeight(), icon_foot_h) + 6;
	
	// item height
	fheight = g_Font[SNeutrinoSettings::FONT_TYPE_CHANNELLIST]->getHeight();
	listmaxshow = (height - theight)/fheight;
	
	// recalculate height
	height = theight + listmaxshow*fheight; // recalc height
	
	// coordinate
	x = (((g_settings.screen_EndX- g_settings.screen_StartX)-width) / 2) + g_settings.screen_StartX;
	y = (((g_settings.screen_EndY- g_settings.screen_StartY)-height) / 2) + g_settings.screen_StartY;
	
	//
	liststart = 0;
	state = beDefault;
	mode = m;
}

// paintItem without selected line
void CBESysInfoWidget::paintItem(int pos)
{
	int ypos = y + theight + pos*fheight;
	uint8_t color;
	fb_pixel_t bgcolor;
	
	color = COL_MENUCONTENT;
	bgcolor = COL_MENUCONTENT_PLUS_0;

	frameBuffer->paintBoxRel(x, ypos, width - SCROLLBAR_WIDTH, fheight, bgcolor);

	if ((int)liststart + pos < syscount)
	{
		char tmpline75[75];

		memcpy(tmpline75,  &sysbuffer[liststart+pos].line[0], 75);
		tmpline75[75] = '\0';

		g_Font[SNeutrinoSettings::FONT_TYPE_CHANNELLIST]->RenderString(x + 5, ypos + fheight, width - 2*SCROLLBAR_WIDTH, tmpline75, color);
	}
}

// paintlistbox
void CBESysInfoWidget::paint()
{
	liststart = (selected/listmaxshow)*listmaxshow;

	for(unsigned int count = 0; count < listmaxshow; count++)
	{
		paintItem(count);
	}

	// scrollbar
	int ypos = y + theight;
	int sb = fheight*listmaxshow;
	
	frameBuffer->paintBoxRel(x + width - SCROLLBAR_WIDTH, ypos, SCROLLBAR_WIDTH, sb, COL_MENUCONTENT_PLUS_1);

	int sbc = (syscount/listmaxshow) + 1;
	sbc = (syscount/listmaxshow) + 1;
	float sbh= (sb - 4)/ sbc;
	int sbs  = (selected/listmaxshow);
	frameBuffer->paintBoxRel(x + width - 13, ypos + 2 + int(sbs* sbh), 11, int(sbh),  COL_MENUCONTENT_PLUS_3);

}

// paint head
void CBESysInfoWidget::paintHead()
{
	char buf[100];

	frameBuffer->paintBoxRel(x, y, width, theight, COL_MENUHEAD_PLUS_0, RADIUS_MID, CORNER_TOP);
	
	// icon
	frameBuffer->paintIcon(NEUTRINO_ICON_SETTINGS, x + BORDER_LEFT, y + (theight - icon_head_h)/2);
	
	if(mode == SYSINFO)
		sprintf((char *) buf, "%s", "System-Info:");
	
	if(mode == DMESGINFO)
		sprintf((char *) buf, "%s", "System-Messages:");
	
	if(mode == CPUINFO)
		sprintf((char *) buf, "%s", "CPU/File-Info:");
	
	if(mode == PSINFO)
		sprintf((char *) buf, "%s", "Prozess-Liste:");
	
	// title
	g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE]->RenderString(x + BORDER_LEFT + icon_head_w + 5, y + theight, width, buf, COL_MENUHEAD);
}

// paint foot
void CBESysInfoWidget::paintFoot()
{
	int ButtonWidth = (width - 28) / 4;
	frameBuffer->paintBoxRel(x, y + height, width, ButtonHeight, COL_MENUHEAD_PLUS_0, RADIUS_MID, CORNER_BOTTOM);
	frameBuffer->paintHLine(x, x + width, y, COL_INFOBAR_SHADOW_PLUS_0);

	// sysinfo (red)
	frameBuffer->paintIcon(NEUTRINO_ICON_BUTTON_RED, x + width - 4*ButtonWidth - icon_foot_w - 5, y + height + (ButtonHeight - icon_foot_h)/2);
	g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL]->RenderString(x + width - 4*ButtonWidth, y + height + (ButtonHeight - g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL]->getHeight())/2 + g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL]->getHeight(), ButtonWidth - 26, "info", COL_INFOBAR, 0, true); //UTF-8
	
	// dmesg (green)
	frameBuffer->paintIcon(NEUTRINO_ICON_BUTTON_GREEN, x + width - 3*ButtonWidth - icon_foot_w - 5, y + height + (ButtonHeight - icon_foot_h)/2);
	g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL]->RenderString(x + width - 3*ButtonWidth, y + height + (ButtonHeight - g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL]->getHeight())/2 + g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL]->getHeight(), ButtonWidth - 26, "dmesg", COL_INFOBAR, 0, true);
	
	// cpuinfo (yellow)
	frameBuffer->paintIcon(NEUTRINO_ICON_BUTTON_YELLOW, x + width - 2*ButtonWidth - icon_foot_w - 5, y + height + (ButtonHeight - icon_foot_h)/2);
	g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL]->RenderString(x + width - 2*ButtonWidth, y + height + (ButtonHeight - g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL]->getHeight())/2 + g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL]->getHeight(), ButtonWidth - 26, "cpu/file", COL_INFOBAR, 0, true);
	
	// psinfo (blue)
	frameBuffer->paintIcon(NEUTRINO_ICON_BUTTON_BLUE, x + width - 1*ButtonWidth - icon_foot_w - 5, y + height + (ButtonHeight - icon_foot_h)/2);
	g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL]->RenderString(x + width - 1*ButtonWidth, y + height + (ButtonHeight - g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL]->getHeight())/2 + g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL]->getHeight(), ButtonWidth - 26, "ps", COL_INFOBAR, 0, true);
}

// hide
void CBESysInfoWidget::hide()
{
	frameBuffer->paintBackgroundBoxRel(x, y, width, height + ButtonHeight);
	
	frameBuffer->blit();
}

// main
int CBESysInfoWidget::exec(CMenuTarget *parent, const std::string &/*actionKey*/)
{
	int res = menu_return::RETURN_REPAINT;

	if(mode == SYSINFO)
	{
		sysinfo();
	}
	else if(mode == DMESGINFO)
	{
		dmesg();
	}
	else if(mode == CPUINFO)
	{
		cpuinfo();
	}
	else if(mode == PSINFO)
	{
		ps();
	}

	if (parent)
		parent->hide();

	paintHead();
	paint();
	paintFoot();
	
	frameBuffer->blit();

	neutrino_msg_t msg; 
	neutrino_msg_data_t data;
	int timercount = 0;
	unsigned long long timeoutEnd = g_RCInput->calcTimeoutEnd(5);

	while (msg != (neutrino_msg_t) g_settings.key_channelList_cancel)
	{
		g_RCInput->getMsgAbsoluteTimeout( &msg, &data, &timeoutEnd );

		if (msg <= CRCInput::RC_MaxRC  ) 
			timeoutEnd = g_RCInput->calcTimeoutEnd(5);
		
		if (msg == CRCInput::RC_timeout)
		{
			if (mode == SYSINFO)
			{
				timercount = 0;
				sysinfo();
				selected = 0;
				paintHead();
				paint();
				paintFoot();
			}
			
			if ((mode == DMESGINFO) && (++timercount>11))
			{
				timercount = 0;
				dmesg();
				paintHead();
				paint();
				paintFoot();
			}
			
			if ((mode == PSINFO)&&(refreshIt == true))
			{
				timercount = 0;
				ps();
				paintHead();
				paint();
				paintFoot();
			}

			timeoutEnd = g_RCInput->calcTimeoutEnd(5);
			g_RCInput->getMsgAbsoluteTimeout( &msg, &data, &timeoutEnd );
		}
		
		if ( ((int) msg == g_settings.key_channelList_pageup) && (mode != SYSINFO))
		{
			int step = 0;
			int prev_selected = selected;

			step =  ((int) msg == g_settings.key_channelList_pageup) ? listmaxshow : 1;  // browse or step 1
			selected -= step;
			if((prev_selected - step) < 0) 
				selected = syscount - 1;
			
			if(state == beDefault)
			{
				paintItem(prev_selected - liststart);
				unsigned int oldliststart = liststart;
				liststart = (selected/listmaxshow)*listmaxshow;
				if(oldliststart!=liststart) 
					paint();
				else 
					paintItem(selected - liststart);
			}
		}
		else if (((int) msg == g_settings.key_channelList_pagedown) && (mode != SYSINFO))
		{
			int step = 0;
			int prev_selected = selected;

			step =  ((int) msg == g_settings.key_channelList_pagedown) ? listmaxshow : 1;  // browse or step 1
			selected += step;
			if((int)selected >= syscount) 
				selected = 0;
			
			if(state == beDefault)
			{
				paintItem(prev_selected - liststart);
				unsigned int oldliststart = liststart;
				liststart = (selected/listmaxshow)*listmaxshow;
				if(oldliststart != liststart) 
					paint();
				else 
					paintItem(selected - liststart);
			}
		}
		else if ((msg == CRCInput::RC_red) && (mode != SYSINFO))
		{
			mode = SYSINFO;
			sysinfo();
			selected = 0;
			paintHead();
			paint();
			paintFoot();

		}
		else if ((msg == CRCInput::RC_green) && (mode != DMESGINFO))
		{
			mode = DMESGINFO;
			timercount = 0;
			dmesg();
			selected = 0;
			paintHead();
			paint();
			paintFoot();
		}
		else if ((msg == CRCInput::RC_yellow) && (mode != CPUINFO))
		{
			mode = CPUINFO;
			cpuinfo();
			selected = 0;
			paintHead();
			paint();
			paintFoot();
		}
		else if (msg == CRCInput::RC_blue)
		{
			mode = PSINFO;
			ps();
			selected = 0;
			paintHead();
			paint();
			paintFoot();
		}
		else
		{
			CNeutrinoApp::getInstance()->handleMsg( msg, data );
			// kein canceling...
		}

		frameBuffer->blit();	
	}
	
	hide();
	
	return res;
}

int CBESysInfoWidget::sysinfo()
{
	static long curCPU[5] = {0, 0, 0, 0, 0};
	static long prevCPU[5] = {0, 0, 0, 0, 0};
	double value[5] = {0, 0, 0, 0, 0};
	float faktor;
	int i, j = 0;
	char strhelp[6];
	FILE *f;
	char line[MAXLINES];
	const char *fmt = " %a %d %b %Y %H:%M";
	long t;

	/* Get and Format the SystemTime */
	t = time(NULL);
	struct tm *tp;
	tp = localtime(&t);
	strftime(line, sizeof(line), fmt, tp);
	/* Get and Format the SystemTime end */

	/* Create tmpfile with date /tmp/sysinfo */
	system("echo 'DATUM:' > /tmp/sysinfo");
	f=fopen("/tmp/sysinfo","a");
	if(f)
		fprintf(f,"%s\n", line);
	fclose(f);
	/* Create tmpfile with date /tmp/sysinfo end */

	/* Get the statistics from /proc/stat */
	if(prevCPU[0] == 0)
	{
		f=fopen("/proc/stat","r");
		if(f)
		{
			fgets(line, 256, f); /* cpu */
			sscanf(line,"cpu %lu %lu %lu %lu", &prevCPU[1], &prevCPU[2], &prevCPU[3], &prevCPU[4]);
			for(i = 1; i < 5; i++)
				prevCPU[0] += prevCPU[i];
		}
		fclose(f);
		sleep(1);
	}
	else
	{
		for(i=0;i<5;i++)
				prevCPU[i]=curCPU[i];
	}

	while(((curCPU[0]-prevCPU[0]) < 100) || (curCPU[0]==0))
	{
		f=fopen("/proc/stat","r");
		if(f)
		{
			curCPU[0]=0;
			fgets(line,256,f); /* cpu */
			sscanf(line,"cpu %lu %lu %lu %lu",&curCPU[1],&curCPU[2],&curCPU[3],&curCPU[4]);
			for(i=1;i<5;i++)
				curCPU[0]+=curCPU[i];
		}
		fclose(f);
		if((curCPU[0]-prevCPU[0])<100)
			sleep(1);
	}
	
	// some calculations
	if(!(curCPU[0] - prevCPU[0])==0)
	{
		faktor = 100.0/(curCPU[0] - prevCPU[0]);
		for(i=0;i<4;i++)
			value[i]=(curCPU[i]-prevCPU[i])*faktor;

		value[4]=100.0-value[1]-value[2]-value[3];

		f=fopen("/tmp/sysinfo","a");
		if(f)
		{
			memset(line,0x20,sizeof(line));
			for(i=1, j=0;i<5;i++)
			{
				memset(strhelp,0,sizeof(strhelp));
				sprintf(strhelp,"%.1f", value[i]);
				memcpy(&line[(++j*7)-2-strlen(strhelp)], &strhelp[0], strlen(strhelp));
				memcpy(&line[(j*7)-2], "%", 1);
			}
			line[(j*7)-1]='\0';
			fprintf(f,"\nPERFORMANCE:\n USER:  NICE:   SYS:  IDLE:\n%s\n", line);
		}
		fclose(f);
	}
	/* Get the statistics from /proc/stat end*/

	/* Get kernel-info from /proc/version*/
	f=fopen("/proc/version","r");
	if(f)
	{
		char* token;
		fgets(line,256,f); // version
		token = strstr(line,") (");
		if(token != NULL)
			*++token = 0x0;
		fclose(f);
		f=fopen("/tmp/sysinfo","a");
		fprintf(f, "\nKERNEL:\n %s\n %s\n", line, ++token);
	}
	fclose(f);
	/* Get kernel-info from /proc/version end*/

	/* Get uptime-info from /proc/uptime*/
	f=fopen("/proc/uptime","r");
	if(f)
	{
		fgets(line,256,f);
		float ret[4];
		const char* strTage[2] = {"Tage", "Tag"};
		const char* strStunden[2] = {"Stunden", "Stunde"};
		const char* strMinuten[2] = {"Minuten", "Minute"};
		sscanf(line,"%f",&ret[0]);
		ret[0]/=60;
		ret[1]=long(ret[0])/60/24; // Tage
		ret[2]=long(ret[0])/60-long(ret[1])*24; // Stunden
		ret[3]=long(ret[0])-long(ret[2])*60-long(ret[1])*60*24; // Minuten
		fclose(f);

		f=fopen("/tmp/sysinfo","a");
		if(f)
			fprintf(f, "UPTIME:\n System laeuft seit: %.0f %s %.0f %s %.0f %s\n", ret[1], strTage[int(ret[1])==1], ret[2], strStunden[int(ret[2])==1], ret[3], strMinuten[int(ret[3])==1]);
	}
	fclose(f);
	/* Get uptime-info from /proc/uptime end*/

	return(readList(sinbuffer));
}

int CBESysInfoWidget::cpuinfo()
{
	char Wert1[30];
	char Wert2[10];
	char Wert3[10];
	char Wert4[10];
	char Wert5[6];
	char Wert6[30];

	FILE *f,*w;
	char line[256];
	int i = 0;
	
	/* Get file-info from /proc/cpuinfo*/
	system("df > /tmp/systmp");
	f=fopen("/tmp/systmp","r");
	if(f)
	{
		w=fopen("/tmp/sysinfo","w");
		if(w)
		{
			while((fgets(line,256, f)!=NULL))
			{
				sscanf(line,"%s %s %s %s %s %s ", &Wert1, &Wert2, &Wert3, &Wert4, &Wert5, &Wert6);
				if(i++)
					fprintf(w,"\nFilesystem: %s\n  1-KBlocks: %s\n  Used: %s\n  Free: %s\n  Use%%: %s\nMounted on: %s\n",Wert1,Wert2,Wert3,Wert4,Wert5,Wert6);
			}
			fprintf(w,"\nCPU:\n\n");
			fclose(w);
		}
	}
	fclose(f);
	/* Get file-info from /proc/cpuinfo end*/

	/* Get cpuinfo from /proc/cpuinfo*/
	system("cat /proc/cpuinfo >> /tmp/sysinfo");
	unlink("/tmp/systmp");
	/* Get cpuinfo from /proc/cpuinfo end*/
	
	return(readList(sinbuffer));

}

int CBESysInfoWidget::dmesg()
{
	/* Get System-Messages from dmesg*/
	system("dmesg > /tmp/sysinfo");
	/* Get System-Messages from dmesg end*/

	return(readList(sinbuffer));
}

int CBESysInfoWidget::ps()
{
	/* Get Processlist from ps*/
	system("ps -A > /tmp/sysinfo");
	/* Get Processlist from ps end*/

	return(readList(sinbuffer));
}

// read infos
int CBESysInfoWidget::readList(struct sfileline *sinbuffer)
{
	FILE *fp;
	char line[256];

	memset(sinbuffer, 0, (3*MAXLINES) * sizeof(struct sfileline));
	memset(sysbuffer, 0, (3*MAXLINES) * sizeof(struct sreadline));

	fp = fopen("/tmp/sysinfo","rb");

	if(fp == NULL)
		return(-1);

	slinecount = 0;
	syscount = 0;

	while(fgets(line, sizeof(line), fp) != NULL)
	{
		//line[256] = '\0';
		memcpy(sysbuffer[syscount].line, line, sizeof(line));
		sinbuffer[slinecount].state = true;
		sinbuffer[slinecount++].addr = sysbuffer[syscount++].line;
	}
	fclose(fp);
	
	if (selected >= slinecount)
		selected = slinecount - 1;
	
	return(0);
}

int plugin_exec(void)
{
	printf("Plugins: starting systeminfo\n");
	
	CBESysInfoWidget * SysInfoWidget = new CBESysInfoWidget();
	
	SysInfoWidget->exec(NULL, "");
	SysInfoWidget->hide();
	
	delete SysInfoWidget;
	
	return 0;
}
