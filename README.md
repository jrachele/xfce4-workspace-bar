# xfce4-workspace-bar

![Preview](preview.png)

A simple xfce4-panel plugin to allow for a Polybar/i3status type workspace preview with the Xfce4 window manager.

The icons are automatically fetched from the current active window on each workspace.

Workspaces are swapped to by either left-clicking on the desired workspace or by using keybindings.


## Dependencies

To install xfce4-workspace-bar, you will need `libxfce4panel`, `libxfce4ui`, `libxfce4util`, and `libwnck`. 

For most distributions the Xfce4 development libraries can be installed via some kind of `xfce4-dev-tools` or `xfce4-panel-dev` package.

`libwnck-3.0` is the required, which allows for workspace manipulation.

## Installation

1. Run `git clone https://github.com/jrachele/xfce4-workspace-bar.git` 

2. In the `xfce4-workspace-bar/` folder, first run `./autogen.sh`. 

3. Run `./configure --prefix=/usr` to direct the installation to `/usr/lib/xfce4`. 

4. In the `panel-plugin/` folder, run `sudo make install`. 

5. Add the widget to your panel.

## Bugs

This plugin was written and tested on Manjaro with bleeding-edge Xfce. Nevertheless it will most likely work fine on your hardware. If you run into any problems, you can try to debug yourself by first running `xfce4-panel -q` followed by `xfce4-panel` and reading stdout/stderr, or you can open an issues thread here.