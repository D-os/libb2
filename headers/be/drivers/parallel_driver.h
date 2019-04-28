/* ++++++++++
	FILE:	parallel_driver.h
	Copyright (c) 1993-1998 by Be Incorporated.  All Rights Reserved.
	Adapted from Christian Bauer's Parallax.h
+++++ */

#ifndef _PARALLEL_DRIVER_H
#define _PARALLEL_DRIVER_H

#include <drivers/Drivers.h>

/* ioctl() opcodes */
enum {
	PAR_RESET = B_DEVICE_OP_CODES_END+1,	/* Send INIT signal to port (this will also terminate ECP mode) */
	PAR_STATUS,								/* (uint8) Read port status register */
	PAR_GET_PORT_TYPE,						/* (int32) Get port type/capabilities (see below) */
	PAR_SET_MODE,							/* (parallel_mode) Set operating mode (see below) */
	PAR_GET_MODE,							/* (parallel_mode) Get operating mode */
	PAR_SET_READ_TIMEOUT,					/* (bigtime_t) Set ECP read timeout */
	PAR_GET_READ_TIMEOUT,					/* (bigtime_t) Get ECP read timeout */
	PAR_REQUEST_DEVICE_ID,					/* (int) request for the DEVICE ID */
	PAR_ABORT								/* (void) abort transfers */
};

/* Port type/capability flags */
enum {
	PAR_TYPE_REVERSE_CHANNEL = 1,		/* True reverse channel exists, byte mode can be used */
	PAR_TYPE_ECP = 2,					/* Port is ECP capable */
	PAR_TYPE_IRQ = 4,					/* Port uses interrupt */
	PAR_TYPE_DMA = 8					/* Port uses DMA */
};

/* Operating modes */
typedef enum {
	PAR_MODE_CENTRONICS,		/* Standard parallel port, nibble mode for reverse channel */
	PAR_MODE_CENTRONICS_BYTE,	/* Standard parallel port, byte mode for reverse channel */
	PAR_MODE_ECP,				/* ECP */
	PAR_MODE_ECP_DEVICE_ID		/* ECP, request device ID (GET_MODE will always return PAR_MODE_ECP) */
} parallel_mode;


/* People who need this know who they are */
enum {
	DGL_READ_STATUS_REGISTER = B_DEVICE_OP_CODES_END+1,	/* (uint8) Read port PSTAT register */
	DGL_READ_CONTROL_REGISTER,							/* (uint8) Read port PCON register */
	DGL_WRITE_CONTROL_REGISTER,							/* (uint8) Write port PCON register */
	DGL_LOCK,											/* Lock the parallel port */
	DGL_UNLOCK											/* Unlock the parallel port */
};


#endif

