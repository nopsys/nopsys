include ../compilation.conf

AR = $(COMPILER_DIR)$(COMPILER_PREFIX)-ar
CP = cp
RM = rm
LD = $(COMPILER_DIR)$(LINKER_PREFIX)
CC = $(COMPILER_DIR)$(COMPILER_PREFIX)

CFLAGS = -std=c11 -m32 -Wall --no-pie -nostdlib -fno-stack-protector -g -O1 -fno-inline
LDFLAGS = -melf_i386 -static -nostdlib -r #-Xlinker
DEFS  = -DNO_STD_FILE_SUPPORT -DDEBUG -DLSB_FIRST -DX86
INCS = -Ilibc

BLDDIR  = ../build
OBJDIR  = $(BLDDIR)/objs
VMSRC   = $(wildcard *.c *.asm)
VMOBJS1  = $(VMSRC:.c=.o)
VMOBJS  = $(VMOBJS1:.asm=.o)
ALLOBJS = $(addprefix $(OBJDIR)/, $(VMOBJS) libgcc.o _libc.o)

# Where to look for files?
#VPATH = $(VMDIR1) $(VMDIR2) $(VMDIR3) $(BLDDIR)

$(BLDDIR)/libnopsys.obj: $(OBJDIR) $(ALLOBJS)
	$(LD) -o $@ $(ALLOBJS) $(LDFLAGS)

$(OBJDIR)/%.o: %.c 
	$(CC) -o $@ $(CFLAGS) $(INCS) $(DEFS) -c $<

$(OBJDIR)/ints-asm.o: ints-asm.asm 
	nasm -f elf -o $@ $<

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm $(OBJDIR)/*.o

LIBCOBJS   = longjmp.o setjmp.o bsd-setjmp.o bsd-_setjmp.o
LIBGCCOBJS = _udivdi3.o _divdi3.o _moddi3.o _umoddi3.o

# Extra specific dependencies

$(OBJDIR)/_libc.o: 
	$(AR) x `$(CC) $(CFLAGS) -print-file-name=libc.a` $(LIBCOBJS)
	$(LD) -r -o $@ $(LDFLAGS) $(LIBCOBJS)
	$(RM) $(LIBCOBJS)

$(OBJDIR)/libgcc.o: 
	$(AR) x `$(CC) $(CFLAGS) -print-libgcc-file-name` $(LIBGCCOBJS)
	$(LD) -r -o $@ $(LDFLAGS) $(LIBGCCOBJS)
	$(RM) $(LIBGCCOBJS)

