# Deployment of kleiner-brauhelfer-2 on Arch-Linux
Arch-Linux and followers do not support the debian package format, they have their own.

This deployment method does not build the kleiner-brauhelfer-2 application for being downloaded from github.  

Instead, it allows the enduser very easily to build and install kleiner-brauhelfer-2 by himself. The method uses [makepkg](https://wiki.archlinux.org/title/makepkg) to build and install the Arch-Linux package. 

The information for building the package is contained in the two files **PKGBUILD** and **kleiner-brauhelfer-2.desktop** stored and stored in the [Arch-Linux User Repository](https://aur.archlinux.org/packages/kleiner-brauhelfer), also called AUR. 

## Prerequisit:

Install the needed tools by executing in a terminal:

```bash
pamac install git base-devel
```

## Building the Installation Package

Open the package information of [kleiner-brauhelfer at arch-linux.org](https://aur.archlinux.org/packages/kleiner-brauhelfer):

Check the version of kleiner-brauhelfer-2 being created fit's your need.

Execute following commands in the shell: 

```bash
git clone https://aur.archlinux.org/kleiner-brauhelfer.git
cd kleiner-brauhelfer
makepkg -s
```
Proceed to [Install the Package](#install-the-package).

If you want to build and install a different version:
- mkdir kleiner-brauhelfer-2.build
- copy PKGBUILD and kleiner-brauhelfer-2.desktop from  deployment/arch-linux directory on github to the working directory
- cd working directory
- makepkg -s 


## Install the Package

Install the package by executing in a shell:

```bash
makepkg -i
```

## Enjoy

Have a lot of fun with kleiner-brauhelfer-2

## Upgrade the Package 

If you still have access to the local build directory from an previous installation:

Execute following commands in a shell:

```bash
cd kleiner-brauhelfer #(the directory of the previous installation)
git pull
makepkg -s
makepkg -i
```

If the directory was deleted, follow the instructions [Build the Installation Package](#building-the-installation-package) and [Install the Package](#install-the-package)