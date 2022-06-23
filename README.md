# deskimg
## purpose
deskimg is a lightweight program that enables users to conveniently display pictures and animations on the linux desktop.

## examples

## builing
deskimg only requires the GNU C standard library, the gcc compiler, and *GTK3.0*, which to my knowledge should be installed
on just about any distro with a desktop environment.

To build, simply run the install script included in the repo like so:
'sh install.sh'
By default, the program installs to ~/.local/bin. If you would like to change that, run
'gcc $(dpkg-config --cflags gtk+-3.0) ./src/deskimg.c -o <desired directory>/deskimg $(dpkg-config --libs gtk+-3.0)'
