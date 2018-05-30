# Nopsys
Nopsys stands for No Operating System. The project seeks the following radical and amazing Dan Ingalls' quote: 'Operating Systems should not exist'

## Rationale
The main idea of nopsys is to provide in a small language-agnostic library all the necessary features to run your application, or alternatively a live integral development environment, on top of the bare metal (without an operating system). 

## Publications
We are working on them. Nothing serious yet :). Hope to have news soon.

## Code structure

<!--In this new iteration, platform code has been modularized. This repo contains _nopsys_, a set of language-agnostic platform files. Aditionally, you will find under the nopsys project umbrella different implementations using different languages or VM implementations (CogNOS, BeeNOS, PyNOS, etc). -->

This repo contains a minimal grub, a few C files for life support and a makefile. The makefile has the following main targets: 
 - `libnopsys`. It takes all C files and generates a libnopsys.obj with a minimal subset of libc, a console, serial I/O for communicating with host system VMs, etc. 
 - `nopsys.kernel`. For building a complete system, an OS implementation is needed. This usually is a language vm (like cog) and an image or something similar. This target expects to find it in a file called vm.obj, which the nopsys implementor has to provide.
 - `iso`. It takes the nopsys.kernel, grub and a loader and links everything together into an ISO image that can be booted.
 
To give an example, for creating yourNOS, you would need something like the following file structure:

     yournos 
     / src       - your NOS files
     / nopsys    - nopsys git submodule
     / Makefile  - compiles your src into vm.obj and recursivelly calls
                   nopsys make iso, providing the path to vm.obj

## Current (No) Operating Systems using Nopsys:
 * [CogNOS](https://github.com/nopsys/CogNOS): A Cog (Spur) Pharo Smalltalk.

## Documentation
We are currently working on providing proper documentation for developers.

Build Status
------------

Thanks to Travis CI, all commits of this repository are tested.
The current build status is: [![Build Status](
https://travis-ci.org/nopsys/nopsys.png)](https://travis-ci.org/nopsys/nopsys)
