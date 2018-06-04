include compilation.conf

NOPSYS_IMAGE   = ../dmr/bee.bsl
NOPSYS_SOURCES = ../dmr/bee.bsc

BLDDIR    = build
OBJDIR    = $(BLDDIR)/objs#   temp .o files go here
EXTRADIR  = $(BLDDIR)/extra#   files needed for execution go here (image, sources, etc)
ISODIR    = $(BLDDIR)/iso#    temp dir to put everything and package as iso
DISTRODIR = $(BLDDIR)/distro

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


.PRECIOUS: %.img

.PHONY: libnopsys iso
# ==================
# real file targets
# ==================
$(BLDDIR):
	mkdir -p $(BLDDIR) $(BLDDIR)/mount $(ISODIR) $(ISODIR)/boot/grub $(DISTRODIR) $(OBJDIR)

# from src dir generate a small generic lib, that later has to be linked to whatever dialect/vm is used
$(BLDDIR)/libnopsys.obj: $(BLDDIR)
	make -C src

# object file to be loaded by grub, your dialect should have generated a vm.obj file and put in BLDDIR 
$(BLDDIR)/nopsys.kernel: $(BLDDIR)/libnopsys.obj $(VM_BUILDDIR)/vm.obj boot/loader.s boot/kernel.ld
	$(AS) -o $(BLDDIR)/loader.o $(ASFLAGS_ARCH) boot/loader.s
	$(LD) -o $(BLDDIR)/nopsys.kernel $(LDFLAGS_ARCH) -T boot/kernel.ld $(BLDDIR)/loader.o $(BLDDIR)/libnopsys.obj $(VM_BUILDDIR)/vm.obj
	nm $(BLDDIR)/nopsys.kernel | grep -v " U " | awk '{print "0x" $$1 " " $$3}' > $(BLDDIR)/nopsys.sym

$(EXTRADIR): $(VM_BUILDDIR)/extra
	mkdir -p $@
	cp -r $(wildcard $(VM_BUILDDIR)/extra/*) $@

# make an iso (CD image)
$(BLDDIR)/nopsys.iso: $(BLDDIR)/nopsys.kernel boot/grub.cfg $(EXTRADIR)
	cp -r boot/grub.cfg $(ISODIR)/boot/grub/
	cp $(EXTRADIR)/* $(ISODIR)/
	cp $(BLDDIR)/nopsys.kernel $(ISODIR)/
	$(MKRESCUE) --xorriso=$(XORRISO_DIR)xorriso -o $(BLDDIR)/nopsys.iso $(ISODIR)
	#mkisofs -J -hide-rr-moved -joliet-long -l -r -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o $@ $(ISODIR)

# make a hard-disk image file
$(BLDDIR)/nopsys.vmdk: $(BLDDIR)/nopsys.iso
	bash -x ./scripts/create-fat32.sh 100 $(BLDDIR)/iso $(BLDDIR)/nopsys.raw
	export LOOP_DEVICE1=`sudo losetup -f` &&\
	sudo losetup $$LOOP_DEVICE1 $(BLDDIR)/nopsys.raw &&\
	export LOOP_DEVICE2=`sudo losetup -f` &&\
	export OFFSET=`parted $(BLDDIR)/nopsys.raw unit b print | tail -2 | head -1 | cut -f 1 --delimit="B" | cut -c 9-` &&\
	sudo losetup $$LOOP_DEVICE2 $(BLDDIR)/nopsys.raw -o $$OFFSET &&\
	sudo mount -t vfat $$LOOP_DEVICE2 $(BLDDIR)/mount/ &&\
	sudo grub-install --target=i386-pc --no-floppy --boot-directory=$(BLDDIR)/mount/boot/ --modules="normal part_msdos fat multiboot" $$LOOP_DEVICE1 &&\
	sudo umount $(BLDDIR)/mount &&\
	sudo losetup -d $$LOOP_DEVICE2 &&\
	sudo losetup -d $$LOOP_DEVICE1
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
	

$(BLDDIR)/vmware.cd.vmx: boot/vmx.cd.template
	cp boot/vmx.cd.template $@
	chmod +x $@
	echo 'ide0:0.fileName = nopsys.iso' >> $@

$(BLDDIR)/bochsrc : boot/bochsrc
	cp boot/bochsrc boot/bochsdbg $(BLDDIR)/

$(BLDDIR)/qemudbg: boot/qemudbg
	cp boot/qemudbg $(BLDDIR)/	

# system vm generation and running 
#----------------------------------

try-vmware: $(BLDDIR)/vmware.cd.vmx $(BLDDIR)/nopsys.iso 
	vmplayer $<
#	vmware-server-console -m -x -l "`pwd`/$<"
#	make clean

try-virtualbox: try-virtualbox-$(STORAGE)

try-virtualbox-iso: $(BLDDIR)/nopsys.iso
	scripts/virtualbox.sh iso

try-virtualbox-hd: $(BLDDIR)/nopsys.vmdk
	scripts/virtualbox.sh hd

try-bochs: $(BLDDIR)/nopsys.iso $(BLDDIR)/bochsrc
	cd build && bochs -q -rc bochsdbg


try-qemu: try-qemu-$(STORAGE)

try-qemu-iso: $(BLDDIR)/nopsys.iso
	qemu-system-x86_64 -boot d -cdrom $(BLDDIR)/nopsys.iso -m 512

try-qemu-hd: $(BLDDIR)/nopsys.vmdk
	qemu-system-x86_64 -boot d -hda $(BLDDIR)/nopsys.vmdk -m 512


try-qemudbg: try-qemudbg-$(STORAGE)

try-qemudbg-iso: $(BLDDIR)/nopsys.iso $(BLDDIR)/qemudbg
	# use setsid so that ctrl+c in gdb doesn't kill qemu
	cd $(BLDDIR) && $(SETSID) qemu-system-x86_64 -s -boot d -cdrom nopsys.iso -m 512 &
	sleep 6
	cd build && $(GDB) nopsys.kernel -x qemudbg

try-qemudbg-hd: $(BLDDIR)/nopsys.vmdk $(BLDDIR)/qemudbg
	# use setsid so that ctrl+c in gdb doesn't kill qemu
	cd $(BLDDIR) && $(SETSID) qemu-system-x86_64 -s -boot d -hda nopsys.vmdk -m 512 &
	sleep 6
	cd build && $(GDB) nopsys.kernel -x qemudbg


