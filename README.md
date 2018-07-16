# Nopsys
Nopsys stands for No Operating System. The project seeks the following radical and amazing Dan Ingalls' quote: 'Operating Systems should not exist'

## Rationale
The main idea of nopsys is to provide, in a small language-agnostic framework, all the necessary features to run your application, or alternatively a live integral development environment, on top of the bare metal (without an operating system). 

Nopsys is _not_ an operating system. It is a set of tools and libraries to run your app without an OS, or to create your own OS if you will. Nopsys has been extracted out of SqueakNOS, and became the framework used to build [CogNOS](https://github.com/nopsys/CogNOS). We tried to make it modular and to remove all dependencies with SqueakNOS. You may still find some references to SqueakNOS here and there, but we would like to clean them up in the near future.

## Design

This repo contains a minimal grub configuration file, a few C and Assembly files for basic machine initialization and life support, and a makefile. The app to be run with nopsys has to provide anything else (interrupt handlers, device drivers, etc). Nopsys will aid in compiling a kernel file which includes the app code, and then will create a hard-disk or ISO image file, with the kernel and grub configured to load and run the kernel. 

Nopsys expects from the app to be run a very small API. At boot time, grub will call `nopsys_main`, which passes the CPU into 64-bit long mode, configures the CPU to ignore all interrupts and calls `nopsys_vm_main`, which has to be implemented by app. The app has to handle all the interrupts in which it is interested, if any. There are 3 kinds of interrupt sources: external interrupts coming from devices, exceptions and system calls. The former are dispatched asynchronically, while the other two have to be handled synchronically if they arrive. To handle an external interrupt, the app has to bind the interrupt number to a semaphore index in the `irq_semaphores` array, and implement a function (semaphore_signal_with_index) which is called signal that semaphores within the app.

Nopsys provides an implementation of a small subset of libc. This is because typical runtimes (specially Cog) require access to the standard C library, and also to provide a minimum of help. We would like to move this libc out as a separate project in the future, letting nopsys be even smaller.

## Publications
We are working on them. Nothing serious yet :). Hope to have news soon.

## Code structure

The nopsys makefile has the following main targets: 
 - `libnopsys`. It takes all C files and generates a libnopsys.obj with a minimal subset of libc, a console, serial I/O for communicating with host system VMs, etc. 
 - `nopsys.kernel`. For building a complete system, an OS implementation is needed. This usually is a language vm (like cog) and an image or something similar. This target expects to find it in a file called vm.obj, which the nopsys implementor has to provide.
 - `iso`. It takes the nopsys.kernel, grub and a loader and links everything together into an ISO image that can be booted.
 - `hd`. Similar to `iso`, but creates a hard-disk image with a fat32 partition instead.
 
### Implementing your own nopsys-based runtime

To give an example, for creating yourNOS, you would need something like the following file structure:

     yournos 
     / vm-src    - your app files (usually a vm for a high-level language)
     / nopsys    - nopsys git submodule
     / Makefile  - compiles your src into vm.obj and recursivelly calls
                   nopsys make iso, providing the path to vm.obj

## Current (No) Operating Systems using Nopsys:
 * [CogNOS](https://github.com/nopsys/CogNOS): A Cog (Spur) Pharo Smalltalk.
 * We would really like to see implementations for other managed languages, we're eager to see people create PyNOS, RubyNOS, etc.

## Documentation
We are currently working on providing proper documentation for developers. The best way to understand how nopsys is applied is to take a look at the [CogNOS makefile](https://github.com/nopsys/opensmalltalk-vm/blob/Cog/platforms/nopsys/Makefile).

Build Status
------------

Thanks to Travis CI, all commits of this repository are tested.
The current build status is: [![Build Status](
https://travis-ci.org/nopsys/nopsys.png)](https://travis-ci.org/nopsys/nopsys)
