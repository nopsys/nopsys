/* multiboot.h - the header for Multiboot */
/* Copyright (C) 1999, 2001  Free Software Foundation, Inc.
   
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
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#ifndef MULTIBOOT_HEADER
#define MULTIBOOT_HEADER 1

#include "types.h"


/* Macros.  */

/* The magic number for the Multiboot header.  */
#define MULTIBOOT_HEADER_MAGIC		0x1BADB002


 /* Flags to be set in the 'flags' member of the multiboot info structure. */
 
 /* is there basic lower/upper memory information? */
 #define MULTIBOOT_INFO_MEMORY                   0x00000001
 /* is there a boot device set? */
 #define MULTIBOOT_INFO_BOOTDEV                  0x00000002
 /* is the command-line defined? */
 #define MULTIBOOT_INFO_CMDLINE                  0x00000004
 /* are there modules to do something with? */
 #define MULTIBOOT_INFO_MODS                     0x00000008
 
 /* These next two are mutually exclusive */
 
 /* is there a symbol table loaded? */
 #define MULTIBOOT_INFO_AOUT_SYMS                0x00000010
 /* is there an ELF section header table? */
 #define MULTIBOOT_INFO_ELF_SHDR                 0X00000020
 
 /* is there a full memory map? */
 #define MULTIBOOT_INFO_MEM_MAP                  0x00000040
 
 /* Is there drive info? */
 #define MULTIBOOT_INFO_DRIVE_INFO               0x00000080
 
 /* Is there a config table? */
 #define MULTIBOOT_INFO_CONFIG_TABLE             0x00000100
 
 /* Is there a boot loader name? */
 #define MULTIBOOT_INFO_BOOT_LOADER_NAME         0x00000200
 
 /* Is there a APM table? */
 #define MULTIBOOT_INFO_APM_TABLE                0x00000400
 
 /* Is there video information? */
 #define MULTIBOOT_INFO_VIDEO_INFO               0x00000800


/* The flags for the Multiboot header.  */
#ifdef __ELF__
# define MULTIBOOT_HEADER_FLAGS		0x00000003
#else
# define MULTIBOOT_HEADER_FLAGS		0x00010003
#endif

/* The magic number passed by a Multiboot-compliant boot loader.  */
#define MULTIBOOT_BOOTLOADER_MAGIC	0x2BADB002

/* The size of our stack (16KB).  */
#define STACK_SIZE			0x4000

/* C symbol format. HAVE_ASM_USCORE is defined by configure.  */
#ifdef HAVE_ASM_USCORE
# define EXT_C(sym)			_ ## sym
#else
# define EXT_C(sym)			sym
#endif

#ifndef ASM
/* Do not include here in boot.S.  */

/* Types.  */

/* The Multiboot header.  */
typedef struct multiboot_header
{
  uint32_t magic;
  uint32_t flags;
  uint32_t checksum;
  uint32_t header_addr;
  uint32_t load_addr;
  uint32_t load_end_addr;
  uint32_t bss_end_addr;
  uint32_t entry_addr;
} multiboot_header_t;

/* The symbol table for a.out.  */
typedef struct aout_symbol_table
{
  uint32_t tabsize;
  uint32_t strsize;
  uint32_t addr;
  uint32_t reserved;
} aout_symbol_table_t;

/* The section header table for ELF.  */
typedef struct elf_section_header_table
{
  uint32_t num;
  uint32_t size;
  uint32_t addr;
  uint32_t shndx;
} elf_section_header_table_t;

/* The Multiboot information.  */
typedef struct multiboot_info
{
  uint32_t flags;
  uint32_t mem_lower;
  uint32_t mem_upper;
  uint32_t boot_device;
  uint32_t cmdline;
  uint32_t mods_count;
  uint32_t mods_addr;
  union
  {
    aout_symbol_table_t aout_sym;
    elf_section_header_table_t elf_sec;
  } u;
  uint32_t mmap_length;
  uint32_t mmap_addr;

  uint32_t drives_length;
  uint32_t drives_addr;
  
  uint32_t config_table;

  uint32_t boot_loader_name;

  uint32_t apm_table;

  uint32_t vbe_control_info;
  uint32_t vbe_mode_info;
  uint16_t vbe_mode;
  uint16_t vbe_interface_seg;
  uint16_t vbe_interface_off;
  uint16_t vbe_interface_len;

  uint64_t framebuffer_addr;
  uint32_t framebuffer_pitch;
  uint32_t framebuffer_width;
  uint32_t framebuffer_height;
  uint8_t  framebuffer_bpp;
  uint8_t  framebuffer_type;
} __attribute__((packed)) multiboot_info_t;

/* The module structure.  */
typedef struct module
{
  uint32_t mod_start;
  uint32_t mod_end;
  uint32_t string;
  uint32_t reserved;
} module_t;

/* The memory map. Be careful that the offset 0 is base_addr_low
   but no size.  */
typedef struct memory_map
{
  uint32_t size;
  uint32_t base_addr_low;
  uint32_t base_addr_high;
  uint32_t length_low;
  uint32_t length_high;
  uint32_t type;
} memory_map_t;

typedef struct drive_t
{
  uint32_t size;
  uint8_t drive_number;
  uint8_t drive_mode;
  uint16_t drive_cylinders;
  uint8_t drive_heads;
  uint16_t drive_sectors;
  uint16_t ports[0];
} __attribute__((packed)) drive_t;

typedef struct apm_table_t
{
  uint16_t version;
  uint16_t cseg;
  uint32_t offset;
  uint16_t cseg_16;
  uint16_t dseg;
  uint16_t flags;
  uint16_t cseg_len;
  uint16_t cseg_16_len;
  uint16_t dseg_len;
} __attribute__((packed)) apm_table_t;
#endif /* ! ASM */

#endif /* ! MULTIBOOT_HEADER */
	 
