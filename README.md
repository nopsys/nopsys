# Nopsys
No Operating System Project: Seeking Dan Ingalls' quote: 'Operating Systems should not exist'

## Rationale
This project is a third iteration of the orginal SqueakNOS project born at early 2000's. SqueakNOS had two previous stages. 
At a first stage, [Gerardo Richarte]() (Richie) and [Luciano Notarfrancesco]() depicted the fundamentals of the project 
and built its basis. Most of the code contained in this and related repositories was developed by them. 
The sources of that stage of the project can be found at: https://sourceforge.net/projects/squeaknos/.

In a second stage, during 2011-2012, [Javier Pimás](https://github.com/melkyades) and [Guido Chari](https://github.com/charig), in collaboration with Richie revived the project and made it compatible with Pharo Smalltalk. In addition, we added Filesystem (FAT32) and Memory Management (paging) support. This made it possible to snapshot images, an important milestone of the project. There is a blog with some documentation, the news of those days, and instructions to download prebuilt images: http://squeaknos.blogspot.com.ar/.

Now, we are working in a second revival! Smalltalk VMs (and also images) have changed a lot since the old times. 
We have decided to give a new name to the project: Nopsys (No Operating System). The reason is that we do not want the 
project to be tightly coupled to any particular Smalltalk dialect. Besides, although we are developing the project in 
Smalltalk, and we considered ourselves Smalltalkers, actually the fundamental ideas are language-agnostic. 

## Publications
We are working on them. Nothing serious yet :). Hope to have news soon.

## Code structure

In this new iteration, platform code has been modularized. This repo contains _nopsys_, a set of language-agnostic platform files. Aditionally, you will find under the nopsys project umbrella different implementations using different languages or VM implementations (CogNOS, BeeNOS, PyNOS, etc). 

This nopsys repo contains a minimal grub, a few C files for life support and a makefile. The makefile has the following main targets: 
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

## Goals for this new version of Nopsys
- [ ] Make Nopsys work with the new family of Open-Smalltalk VMs (Stack, Cog, Spur) and with the up to date images 
(Pharo, Squeak, Cuis). 
- [ ] ... 

Build Status
------------

Thanks to Travis CI, all commits of this repository are tested.
The current build status is: [![Build Status](
https://travis-ci.org/nopsys/nopsys.png)](https://travis-ci.org/nopsys/nopsys)
