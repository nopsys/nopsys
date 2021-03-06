include compilation.conf

NOPSYS_IMAGE   = ../dmr/bee.bsl
NOPSYS_SOURCES = ../dmr/bee.bsc

BLDDIR    = build
OBJDIR    = $(BLDDIR)/objs#   temp .o files go here
DISKDIR    = $(BLDDIR)/disk#    temp dir to put everything and package as image file
DISTRODIR = $(BLDDIR)/distro

ifneq ("$(wildcard  $(VM_BUILDDIR)/extra-files)","")
EXTRAFILES=$(shell cat $(VM_BUILDDIR)/extra-files)
endif
DESTFILES = $(addprefix $(DISKDIR)/,$(notdir $(EXTRAFILES)))

-include vm.conf # '-include' doesn't fail if the file doesn't exist

# ==============
# pseudo targets
# ==============

libnopsys: $(BLDDIR)/libnopsys.obj

all: iso

iso: $(BLDDIR)/nopsys.iso

hd: $(BLDDIR)/nopsys.vmdk

# generate a zip with all the current files, that can both be used to run or rebuild this nopsys
distro: iso
	rsync -a --exclude=.git --exclude=$(DISTRODIR) . $(DISTRODIR)   # is second exclude needed?
	cd $(BLDDIR) && tar cjf nopsys-`date +%d-%b-%Y`.tar.bz2 nopsys.iso nopsys.cd.vmx
	cd $(BLDDIR) && zip -9 nopsys-`date +%d-%b-%Y`.zip nopsys.iso nopsys.cd.vmx

clean:
	rm -rf $(BLDDIR) # -rm -rf $(BLDDIR)

.PHONY: libnopsys

# ==================
# real file targets
# ==================
$(BLDDIR):
	mkdir -p $(BLDDIR) $(BLDDIR)/mount $(DISKDIR) $(DISKDIR)/boot/grub $(DISTRODIR) $(OBJDIR)

# from src dir generate a small generic lib, that later has to be linked to whatever dialect/vm is used
$(BLDDIR)/libnopsys.obj: $(BLDDIR)
	make -C src

# object file to be loaded by grub, your dialect should have generated a vm.obj file and put in BLDDIR 
$(BLDDIR)/nopsys.kernel: $(BLDDIR)/libnopsys.obj $(VM_BUILDDIR)/vm.obj boot/loader.s boot/kernel.ld
	$(AS) -o $(BLDDIR)/loader.o $(ASFLAGS_ARCH) boot/loader.s
	$(LD) -o $(BLDDIR)/nopsys.kernel $(LDFLAGS_ARCH) -T boot/kernel.ld $(BLDDIR)/loader.o $(BLDDIR)/libnopsys.obj $(VM_BUILDDIR)/vm.obj
	nm $(BLDDIR)/nopsys.kernel | grep -v " U " | awk '{print "0x" $$1 " " $$3}' > $(BLDDIR)/nopsys.sym



$(DESTFILES): $(EXTRAFILES)
	cp $? $(DISKDIR)/

#vpath $(EXTRADIRS)

$(DISKDIR): $(BLDDIR)/nopsys.kernel boot/grub.cfg $(DESTFILES)
	cp $(BLDDIR)/nopsys.kernel $(DISKDIR)/
	touch $(DISKDIR) # nopsys.iso doesn't rebuild without this, why?
	
# make an iso (CD image)
$(BLDDIR)/nopsys.iso: $(DISKDIR)
	mkdir -p $(DISKDIR)/boot/grub
	cp boot/grub.cfg $(DISKDIR)/boot/grub
	$(MKRESCUE) --xorriso=$(XORRISO_DIR)xorriso -o $(BLDDIR)/nopsys.iso $(DISKDIR)
	#mkisofs -J -hide-rr-moved -joliet-long -l -r -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o $@ $(ISODIR)

MODULES=biosdisk normal part_msdos fat iso9660 hfs ls multiboot search help

# make a hard-disk image file
$(BLDDIR)/nopsys.vmdk: $(DISKDIR)	
	bash ./scripts/create-fat32.sh 200 $(DISKDIR) $(BLDDIR)/nopsys.raw
	
	# copy first sector without overwriting the MBR
	dd conv=notrunc if=/usr/lib/grub/i386-pc/boot.img of=$(BLDDIR)/nopsys.raw bs=1 count=446	
	
	# build and copy the rest of grub bootloader
	grub-mkimage -c boot/grub.cfg --format=i386-pc -p "" $(MODULES) >$(BLDDIR)/boot-image.raw
	dd conv=notrunc if=$(BLDDIR)/boot-image.raw of=$(BLDDIR)/nopsys.raw bs=512 seek=1
	
	qemu-img convert -f raw $(BLDDIR)/nopsys.raw -O vmdk $(BLDDIR)/nopsys.vmdk


