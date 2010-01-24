
/* Name: uart.h
 * Project: AVR USB driver for CDC interface on Low-Speed USB
 * Author: Osamu Tamura
 * Creation Date: 2006-06-18
 * Tabsize: 4
 * Copyright: (c) 2006 by Recursion Co., Ltd.
 * License: Proprietary, free under certain conditions. See Documentation.
 */

#ifndef __uart_h_included__
#define __uart_h_included__

#ifndef uchar
#define uchar   unsigned char
#endif

#ifndef uint
#define uint    unsigned int
#endif

#define HW_CDC_BULK_OUT_SIZE     1
#define HW_CDC_BULK_IN_SIZE      8
/* Size of bulk transfer packets. The standard demands 8 bytes, but we may
 * be better off with less. Try smaller values if the communication hangs.
 */

#ifndef UART_DEFAULT_BPS
#define UART_DEFAULT_BPS     4800
#endif
/* 4800bps is the maximum speed by software UART.
   The baud rate will be automatically configured after opening device anyway.
*/
/*  #define UART_INVERT */

/* These are the USART port and TXD, RXD bit numbers.
*/
/* ATtiny45/85 */
#define UART_CFG_PORTNAME   B

#define UART_CFG_TXD        1

#if USB_CFG_DPLUS_BIT==2
#if USB_CFG_DMINUS_BIT==0
#define UART_CFG_RXD        PCINT5  /* HVSP */
#elif USB_CFG_DMINUS_BIT==3
#define UART_CFG_RXD        PCINT0
#endif

#define UART_INTR_CFG            PCMSK
#define UART_INTR_CFG_SET        _BV(UART_CFG_RXD)
#define UART_INTR_ENABLE         GIMSK
#define UART_INTR_ENABLE_BIT     PCIE
#define UART_INTR_PENDING        GIFR
#define UART_INTR_PENDING_BIT    PCIF
#define UART_INTR_VECTOR         SIG_PIN_CHANGE

#elif USB_CFG_DPLUS_BIT==4
#define UART_CFG_RXD        2   /*  INT0    */

#define UART_INTR_CFG            MCUCR
#define UART_INTR_CFG_SET        _BV(ISC01)
#define UART_INTR_ENABLE         GIMSK
#define UART_INTR_ENABLE_BIT     INT0
#define UART_INTR_PENDING        GIFR
#define UART_INTR_PENDING_BIT    INTF0
#define UART_INTR_VECTOR         SIG_INTERRUPT0
#else
#error "uart.h : invalid USB_CFG_DPLUS_BIT"
#endif


#define	RX_SIZE		8       /* UART receive buffer size */
#define	TX_SIZE		128     /* UART transmit buffer size */

#define RX_DELAY    DT1A
#define RX_READY    DT1B

#define	TX_MASK		(TX_SIZE-1)

/* ------------------------------------------------------------------------- */
/* ------------------------ General Purpose Macros ------------------------- */
/* ------------------------------------------------------------------------- */
#define OD_CONCAT(a, b)             a ## b
#define UART_OUTPORT(name)          OD_CONCAT(PORT, name)
#define UART_INPORT(name)           OD_CONCAT(PIN, name)
#define UART_DDRPORT(name)          OD_CONCAT(DDR, name)

#define UART_CFG_PORT     UART_OUTPORT(UART_CFG_PORTNAME)
#define UART_PIN          UART_INPORT(UART_CFG_PORTNAME)
#define UART_DDR          UART_DDRPORT(UART_CFG_PORTNAME)


#ifndef __ASSEMBLER__

extern uchar    sendEmptyFrame;
extern uchar    urptr, uwptr, irptr, iwptr;
extern uchar    rx_buf[RX_SIZE], tx_buf[TX_SIZE]; 

extern void     uartInit(uint baudrate);
extern void     uartPoll(void);
extern uchar    bit_reverse( uchar x );

#endif	/*  #ifndef __ASSEMBLER__  */
#endif  /*  __uart_h_included__  */

