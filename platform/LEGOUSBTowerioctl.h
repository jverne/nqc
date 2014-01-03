//	LEGOUSBTowerioctl.h
//
//	Define control codes for LEGOUSBTower driver
//
//	Written by AS
//	Copyright (C) 1999-2000 The LEGO Company. All rights reserved.
//
//	@doc
//
//

#if _MSC_VER > 1000
	#pragma once
#endif // _MSC_VER > 1000

#ifndef __LEGOUSBTowerioctl__h_
#define __LEGOUSBTowerioctl__h_

	#ifndef CTL_CODE
		#include <winioctl.h>
	#endif

	#define LEGOUSBTOWER_IOCTL_SETRANGE			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
	#define LEGOUSBTOWER_IOCTL_GETRANGE			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)
	#define LEGOUSBTOWER_IOCTL_SETLINK			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS)
	#define LEGOUSBTOWER_IOCTL_GETLINK			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x803, METHOD_BUFFERED, FILE_ANY_ACCESS)
	#define LEGOUSBTOWER_IOCTL_GETCAPS			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x804, METHOD_BUFFERED, FILE_ANY_ACCESS)
	#define LEGOUSBTOWER_IOCTL_GETVERSION		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x805, METHOD_BUFFERED, FILE_ANY_ACCESS)
	#define LEGOUSBTOWER_IOCTL_FLUSH			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x806, METHOD_BUFFERED, FILE_ANY_ACCESS)
	#define LEGOUSBTOWER_IOCTL_SETTIMEOUTS		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x807, METHOD_BUFFERED, FILE_ANY_ACCESS)
	#define LEGOUSBTOWER_IOCTL_GETTIMEOUTS		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x808, METHOD_BUFFERED, FILE_ANY_ACCESS)
	// NOT USED #define LEGOUSBTOWER_IOCTL_RESERVED			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x809, METHOD_BUFFERED, FILE_ANY_ACCESS)
	#define LEGOUSBTOWER_IOCTL_SETBUFFERSIZE	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80A, METHOD_BUFFERED, FILE_ANY_ACCESS)
	#define LEGOUSBTOWER_IOCTL_GETBUFFERSIZE	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80B, METHOD_BUFFERED, FILE_ANY_ACCESS)
	#define LEGOUSBTOWER_IOCTL_RESET			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80C, METHOD_BUFFERED, FILE_ANY_ACCESS)
	#define LEGOUSBTOWER_IOCTL_GETCOPYRIGHT		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80D, METHOD_BUFFERED, FILE_ANY_ACCESS)
	#define LEGOUSBTOWER_IOCTL_GETCREDITS		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80E, METHOD_BUFFERED, FILE_ANY_ACCESS)

	#define LEGOUSBTOWER_IOCTL_SETLEDMODE		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80F, METHOD_BUFFERED, FILE_ANY_ACCESS)
	#define LEGOUSBTOWER_IOCTL_GETLEDMODE		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x810, METHOD_BUFFERED, FILE_ANY_ACCESS)

	#define LEGOUSBTOWER_IOCTL_SETLEDSTATE		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x811, METHOD_BUFFERED, FILE_ANY_ACCESS)
	#define LEGOUSBTOWER_IOCTL_GETLEDSTATE		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x812, METHOD_BUFFERED, FILE_ANY_ACCESS)

	// 0x813 is reserved	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x813, METHOD_BUFFERED, FILE_ANY_ACCESS)
	#define LEGOUSBTOWER_IOCTL_GETERRORSTATE	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x814, METHOD_BUFFERED, FILE_ANY_ACCESS)

	#define LEGOUSBTOWER_IOCTL_SETENDIANNESS	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x815, METHOD_BUFFERED, FILE_ANY_ACCESS)
	#define LEGOUSBTOWER_IOCTL_GETENDIANNESS	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x816, METHOD_BUFFERED, FILE_ANY_ACCESS)

	#define LEGOUSBTOWER_IOCTL_SETCOMMSPEED		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x817, METHOD_BUFFERED, FILE_ANY_ACCESS)
	#define LEGOUSBTOWER_IOCTL_GETCOMMSPEED		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x818, METHOD_BUFFERED, FILE_ANY_ACCESS)

	#define LEGOUSBTOWER_IOCTL_RESETCOMMSTATS	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x819, METHOD_BUFFERED, FILE_ANY_ACCESS)
	#define LEGOUSBTOWER_IOCTL_GETCOMMSTATS		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x81A, METHOD_BUFFERED, FILE_ANY_ACCESS)

	#define LEGOUSBTOWER_IOCTL_SETCARRIERFREQUENCY		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x81B, METHOD_BUFFERED, FILE_ANY_ACCESS)
	#define LEGOUSBTOWER_IOCTL_GETCARRIERFREQUENCY		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x81C, METHOD_BUFFERED, FILE_ANY_ACCESS)
	#define LEGOUSBTOWER_IOCTL_SETCARRIERDUTYCYCLE		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x81D, METHOD_BUFFERED, FILE_ANY_ACCESS)
	#define LEGOUSBTOWER_IOCTL_GETCARRIERDUTYCYCLE		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x81E, METHOD_BUFFERED, FILE_ANY_ACCESS)

	#define LEGOUSBTOWER_IOCTL_SETPARM					CTL_CODE(FILE_DEVICE_UNKNOWN, 0x820, METHOD_BUFFERED, FILE_ANY_ACCESS)
	#define LEGOUSBTOWER_IOCTL_GETPARM					CTL_CODE(FILE_DEVICE_UNKNOWN, 0x821, METHOD_BUFFERED, FILE_ANY_ACCESS)

	#define LEGOUSBTOWER_IOCTL_SETPARM_IRC				CTL_CODE(FILE_DEVICE_UNKNOWN, 0x822, METHOD_BUFFERED, FILE_ANY_ACCESS)
	#define LEGOUSBTOWER_IOCTL_GETPARM_IRC				CTL_CODE(FILE_DEVICE_UNKNOWN, 0x823, METHOD_BUFFERED, FILE_ANY_ACCESS)

	// 824 is reserved		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x824, METHOD_BUFFERED, FILE_ANY_ACCESS)
	#define LEGOUSBTOWER_IOCTL_GETTOWERSTATE			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x825, METHOD_BUFFERED, FILE_ANY_ACCESS)

	#define LEGOUSBTOWER_IOCTL_RESTART					CTL_CODE(FILE_DEVICE_UNKNOWN, 0x826, METHOD_BUFFERED, FILE_ANY_ACCESS)
	// 827 is reserved		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x827, METHOD_BUFFERED, FILE_ANY_ACCESS)


	//	------------------------------------------------------------------
	//	------------------------------------------------------------------
	//	------------------------------------------------------------------
	//
	//	COMMAND STRUCTURES AND CONSTANTS
	//

	//	------------------------------------------------------------------
	//	RANGE
	//
	typedef struct LT_RANGE	{

		DWORD		dwsize;			// Size of this structure
		WORD		wfwresult;		// Firmware return code
		WORD		wspare;			// Reserved
		// --------------------------------------------------

		DWORD	dwrange;			// One of the LT_RANGE... constants

	}LT_RANGE, *PLT_RANGE;

	#define LT_RANGE_INVALID		0x00

	#define LT_RANGE_SHORT			0x01 // - Short
	#define LT_RANGE_MEDIUM			0x02 // - Medium
	#define LT_RANGE_LONG			0x03 // - Long

	//	------------------------------------------------------------------
	//	LINK MODE
	//
	typedef struct LT_LINK	{

		DWORD		dwsize;			// Size of this structure
		WORD		wfwresult;		// Firmware return code
		WORD		wspare;			// Reserved
		// --------------------------------------------------

		DWORD	dwlink;				// One of the LT_LINK... constants

	}LT_LINK, *PLT_LINK;

	#define LT_LINK_INVALID		0x00

	#define LT_LINK_VLL			0x01 // Send VLL
	#define LT_LINK_IR			0x02 // Send/Receive IR
	#define LT_LINK_IRC			0x04 // Send/Receive IR according to the LEGO Infrared Remote Control protocol (Technic Cars)
	#define LT_LINK_RADIO		0x08 // Send/Receive Radio signals

	#define LT_MODE_VLL			LT_LINK_VLL		// Defined for historical/compatibility reasons
	#define LT_MODE_IR			LT_LINK_IR
	#define LT_MODE_IRC			LT_LINK_IRC
	#define LT_MODE_RADIO		LT_LINK_RADIO

	//	------------------------------------------------------------------
	//	CAPABILITIES
	//


	//	@struct LT_CAPS | LEGO Tower Device capabilities (returned by <f LEGOTowerGetCaps>)
	//	<nl>
	//	Tx frequency range is given by min and max values (in KHertz)
	//	Only integer values are allowed (eg 38 == 38 KHz)<nl>
	//	<nl>
	//
	//	Allowed duty cycle values (min-max) in us multiplied by 100. E.g.  1,25 us = 125
	//	Is it possible to set different duty cycle for each range (long-medium-short).
	//	The duty cycle value actually DETERMINES the range (but also the power consumption
	//	of the tower)
	//
	//	Please tinker with these values only if you KNOW VERY WELL what you're doing -- AS
	//
	typedef struct LT_CAPS
	{
		DWORD		dwsize;			// @field Size of this structure
		WORD		wfwresult;		// @field Firmware return code
		WORD		wspare;			// @field Reserved
		// --------------------------------------------------

		WORD		wlink;			// @field Type of capability requested (VLL/IR/IRC)

		// @field <t comm> | comm | Communication capabilities

		//	@struct comm | Communication capabilities
		//
		struct {

			BYTE	bdirections;		// @field LT_CAPS_COMM_DIRECTION_xxx
			BYTE	brange;				// @field LT_CAPS_COMM_RANGE_xxx
			WORD	wtxspeed;			// @field LT_CAPS_COMM_SPEED_xxx
			WORD	wrxspeed;			// @field LT_CAPS_COMM_SPEED_xxx

			// @field <t frequency> | frequency | Minimum/maximum frequency
			// @field <t dutycycle> | dutycycle | Minimum/maximum duty cycle

			//	@struct frequency |
			//
			//	Tx frequency range is given by min and max values (in KHertz)
			//	Only integer values are allowed (eg 38 == 38 KHz)
			//
			struct {
						WORD wmin;		// @field Minimum frequency supported
						WORD wmax;		// @field Maximum frequency supported
			}frequency;

			//	@struct dutycycle |
			//
			//	Allowed duty cycle values (min-max) in us multiplied by 100. E.g.  1,25 us = 125
			//	Is it possible to set different duty cycle for each range (long-medium-short).
			//	The duty cycle value actually DETERMINES the range (but also the power consumption
			//	of the tower)
			//
			//	Please tinker with these values only if you KNOW VERY WELL what you're doing -- AS
			//
			struct {
						WORD wmin;		// @field Minimum duty cycle value
						WORD wmax;		// @field Maximum duty cycle value
			}dutycycle;

		}comm;

		//	Insert other capability types here...
		//

	}LT_CAPS, *PLT_CAPS;


	#define LT_CAPS_COMM_DIRECTION_TRANSMIT		0x01	// - Transmit
	#define LT_CAPS_COMM_DIRECTION_RECEIVE		0x02	// - Receive
	#define LT_CAPS_COMM_DIRECTION_BOTH			(LT_CAPS_COMM_DIRECTION_TRANSMIT | LT_CAPS_COMM_DIRECTION_RECEIVE)	// - Both

	#define LT_CAPS_COMM_RANGE_SHORT			0x01	// - Short
	#define LT_CAPS_COMM_RANGE_MEDIUM			0x02	// - Medium
	#define LT_CAPS_COMM_RANGE_LONG				0x04	// - Long
	#define LT_CAPS_COMM_RANGE_ALL				(LT_CAPS_COMM_RANGE_SHORT | 	LT_CAPS_COMM_RANGE_MEDIUM | LT_CAPS_COMM_RANGE_LONG	) // - All ranges

	#define LT_CAPS_SPEED_BPS_300				LT_SPEED_BPS_300
	#define LT_CAPS_SPEED_BPS_600				LT_SPEED_BPS_600
	#define LT_CAPS_SPEED_BPS_1200				LT_SPEED_BPS_1200
	#define LT_CAPS_SPEED_BPS_2400				LT_SPEED_BPS_2400
	#define LT_CAPS_SPEED_BPS_4800				LT_SPEED_BPS_4800
	#define LT_CAPS_SPEED_BPS_9600				LT_SPEED_BPS_9600
	#define LT_CAPS_SPEED_BPS_19200				LT_SPEED_BPS_19200
	#define LT_CAPS_SPEED_BPS_38400				LT_SPEED_BPS_38400

	#define LT_CAPS_LINK_VLL					LT_LINK_VLL			// Send VLL
	#define LT_CAPS_LINK_IR						LT_LINK_IR			// Send/Receive IR
	#define LT_CAPS_LINK_IRC					LT_LINK_IRC			// Send/Receive IR according to the LEGO Infrared Remote Control protocol (Technic Cars)
	#define LT_CAPS_LINK_RADIO					LT_LINK_RADIO		// Send/Receive Radio signals

	//	------------------------------------------------------------------
	//	TIMEOUT
	//
	typedef struct LT_TIMEOUT
	{
		DWORD		dwsize;			// Size of this structure
		WORD		wfwresult;		// Firmware return code
		WORD		wspare;			// Reserved
		// --------------------------------------------------

		DWORD		dwspare;			// reserved
		DWORD		dwwrite;			// write timeout
		DWORD		dwread_first;		// read timeout (first byte)
		DWORD		dwread_ic;			// read timeout (intercharacter timeout)

	}LT_TIMEOUT, *PLT_TIMEOUT;

	//	------------------------------------------------------------------
	//	VERSION
	//
	//
	//	@struct LT_VER |
	//
	//	Version description structure -- used internally by <t LT_VERSION> structure
	//
	typedef struct LT_VER {

		unsigned char	bmajor;		// @field	Major version number
		unsigned char	bminor;		// @field	Minor version number
		unsigned short	wbuild;		// @field	Build number

	}LT_VER, *PLT_VER;

	//	@struct LT_VERSION |
	//
	//	Version description structure, filled in by <f LEGOTowerGetVersion>.
	//
	typedef struct LT_VERSION {

		DWORD		dwsize;			// @field Size of this structure
		WORD		wfwresult;		// @field Firmware return code
		WORD		wspare;			// @field Reserved
		// --------------------------------------------------

		LT_VER		driver;			// @field <t LT_VER> | driver	| driver version information
		LT_VER		firmware;		// @field <t LT_VER> | firmware | firmware version information

	}LT_VERSION, *PLT_VERSION;


	// -------------------------------------------------------------
	// FLUSH
	//

	typedef struct LT_FLUSH {

		DWORD		dwsize;			// Size of this structure
		WORD		wfwresult;		// Firmware return code
		WORD		wspare;			// Reserved
		// --------------------------------------------------

		WORD		wflags;			// One or more of the LT_FLUSH flags, ORred together

	}LT_FLUSH, *PLT_FLUSH;

	#define LT_FLUSH_TX_BUFFER		0x01		// Transmission buffer
	#define LT_FLUSH_RX_BUFFER		0x02		// Receiver buffer
	#define LT_FLUSH_ALL			0x03		// All buffers


	// -------------------------------------------------------------
	// BUFFER SIZE
	//

	typedef struct LT_BUFFERSIZE {

		DWORD		dwsize;			// Size of this structure
		WORD		wfwresult;		// Firmware return code		// Not used for this command
		WORD		wspare;			// Reserved
		// --------------------------------------------------

		DWORD		dwbufsize;		// New driver's receive buffer size (in bytes)

	}LT_BUFFERSIZE, *PLT_BUFFERSIZE;


	// -------------------------------------------------------------
	// RESET TOWER
	//
	typedef struct LT_RESET	{

		DWORD		dwsize;			// Size of this structure
		WORD		wfwresult;		// Firmware return code
		WORD		wspare;			// Reserved
		// --------------------------------------------------

		WORD		wflags;			// no flags defined in this version. Just set it to 0

	}LT_RESET, *PLT_RESET;


	// -------------------------------------------------------------
	// COPYRIGHT
	//

	//	There is no structure for copyright info. You just pass DeviceIoControl a
	//	ptr to the buffer and set the size field accordingly.
	//
	//	The copyright string is returned in UNICODE format. Size is given to the
	//	driver in BYTES
	//

	// -------------------------------------------------------------
	// LEDMODE and LEDSTATE
	//
	typedef struct LT_LEDMODE	{

		DWORD		dwsize;			// Size of this structure
		WORD		wfwresult;		// Firmware return code
		WORD		wspare;			// Reserved
		// --------------------------------------------------

		WORD		wledid;			// id of the led to set mode (not used in this version; set to LT_LED_ID)
		WORD		wledmode;		// new led mode -- one of LT_LEDMODE_xxx constants

	}LT_LEDMODE, *PLT_LEDMODE;

	#define LT_LED_ID				0x01		// ID LED (the green led)
	#define LT_LED_VLL				0x02		// VLL LED (the red led)

	#define LT_LED_CTRL_HW			0x01		// the led is automatically controlled by the tower itself
	#define LT_LED_CTRL_SW			0x02		// the led is controlled by the host software via the SETLEDSTATE command

	#define LT_LED_COLOR_BLACK		0x00		// Black = same as off
	#define LT_LED_COLOR_GREEN		0x01		// Green
	#define LT_LED_COLOR_YELLOW		0x02		// Yellow
	#define LT_LED_COLOR_ORANGE		0x04		// Orange
	#define LT_LED_COLOR_RED		0x08		// Red

	//	more color codes can be added here...
	//
	#define LT_LED_COLOR_DEFAULT	0xFF		// Whatever color is the default

	#define LT_LED_COLOR_ON			LT_LED_COLOR_DEFAULT		// generic ON
	#define LT_LED_COLOR_OFF		LT_LED_COLOR_BLACK			// generic OFF

	typedef struct LT_LEDSTATE
	{
		DWORD		dwsize;			// Size of this structure
		WORD		wfwresult;		// Firmware return code
		WORD		wspare;			// Reserved
		// --------------------------------------------------

		WORD		wledid;			// id of the led to set mode (not used in this version; set to LT_LED_ID)
		WORD		wcolor;			// led color -- one of the LT_LED_COLOR_xxx constants

	}LT_LEDSTATE, *PLT_LEDSTATE;


	// -------------------------------------------------------------
	// ERRORSTATE
	//
	typedef struct LT_ERRORSTATE
	{
		DWORD		dwsize;			// Size of this structure
		WORD		wfwresult;		// Firmware return code
		WORD		wspare;			// Reserved
		// --------------------------------------------------

		WORD		werrorstate;	// current error state of the firmware

	}LT_ERRORSTATE, *PLT_ERRORSTATE;


	// -------------------------------------------------------------
	// ENDIANNESS
	//
	typedef struct LT_ENDIANNESS
	{
		DWORD		dwsize;			// Size of this structure
		WORD		wfwresult;		// Firmware return code
		WORD		wspare;			// Reserved
		// --------------------------------------------------

		WORD		wendianness;	// one of the LT_ENDIAN_xxx constants

	}LT_ENDIANNESS, *PLT_ENDIANNESS;

	#define LT_ENDIAN_LITTLE		0x01	// Little endian (Intel word format: LSB first, MSB last)
	#define LT_ENDIAN_BIG			0x02	// Big endian (Others word format: MSB first, LSB last)

	//	-------------------------------------------------------------
	//	COMMUNICATION SPEED
	//

	typedef struct LT_COMMSPEED
	{
		DWORD		dwsize;			// Size of this structure
		WORD		wfwresult;		// Firmware return code
		WORD		wspare;			// Reserved
		// --------------------------------------------------

		WORD		wtxspeed;		// transmission speed (a LT_SPEED_xxx constant)
		WORD		wrxspeed;		// receive speed
		WORD		wlink;			// NOT USED; set to 0 or (better for future compatibility) to LT_LINK_IR
		WORD		wspare2;

	} LT_COMMSPEED, *PLT_COMMSPEED;

	#define LT_SPEED_BPS_300				0x0001
	#define LT_SPEED_BPS_600				0x0002
	#define LT_SPEED_BPS_1200				0x0004
	#define LT_SPEED_BPS_2400				0x0008
	#define LT_SPEED_BPS_4800				0x0010
	#define LT_SPEED_BPS_9600				0x0020
	#define LT_SPEED_BPS_19200				0x0040
	#define LT_SPEED_BPS_38400				0x0080

	//	-------------------------------------------------------------
	//	COMMSTATS
	//

	//	@struct LT_COMMSTATS | Communication statistics (returned by <f LEGOTowerGetCommStats>)
	//
	//
	typedef struct LT_COMMSTATS
	{
		DWORD		dwsize;				// @field Size of this structure
		WORD		wfwresult;			// @field Firmware return code
		WORD		wspare;				// @field Reserved
		// --------------------------------------------------

		//	Communication statistics
		//

		DWORD dwrxbytes;				// @field total received bytes
		DWORD dwoverrun;				// @field # of times overrun error has happen
		DWORD dwnoise;					// @field # of bytes with wrong bits
		DWORD dwframing;				// @field # of bytes with framing errors

	}LT_COMMSTATS, *PLT_COMMSTATS;


	//	--------------------------------------------------------------
	//	FREQUENCY (valid for IR and IRC modes)
	//
	typedef struct LT_FREQUENCY
	{
		DWORD		dwsize;			// Size of this structure
		WORD		wfwresult;		// Firmware return code
		WORD		wspare;			// Reserved
		// --------------------------------------------------

		WORD		wfrequency;		// carrier frequency. Allowable values are between the ranges
									//	given in the caps structure (typically between 30-99 KHz)

	} LT_FREQUENCY, *PLT_FREQUENCY;


	//	--------------------------------------------------------------
	//	DUTY CYCLE (valid for IR and IRC modes)
	//

	typedef struct LT_DUTYCYCLE
	{
		DWORD		dwsize;			// Size of this structure
		WORD		wfwresult;		// Firmware return code
		WORD		wspare;			// Reserved
		// --------------------------------------------------

		WORD		wrange;			// Range for which we set/get the duty cycle --> LT_RANGE_xxx
		WORD		wdutycycle;		// Duty cycle value, in 1/100 of usec

	} LT_DUTYCYCLE, *PLT_DUTYCYCLE;


	//	--------------------------------------------------------------
	//	IRC (Technic cars) protocol-specific parameters
	//
	#define LT_PARM_IRC_PACKETSIZE       	0x01	// Packet size, in bytes (default = 2)
	#define LT_PARM_IRC_TIMEFRAME       	0x02	// Time frame for packet syncronization, in ms (default = 80)

	typedef struct LT_PARM
	{
		DWORD		dwsize;			// Size of this structure
		WORD		wfwresult;		// Firmware return code
		WORD		wspare;			// Reserved

		// --------------------------------------------------

		BYTE		bparm;			// parameter code
		BYTE		barg;			// Parameter value
		WORD		windex;			// index

	} LT_PARM, *PLT_PARM;


	//	--------------------------------------------------------------
	//	GETTOWERSTATE
	//

	//	@enum LT_TOWERSTATE_xxx | Flags returned by <f LEGOTowerGetState>
	//
	#define LT_TOWERSTATE_CONNECTED		0x01	// @emem LT_TOWERSTATE_CONNECTED	| Tower is connected
	#define LT_TOWERSTATE_STARTED		0x02	// @emem LT_TOWERSTATE_STARTED		| Tower is started
	#define LT_TOWERSTATE_OPEN			0x04	// @emem LT_TOWERSTATE_OPEN			| Tower is open
	#define LT_TOWERSTATE_READY			0x08	// @emem LT_TOWERSTATE_READY		| Tower is ready

	#define LT_TOWERSTATE_ALLRIGHT		0x0F	// @emem LT_TOWERSTATE_ALLRIGHT		| all of the above


	typedef struct LT_TOWERSTATE
	{
		DWORD		dwsize;			// Size of this structure
		WORD		wfwresult;		// Firmware return code	(NOT USED)
		WORD		wtowerstate;	// LT_TOWERSTATE_xxx (can be more than one)

	} LT_TOWERSTATE, *PLT_TOWERSTATE;

#endif
