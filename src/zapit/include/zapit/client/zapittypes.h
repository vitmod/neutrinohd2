/*
 * $Id: zapittypes.h,v 1.23 2004/02/24 23:50:57 thegoodguy Exp $
 *
 * zapit's types which are used by the clientlib - d-box2 linux project
 *
 * (C) 2002 by thegoodguy <thegoodguy@berlios.de>
 * (C) 2002, 2003 by Andreas Oberritter <obi@tuxbox.org>
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

#ifndef __zapittypes_h__
#define __zapittypes_h__

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdint.h>

#include <inttypes.h>
#include <string>
#include <map>

#include <linux/dvb/frontend.h>


/*zapit types*/
typedef uint16_t t_service_id;
#define SCANF_SERVICE_ID_TYPE "%hx"

typedef uint16_t t_original_network_id;
#define SCANF_ORIGINAL_NETWORK_ID_TYPE "%hx"

typedef uint16_t t_transport_stream_id;
#define SCANF_TRANSPORT_STREAM_ID_TYPE "%hx"

typedef int16_t t_satellite_position;
#define SCANF_SATELLITE_POSITION_TYPE "%hd"

typedef uint16_t t_network_id;
//Introduced by Nirvana 11/05. Didn't check if there are similar types

typedef uint16_t t_bouquet_id;
//Introduced by Nirvana 11/05. Didn't check if there are similar types

typedef uint32_t t_transponder_id;

#define CREATE_TRANSPONDER_ID_FROM_ORIGINALNETWORK_TRANSPORTSTREAM_ID(original_network_id,transport_stream_id) ((((t_original_network_id) original_network_id) << 16) | (t_transport_stream_id) transport_stream_id)

/* unique channel identification */
typedef uint64_t t_channel_id;

#define CREATE_CHANNEL_ID_FROM_SERVICE_ORIGINALNETWORK_TRANSPORTSTREAM_ID(service_id,original_network_id,transport_stream_id) ((((t_channel_id)transport_stream_id) << 32) | (((t_channel_id)original_network_id) << 16) | (t_channel_id)service_id)
#define CREATE_CHANNEL_ID CREATE_CHANNEL_ID_FROM_SERVICE_ORIGINALNETWORK_TRANSPORTSTREAM_ID(service_id, original_network_id, transport_stream_id)
#define GET_ORIGINAL_NETWORK_ID_FROM_CHANNEL_ID(channel_id) ((t_original_network_id)((channel_id) >> 16))
#define GET_SERVICE_ID_FROM_CHANNEL_ID(channel_id) ((t_service_id)(channel_id))
#define PRINTF_CHANNEL_ID_TYPE "%16llx"
#define PRINTF_CHANNEL_ID_TYPE_NO_LEADING_ZEROS "%llx"
#define SCANF_CHANNEL_ID_TYPE "%llx"
#define CREATE_CHANNEL_ID64 (((uint64_t)(satellitePosition+freq*4) << 48) | ((uint64_t) transport_stream_id << 32) | ((uint64_t)original_network_id << 16) | (uint64_t)service_id)

#define SAME_TRANSPONDER(id1, id2) ((id1 >> 16) == (id2 >> 16))

typedef uint64_t transponder_id_t;
typedef uint16_t freq_id_t;

#define PRINTF_TRANSPONDER_ID_TYPE "%12llx"
#define TRANSPONDER_ID_NOT_TUNED 0

#define CREATE_TRANSPONDER_ID_FROM_SATELLITEPOSITION_ORIGINALNETWORK_TRANSPORTSTREAM_ID(freq, satellitePosition, original_network_id, transport_stream_id) ( ((uint64_t)freq << 48) |  ((uint64_t) ( satellitePosition >= 0 ? satellitePosition : (uint64_t)(0xF000+ abs(satellitePosition))) << 32) | ((uint64_t)transport_stream_id << 16) | (uint64_t)original_network_id)

