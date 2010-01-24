

                                    CDC-232


    This is the Readme file to firmware-only CDC driver for Atmel AVR
    microcontrollers. For more information please visit
    http://www.recursion.jp/avrcdc/


SUMMARY
=======
    The CDC-232 performs the CDC (Communication Device Class) connection over
    low-speed USB. It provides the RS-232C interface through virtual COM
    port. The AVR-CDC is originally developed by Osamu Tamura.
    Akira Kitazawa has significantly contributed to improve this software. 


SPECIFICATION
=============
    AVR-CDC with USART (ATmega8/48/88/168)
        speed:    1200 - 38400bps
        datasize: 5-8
        parity:   none/even/odd
        stopbit:  1/2

    AVR-CDC with USART (ATtiny2313)
        speed:    2400,9600,38400bps Fixed (rebuild to use 600-57600bps)
        datasize: 8
        parity:   none
        stopbit:  1

    AVR-CDC without USART (ATtiny45/85)
        speed:    1200 - 4800bps
        datasize: 8
        parity:   none
        stopbit:  1
        supply current: 8-15mA

    Internal RC Oscillator is calibrated at startup time. It may be unstable 
    after a long time operation (ATtiny45/85).

    Although the CDC is supported by Windows 2000/XP/Vista, Mac OS 9.1/X,
    and Linux 2.4, low-speed bulk transfer is not allowed by the USB standard.
    Use CDC-IO at your own risk.


USAGE
=====
    [Windows XP/2000/Vista]
    Download "avrcdc_inf.zip" and read the instruction carefully.
 
    [Mac OS X]
    You'll see the device /dev/cu.usbmodem***.

    [Linux]
    The device will be /dev/ttyACM*.
    Linux 2.6 does not accept low-speed CDC without patching the kernel.


DEVELOPMENT
===========
    Build your circuit and write firmware (cdcmega*.hex/cdctiny*.hex) into it.
    C1:104 means 0.1uF, R3:1K5 means 1.5K ohms.

    This firmware has been developed on AVR Studio 4.16 and WinAVR 20090313.
    If you couldn't invoke the project from cdc*.aps, create new GCC project
    named "at***" under "cdc232.****-**-**/" without creating initial file. 
    Select each "default/Makefile" at "Configuration Options" menu.

    There are several options you can configure in
    "Project/Configuration Options" menu, or in Makefile

    (General)
    Device      Select MCU type.   
    Frequency   Select clock. 16.5MHz is the internal RC oscillator.
                (ATtiny45/85)
                3.3V Vcc may not be enough for the higher clock operation.

    (Custom Options) add -D*** to select options below.
    INVERT      Reverse the polarity of TXD and RXD to connect to RS-232C
                directly (ATtiny45/85).
    INTERRUPT   Using RxD interrupt achieves the higher transfer rate, but
                it causes obstacle in USB communication.
                Try this if the USB connection is stable. (ATmega)

    Rebuild all the codes after modifying Makefile.

    Fuse bits
                          ext  H-L
        ATtiny2313         FF CD-FF
        ATtiny45/85        FF CE-F1
        ATtiny461/861      FF C8-F1
        ATmega8               8F-FF
        ATmega48/88/168    FF CE-FF

	SPIEN=0, WDTON=0, CKOPT(mega8)=0,
	Crystal: Ex.8MHz/PLL(45,461), BOD: 1.8-2.7V

    * Detach the ISP programmer before restarting the device.

    The code size of AVR-CDC is 2-3KB, and 128B RAM is required at least.


USING AVR-CDC FOR FREE
======================
    The AVR-CDC is published under an Open Source compliant license.
    See the file "License.txt" for details.

    You may use this driver in a form as it is. However, if you want to
    distribute a system with your vendor name, modify these files and recompile
    them;
        1. Vendor String in usbconfig.h
        2. COMPANY and MFGNAME strings in avrcdc.inf/lowbulk.inf 



    Osamu Tamura @ Recursion Co., Ltd.
    http://www.recursion.jp/avrcdc/
    26 June 2006
    7 April 2007
    7 July 2007
    27 January 2008
    25 August 2008
    10 April 2009
    18 July 2009

