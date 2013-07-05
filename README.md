SoftWire
========

SoftWire is a software I2C implementation for Arduino and other
Wiring-type environments. It utilises the `pinMode()`,
`digitalWrite()` and `digitalRead()` functions. The pins to be used
for serial data (SDA) and serial clock (SCL) control lines can be
defined at run-time. Multiple instances are supported.
Clock-stretching by slave devices is supported. Operations which loop
(eg `startWait()`) or support clock-stretching include a timeout
feature to prevent lockups by faulty or missing hardware. The
microntroller must function as the master device, multiple masters are
not supported.


License
-------

The SoftWire library is licensed with the GNU Lesser General Public
License. See LICENSE.txt for details.


Requirements
------------

The AsyncDelay library is required, see
https://github.com/stevemarple/AsyncDelay


Credits
-------

This library is inspired by Peter Fleury's i2cmaster library.
