/**
 * funkcje obsługugi RS 485
 * zgodne z protokołem komunikacyjnym używanym w moich projektach
 *
 * dwa pierwsze bity (uart_buf_head) oznaczają typ komunikatu:
 *  00 (0x00) - identyfikator urzadzenia
 *  01 (0x40) - identyfikator lini
 *  10 (0x80) - komenda
 *  11 (0xC0) - odpowiedz (ignorowane przez slave)
 */

/** funkcja wysyłająca komendę RS **/
void send_rs_cmd(uint8_t dev, uint8_t line, uint8_t cmd) {
#ifdef DRIVER_CTRL_PIN_SET_DDR
	DRIVER_CTRL_PIN_SET_DDR = DRIVER_CTRL_PIN_SET_DDR | (1 << DRIVER_CTRL_PIN);
#endif
	DRIVER_CTRL_PORT = DRIVER_CTRL_PORT | (1 << DRIVER_CTRL_PIN);
	_delay_ms(5); // 5ms
	UDR=dev;
	_delay_cs(5); // 50ms
	UDR=line;
	_delay_cs(5); // 50ms
	UDR=cmd;
	_delay_ms(5); // 5ms
	DRIVER_CTRL_PORT = DRIVER_CTRL_PORT & ( ~(1 << DRIVER_CTRL_PIN));
#ifdef DRIVER_CTRL_PIN_SET_DDR
	DRIVER_CTRL_PIN_SET_DDR = DRIVER_CTRL_PIN_SET_DDR & ( ~(1 << DRIVER_CTRL_PIN));
#endif
}

/** funkcja wysyłająca odpowiedz przez RS **/
void send_rs(uint8_t reply) {
#ifdef DRIVER_CTRL_PIN_SET_DDR
	DRIVER_CTRL_PIN_SET_DDR = DRIVER_CTRL_PIN_SET_DDR | (1 << DRIVER_CTRL_PIN);
#endif
	DRIVER_CTRL_PORT = DRIVER_CTRL_PORT | (1 << DRIVER_CTRL_PIN);
	_delay_ms(5); // 5ms
	UDR = 0xC0 | reply;
	_delay_ms(5); // 5ms
	DRIVER_CTRL_PORT = DRIVER_CTRL_PORT & ( ~(1 << DRIVER_CTRL_PIN));
#ifdef DRIVER_CTRL_PIN_SET_DDR
	DRIVER_CTRL_PIN_SET_DDR = DRIVER_CTRL_PIN_SET_DDR & ( ~(1 << DRIVER_CTRL_PIN));
#endif
}

