/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# "Insert system clock initialization code here" comment
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
 /**
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include "onepos.h"

#define FALSE 0
#define TRUE 1
 
#define USART_RS485						 &USARTE0
#define USART_SERIAL_BAUDRATE            115200
#define USART_SERIAL_CHAR_LENGTH         USART_CHSIZE_8BIT_gc
#define USART_SERIAL_PARITY              USART_PMODE_DISABLED_gc
#define USART_SERIAL_STOP_BIT            false





#ifdef DWT_DS_TWR_RESP2
	/* Default communication configuration. We use here EVK1000's default mode (mode 3). */
	static dwt_config_t config = {
		2,               /* Channel number. */
		DWT_PRF_64M,     /* Pulse repetition frequency. */
		DWT_PLEN_1024,   /* Preamble length. Used in TX only. */
		DWT_PAC32,       /* Preamble acquisition chunk size. Used in RX only. */
		9,               /* TX preamble code. Used in TX only. */
		9,               /* RX preamble code. Used in RX only. */
		1,               /* 0 to use standard SFD, 1 to use non-standard SFD. */
		DWT_BR_110K,     /* Data rate. */
		DWT_PHRMODE_STD, /* PHY header mode. */
		(1025 + 64 - 32) /* SFD timeout (preamble length + 1 + SFD length - PAC size). Used in RX only. */
	};

	/* Default antenna delay values for 64 MHz PRF. See NOTE 1 below. */
	#define TX_ANT_DLY 16436
	#define RX_ANT_DLY 16436

	/* Frames used in the ranging process. See NOTE 2 below. */
	static uint8 rx_poll_msg[] = {0x41, 0x88, 0, 0xCA, 0xDE, 'W', 'A', 'V', 'E', 0x21, 0, 0};
	static uint8 tx_resp_msg[] = {0x41, 0x88, 0, 0xCA, 0xDE, 'V', 'E', 'W', 'A', 0x10, 0x02, 0, 0, 0, 0};
	static uint8 rx_final_msg[] = {0x41, 0x88, 0, 0xCA, 0xDE, 'W', 'A', 'V', 'E', 0x23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	/* Length of the common part of the message (up to and including the function code, see NOTE 2 below). */
	#define ALL_MSG_COMMON_LEN 10
	/* Index to access some of the fields in the frames involved in the process. */
	#define ALL_MSG_SN_IDX 2
	#define FINAL_MSG_POLL_TX_TS_IDX 10
	#define FINAL_MSG_RESP_RX_TS_IDX 14
	#define FINAL_MSG_FINAL_TX_TS_IDX 18
	#define FINAL_MSG_TS_LEN 4
	/* Frame sequence number, incremented after each transmission. */
	static uint8 frame_seq_nb = 0;

	/* Buffer to store received messages.
	 * Its size is adjusted to longest frame that this example code is supposed to handle. */
	#define RX_BUF_LEN 24
	static uint8 rx_buffer[RX_BUF_LEN];

	/* Hold copy of status register state here for reference so that it can be examined at a debug breakpoint. */
	static uint32 status_reg = 0;

	/* UWB microsecond (uus) to device time unit (dtu, around 15.65 ps) conversion factor.
	 * 1 uus = 512 / 499.2 �s and 1 �s = 499.2 * 128 dtu. */
	#define UUS_TO_DWT_TIME 65536

	/* Delay between frames, in UWB microseconds. See NOTE 4 below. */
	/* This is the delay from Frame RX timestamp to TX reply timestamp used for calculating/setting the DW1000's delayed TX function. This includes the
	 * frame length of approximately 2.46 ms with above configuration. */
	#define POLL_RX_TO_RESP_TX_DLY_UUS 3900
	/* This is the delay from the end of the frame transmission to the enable of the receiver, as programmed for the DW1000's wait for response feature. */
	#define RESP_TX_TO_FINAL_RX_DLY_UUS 500
	/* Receive final timeout. See NOTE 5 below. */
	#define FINAL_RX_TIMEOUT_UUS 3300
	/* Preamble timeout, in multiple of PAC size. See NOTE 6 below. */
	#define PRE_TIMEOUT 8

	/* Timestamps of frames transmission/reception.
	 * As they are 40-bit wide, we need to define a 64-bit int type to handle them. */
	static uint64_t poll_rx_ts;
	static uint64_t resp_tx_ts;
	static uint64_t final_rx_ts;

	/* Speed of light in air, in metres per second. */
	#define SPEED_OF_LIGHT 299702547

	/* Hold copies of computed time of flight and distance here for reference so that it can be examined at a debug breakpoint. */
	static double tof;
	static double distance;

	/* String used to display measured distance on LCD screen (16 characters maximum). */
	char dist_str[16] = {0};

	/* Declaration of static functions. */
	static uint64_t get_tx_timestamp_u64(void);
	static uint64_t get_rx_timestamp_u64(void);
	static void final_msg_get_ts(const uint8 *ts_field, uint32 *ts);
	
		/*! ------------------------------------------------------------------------------------------------------------------
	 * @fn get_tx_timestamp_u64()
	 *
	 * @brief Get the TX time-stamp in a 64-bit variable.
	 *        /!\ This function assumes that length of time-stamps is 40 bits, for both TX and RX!
	 *
	 * @param  none
	 *
	 * @return  64-bit value of the read time-stamp.
	 */
	static uint64_t get_tx_timestamp_u64(void)
	{
		uint8 ts_tab[5];
		uint64_t ts = 0;
		int i;
		dwt_readtxtimestamp(ts_tab);
		for (i = 4; i >= 0; i--)
		{
			ts <<= 8;
			ts |= (uint64_t)(ts_tab[i]);
		}
		return ts;
	}

	/*! ------------------------------------------------------------------------------------------------------------------
	 * @fn get_rx_timestamp_u64()
	 *
	 * @brief Get the RX time-stamp in a 64-bit variable.
	 *        /!\ This function assumes that length of time-stamps is 40 bits, for both TX and RX!
	 *
	 * @param  none
	 *
	 * @return  64-bit value of the read time-stamp.
	 */
	static uint64_t get_rx_timestamp_u64(void)
	{
		uint8 ts_tab[5];
		uint64_t ts = 0;
		int i;
		dwt_readrxtimestamp(ts_tab);
		for (i = 4; i >= 0; i--)
		{
			ts <<= 8;
			ts |= (uint64_t)(ts_tab[i]);
		}
		return ts;
	}

	/*! ------------------------------------------------------------------------------------------------------------------
	 * @fn final_msg_get_ts()
	 *
	 * @brief Read a given timestamp value from the final message. In the timestamp fields of the final message, the least
	 *        significant byte is at the lower address.
	 *
	 * @param  ts_field  pointer on the first byte of the timestamp field to read
	 *         ts  timestamp value
	 *
	 * @return none
	 */
	static void final_msg_get_ts(const uint8 *ts_field, uint32 *ts)
	{
		int i;
		*ts = 0;
		for (i = 0; i < FINAL_MSG_TS_LEN; i++)
		{
			*ts += (uint64_t)(ts_field[i]) << (i * 8);
		}
	}
	
	void dwt_ds_twr_resp(void)
	{
		if (dwt_initialise(DWT_LOADUCODE) == DWT_ERROR)
		{
			printf("INIT FAILED");
			while (1)
			{ };
		}
		fast_spi();

		/* Configure DW1000. See NOTE 7 below. */
		dwt_configure(&config);
		dwt_setleds(DWT_LEDS_ENABLE|DWT_LEDS_INIT_BLINK);
	
		/* Apply default antenna delay value. See NOTE 1 below. */
		dwt_setrxantennadelay(RX_ANT_DLY);
		dwt_settxantennadelay(TX_ANT_DLY);

		/* Set preamble timeout for expected frames. See NOTE 6 below. */
		dwt_setpreambledetecttimeout(PRE_TIMEOUT);

		/* Loop forever responding to ranging requests. */
		while (1)
		{
			/* Clear reception timeout to start next ranging process. */
			dwt_setrxtimeout(0);

			/* Activate reception immediately. */
			dwt_rxenable(DWT_START_RX_IMMEDIATE);

			/* Poll for reception of a frame or error/timeout. See NOTE 8 below. */
			while (!((status_reg = dwt_read32bitreg(SYS_STATUS_ID)) & (SYS_STATUS_RXFCG | SYS_STATUS_ALL_RX_TO | SYS_STATUS_ALL_RX_ERR)))
			{led4_toogle(); };

			if (status_reg & SYS_STATUS_RXFCG)
			{
				uint32 frame_len;

				/* Clear good RX frame event in the DW1000 status register. */
				dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_RXFCG);

				/* A frame has been received, read it into the local buffer. */
				frame_len = dwt_read32bitreg(RX_FINFO_ID) & RX_FINFO_RXFL_MASK_1023;
				if (frame_len <= RX_BUFFER_LEN)
				{
					dwt_readrxdata(rx_buffer, frame_len, 0);
					printf("%s\n",rx_buffer);
				}

				/* Check that the frame is a poll sent by "DS TWR initiator" example.
				 * As the sequence number field of the frame is not relevant, it is cleared to simplify the validation of the frame. */
				rx_buffer[ALL_MSG_SN_IDX] = 0;
				
				printf("Step 0\n");
				
				if (memcmp(rx_buffer, rx_poll_msg, ALL_MSG_COMMON_LEN) == 0)
				{
					printf("Step 1\n");
					uint32 resp_tx_time;
					int ret;

					/* Retrieve poll reception timestamp. */
					poll_rx_ts = get_rx_timestamp_u64();

					/* Set send time for response. See NOTE 9 below. */
					resp_tx_time = (uint64_t)(poll_rx_ts + (POLL_RX_TO_RESP_TX_DLY_UUS * UUS_TO_DWT_TIME)) >> 8;
					dwt_setdelayedtrxtime(resp_tx_time); 
					
					printf("%lX%lX vs %lX%lX \n", (uint32_t)(poll_rx_ts >> 32), (uint32_t)(poll_rx_ts),(uint32_t)(resp_tx_time >> 32),(uint32_t)(resp_tx_time));
					
					/* Set expected delay and timeout for final message reception. See NOTE 4 and 5 below. */
					dwt_setrxaftertxdelay(RESP_TX_TO_FINAL_RX_DLY_UUS);  
					dwt_setrxtimeout(FINAL_RX_TIMEOUT_UUS); 

					/* Write and send the response message. See NOTE 10 below.*/
					tx_resp_msg[ALL_MSG_SN_IDX] = frame_seq_nb;
					dwt_writetxdata(sizeof(tx_resp_msg), tx_resp_msg, 0); /* Zero offset in TX buffer. */
					dwt_writetxfctrl(sizeof(tx_resp_msg), 0, 1); /* Zero offset in TX buffer, ranging. */
					
					ret = dwt_starttx(DWT_START_TX_DELAYED | DWT_RESPONSE_EXPECTED);
				
					/* If dwt_starttx() returns an error, abandon this ranging exchange and proceed to the next one. See NOTE 11 below. */
					if (ret == DWT_ERROR)
					{
						printf("DWT_ERROR\n");
						continue;
					}

					/* Poll for reception of expected "final" frame or error/timeout. See NOTE 8 below. */
					while (!((status_reg = dwt_read32bitreg(SYS_STATUS_ID)) & (SYS_STATUS_RXFCG | SYS_STATUS_ALL_RX_TO | SYS_STATUS_ALL_RX_ERR)))
					{ 
						led1_toogle();
					}
					printf("Step 2\n");
					/* Increment frame sequence number after transmission of the response message (modulo 256). */
					frame_seq_nb++;

					if (status_reg & SYS_STATUS_RXFCG)
					{
						printf("Step 3\n");
						led2_toogle();
						/* Clear good RX frame event and TX frame sent in the DW1000 status register. */
						dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_RXFCG | SYS_STATUS_TXFRS);

						/* A frame has been received, read it into the local buffer. */
						frame_len = dwt_read32bitreg(RX_FINFO_ID) & RX_FINFO_RXFLEN_MASK;
						if (frame_len <= RX_BUF_LEN)
						{
							dwt_readrxdata(rx_buffer, frame_len, 0);
							printf("%s\n",rx_buffer);
						}

						/* Check that the frame is a final message sent by "DS TWR initiator" example.
						 * As the sequence number field of the frame is not used in this example, it can be zeroed to ease the validation of the frame. */
						rx_buffer[ALL_MSG_SN_IDX] = 0;
						if (memcmp(rx_buffer, rx_final_msg, ALL_MSG_COMMON_LEN) == 0)
						{
							led3_toogle();
							uint32 poll_tx_ts, resp_rx_ts, final_tx_ts;
							uint32 poll_rx_ts_32, resp_tx_ts_32, final_rx_ts_32;
							double Ra, Rb, Da, Db;
							int64_t tof_dtu;

							/* Retrieve response transmission and final reception timestamps. */
							resp_tx_ts = get_tx_timestamp_u64();
							final_rx_ts = get_rx_timestamp_u64();

							/* Get timestamps embedded in the final message. */
							final_msg_get_ts(&rx_buffer[FINAL_MSG_POLL_TX_TS_IDX], &poll_tx_ts);
							final_msg_get_ts(&rx_buffer[FINAL_MSG_RESP_RX_TS_IDX], &resp_rx_ts);
							final_msg_get_ts(&rx_buffer[FINAL_MSG_FINAL_TX_TS_IDX], &final_tx_ts);

							/* Compute time of flight. 32-bit subtractions give correct answers even if clock has wrapped. See NOTE 12 below. */
							poll_rx_ts_32 = (uint32)poll_rx_ts;
							resp_tx_ts_32 = (uint32)resp_tx_ts;
							final_rx_ts_32 = (uint32)final_rx_ts;
							Ra = (double)(resp_rx_ts - poll_tx_ts);
							Rb = (double)(final_rx_ts_32 - resp_tx_ts_32);
							Da = (double)(final_tx_ts - resp_rx_ts);
							Db = (double)(resp_tx_ts_32 - poll_rx_ts_32);
							tof_dtu = (int64_t)((Ra * Rb - Da * Db) / (Ra + Rb + Da + Db));

							tof = tof_dtu * DWT_TIME_UNITS;
							distance = tof * SPEED_OF_LIGHT;

							/* Display computed distance on LCD. */
							printf("DIST: %d mm\n", (uint16_t)(distance*100));
						}
					}
					else
					{
						printf("Step 3 failed\n");
						/* Clear RX error/timeout events in the DW1000 status register. */
						dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_ALL_RX_TO | SYS_STATUS_ALL_RX_ERR);

						/* Reset RX to properly reinitialise LDE operation. */
						dwt_rxreset();
					}
				}
			}
			else
			{
				/* Clear RX error/timeout events in the DW1000 status register. */
				dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_ALL_RX_TO | SYS_STATUS_ALL_RX_ERR);

				/* Reset RX to properly reinitialise LDE operation. */
				dwt_rxreset();
			}
		}
		}
	
