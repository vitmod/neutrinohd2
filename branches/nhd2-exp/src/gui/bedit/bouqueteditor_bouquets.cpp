/*
	Neutrino-GUI  -   DBoxII-Project
	
	$Id: bouqueteditor_bouquets.cpp 2013/10/12 mohousch Exp $

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
#endif

#include <gui/bedit/bouqueteditor_bouquets.h>

#include <global.h>
#include <neutrino.h>

#include <driver/fontrenderer.h>
#include <driver/screen_max.h>
#include <gui/bedit/bouqueteditor_channels.h>
#include <gui/widget/buttons.h>
#include <gui/widget/hintbox.h>
#include <gui/widget/icons.h>
#include <gui/widget/messagebox.h>
#include <gui/widget/stringinput.h>

/*zapit includes*/
#include <client/zapitclient.h>
#include <client/zapittools.h>


extern CBouquetManager * g_bouquetManager;


CBEBouquetWidget::CBEBouquetWidget()
{
	frameBuffer = CFrameBuffer::getInstance();

	// foot
	frameBuffer->getIconSize(NEUTRINO_ICON_BUTTON_RED, &icon_foot_w, &icon_foot_h);
	ButtonHeight = std::max(g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL]->getHeight(), icon_foot_h) + 6;

	theight = g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE]->getHeight() + 6;
	fheight = g_Font[SNeutrinoSettings::FONT_TYPE_CHANNELLIST]->getHeight();

	selected = 0;
	liststart = 0;
	state = beDefault;
	blueFunction = beRename;
	
	Bouquets = &g_bouquetManager->Bouquets;
}

void CBEBouquetWidget::paintItem(int pos)
{
	uint8_t    color;
	fb_pixel_t bgcolor;
	int ypos = y + theight + pos*fheight;
	unsigned int current = liststart + pos;

	if (current == selected) 
	{
		color   = COL_MENUCONTENTSELECTED;
		bgcolor = COL_MENUCONTENTSELECTED_PLUS_0;
		
		// itemBox
		frameBuffer->paintBoxRel(x, ypos, width - SCROLLBAR_WIDTH, fheight, COL_MENUCONTENT_PLUS_0);
		frameBuffer->paintBoxRel(x, ypos, width - SCROLLBAR_WIDTH, fheight, bgcolor);
	} 
	else 
	{
		bool has_channels = true;
		if(current < Bouquets->size())
			has_channels = ((*Bouquets)[current]->tvChannels.size() > 0) || ((*Bouquets)[current]->radioChannels.size() > 0);
		
		color   = has_channels ? COL_MENUCONTENT : COL_MENUCONTENTINACTIVE;
		bgcolor = has_channels ? COL_MENUCONTENT_PLUS_0 : COL_MENUCONTENTINACTIVE_PLUS_0;
		
		// itemBox
		frameBuffer->paintBoxRel(x, ypos, width - SCROLLBAR_WIDTH, fheight, bgcolor);
	}

	if(current < Bouquets->size()) 
	{
		if ((current == selected) && (state == beMoving))
			frameBuffer->paintIcon(NEUTRINO_ICON_BUTTON_YELLOW, x + 8, ypos + 4);

		if ((*Bouquets)[current]->bLocked)
			frameBuffer->paintIcon(NEUTRINO_ICON_LOCK, x + 7, ypos);

		if ((*Bouquets)[current]->bHidden)
			frameBuffer->paintIcon(NEUTRINO_ICON_HIDDEN, x + 37, ypos);

		g_Font[SNeutrinoSettings::FONT_TYPE_CHANNELLIST]->RenderString(x + 68, ypos + fheight, width - 68, (*Bouquets)[current]->bFav ? g_Locale->getText(LOCALE_FAVORITES_BOUQUETNAME) : (*Bouquets)[current]->Name, color, 0, true);
	}
}

void CBEBouquetWidget::paint()
{
	liststart = (selected/listmaxshow)*listmaxshow;

	for(unsigned int count = 0; count < listmaxshow; count++)
	{
		paintItem(count);
	}

	int ypos = y + theight;
	
	// scrollbar
	int sb = fheight*listmaxshow;
	frameBuffer->paintBoxRel(x + width - SCROLLBAR_WIDTH, ypos, SCROLLBAR_WIDTH, sb,  COL_MENUCONTENT_PLUS_1);

	int sbc= ((Bouquets->size() - 1)/ listmaxshow)+ 1;
	float sbh= (sb- 4)/ sbc;
	int sbs= (selected/listmaxshow);

	frameBuffer->paintBoxRel(x + width - 13, ypos + 2 + int(sbs* sbh) , 11, int(sbh),  COL_MENUCONTENT_PLUS_3);
}

