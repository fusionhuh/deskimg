# deskimg 
deskimg is a lightweight program that enables users to conveniently display pictures and animations on the linux desktop. 

## examples
![Peek 2022-06-23 01-59](https://user-images.githubusercontent.com/59339739/175344013-24051e08-4eed-4719-8a31-9d6431bfeef5.gif)

![gaming](https://user-images.githubusercontent.com/59339739/175719187-1cf33db7-e153-4e37-8d67-e96f054c0fe7.gif)

credit: 

https://imgur.com/gallery/lDYbKsd

https://giphy.com/stickers/pixel-pixelart-nintendods-SUQWUAqVuV9uZbALUp

https://purplehearts-pinkfarts.tumblr.com/post/187635226863

![eyes](https://user-images.githubusercontent.com/59339739/175721106-2c23265d-7852-488c-a7fc-14c24ab8a1d3.gif)

i could not find the original creator, but here is the place where i downloaded from: https://vsgif.com/gif/2937394

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

in order to save configurations, deskimg can write commands to a bash script `~/.config/deskimg/config.sh` that can be executed later. for the best experience, it is recommended that you configure the config script to run on startup. if you don't want to do that or don't know how, just run `sh ~/.config/deskimg/config.sh` when desired.

**NOTE**: transparency effects require a compositor. 

## requirements
in order to build deskimg you need the gtk 3.0 header files and libraries.

## installation
to build and install deskimg (if necessary as root):
```
make install
```

by default, deskimg is installed to /usr/local/bin. if you would like to change that, run
```
make install PREFIX=~/.local
```

to uninstall (if necessary, apply the `PREFIX` you had used to install before.):
```
make uninstall
```
