
/* Name: uart-asm.S
 * Project: AVR USB driver for CDC interface on USB1.1
 * Author: Akira Kitazawa
 * Creation Date: 2006-07-08
 * Tabsize: 4
 * License: Proprietary, free under certain conditions. See Documentation.
 */

/*
General Description:
    This module implements the assembler part of the USB-CDC driver.

Note: This module violates the rule that interrupts must not be disabled for
longer than a couple of instructions (see usbdrv.h). Running UART interrupt
handlers with sei as the first instruction is not possible because it would
recurse immediately (the cause of the interrupt has not been removed). If
we collect the data and then call sei(), we win little. We therefore decide
to violate the rule. The effect on USB operation is, that packages may be
lost. This is equivalent to a package being dropped due to a CRC error. The
host will therefore retry the transfer after a timeout. It is therefore very
likely that no effect is seen at the application layer.
*/

#define __SFR_OFFSET 0      /* used by avr-libc's register definitions */
#include "usbportability.h"

#ifndef __IAR_SYSTEMS_ASM__
    /* configs for io.h */
#   define __SFR_OFFSET 0
#   define _VECTOR(N)   __vector_ ## N   /* io.h does not define this for asm */
#   include <avr/io.h> /* for CPU I/O register definitions and vectors */
#endif  /* __IAR_SYSTEMS_ASM__ */
#include "usbdrv.h" /* for common defs */
#include "uart.h"

#if UART_RXINT

/* register names */
#define x1		r16
#define x2		r17
#define x3		r20
#define x4		r21

/* Some assembler dependent definitions and declarations: */

#ifdef __IAR_SYSTEMS_ASM__

#define nop2     rjmp    $+2 /* jump to next instruction */
#define XL       r26
#define XH       r27
#define YL       r28
#define YH       r29
#define ZL       r30
#define ZH       r31
#define lo8(x)   LOW(x)
#define hi8(x)   ((x)>>8)    /* not HIGH to allow XLINK to make a proper range check */

    extern  iwptr, rx_buf

#ifndef IVT_BASE_ADDRESS
#define IVT_BASE_ADDRESS 0
#endif

    ASEG
    ORG     USART0_RX_vect + IVT_BASE_ADDRESS
    rjmp    SIG_USART_RECV
    RSEG    CODE

#else /* __IAR_SYSTEMS_ASM__ */

    .text

    .global SIG_USART_RECV
    .type   SIG_USART_RECV, @function


#endif /* __IAR_SYSTEMS_ASM__ */


; ######################## RS-232C functions ########################

SIG_USART_RECV:
		out		GPIOR0, x1								;1
		in		x1, SREG								;1
		out		GPIOR1, x1								;1
		out		GPIOR2, ZL								;1
		push	ZH										;2

		lds		x1, iwptr								;2
		mov		ZL, x1									;1
		inc		x1										;1
		andi	x1, RX_MASK								;1
		sts		iwptr, x1								;2

		clr		ZH										;1
		subi	ZL, lo8(-(rx_buf))						;1
		sbci	ZH, hi8(-(rx_buf))						;1
		lds		x1, UDR0								;2
		st		Z, x1									;2

		pop		ZH										;2
		in		ZL, GPIOR2								;1
		in		x1, GPIOR1								;1
		out		SREG, x1								;1
		in		x1, GPIOR0								;1
		reti											;4 {30}


#endif		/* UART_RXINT Add 0708 Kitazawa */