void CBEBouquetWidget::paintHead()
{
	frameBuffer->paintBoxRel(x, y, width, theight, COL_MENUHEAD_PLUS_0, RADIUS_MID, CORNER_TOP);
	
	g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE]->RenderString(x + BORDER_LEFT, y + (theight - g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE]->getHeight())/2 + g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE]->getHeight() , width, g_Locale->getText(LOCALE_BOUQUETLIST_HEAD), COL_MENUHEAD, 0, true); // UTF-8
}

const struct button_label CBEBouquetWidgetButtons[3] =
{
	{ NEUTRINO_ICON_BUTTON_RED   , LOCALE_BOUQUETEDITOR_DELETE },
	{ NEUTRINO_ICON_BUTTON_GREEN , LOCALE_BOUQUETEDITOR_ADD    },
	{ NEUTRINO_ICON_BUTTON_YELLOW, LOCALE_BOUQUETEDITOR_MOVE   }
};

void CBEBouquetWidget::paintFoot()
{
	struct button_label Button[4];
	Button[0] = CBEBouquetWidgetButtons[0];
	Button[1] = CBEBouquetWidgetButtons[1];
	Button[2] = CBEBouquetWidgetButtons[2];
	Button[3].button = NEUTRINO_ICON_BUTTON_BLUE;

	frameBuffer->paintBoxRel(x, y + height, width, ButtonHeight, COL_MENUHEAD_PLUS_0, RADIUS_MID, CORNER_BOTTOM);

	switch( blueFunction)
	{
		case beRename:
			Button[3].locale = LOCALE_BOUQUETEDITOR_RENAME;
			break;
			
		case beHide:
			Button[3].locale = LOCALE_BOUQUETEDITOR_HIDE;
			break;
			
		case beLock:
			Button[3].locale = LOCALE_BOUQUETEDITOR_LOCK;
			break;
	}
	::paintButtons(frameBuffer, g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL], g_Locale, x + BORDER_LEFT, y + height, (width - icon_foot_w - BORDER_LEFT) / 4, 4, Button, ButtonHeight);
	
	// setup icon
	int icon_w, icon_h;
	frameBuffer->getIconSize(NEUTRINO_ICON_BUTTON_SETUP, &icon_w, &icon_h);
	frameBuffer->paintIcon(NEUTRINO_ICON_BUTTON_SETUP, x + width - icon_w - BORDER_RIGHT, y + height + (ButtonHeight - icon_h)/2);
}

void CBEBouquetWidget::hide()
{
	frameBuffer->paintBackgroundBoxRel(x, y, width, height + ButtonHeight);
	frameBuffer->blit();
}

