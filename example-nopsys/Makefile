
export VM_BUILDDIR=example-nopsys

vm:
	nasm -o vm.obj -f elf nopsys.asm

iso: vm
	make -C .. iso

run: iso
	cd ../build && bochs -q -rc bochsdbg

clean:
	make -C .. clean


