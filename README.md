# CBitmap
A C library for reading and writing bitmaps image files.

## Features
* Uses ANSI C.
* Defines a minimalist struct for holding image data.
* Converts to RGBA format.
* Easily integrated with OpenGL.
* Simple design and usage.

## Installation
None required; simply dowload the library, then include it in your main.c
```c
#include "bmp.h"
```

```console
$ gcc main.c
```

## Usage
See the Duplicate Image example.

## Further Work
* Only really works with 32 bit pixels: those with an alpha channel.
* A lot of formatting nuances that are not addressed.
