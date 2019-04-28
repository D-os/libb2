/*******************************************************************************
/
/	File:		PCI.h
/
/	Description:	Interface to the PCI bus.
/	For more information, see "PCI Local Bus Specification, Revision 2.1",
/	PCI Special Interest Group, 1995.
/
/	Copyright 1993-98, Be Incorporated, All Rights Reserved.
/
*******************************************************************************/


#ifndef _PCI_H
#define _PCI_H

#include <BeBuild.h>
#include <SupportDefs.h>
#include <bus_manager.h>

#ifdef __cplusplus
extern "C" {
#endif


/* -----
	pci device info
----- */

typedef struct pci_info {
	ushort	vendor_id;				/* vendor id */
	ushort	device_id;				/* device id */
	uchar	bus;					/* bus number */
	uchar	device;					/* device number on bus */
	uchar	function;				/* function number in device */
	uchar	revision;				/* revision id */
	uchar	class_api;				/* specific register interface type */
	uchar	class_sub;				/* specific device function */
	uchar	class_base;				/* device type (display vs network, etc) */
	uchar	line_size;				/* cache line size in 32 bit words */
	uchar	latency;				/* latency timer */
	uchar	header_type;			/* header type */
	uchar	bist;					/* built-in self-test */
	uchar	reserved;				/* filler, for alignment */
	union {
		struct {
			ulong	cardbus_cis;			/* CardBus CIS pointer */
			ushort	subsystem_id;			/* subsystem (add-in card) id */
			ushort	subsystem_vendor_id;	/* subsystem (add-in card) vendor id */
			ulong	rom_base;				/* rom base address, viewed from host */
			ulong	rom_base_pci;			/* rom base addr, viewed from pci */
			ulong	rom_size;				/* rom size */
			ulong	base_registers[6];		/* base registers, viewed from host */
			ulong	base_registers_pci[6];	/* base registers, viewed from pci */
			ulong	base_register_sizes[6];	/* size of what base regs point to */
			uchar	base_register_flags[6];	/* flags from base address fields */
			uchar	interrupt_line;			/* interrupt line */
			uchar	interrupt_pin;			/* interrupt pin */
			uchar	min_grant;				/* burst period @ 33 Mhz */
			uchar	max_latency;			/* how often PCI access needed */
		} h0;
		struct {
			ulong	base_registers[2];		/* base registers, viewed from host */
			ulong	base_registers_pci[2];	/* base registers, viewed from pci */
			ulong	base_register_sizes[2];	/* size of what base regs point to */
			uchar	base_register_flags[2];	/* flags from base address fields */
			uchar	primary_bus;
			uchar	secondary_bus;
			uchar	subordinate_bus;
			uchar	secondary_latency;
			uchar	io_base;
			uchar	io_limit;
			ushort	secondary_status;
			ushort	memory_base;
			ushort	memory_limit;
			ushort  prefetchable_memory_base;
			ushort  prefetchable_memory_limit;
			ulong	prefetchable_memory_base_upper32;
			ulong	prefetchable_memory_limit_upper32;
			ushort	io_base_upper16;
			ushort	io_limit_upper16;
			ulong	rom_base;				/* rom base address, viewed from host */
			ulong	rom_base_pci;			/* rom base addr, viewed from pci */
			uchar	interrupt_line;			/* interrupt line */
			uchar	interrupt_pin;			/* interrupt pin */
			ushort	bridge_control;		
		} h1; 
	} u;
} pci_info;


typedef struct pci_module_info pci_module_info;

struct pci_module_info {
	bus_manager_info	binfo;

	uint8			(*read_io_8) (int mapped_io_addr);
	void			(*write_io_8) (int mapped_io_addr, uint8 value);
	uint16			(*read_io_16) (int mapped_io_addr);
	void			(*write_io_16) (int mapped_io_addr, uint16 value);
	uint32			(*read_io_32) (int mapped_io_addr);
	void			(*write_io_32) (int mapped_io_addr, uint32 value);

	long			(*get_nth_pci_info) (
						long		index,	/* index into pci device table */
						pci_info 	*info	/* caller-supplied buffer for info */
					);
	uint32			(*read_pci_config) (
						uchar	bus,		/* bus number */
						uchar	device,		/* device # on bus */
						uchar	function,	/* function # in device */
						uchar	offset,		/* offset in configuration space */
						uchar	size		/* # bytes to read (1, 2 or 4) */
					);
	void			(*write_pci_config) (
						uchar	bus,		/* bus number */
						uchar	device,		/* device # on bus */
						uchar	function,	/* function # in device */
						uchar	offset,		/* offset in configuration space */
						uchar	size,		/* # bytes to write (1, 2 or 4) */
						uint32	value		/* value to write */
					);

	void *			(*ram_address) (const void *physical_address_in_system_memory);
};

#define	B_PCI_MODULE_NAME		"bus_managers/pci/v1"

/* ---
	offsets in PCI configuration space to the elements of the predefined
	header common to all header types
--- */

#define PCI_vendor_id			0x00		/* (2 byte) vendor id */
#define PCI_device_id			0x02		/* (2 byte) device id */
#define PCI_command				0x04		/* (2 byte) command */
#define PCI_status				0x06		/* (2 byte) status */
#define PCI_revision			0x08		/* (1 byte) revision id */
#define PCI_class_api			0x09		/* (1 byte) specific register interface type */
#define PCI_class_sub			0x0a		/* (1 byte) specific device function */
#define PCI_class_base			0x0b		/* (1 byte) device type (display vs network, etc) */
#define PCI_line_size			0x0c		/* (1 byte) cache line size in 32 bit words */
#define PCI_latency				0x0d		/* (1 byte) latency timer */
#define PCI_header_type			0x0e		/* (1 byte) header type */
#define PCI_bist				0x0f		/* (1 byte) built-in self-test */



/* ---
	offsets in PCI configuration space to the elements of the predefined
	header common to header types 0x00 and 0x01
--- */
#define PCI_base_registers		0x10		/* base registers (size varies) */
#define PCI_interrupt_line		0x3c		/* (1 byte) interrupt line */
#define PCI_interrupt_pin		0x3d		/* (1 byte) interrupt pin */



/* ---
	offsets in PCI configuration space to the elements of header type 0x00
--- */

#define PCI_cardbus_cis			0x28		/* (4 bytes) CardBus CIS (Card Information Structure) pointer (see PCMCIA v2.10 Spec) */
#define PCI_subsystem_vendor_id	0x2c		/* (2 bytes) subsystem (add-in card) vendor id */
#define PCI_subsystem_id		0x2e		/* (2 bytes) subsystem (add-in card) id */
#define PCI_rom_base			0x30		/* (4 bytes) expansion rom base address */
#define PCI_min_grant			0x3e		/* (1 byte) burst period @ 33 Mhz */
#define PCI_max_latency			0x3f		/* (1 byte) how often PCI access needed */


/* ---
	offsets in PCI configuration space to the elements of header type 0x01 (PCI-to-PCI bridge)
--- */

#define PCI_primary_bus								0x18
#define PCI_secondary_bus							0x19
#define PCI_subordinate_bus							0x1A
#define PCI_secondary_latency						0x1B	
#define PCI_io_base									0x1C
#define PCI_io_limit								0x1D
#define PCI_secondary_status						0x1E
#define PCI_memory_base								0x20
#define PCI_memory_limit							0x22
#define PCI_prefetchable_memory_base				0x24
#define PCI_prefetchable_memory_limit				0x26
#define PCI_prefetchable_memory_base_upper32		0x28
#define PCI_prefetchable_memory_limit_upper32		0x2C
#define PCI_io_base_upper16							0x30
#define PCI_io_limit_upper16						0x32
#define PCI_bridge_rom_base							0x38
#define PCI_bridge_control							0x3E																		



/* ---
	values for the class_base field in the common header
--- */

#define PCI_early					0x00	/* built before class codes defined */
#define PCI_mass_storage			0x01	/* mass storage_controller */
#define PCI_network					0x02	/* network controller */
#define PCI_display					0x03	/* display controller */
#define PCI_multimedia				0x04	/* multimedia device */
#define PCI_memory					0x05	/* memory controller */
#define PCI_bridge					0x06	/* bridge controller */
#define PCI_simple_communications	0x07	/* simple communications controller */
#define PCI_base_peripheral			0x08	/* base system peripherals */
#define PCI_input					0x09	/* input devices */
#define PCI_docking_station			0x0a	/* docking stations */
#define PCI_processor				0x0b	/* processors */
#define PCI_serial_bus				0x0c	/* serial_bus_controller */

#define PCI_undefined				0xFF	/* not in any defined class */


/* ---
	values for the class_sub field for class_base = 0x00 (built before
	class codes were defined)
--- */

#define PCI_early_not_vga	0x00			/* all except vga */
#define PCI_early_vga		0x01			/* vga devices */


/* ---
	values for the class_sub field for class_base = 0x01 (mass storage)
--- */

#define PCI_scsi			0x00			/* SCSI controller */
#define PCI_ide				0x01			/* IDE controller */
#define PCI_floppy			0x02			/* floppy disk controller */
#define PCI_ipi				0x03			/* IPI bus controller */
#define PCI_raid			0x03			/* RAID controller */
#define PCI_mass_storage_other 0x80			/* other mass storage controller */


/* ---
	values for the class_sub field for class_base = 0x02 (network)
--- */

#define PCI_ethernet		0x00			/* Ethernet controller */
#define PCI_token_ring		0x01			/* Token Ring controller */
#define PCI_fddi			0x02			/* FDDI controller */
#define PCI_atm				0x03			/* ATM controller */
#define PCI_network_other	0x80			/* other network controller */


/* ---
	values for the class_sub field for class_base = 0x03 (display)
--- */

#define PCI_vga				0x00			/* VGA controller */
#define PCI_xga				0x01			/* XGA controller */
#define PCI_display_other	0x80			/* other display controller */


/* ---
	values for the class_sub field for class_base = 0x04 (multimedia device)
--- */

#define PCI_video			0x00			/* video */
#define PCI_audio			0x01			/* audio */
#define PCI_multimedia_other 0x80			/* other multimedia device */


/* ---
	values for the class_sub field for class_base = 0x05 (memory)
--- */

#define PCI_ram				0x00			/* RAM */
#define PCI_flash			0x01			/* flash */
#define PCI_memory_other	0x80			/* other memory controller */


/* ---
	values for the class_sub field for class_base = 0x06 (bridge)
--- */

#define PCI_host			0x00			/* host bridge */
#define PCI_isa				0x01			/* ISA bridge */
#define PCI_eisa			0x02			/* EISA bridge */
#define PCI_microchannel	0x03			/* MicroChannel bridge */
#define PCI_pci				0x04			/* PCI-to-PCI bridge */
#define PCI_pcmcia			0x05			/* PCMCIA bridge */
#define PCI_nubus			0x06			/* NuBus bridge */
#define PCI_cardbus			0x07			/* CardBus bridge */
#define PCI_bridge_other	0x80			/* other bridge device */


/* ---
	values for the class_sub field for class_base = 0x07 (simple
	communications controllers)
--- */

#define PCI_serial						0x00	/* serial port controller */
#define PCI_parallel					0x01	/* parallel port */
#define PCI_simple_communications_other	0x80	/* other communications device */

/* ---
	values of the class_api field for
		class_base	= 0x07 (simple communications), and
		class_sub	= 0x00 (serial port controller)
--- */

#define PCI_serial_xt		0x00			/* XT-compatible serial controller */
#define PCI_serial_16450	0x01			/* 16450-compatible serial controller */
#define PCI_serial_16550	0x02			/* 16550-compatible serial controller */


/* ---
	values of the class_api field for
		class_base	= 0x07 (simple communications), and
		class_sub	= 0x01 (parallel port)
--- */

#define PCI_parallel_simple			0x00	/* simple (output-only) parallel port */
#define PCI_parallel_bidirectional	0x01	/* bidirectional parallel port */
#define PCI_parallel_ecp			0x02	/* ECP 1.x compliant parallel port */


/* ---
	values for the class_sub field for class_base = 0x08 (generic
	system peripherals)
--- */

#define PCI_pic						0x00	/* periperal interrupt controller */
#define PCI_dma						0x01	/* dma controller */
#define PCI_timer					0x02	/* timers */
#define PCI_rtc						0x03	/* real time clock */
#define PCI_system_peripheral_other	0x80	/* other generic system peripheral */

/* ---
	values of the class_api field for
		class_base	= 0x08 (generic system peripherals)
		class_sub	= 0x00 (peripheral interrupt controller)
--- */

#define PCI_pic_8259			0x00	/* generic 8259 */
#define PCI_pic_isa				0x01	/* ISA pic */
#define PCI_pic_eisa			0x02	/* EISA pic */

/* ---
	values of the class_api field for
		class_base	= 0x08 (generic system peripherals)
		class_sub	= 0x01 (dma controller)
--- */

#define PCI_dma_8237			0x00	/* generic 8237 */
#define PCI_dma_isa				0x01	/* ISA dma */
#define PCI_dma_eisa			0x02	/* EISA dma */

/* ---
	values of the class_api field for
		class_base	= 0x08 (generic system peripherals)
		class_sub	= 0x02 (timer)
--- */

#define PCI_timer_8254			0x00	/* generic 8254 */
#define PCI_timer_isa			0x01	/* ISA timer */
#define PCI_timer_eisa			0x02	/* EISA timers (2 timers) */


/* ---
	values of the class_api field for
		class_base	= 0x08 (generic system peripherals)
		class_sub	= 0x03 (real time clock
--- */

#define PCI_rtc_generic			0x00	/* generic real time clock */
#define PCI_rtc_isa				0x01	/* ISA real time clock */


/* ---
	values for the class_sub field for class_base = 0x09 (input devices)
--- */

#define PCI_keyboard			0x00	/* keyboard controller */
#define PCI_pen					0x01	/* pen */
#define PCI_mouse				0x02	/* mouse controller */
#define PCI_input_other			0x80	/* other input controller */


/* ---
	values for the class_sub field for class_base = 0x0a (docking stations)
--- */

#define PCI_docking_generic		0x00	/* generic docking station */

/* ---
	values for the class_sub field for class_base = 0x0b (processor)
--- */

#define PCI_386					0x00	/* 386 */
#define PCI_486					0x01	/* 486 */
#define PCI_pentium				0x02	/* Pentium */
#define PCI_alpha				0x10	/* Alpha */
#define PCI_PowerPC				0x20	/* PowerPC */
#define PCI_coprocessor			0x40	/* co-processor */

/* ---
	values for the class_sub field for class_base = 0x0c (serial bus
	controller)
--- */

#define PCI_firewire			0x00	/* FireWire (IEEE 1394) */
#define PCI_access				0x01	/* ACCESS bus */
#define PCI_ssa					0x02	/* SSA */
#define PCI_usb					0x03	/* Universal Serial Bus */
#define PCI_fibre_channel		0x04	/* Fibre channel */

/* ---
	values of the class_api field for
		class_base	= 0x0c ( serial bus controller )
		class_sub	= 0x03 ( Universal Serial Bus  )
--- */

#define PCI_usb_uhci			0x00	/* Universal Host Controller Interface */
#define PCI_usb_ohci			0x10	/* Open Host Controller Interface */


/* ---
	masks for command register bits
--- */

#define PCI_command_io				0x001		/* 1/0 i/o space en/disabled */
#define PCI_command_memory			0x002		/* 1/0 memory space en/disabled */
#define PCI_command_master			0x004		/* 1/0 pci master en/disabled */
#define PCI_command_special			0x008		/* 1/0 pci special cycles en/disabled */
#define PCI_command_mwi				0x010		/* 1/0 memory write & invalidate en/disabled */
#define PCI_command_vga_snoop		0x020		/* 1/0 vga pallette snoop en/disabled */
#define PCI_command_parity			0x040		/* 1/0 parity check en/disabled */
#define PCI_command_address_step	0x080		/* 1/0 address stepping en/disabled */
#define PCI_command_serr			0x100		/* 1/0 SERR# en/disabled */
#define PCI_command_fastback		0x200		/* 1/0 fast back-to-back en/disabled */


/* ---
	masks for status register bits
--- */

#define PCI_status_66_MHz_capable			0x0020	/* 66 Mhz capable */
#define PCI_status_udf_supported			0x0040	/* user-definable-features (udf) supported */
#define PCI_status_fastback					0x0080	/* fast back-to-back capable */
#define PCI_status_parity_signalled		0x0100	/* parity error signalled */
#define PCI_status_devsel					0x0600	/* devsel timing (see below) */
#define PCI_status_target_abort_signalled	0x0800	/* signaled a target abort */
#define PCI_status_target_abort_received	0x1000	/* received a target abort */
#define PCI_status_master_abort_received	0x2000	/* received a master abort */
#define PCI_status_serr_signalled			0x4000	/* signalled SERR# */
#define PCI_status_parity_error_detected	0x8000	/* parity error detected */


/* ---
	masks for devsel field in status register
--- */

#define PCI_status_devsel_fast		0x0000		/* fast */
#define PCI_status_devsel_medium	0x0200		/* medium */
#define PCI_status_devsel_slow		0x0400		/* slow */


/* ---
	masks for header type register
--- */

#define PCI_header_type_mask	0x7F		/* header type field */
#define PCI_multifunction		0x80		/* multifunction device flag */


/* ---
	defines for header type register
--- */

#define PCI_header_type_generic				0x00		
#define PCI_header_type_PCI_to_PCI_bridge		0x01		



/* ---
	masks for built in self test (bist) register bits
--- */

#define PCI_bist_code			0x0F		/* self-test completion code, 0 = success */
#define PCI_bist_start			0x40		/* 1 = start self-test */
#define PCI_bist_capable		0x80		/* 1 = self-test capable */


/* ---
	masks for flags in the various base address registers
--- */

#define PCI_address_space		0x01		/* 0 = memory space, 1 = i/o space */


/* ---
	masks for flags in memory space base address registers
--- */

#define PCI_address_type			0x06	/* type (see below) */
#define PCI_address_prefetchable	0x08	/* 1 if prefetchable (see PCI spec) */

#define PCI_address_type_32			0x00	/* locate anywhere in 32 bit space */
#define PCI_address_type_32_low		0x02	/* locate below 1 Meg */
#define PCI_address_type_64			0x04	/* locate anywhere in 64 bit space */

#define PCI_address_memory_32_mask	0xFFFFFFF0	/* mask to get 32bit memory space base address */


/* ---
	masks for flags in i/o space base address registers
--- */

#define PCI_address_io_mask		0xFFFFFFFC	/* mask to get i/o space base address */


/* ---
	masks for flags in expansion rom base address registers
--- */

#define PCI_rom_enable			0x00000001	/* 1 = expansion rom decode enabled */
#define PCI_rom_address_mask	0xFFFFF800	/* mask to get expansion rom addr */

#ifdef __cplusplus
}
#endif

#endif