ccred=@echo -n "\033[0;31m"
ccend=@echo -n "\033[0m"

$(VM_BUILDDIR)/vm.obj:
	@echo "------------"
	$(ccred)
	@echo "$@ not found in $(shell pwd)...\n"
	@echo "for your dialect you should have generated a vm.obj file. That file includes"
	@echo "your vm and the entrypoint 'nopsys_vm_main(image_bytes, image_size)'."
	@echo "This makefile can then try to link everything, without caring which"
	@echo "dialect you are building for."
	$(ccend)
	@echo "------------"
	@exit 1
	
#----------------------------------
# system vm generation
#----------------------------------

$(BLDDIR)/vmware.cd.vmx: boot/vmx.template  $(BLDDIR)
	cp boot/vmx.template $@
	chmod +x $@
	echo 'ide0:0.fileName = nopsys.iso' >> $@
	echo 'ide0:0.deviceType = "cdrom-image"' >> $@

$(BLDDIR)/vmware.hd.vmx: boot/vmx.template $(BLDDIR)
	cp boot/vmx.template $@
	chmod +x $@
	echo 'ide0:0.fileName = nopsys.vmdk' >> $@


$(BLDDIR)/bochsrc : boot/bochsrc boot/bochsrc-hd
	cp boot/bochsrc boot/bochsrc-hd boot/bochsdbg $(BLDDIR)/

$(BLDDIR)/qemudbg: boot/qemudbg
	cp boot/qemudbg $(BLDDIR)/	

#----------------------------------
# system vm running 
#----------------------------------
try-vmware: try-vmware-$(STORAGE)

try-vmware-iso: $(BLDDIR)/vmware.cd.vmx $(BLDDIR)/nopsys.iso 
	vmplayer $<
#	vmware-server-console -m -x -l "`pwd`/$<"
#	make clean

try-vmware-hd: $(BLDDIR)/vmware.hd.vmx $(BLDDIR)/nopsys.vmdk
	vmplayer $<

try-virtualbox: try-virtualbox-$(STORAGE)

try-virtualbox-iso: $(BLDDIR)/nopsys.iso
	scripts/virtualbox.sh iso

try-virtualbox-hd: $(BLDDIR)/nopsys.vmdk
	scripts/virtualbox.sh hd

try-bochs: $(BLDDIR)/nopsys.iso $(BLDDIR)/bochsrc
	cd build && bochs -q -rc bochsdbg

try-bochs-hd: $(BLDDIR)/nopsys.vmdk $(BLDDIR)/bochsrc
	cd build && bochs -f bochsrc-hd -q


try-qemu: try-qemu-$(STORAGE)

try-qemu-iso: $(BLDDIR)/nopsys.iso
	#kvm not supported in mac
	qemu-system-x86_64 -boot d -cdrom $(BLDDIR)/nopsys.iso -m 512 -enable-kvm -netdev user,id=user0 -device rtl8139,netdev=user0

try-qemu-hd: $(BLDDIR)/nopsys.vmdk
	#kvm not supported in mac
	qemu-system-x86_64 -boot d -hda $(BLDDIR)/nopsys.vmdk -m 512 -enable-kvm -netdev user,id=user0 -device rtl8139,netdev=user0


try-qemudbg: try-qemudbg-$(STORAGE)

try-qemudbg-iso: $(BLDDIR)/nopsys.iso $(BLDDIR)/qemudbg
	# use setsid so that ctrl+c in gdb doesn't kill qemu
	#kvm not supported in mac
	cd $(BLDDIR) && $(SETSID) qemu-system-x86_64 -s -boot d -cdrom nopsys.iso -m 512 -enable-kvm --netdev user,id=user0 -device rtl8139,netdev=user0 &
	sleep 6
	cd build && $(GDB) nopsys.kernel -x qemudbg

try-qemudbg-hd: $(BLDDIR)/nopsys.vmdk $(BLDDIR)/qemudbg
	# use setsid so that ctrl+c in gdb doesn't kill qemu
	#kvm not supported in mac
	cd $(BLDDIR) && $(SETSID) qemu-system-x86_64 -s -boot d -hda nopsys.vmdk -m 512 -enable-kvm --netdev user,id=user0 -device rtl8139,netdev=user0 &
	sleep 14
	cd build && $(GDB) nopsys.kernel -x qemudbg


