# xfce4-windowck-plugin

Xfce panel plugin which allows to put the maximized window title and windows buttons on the panel.

Original plugin's features:

* Show the title and buttons of the maximized window on the panel.
* Allow window actions on buttons and title clicks (activate, (un)maximize, close).
* Allow window action menu on left button click.
* Title formatting options.
* xfwm4/unity theming support for buttons.

**FAQ:**  
Q: How do I hide the window decorations on maximized windows on Xfce?  
A: Xfce 4.12 now support Titileless feature!! Enable it in window manager tweaks → accessibility.

> This code is derived from original 'Window Applets' from Andrej Belcijan.
See http://gnome-look.org/content/show.php?content=103732 for infos.
 
## Xfce wiki

* https://goodies.xfce.org/projects/panel-plugins/xfce4-windowck-plugin

## Screenshots

![this page](https://goodies.xfce.org/_media/projects/panel-plugins/windowck-plugin-1.png)

## Installation

### Arch Linux

For Arch Linux users [AUR package](https://aur.archlinux.org/packages/xfce4-windowck-plugin/) is available

### Debian / Ubuntu

* Grab ``.deb`` file from [Releases](https://github.com/zlamalp/xfce4-windowck-plugin/releases) page.
* Install it manually using ``sudo apt install ./xfce4-windowck-plugin_[version]_amd64.deb``

#### Available versions / Compatibility

* Xubuntu 20.04 / GTK-3 / Xfce 4.14 -> [version 0.5.0](https://github.com/zlamalp/xfce4-windowck-plugin/releases/tag/v0.5.0-fix)
* Xubuntu 20.04 / GTK-2 / Xfce 4.14 -> [version 0.4.8](https://github.com/zlamalp/xfce4-windowck-plugin/releases/tag/v0.4.8-fix)
* Xubuntu 18.04 / GTK-2 / Xfce 4.12 -> [version 0.4.7](https://github.com/zlamalp/xfce4-windowck-plugin/releases/tag/v0.4.7-fix)

#### Other Linux distributions

* Install dependencies:
  - For debian/ubuntu see debian/control folder and follow debian packaging guidelines
* Generate common makefiles:
  ```bash
  ./autogen.sh --prefix=/usr
  ```
* compile and install the plugin
  ```bash
  make
  sudo make install
  ```

## How to report bugs?

I only package this thing. Please report bugs [upstream](https://github.com/cedl38/xfce4-windowck-plugin/issues).
