# loader.s

.global _loader           # making entry point visible to linker

# setting up the Multiboot header - see GRUB docs for details
.set ALIGN,    1<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # provide memory map
.set VIDINFO,  1<<2             # provide video table
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header

 .set FLAGS,    MEMINFO	# only Memory info 
#.set FLAGS,    MEMINFO | VIDINFO# Memory and Video info
.set CHECKSUM, -(MAGIC + FLAGS) # checksum required

.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM
.long 0			# header_addr
.long 0			# load_addr
.long 0			# load_end_addr
.long 0			# bss_end_addr
.long 0			# entry_addr
.long 0			# mode_type	0: linear, 1: EGA
.long 1024		# width
.long 768		# height
.long 32		# depth

# reserve initial kernel stack space
.set STACKSIZE, 0x4000          # that is, 16k.
.comm stack, STACKSIZE, 32      # reserve 16k stack on a quadword boundary

_loader:       mov   $(stack + STACKSIZE), %esp # set up the stack
               # mov   _loader, %esp
               push  %ebx                       # Multiboot data structure
               push  %eax                       # Multiboot magic number

               call  nopsys_main            # call kernel proper
               hlt                    # halt machine should kernel return
