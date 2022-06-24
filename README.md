# deskimg
## purpose
deskimg is a lightweight program that enables users to conveniently display pictures and animations on the linux desktop.

## examples
![Peek 2022-06-23 01-59](https://user-images.githubusercontent.com/59339739/175344013-24051e08-4eed-4719-8a31-9d6431bfeef5.gif)

![gaming](https://user-images.githubusercontent.com/59339739/175719187-1cf33db7-e153-4e37-8d67-e96f054c0fe7.gif)

credit: 

https://imgur.com/gallery/lDYbKsd

https://giphy.com/stickers/pixel-pixelart-nintendods-SUQWUAqVuV9uZbALUp

https://purplehearts-pinkfarts.tumblr.com/post/187635226863

## usage
```
  -f, --file                                   Select the image/animation file to be displayed.
  -o, --opacity=Decimal value from 0 to 1.     Set the opacity of the graphic.
  -x, --x_pos=Integer values only.             Set the x-position of the graphic on the screen.
  -y, --y_pos=Integer values only.             Set the y-position of the graphic on the screen.
  -s, --save                                   Allow the program to save the current graphic and its position to the config file located in ~/.deskimg/config.sh.
  -t, --toggle                                 Allows you to toggle between keeping the image on top of all other windows or below. Toggle by right clicking on the graphic.
  -r, --reset                                  Resets the config file. Will immediately exit after config file is reset.
```
certain mouse actions can trigger events that aid the user experience. a single mouse click on the graphic will begin to move it; a double middle click will close it; if the --toggle option is set, a single right click will move the graphic above or below all windows; if the --save option is set, a double right click will save the image to the config file.

the config file is written to ~/.deskimg/config.sh. creating this directory in advance is not necessary; the program will handle creation of the directory as well as the file.

for the best experience, it is recommended that you configure the config script to run on startup. if you don't want to do that or don't know how, just run `sh ~/.deskimg/config.sh` when desired.

note: transparency effects (and some other things probably?) require a compositing window manager.

## building
deskimg only requires the GNU C standard library, the gcc compiler, and GTK3.0, which to my knowledge should be installed
on just about any distro with a desktop environment.

to build, simply run the install script included in the repo like so:
`sh install.sh`

by default, the program installs to ~/.local/bin. if you would like to change that, run
`gcc $(pkg-config --cflags gtk+-3.0) ./src/deskimg.c -o <desired directory>/deskimg $(pkg-config --libs gtk+-3.0)`

if you installed the program with the `install.sh` script, you can simply uninstall with
`sh uninstall.sh`

if you want to uninstall and compiled to a custom directory different from ~/.local/bin, for the time being you will have to remove the binary and config directory manually.
