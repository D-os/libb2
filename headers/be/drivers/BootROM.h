/*******************************************************************************
/
/	File:		BootROM.h
/
/	Description:	Interface for BeBox BootROM PCI graphics drivers
/
/	Copyright 1993-98, Be Incorporated, All Rights Reserved.
/
*******************************************************************************/


#ifndef _BOOTROM_H
#define _BOOTROM_H

#include <PCI.h>

/**/
/* To write a BeBox boot ROM driver, you need to provide two routines:*/
/* 	- an entrypoint that either initializes the specified card, or*/
/*    returns NULL indicating that it cannot be used for this card.*/
/*  - a BootCallbackProc to perform a few simple tasks when requested.*/
/**/
/* If the entrypoint returns a FrameBufferPtr, then it MUST also return*/
/* the PerCardData and the BootCallbackProc.  The PerCardData is not*/
/* interpreted by the BeOS.*/
/**/
/* KEEP IT SMALL!  There is not a lot of room in the boot ROM.*/
/**/

typedef uchar *FrameBufferPtr;
typedef void  *PerCardData;


/**/
/* BootCallbackProc*/
/**/
/* If the index (i) is positive, then it represents an entry*/
/* in the color table to modify.  If it is -1, then this is the*/
/* vid_uninit call, and you should prepare the card to be*/
/* taken over by the real graphics driver.  All other*/
/* negative values should be ignored.*/
/**/
/* NOTE: the colors are specified for a 6-bit color table.*/
/* You must shift them left 2 if you're stuffing them into*/
/* an 8-bit color table.*/
/**/

typedef void  (*BootCallbackProc)( PerCardData pcd, int i,
								   int r, int g, int b );


/**/
/* CardEntrypoint*/
/**/
/* When the BootROM finds a card that matches your card's vendor*/
/* and device IDs, it will call your entrypoint with the appropriate*/
/* pci_info.  Your driver must either return the BootCallbackProc*/
/* and fill in a good FrameBufferPtr and PerCardData upon success,*/
/* or return NULL upon failure.*/
/**/

typedef BootCallbackProc (*CardEntrypoint)(
							 const pci_info* h,
							 FrameBufferPtr *fbp,
						   	 PerCardData *pcd );

#endif