int CBEBouquetWidget::exec(CMenuTarget* parent, const std::string &/*actionKey*/)
{
	neutrino_msg_t      msg;
	neutrino_msg_data_t data;

	int res = menu_return::RETURN_REPAINT;

	if (parent)
		parent->hide();

	width = w_max ( (frameBuffer->getScreenWidth() / 20 * 17), (frameBuffer->getScreenWidth() / 20 ));
	height = h_max ( (frameBuffer->getScreenHeight() / 20 * 16), (frameBuffer->getScreenHeight() / 20));
	
	listmaxshow = (height - theight)/fheight;
	height = theight + listmaxshow*fheight; // recalc height
	
        x = frameBuffer->getScreenX() + (frameBuffer->getScreenWidth() - width) / 2;
        y = frameBuffer->getScreenY() + (frameBuffer->getScreenHeight() - height - ButtonHeight) / 2;
	
	Bouquets = &g_bouquetManager->Bouquets;
	
	paintHead();
	paint();
	paintFoot();
	
	frameBuffer->blit();

	bouquetsChanged = false;

	unsigned long long timeoutEnd = CRCInput::calcTimeoutEnd(g_settings.timing[SNeutrinoSettings::TIMING_EPG]);

	bool loop=true;
	while (loop)
	{
		g_RCInput->getMsgAbsoluteTimeout( &msg, &data, &timeoutEnd );

		if ( msg <= CRCInput::RC_MaxRC )
			timeoutEnd = CRCInput::calcTimeoutEnd(g_settings.timing[SNeutrinoSettings::TIMING_EPG]);

		if ((msg == CRCInput::RC_timeout) || (msg == (neutrino_msg_t)g_settings.key_channelList_cancel))
		{
			if (state == beDefault)
			{
				if (bouquetsChanged)
				{
					int result = MessageBox(LOCALE_BOUQUETEDITOR_NAME, LOCALE_BOUQUETEDITOR_SAVECHANGES, CMessageBox::mbrYes, CMessageBox::mbAll);

					switch( result )
					{
						case CMessageBox::mbrYes :
							loop=false;
							saveChanges();
						break;
						
						case CMessageBox::mbrNo :
							loop=false;
							discardChanges();
						break;
						
						case CMessageBox::mbrCancel :
							paintHead();
							paint();
							paintFoot();
						break;
					}
				}
				else
				{
					loop = false;
				}
			}
			else if (state == beMoving)
			{
				cancelMoveBouquet();
			}
		}
		//
		// -- For more convenience: include browsing of list (paging)  (rasc, 2002-04-02)
		// -- The keys should be configurable. Problem is: red/green key, which is the
		// -- default in neutrino is used as a function key here... so use left/right
		//
		else if (msg==CRCInput::RC_up || msg==(neutrino_msg_t)g_settings.key_channelList_pageup)
		{
			if (!(Bouquets->empty()))
			{
				int step = 0;
				int prev_selected = selected;

				step = (msg == (neutrino_msg_t)g_settings.key_channelList_pageup) ? listmaxshow : 1;  // browse or step 1
				selected -= step;
				if((prev_selected - step) < 0)		// because of uint
				{
					selected = Bouquets->size() - 1;
				}

				if (state == beDefault)
				{
					paintItem(prev_selected - liststart);
					unsigned int oldliststart = liststart;
					liststart = (selected/listmaxshow)*listmaxshow;
					if(oldliststart!=liststart)
					{
						paint();
					}
					else
					{
						paintItem(selected - liststart);
					}
				}
				else if (state == beMoving)
				{
					internalMoveBouquet(prev_selected, selected);
				}
			}
		}
		else if (msg==CRCInput::RC_down || msg==(neutrino_msg_t)g_settings.key_channelList_pagedown)
		{
			unsigned int step = 0;
			int prev_selected = selected;

			step = (msg == (neutrino_msg_t)g_settings.key_channelList_pagedown) ? listmaxshow : 1;  // browse or step 1
			selected += step;

			if(selected >= Bouquets->size())
			{
				if (((Bouquets->size() / listmaxshow) + 1) * listmaxshow == Bouquets->size() + listmaxshow) // last page has full entries
					selected = 0;
				else
					selected = ((step == listmaxshow) && (selected < (((Bouquets->size() / listmaxshow) + 1) * listmaxshow))) ? (Bouquets->size() - 1) : 0;
			}

			if (state == beDefault)
			{
				paintItem(prev_selected - liststart);
				unsigned int oldliststart = liststart;
				liststart = (selected/listmaxshow)*listmaxshow;
				if(oldliststart!=liststart)
				{
					paint();
				}
				else
				{
					paintItem(selected - liststart);
				}
			}
			else if (state == beMoving)
			{
				internalMoveBouquet(prev_selected, selected);
			}
		}
		else if(msg==CRCInput::RC_red)
		{
			if (state == beDefault)
				deleteBouquet();
		}
		else if(msg==CRCInput::RC_green)
		{
			if (state == beDefault)
				addBouquet();
		}
		else if(msg==CRCInput::RC_yellow)
		{
			if (selected < Bouquets->size()) /* Bouquets->size() might be 0 */
			{
				liststart = (selected/listmaxshow)*listmaxshow;
				if (state == beDefault)
					beginMoveBouquet();
				paintItem(selected - liststart);
			}
		}
		else if(msg==CRCInput::RC_blue)
		{
			if (selected < Bouquets->size()) /* Bouquets->size() might be 0 */
			{
				if (state == beDefault)
					switch (blueFunction)
					{
						case beRename:
							renameBouquet();
							break;
						case beHide:
							switchHideBouquet();
							break;
						case beLock:
							switchLockBouquet();
							break;
					}
			}
		}
		else if(msg==CRCInput::RC_setup)
		{
			if (state == beDefault)
			switch (blueFunction)
			{
				case beRename:
					blueFunction = beHide;
				break;
				case beHide:
					blueFunction = beLock;
				break;
				case beLock:
					blueFunction = beRename;
				break;
			}
			paintFoot();
		}
		else if(msg==CRCInput::RC_ok)
		{
			if (state == beDefault)
			{
				if (selected < Bouquets->size()) /* Bouquets->size() might be 0 */
				{
					CBEChannelWidget* channelWidget = new CBEChannelWidget((*Bouquets)[selected]->bFav ? g_Locale->getText(LOCALE_FAVORITES_BOUQUETNAME) : (*Bouquets)[selected]->Name, selected);
					channelWidget->exec( this, "");
					if (channelWidget->hasChanged())
						bouquetsChanged = true;
					delete channelWidget;
					paintHead();
					paint();
					paintFoot();
				}
			}
			else if (state == beMoving)
			{
				finishMoveBouquet();
			}
		}
		else if( CRCInput::isNumeric(msg) )
		{
			if (state == beDefault)
			{
				//kein pushback - wenn man versehentlich wo draufkommt is die edit-arbeit umsonst
				//selected = oldselected;
				//g_RCInput->postMsg( msg, data );
				//loop=false;
			}
			else if (state == beMoving)
			{
				cancelMoveBouquet();
			}
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

void CBEBouquetWidget::deleteBouquet()
{
	if (selected >= Bouquets->size()) /* Bouquets->size() might be 0 */
		return;

	if (MessageBox(LOCALE_FILEBROWSER_DELETE, (*Bouquets)[selected]->bFav ? g_Locale->getText(LOCALE_FAVORITES_BOUQUETNAME) : (*Bouquets)[selected]->Name, CMessageBox::mbrNo, CMessageBox::mbYes|CMessageBox::mbNo)!=CMessageBox::mbrYes)
		return;

	g_bouquetManager->deleteBouquet(selected);
	Bouquets = &g_bouquetManager->Bouquets;
	if (selected >= Bouquets->size())
		selected = Bouquets->empty() ? 0 : (Bouquets->size() - 1);
	bouquetsChanged = true;
	paint();
}

void CBEBouquetWidget::addBouquet()
{
	std::string newName = inputName("", LOCALE_BOUQUETEDITOR_BOUQUETNAME);
	if (!(newName.empty()))
	{
		g_bouquetManager->addBouquet(newName, true);
		Bouquets = &g_bouquetManager->Bouquets;
		selected = Bouquets->empty() ? 0 : (Bouquets->size() - 1);
		bouquetsChanged = true;
	}
	
	paintHead();
	paint();
	paintFoot();
}

void CBEBouquetWidget::beginMoveBouquet()
{
	state = beMoving;
	origPosition = selected;
	newPosition = selected;
}

void CBEBouquetWidget::finishMoveBouquet()
{
	state = beDefault;
	if (newPosition != origPosition)
	{
		Bouquets = &g_bouquetManager->Bouquets;
		bouquetsChanged = true;
	}
	paint();
}

void CBEBouquetWidget::cancelMoveBouquet()
{
	state = beDefault;
	internalMoveBouquet( newPosition, origPosition);
	bouquetsChanged = false;
}

void CBEBouquetWidget::internalMoveBouquet( unsigned int fromPosition, unsigned int toPosition)
{
	if ( (int) toPosition == -1 ) return;
	if ( toPosition == Bouquets->size()) return;

	g_bouquetManager->moveBouquet(fromPosition, toPosition);
	Bouquets = &g_bouquetManager->Bouquets;
	bouquetsChanged = true;

	selected = toPosition;
	newPosition = toPosition;
	paint();
}

void CBEBouquetWidget::renameBouquet()
{
	if ((*Bouquets)[selected]->bFav)
		return;

	std::string newName = inputName((*Bouquets)[selected]->Name.c_str(), LOCALE_BOUQUETEDITOR_NEWBOUQUETNAME);
	if (newName != (*Bouquets)[selected]->Name)
	{
		g_bouquetManager->Bouquets[selected]->Name = newName;
		g_bouquetManager->Bouquets[selected]->bUser = true;

		bouquetsChanged = true;
	}
	paintHead();
	paint();
	paintFoot();
}

void CBEBouquetWidget::switchHideBouquet()
{
	bouquetsChanged = true;
	(*Bouquets)[selected]->bHidden = !(*Bouquets)[selected]->bHidden;

	paint();
}

void CBEBouquetWidget::switchLockBouquet()
{
	bouquetsChanged = true;
	(*Bouquets)[selected]->bLocked = !(*Bouquets)[selected]->bLocked;

	paint();
}

std::string CBEBouquetWidget::inputName(const char * const defaultName, const neutrino_locale_t caption)
{
	//FIXME: max input it too long than bqt window width
	char Name[MAX_INPUT_CHARS + 1];

	strncpy(Name, defaultName, MAX_INPUT_CHARS + 1);

	CStringInputSMS *nameInput = new CStringInputSMS(caption, Name);
	nameInput->exec(this, "");
	delete nameInput;

	return std::string(Name);
}

void CBEBouquetWidget::saveChanges()
{
	CHintBox* hintBox= new CHintBox(LOCALE_BOUQUETEDITOR_NAME, g_Locale->getText(LOCALE_BOUQUETEDITOR_SAVINGCHANGES), 480); // UTF-8
	hintBox->paint();
	
	g_Zapit->saveBouquets();
	g_Zapit->reinitChannels();
	
	hintBox->hide();
	delete hintBox;
}

void CBEBouquetWidget::discardChanges()
{
	CHintBox* hintBox= new CHintBox(LOCALE_BOUQUETEDITOR_NAME, g_Locale->getText(LOCALE_BOUQUETEDITOR_DISCARDINGCHANGES), 480); // UTF-8
	hintBox->paint();
	
	g_Zapit->restoreBouquets();
	
	hintBox->hide();
	delete hintBox;
}
