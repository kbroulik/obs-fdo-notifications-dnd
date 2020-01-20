# OBS FDO Notifications DND
*(also: OBS Freedesktop Notifications Do Not Disturb)*

This provides an [OBS](https://obsproject.com/) plug-in for automatically enabling "do not disturb" mode on supported desktops (such as [KDE Plasma](https://kde.org/plasma-desktop) 5.16 or newer) while a stream or recording is in progress.

![Screenshot with KDE Plasma's notification center stating "Do not disturb while OBS is active (Recording in progress)"](obs_dnd_screenshot.png?raw=true "Recording in progress")

## How to install

This project is configured using `cmake`.
You need to have development packages for OBS and DBus installed to build it, for example `libobs-dev` and `libdbus-1-dev`.

Basically:
```
mkdir build
cd build
cmake ..
make
make install
```

## License

```
The MIT License (MIT)

Copyright 2020 Kai Uwe Broulik

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
```

This repository contains a copy of the `obs-frontend-api`.
OBS Studio is distributed under the [GNU General Public License v2](https://www.gnu.org/licenses/old-licenses/gpl-2.0.html) (or any later version).