#define GET_ORIGINAL_NETWORK_ID_FROM_TRANSPONDER_ID(transponder_id) ((t_original_network_id)(transponder_id      ))
#define GET_TRANSPORT_STREAM_ID_FROM_TRANSPONDER_ID(transponder_id) ((t_transport_stream_id)(transponder_id >> 16))
#define GET_SATELLITEPOSITION_FROM_TRANSPONDER_ID(transponder_id)   ((t_satellite_position )(transponder_id >> 32))
#define GET_SAT_FROM_TPID(transponder_id)   ((t_satellite_position )(transponder_id >> 32) & 0xFFFF)
#define GET_FREQ_FROM_TPID(transponder_id) ((freq_id_t)(transponder_id >> 48))


/* diseqc types */
typedef enum {
	NO_DISEQC,
	MINI_DISEQC,
	SMATV_REMOTE_TUNING,
	DISEQC_1_0,
	DISEQC_1_1,
	DISEQC_1_2,
	DISEQC_ADVANCED
} diseqc_t;

/* dvb transmission types */
typedef enum {
	DVB_C,
	DVB_S,
	DVB_T
} delivery_system_t;

/* service types */
typedef enum {
	ST_RESERVED,
	ST_DIGITAL_TELEVISION_SERVICE,
	ST_DIGITAL_RADIO_SOUND_SERVICE,
	ST_TELETEXT_SERVICE,
	ST_NVOD_REFERENCE_SERVICE,
	ST_NVOD_TIME_SHIFTED_SERVICE,
	ST_MOSAIC_SERVICE,
	ST_PAL_CODED_SIGNAL,
	ST_SECAM_CODED_SIGNAL,
	ST_D_D2_MAC,
	ST_FM_RADIO,
	ST_NTSC_CODED_SIGNAL,
	ST_DATA_BROADCAST_SERVICE,
	ST_COMMON_INTERFACE_RESERVED,
	ST_RCS_MAP,
	ST_RCS_FLS,
	ST_DVB_MHP_SERVICE,
	
	ST_MPEG_2_HD_TELEVISION_SERVICE = 0x11, 	//0x11
	
	/* 0x12 to 0x15: reserved for future use */
	
	ST_AVC_SD_DIGITAL_TV_SERVICE 		= 0x16,
	ST_AVC_SD_NVOD_TIME_SHIFTED_SERVICE 	= 0x17,
	ST_AVC_SD_NVOD_REFERENCE_SERVICE 	= 0x18,
	ST_AVC_HD_DIGITAL_TV_SERVICE 		= 0x19,
	ST_AVC_HD_NVOD_TIME_SHIFTED_SERVICE 	= 0x1A,
	ST_AVC_HD_NVOD_REFERENCE_SERVICE 	= 0x1B,
	
	/* 3DTV */
	ST_3DTV1_TELEVISION_SERVICE = 0x1C,
	ST_3DTV2_TELEVISION_SERVICE = 0x1D,
	ST_3DTV3_TELEVISION_SERVICE = 0x1E,
	
	ST_MULTIFEED				= 0x69
	
	/* 0x80 - 0xFE: user defined*/
	/* 0xFF: reserved for future use*/
} service_type_t;

/* complete transponder-parameters in a struct */
typedef struct TP_parameter
{
	uint64_t TP_id;					/* diseqc<<24 | feparams->frequency>>8 */
	uint8_t polarization;
	uint8_t diseqc;
	int scan_mode;
	
	struct dvb_frontend_parameters feparams;
} TP_params;
 
typedef struct Zapit_config {
	int makeRemainingChannelsBouquet;
	int saveLastChannel;
	int scanSDT;
} t_zapit_config;

//complete zapit start thread-parameters in a struct
typedef struct ZAPIT_start_arg
{
	int lastchannelmode;
	t_channel_id startchanneltv_id;
	t_channel_id startchannelradio_id;
	int startchanneltv_nr;
	int startchannelradio_nr;
	int uselastchannel;
	
	int video_mode;
} Z_start_arg;
//

typedef enum {
	FE_SINGLE,
	//FE_TWIN,
	FE_LOOP,
	FE_NOTCONNECTED, // do we really need this
} fe_mode_t;


#endif /* __zapittypes_h__ */