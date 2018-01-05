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
  unsigned long magic;
  unsigned long flags;
  unsigned long checksum;
  unsigned long header_addr;
  unsigned long load_addr;
  unsigned long load_end_addr;
  unsigned long bss_end_addr;
  unsigned long entry_addr;
} multiboot_header_t;

/* The symbol table for a.out.  */
typedef struct aout_symbol_table
{
  unsigned long tabsize;
  unsigned long strsize;
  unsigned long addr;
  unsigned long reserved;
} aout_symbol_table_t;

/* The section header table for ELF.  */
typedef struct elf_section_header_table
{
  unsigned long num;
  unsigned long size;
  unsigned long addr;
  unsigned long shndx;
} elf_section_header_table_t;

/* The Multiboot information.  */
typedef struct multiboot_info
{
  unsigned long flags;
  unsigned long mem_lower;
  unsigned long mem_upper;
  unsigned long boot_device;
  unsigned long cmdline;
  unsigned long mods_count;
  unsigned long mods_addr;
  union
  {
    aout_symbol_table_t aout_sym;
    elf_section_header_table_t elf_sec;
  } u;
  unsigned long mmap_length;
  unsigned long mmap_addr;

  unsigned long drives_length;
  unsigned long drives_addr;
  
  unsigned long config_table;

  unsigned long boot_loader_name;

  unsigned long apm_table;

  unsigned long vbe_control_info;
  unsigned long vbe_mode_info;
  unsigned short vbe_mode;
  unsigned short vbe_interface_seg;
  unsigned short vbe_interface_off;
  unsigned long vbe_interface_len;
} multiboot_info_t;

/* The module structure.  */
typedef struct module
{
  unsigned long mod_start;
  unsigned long mod_end;
  unsigned long string;
  unsigned long reserved;
} module_t;

/* The memory map. Be careful that the offset 0 is base_addr_low
   but no size.  */
typedef struct memory_map
{
  unsigned long size;
  unsigned long base_addr_low;
  unsigned long base_addr_high;
  unsigned long length_low;
  unsigned long length_high;
  unsigned long type;
} memory_map_t;

typedef struct
{
  unsigned long size;
  unsigned char drive_number;
  unsigned char drive_mode;
  unsigned short drive_cylinders;
  unsigned char drive_heads;
  unsigned short drive_sectors;
  unsigned short ports[0];
} drive_t;

typedef struct {
  unsigned short version;
  unsigned short cseg;
  unsigned long offset;
  unsigned short cseg_16;
  unsigned short dseg;
  unsigned short flags;
  unsigned short cseg_len;
  unsigned short cseg_16_len;
  unsigned short dseg_len;
} apm_table_t;
#endif /* ! ASM */

#endif /* ! MULTIBOOT_HEADER */
	 