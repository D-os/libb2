/******************************************************************************
/
/	File:		scsiprobe_driver.h
/
/	Description:	interface for scsiprobe driver
/
/	Copyright 1992-98, Be Incorporated
/
*******************************************************************************/


#ifndef _SCSIPROBE_DRIVER_H
#define _SCSIPROBE_DRIVER_H

#include <Drivers.h>
#include <CAM.h>

#define B_SCSIPROBE_DRIVER	"/dev/bus/scsi/probe"

/* -----
	ioctl opcodes
----- */

enum {
	B_SCSIPROBE_INQUIRY = B_DEVICE_OP_CODES_END + 1,
	B_SCSIPROBE_RESET,
	B_SCSIPROBE_VERSION,
	B_SCSIPROBE_HIGHEST_PATH,
	B_SCSIPROBE_PATH_INQUIRY,
	B_SCSIPROBE_PLATFORM,
	B_SCSIPROBE_EXTENDED_PATH_INQUIRY,
	B_SCSIPROBE_FORMAT
};

typedef struct {
	uchar	path;
	uchar	id;
	uchar	lun;
	uchar	len;
	uchar	data[256];
} scsiprobe_inquiry;

typedef struct {
	uchar	path;
} scsiprobe_reset;

typedef struct {
	uchar		path;
	CCB_PATHINQ	data;
} scsiprobe_path_inquiry;

typedef struct {
	uchar					path;
	CCB_EXTENDED_PATHINQ	data;
} scsiprobe_extended_path_inquiry;

typedef struct {
	uchar	path;
	uchar	id;
	uchar	lun;
} scsiprobe_format;

#endif
