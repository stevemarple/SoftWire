SoftWire
========

SoftWire is a software I2C implementation for Arduino and other
Wiring-type environments. It utilises the `pinMode()`,
`digitalWrite()` and `digitalRead()` functions. The pins to be used
for the serial data (SDA) and serial clock (SCL) control lines can be
defined at run-time. Alternatively it is possible to pass references
to functions which control the SDA and SCL lines, and read SDA and
SCL, thereby allowing direct port manipulation to be used if
preferred.

Multiple objects (for multiple software I2C buses) and
clock-stretching by slave devices are supported. A timeout feature is
included to prevent lockups by faulty or missing hardware. The
microcontroller must function as the master device and multiple
masters are not supported.


License
-------

The SoftWire library is licensed with the GNU Lesser General Public
License. See LICENSE.txt for details.


Requirements
------------

The AsyncDelay library is required, see
https://github.com/stevemarple/AsyncDelay

Contributors
------------

Steve Marple
Matthijs Kooijman

Credits
-------

This library is inspired by Peter Fleury's i2cmaster library.
