/*
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef __PLAYBACK_CS_H
#define __PLAYBACK_CS_H

#include <string>
#include <stdint.h>

#include <vector>

#include <config.h>

#if defined (ENABLE_PVR)
#define INBUF_SIZE (256 * 1024)

typedef enum {
	PLAYMODE_TS = 0,
	PLAYMODE_FILE,
} playmode_t;

typedef enum {
	FILETYPE_UNKNOWN,
	FILETYPE_TS,
	FILETYPE_MPG,
	FILETYPE_VDR
} filetype_t;

/*
typedef enum {
	STATE_STOP,
	STATE_PLAY,
	STATE_PAUSE,
	STATE_FF,
	STATE_REW
} playstate_t;
*/

typedef struct {
	std::string Name;
	off_t Size;
} filelist_t;
#endif

#if defined (ENABLE_LIBEPLAYER3)
#include <libeplayer3/include/common.h>
#include <libeplayer3/include/subtitle.h>
#include <linux/fb.h>


extern OutputHandler_t		OutputHandler;
extern PlaybackHandler_t	PlaybackHandler;
extern ContainerHandler_t	ContainerHandler;
extern ManagerHandler_t		ManagerHandler;
#endif

#if defined (ENABLE_GSTREAMER)
#include <gst/gst.h>
#include <gst/pbutils/missing-plugins.h>
#endif

typedef enum {
	STATE_STOP,
	STATE_PLAY,
	STATE_PAUSE,
	STATE_FF,
	STATE_REW,
	STATE_SLOW
} playstate_t;

class cPlayback
{
	private:
#if defined (ENABLE_PVR)		
		uint8_t *inbuf;
		ssize_t inbuf_pos;
		ssize_t inbuf_sync;
		uint8_t *pesbuf;
		ssize_t pesbuf_pos;
		ssize_t inbuf_read(void);
		ssize_t read_ts(void);
		ssize_t read_mpeg(void);

		uint8_t cc[256];

		int in_fd;

		int video_type;
		//int playback_speed;
		
		std::vector<filelist_t> filelist; /* for multi-file playback */

		bool filelist_auto_add(void);
		int mf_open(int fileno);
		int mf_close(void);
		off_t mf_lseek(off_t pos);
		off_t mf_getsize(void);
		int curr_fileno;
		off_t curr_pos;
		off_t bytes_per_second;

		uint16_t vPid;
		uint16_t aPid;
		bool AC3;
		uint16_t aPids[10];
		unsigned short AC3flags[10];
		std::string aLang[10];
		uint16_t numPida;

		int64_t pts_start;
		int64_t pts_end;
		int64_t pts_curr;
		int64_t get_pts(uint8_t *p, bool pes);

		filetype_t filetype;
		//playstate_t playstate;

		off_t seek_to_pts(int64_t pts);
		off_t mp_seekSync(off_t pos);
		int64_t get_PES_PTS(uint8_t *buf, int len, bool until_eof);

		pthread_t thread;
		bool thread_started;
#endif

#if defined (ENABLE_LIBEPLAYER3)
		Context_t *player;
#endif

#if defined (ENABLE_GSTREAMER)
		GstElement *m_gst_playbin;
#endif
		bool playing;

		playstate_t playstate;

		int mSpeed;
		int mAudioStream;

	public:
#if defined (ENABLE_PVR)	  
		void playthread();
#endif
		bool Open();
		void Close(void);
		bool Start(char * filename, unsigned short vpid, int vtype, unsigned short apid, bool ac3);
		
		bool Play(void);
		bool SyncAV(void);
		
		bool Stop(void);
		bool SetAPid(unsigned short pid, bool ac3);
		bool SetSpeed(int speed);
		bool SetSlow(int slow);
		bool GetSpeed(int &speed) const;
		bool GetPosition(int &position, int &duration);

		bool SetPosition(int position, bool absolute = false);

		void FindAllPids(uint16_t *apids, unsigned short *ac3flags, uint16_t *numpida, std::string *language);

		cPlayback(int num = 0);
		~cPlayback();
		void getMeta();
};

#endif