#endif //end #ifdef DWT_DS_TWR_RESP




void rs485_send(char a)
{
	
}



void test_endian(void)
{
	union e {
		unsigned long int var;
		unsigned char array[4];
	}e;
	
	e.array[0] = 0x0D;
	e.array[1] = 0x0C;
	e.array[2] = 0x0B;
	e.array[3] = 0x0A;
	
	if ( e.var == 0x0A0B0C0D )
	{
		printf("Little-endian\n\r");
	}
	else if (e.var == 0x0D0C0B0A)
	{
		printf("Big-endian\n\r");
	}
	else
	{
		printf("Middle-endian or unknown storage type. Variable= %x", e.var);
	}
}

int main (void)
{
	init_onepos();
	
	
	static usart_rs232_options_t RS485_SERIAL_OPTIONS = {
		.baudrate = USART_SERIAL_BAUDRATE,
		.charlength = USART_SERIAL_CHAR_LENGTH,
		.paritytype = USART_SERIAL_PARITY,
		.stopbits = USART_SERIAL_STOP_BIT
	};
	usart_serial_init(USART_RS485, &RS485_SERIAL_OPTIONS);
	
	init_animation();
	
	uint8_t ret;
	printf("READ_CFG: %d. MEMCHECK = %x\n",ret,onepos_get_mem_check());
	ret = onepos_read_cfg();
	
	if (sw2_status())
	{
		onepos_configure_interface();
	}
	
	uint8_t status;
	//dwt_show_sys_info();
	
	
	//dwt_run_examples();

	
	//Test of sending messages with library D:
	dwt_onepos_init(1);
	for (;;)
	{
		status = dwt_send_msg_w_ack("You got a message",1);
		delay_ms(1000);
		
		//status = dwt_receive_msg_w_ack();
		
		if (status == UWB_RX_OK)
		{
			printf("SUCCESS!!!\n");
		}
	}
	
	
	for (;;)
	{
		led1_toogle();
		delay_ms(500);
	}
	
	for(;;) //USB-USART_BLE bridge
	{
		if (usb_is_rx_ready())
		{
			usart_putchar(USART_BLE, usb_getchar());
			led1_toogle();
		}
		if (usart_rx_is_complete(USART_BLE))
		{
			usb_putchar(usart_getchar(USART_BLE));
			led2_toogle();
		}
		
	}
	
	uint32_t dev_id;
	
	for (;;) //spi test
	{
		dev_id = dwt_readdevid();
		printf("%#04X %#04X\n", (uint16_t)(dev_id>>16),(uint16_t)dev_id);
		led1_toogle();
		delay_ms(1000);
	}
	
	
	
	
	for (;;) //Basic USB printf example
	{
		printf("hello!\n");
		delay_ms(500);
	}
}
