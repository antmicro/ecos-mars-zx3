ECos for Enclustra Zynq Mars ZX3
================================

This is a port of the eCos 3.0 real-time operating system for `Enclustra's Mars ZX3 Zynq module <http://www.enclustra.com/en/products/system-on-chip-modules/mars-zx3/>`_.

The full documentation for the port can be acquired from the following links:

* `HTML <https://ecos-for-enclustra-mars-zx3-zynq-module.readthedocs.org/en/latest/>`_
* `PDF <https://media.readthedocs.org/pdf/ecos-for-enclustra-mars-zx3-zynq-module/latest/ecos-for-enclustra-mars-zx3-zynq-module.pdf>`_
* `RST fallback <https://github.com/antmicro/ecos-mars-zx3/blob/master/doc/source/introduction.rst>`_

ECos
----

ECos is an open-source configurable real-time operating system intended for use in embedded applications. The documentation for eCos 3.0, which is the most recent version of the system as well as the one ported to Mars ZX3, can be found at http://ecos.sourceware.org/docs-3.0/.

Acknowledgements
----------------

We would like to thank Enclustra GmbH (http://enclustra.com) for co-sponsoring the port as well as Richard Rauch (http://www.itrgmbh.de) for his help in the porting effort.

The remarks of Garry Jeromson, Christoph Glattfelder and Sven Meier of Enclustra GmbH, the manufacturer of the module, have been extremely helpful in making this document more complete and accurate.

Licence
-------

*(based on the* `eCos licence overview <http://ecos.sourceware.org/license-overview.html>`_\ *)*

ECos is released under a modified version of the well known `GNU General Public License (GPL) <http://www.gnu.org/copyleft/gpl.html>`_. The eCos license is officially recognised as a GPL-compatible Free Software License. An **exception clause** has been added which limits the circumstances in which the license applies to other code when used in conjunction with eCos. The exception clause is as follows:

   As a special exception, if other files instantiate templates or use macros or inline functions from this file, or you compile this file and link it with other works to produce a work based on this file, this file does not by itself cause the resulting work to be covered by the GNU General Public License. However the source code for this file must still be made available in accordance with section (3) of the GNU General Public License.

   This exception does not invalidate any other reasons why a work based on this file might be covered by the GNU General Public License.

**The license does not require users to release the source code of any** *applications* **that are developed with eCos.**

Supported features
------------------

This eCos port provides the following software packages specific for Enclustra's Mars ZX3 Zynq module:

* HAL package
* SMP (symmetric multiprocessing) support
* L1 cache (DCACHE & ICACHE) support
* debug UART driver
* serial port (UART) driver
* Ethernet driver
* I2C driver
* SPI driver
* QSPI Flash driver
* RedBoot with Ethernet and Flash support
