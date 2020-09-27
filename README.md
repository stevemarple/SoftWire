# SoftWire

## Introduction

SoftWire is a software I2C implementation for Arduino and other
Wiring-type environments. It utilises the `pinMode()`,
`digitalWrite()` and `digitalRead()` functions. The pins to be used
for the serial data (SDA) and serial clock (SCL) control lines can be
defined at run-time. Alternatively it is possible to override the
functions which control the SDA and SCL lines, and read SDA and SCL,
thereby allowing direct port manipulation to be used if preferred.

Multiple objects (for multiple software I2C buses) and
clock-stretching by slave devices are supported. A timeout feature is
included to prevent lockups by faulty or missing hardware. The
microcontroller must function as the master device and multiple
masters are not supported.

Low-level functions can be used to send start, and stop signals, and
to read and write data; no buffers are required. In addition
high-level functions provide almost direct compatibility with the Wire
library. However, the user must first declare transmit and receive
buffers, and configure SoftWire to use them before the high-level
functions `beginTransmission()`, `endTransmission()`, `read()`, `write()` and
`requestFrom ()` can be used. Notify SoftWire of the buffers by calling the
`setTxBuffer()` and `setRxBuffer()` functions, passing in a pointer to a
byte or character buffer and the size of the buffer. See the [ReadDS1307](examples/ReadDS1307/ReadDS1307.ino) fort an exmaple of how this is done.

It is possible to assign (and reassign) the pins associated with SCL and SDA
at run-time using the functions `setScl()` and `setSda()`.

## Important changes for users of the v1.* library

To support the high-level functions required for compatibility with
the Wire library the original low-level `write()` function has been
renamed `llWrite()`.

The setter functions `setSdaLow()`, `setSdaHigh()`, `setSclLow()`,
`setSclHigh()`, `setReadSda()`, `setReadScl()` must be used to
override the functions which control and read the SDA and SCL signals.

The functions which actually set SCL/SDA  low/high are now named
`sclLow()`, `sdaLow()`, `sclHigh()` and `sdaHigh()`. These functions
should not be called directly in normal use but may be required to
force some devices into low-power mode.

## License

The SoftWire library is licensed with the GNU Lesser General Public
License. See LICENSE.txt for details.

## Requirements

The AsyncDelay library is required, see
[https://github.com/stevemarple/AsyncDelay](https://github.com/stevemarple/AsyncDelay)

## Contributors

* [Steve Marple](https://github.com/stevemarple)
* [Matthijs Kooijman](https://github.com/matthijskooijman)
* [Phonog](https://github.com/Phonog)

## Credits

This library is inspired by Peter Fleury's i2cmaster library.
