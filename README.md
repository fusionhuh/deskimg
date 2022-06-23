# deskimg
## purpose
deskimg is a lightweight program that enables users to conveniently display pictures and animations on the linux desktop.

## examples
![Peek 2022-06-23 01-59](https://user-images.githubusercontent.com/59339739/175344013-24051e08-4eed-4719-8a31-9d6431bfeef5.gif)

## building
deskimg only requires the GNU C standard library, the gcc compiler, and GTK3.0, which to my knowledge should be installed
on just about any distro with a desktop environment.

to build, simply run the install script included in the repo like so:
`sh install.sh`
by default, the program installs to ~/.local/bin. if you would like to change that, run
`gcc $(pkg-config --cflags gtk+-3.0) ./src/deskimg.c -o <desired directory>/deskimg $(pkg-config --libs gtk+-3.0)`
