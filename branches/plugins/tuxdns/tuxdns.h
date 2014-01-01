#ifndef __tuxdnsconf__
#define __tuxdnsconf__

#include <plugin.h>

using namespace std;

class CTuxdnsConf : public CMenuTarget
{
	private:

		CFrameBuffer	*frameBuffer;
		int x;
		int y;
		int width;
		int height;
		int hheight,mheight; // head/menu font height

		//int	TE;
		char	pause[5];
		int	verbose;
		char	user[21];
		char	pass[21];
		char	host[32];

	public:

		CTuxdnsConf();
		void paint();
		void hide();
		int  exec(CMenuTarget* parent, const std::string & actionKey);
		void TuxdnsSettings();
		bool SaveSettings();
};

#endif //__tuxdnsconf__
 
