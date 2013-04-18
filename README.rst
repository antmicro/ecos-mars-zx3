Introduction
============

This is a port of the eCos 3.0 real-time operating system for `Enclustra's Mars ZX3 Zynq module <http://www.enclustra.com/en/products/system-on-chip-modules/mars-zx3/>`_.

The port is undergoing verification by Enclustra, however it is made available for evaluation purposes.

The full documentation for the port can be acquired from the following links:

* https://ecos-for-enclustra-mars-zx3-zynq-module.readthedocs.org/en/latest/ (HTML)
* https://media.readthedocs.org/pdf/ecos-for-enclustra-mars-zx3-zynq-module/latest/ecos-for-enclustra-mars-zx3-zynq-module.pdf (PDF)

ECos
----

ECos is an open-source configurable real-time operating system intended for use in embedded applications. The documentation for eCos 3.0, which is the most recent version of the system as well as the one ported to Mars ZX3, can be found at http://ecos.sourceware.org/docs-3.0/.

Acknowledgements
----------------

We would like to thank Enclustra GmbH (http://enclustra.com) for co-sponsoring the port as well as Richard Rauch (http://www.itrgmbh.de) for his help in the porting effort.

The remarks of Garry Jeromson, Christoph Glattfelder and Sven Meier of Enclustra GmbH, the manufacturer of the module, have been extremely helpful in making this document more complete and accurate.

Supported features
------------------

This eCos port provides the following software packages specific for Enclustra's Mars ZX3 Zynq module:

* HAL package
* cache support
* debug UART driver
* serial port driver
* Ethernet driver
* I2C driver
* QSPI Flash driver
* RedBoot with Ethernet and Flash support
