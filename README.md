# 3DViewer
* [Description](#description)
* [Build](#build)
* [Tests](#tests)
* [Documentation](#documentation)
* [Test Converage](#test-coverage)
* [Archive](#archive)
* [Credits](#credits)

## Description
An application for viewing 3D models, with their display settings.

## Build
To build, you need to have `qmake` on your device.

    $ git clone https://github.com/ksilisk/3DViewer.git
    $ cd 3DViewer
    $ make install
## Tests
Unit tests with `check.h` C library.
    
    $ cd 3DViewer
    $ make test
## Documentation
Documentation is generated using `Doxygen`. Before use, you need to change the `INPUT` value in the Doxygen file.

    $ cd 3DViewer
    $ make dvi
## Test Coverage
Test coverage is estimated using `lcov`.

    $ cd 3DViewer
    $ make gcov_report
## Archive
The archive is built using `tar`.

    $ cd 3DViewer
    $ make dist
## Credits
Developers: [Shaliko Salimov (@wormbett)](https://github.com/ksilisk), [Sergei Mahinov (@yuehbell)](https://github.com/sermah), [Alexey Yablokov (@earthcar)](https://github.com/XmasApple)
