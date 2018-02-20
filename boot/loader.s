# loader.s

.global _loader           # making entry point visible to linker

# setting up the Multiboot header - see GRUB docs for details
.set ALIGN,    1<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # provide memory map
.set VIDINFO,  1<<2             # provide video table
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header

# .set FLAGS,    MEMINFO	# only Memory info 
.set FLAGS,    MEMINFO | VIDINFO # Memory and Video info
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
.set STACKSIZE, 0x100000          # that is, 1mb (Cog uses plenty of stack).
.comm stack, STACKSIZE, 32      # reserve 16k stack on a 32-byte boundary

.code32
_loader:
        mov   $(stack + STACKSIZE), %esp  # set up the stack
        # mov   _loader, %esp

		# we have to align the stack to 16 bytes, adding extra space if needed.
		# currently, we push 16 bytes and do a jump, so there's nothing to add
		# sub $8, %esp
		
		# push multiboot arguments, taking into account that we are in 32 bits mode and we will
		# pop them in 64 bits mode.s
		push  $0
        push  %ebx                        # Multiboot data structure
		push  $0
        push  %eax                        # Multiboot magic number

		jmp enable_long_mode              # and never come back
