/* Name: uart.c
 * Project: AVR USB driver for CDC interface on Low-Speed USB
 * Author: Osamu Tamura
 * Creation Date: 2006-06-18
 * Tabsize: 4
 * Copyright: (c) 2006 by Recursion Co., Ltd.
 * License: Proprietary, free under certain conditions. See Documentation.
 *
 * 2006-07-08   adapted to higher baud rate by T.Kitazawa
 */
/*
General Description:
    This module implements the UART rx/tx system of the USB-CDC driver.

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

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>   /* needed by usbdrv.h */
#include "oddebug.h"
#include "usbdrv.h"
#include "uart.h"


extern uchar    sendEmptyFrame;

/* UART buffer */
uchar    rx_buf[RX_SIZE], tx_buf[TX_SIZE];
uchar    urptr, uwptr, irptr, iwptr;


#ifndef URSEL
#   define URSEL_MASK   0
#else
#   define URSEL_MASK   (1 << URSEL)
#endif

void uartInit(ulong baudrate, uchar parity, uchar stopbits, uchar databits)
{
usbDWord_t   br;

    br.dword = F_CPU / (8L * baudrate) - 1;
	UCSR0A  |= (1<<U2X0);

#if DEBUG_LEVEL < 1
    /*    USART configuration    */
    UCSR0B  = 0;
    UCSR0C  = URSEL_MASK | ((parity==1? 3:parity)<<UPM00) | ((stopbits>>1)<<USBS0) | ((databits-5)<<UCSZ00);
    UBRR0L  = br.bytes[0];
    UBRR0H  = br.bytes[1];
#else
    DBG1(0xf0, br.bytes, 2);
#endif /* DEBUG_LEVEL */

#if UART_RXINT
    UCSR0B  = (1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0);
#else
    UCSR0B  = (1<<RXEN0) | (1<<TXEN0);
#endif
}

void uartPoll(void)
{

	/*  host => device  */
	if( (UCSR0A&(1<<UDRE0)) && uwptr!=irptr ) {
        UDR0    = tx_buf[irptr];
        irptr   = (irptr + 1) & TX_MASK;

        if( usbAllRequestsAreDisabled() && uartTxBytesFree()>8 ) {
            usbEnableAllRequests();
        }
    }

	/*  host <= device  */
    if( usbInterruptIsReady() && (iwptr!=urptr || sendEmptyFrame) ) {
        uchar   bytesRead;

        bytesRead = iwptr>=urptr? (iwptr-urptr):(RX_SIZE-urptr);
        if(bytesRead>8)
            bytesRead = 8;
        usbSetInterrupt(rx_buf+urptr, bytesRead);
        urptr   += bytesRead;
        urptr   &= RX_MASK;

        /* send an empty block after last data block to indicate transfer end */
        sendEmptyFrame = (bytesRead==8 && iwptr==urptr)? 1:0;
    }

#if !UART_RXINT
	/*  recieve char from USART */
	if( UCSR0A&(1<<RXC0) ) {
        uchar   status, data;

        status  = UCSR0A;
        data    = UDR0;
        status  &= (1<<FE0) | (1<<DOR0) | (1<<UPE0);
        if(status == 0) { /* no receiver error occurred */
            uchar   iwnxt;

            iwnxt = (iwptr+1) & RX_MASK;
            if( iwnxt!=urptr ) {
                rx_buf[iwptr] = data;
                iwptr = iwnxt;
            }
        }
    }
#endif
}



