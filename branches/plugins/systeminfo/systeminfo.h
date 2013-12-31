#ifndef __sysinfo__
#define __sysinfo__

#include <plugin.h>

#define MAXLINES 255


typedef struct sfileline
{
	bool state;
	char *addr;
}sfileline;

typedef struct sreadline
{
	char line[256];
}sreadline;

class CBESysInfoWidget : public CMenuWidget
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
		CBESysInfoWidget(int m = SYSINFO);
		int exec(CMenuTarget* parent, const std::string & actionKey);
		void hide();
};

#endif

