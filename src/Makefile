include ../compilation.conf

AR = $(COMPILER_DIR)$(COMPILER_PREFIX)-ar
CP = cp
RM = rm
LD = $(COMPILER_DIR)$(LINKER_PREFIX)
CC = $(COMPILER_DIR)$(COMPILER_PREFIX)

CFLAGS  = $(CFLAGS_ARCH) -std=c11 -Wall --no-pie -ffreestanding -nostdlib -fno-stack-protector -g -O1 -fno-inline -fno-omit-frame-pointer
LDFLAGS = $(LDFLAGS_ARCH) -static -nostdlib -r #-Xlinker
DEFS = -DNO_STD_FILE_SUPPORT -DDEBUG -DLSB_FIRST $(DEFS_ARCH)
INCS = -Ilibc


BLDDIR  = ../build
OBJDIR  = $(BLDDIR)/objs

VMSRC   = $(wildcard *.c ints-asm.asm setjmp-x64.asm)
VMOBJS1  = $(VMSRC:.c=.o)
VMOBJS  = $(VMOBJS1:.asm=.o)
ALLOBJS = $(addprefix $(OBJDIR)/, $(VMOBJS) libgcc.o)

# Where to look for files?
#VPATH = $(VMDIR1) $(VMDIR2) $(VMDIR3) $(BLDDIR)

$(BLDDIR)/libnopsys.obj: $(OBJDIR) $(ALLOBJS)
	$(LD) -o $@ $(ALLOBJS) $(LDFLAGS)

$(OBJDIR)/%.o: %.c 
	$(CC) -o $@ $(CFLAGS) $(INCS) $(DEFS) -c $<

$(OBJDIR)/%.o: %.asm 
	$(NASM) -g -F dwarf $(NASMFLAGS_ARCH) -o $@ $<

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -f $(OBJDIR)/*.o

# Extra specific dependencies
LIBGCCOBJS = _udivdi3.o _divdi3.o _moddi3.o _umoddi3.o

$(OBJDIR)/libgcc.o: 
	$(AR) x `$(CC) $(CFLAGS) -print-libgcc-file-name` $(LIBGCCOBJS)
	$(LD) -r -o $@ $(LDFLAGS) $(LIBGCCOBJS)
	$(RM) $(LIBGCCOBJS)

