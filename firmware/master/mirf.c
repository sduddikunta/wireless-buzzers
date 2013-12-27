/*
	Copyright (c) Alex from Inside Gadgets (http://www.insidegadgets.com)
	
	mirf.c: functions for comunications with MiRF under Arduino-compatible IDE
*/

// Defines for setting the MiRF registers for transmitting or receiving mode
#define TX_POWERUP mirf_config_register(CONFIG, mirf_CONFIG | ( (1<<PWR_UP) | (0<<PRIM_RX) ) )
#define RX_POWERUP mirf_config_register(CONFIG, mirf_CONFIG | ( (1<<PWR_UP) | (1<<PRIM_RX) ) )

// Flag which denotes transmitting or receiving mode
volatile uint8_t PMODE;

// Write data using SPI
void spi_write_data(uint8_t * dataout, uint8_t len) {
  uint8_t i;
  for (i = 0; i < len; i++) {
    SPI.transfer(dataout[i]);
  }
}

// Read data using SPI
void spi_read_data(uint8_t * datain, uint8_t len) {
  uint8_t i;
  for (i = 0; i < len; i++) {
    datain[i] = SPI.transfer(0x00);
  }
}

// Initializes pins ans interrupt to communicate with the MiRF module
// Should be called in the early initializing phase at startup.
void mirf_init(void) {
  // Define CSN and CE as Output and set them to default
  pinMode (CSN, OUTPUT);
  pinMode (CE, OUTPUT);

  mirf_CE_lo;
  mirf_CSN_hi;
}

// Sets the important registers in the MiRF module and powers the module
// in receiving mode
void mirf_config(void) {
  // Set RF channel
  mirf_config_register(RF_CH, mirf_CH);

  // Set length of incoming payload 
  mirf_config_register(RX_PW_P0, mirf_PAYLOAD);

  // Set RADDR and TADDR
  mirf_write_register(RX_ADDR_P0, RADDR, 5);
  mirf_write_register(TX_ADDR, TADDR, 5);
  
  // Only enable RX_ADDR_P0 address matching
  mirf_config_register(EN_RXADDR, (1<<ERX_P0));
  mirf_config_register(EN_AA, 0x00);

  // Start receiver 
  PMODE = RXMODE; // Start in receiving mode
  RX_POWERUP;     // Power up in receiving mode
}

// Checks if data is available for reading
uint8_t mirf_data_ready(void) {
  mirf_CSN_lo;                                // Pull down chip select
  SPI.transfer(R_REGISTER | (REGISTER_MASK & STATUS));
  uint8_t status = SPI.transfer(0x00);        // Read status register
  mirf_CSN_hi;                                // Pull up chip select
  return status & (1<<RX_DR);
}

// Reads mirf_PAYLOAD bytes into data array
void mirf_get_data(uint8_t *data) {
  mirf_CSN_lo;                               // Pull down chip select
  SPI.transfer(R_RX_PAYLOAD);            // Send cmd to read rx payload
  spi_read_data(data, mirf_PAYLOAD); // Read payload
  mirf_CSN_hi;                               // Pull up chip select
  mirf_config_register(STATUS,(1<<RX_DR));   // Reset status register
}

// Write one byte into the given MiRF register
void mirf_config_register(uint8_t reg, uint8_t value) {
  mirf_CSN_lo;
  SPI.transfer(W_REGISTER | (REGISTER_MASK & reg));
  SPI.transfer(value);
  mirf_CSN_hi;
}

// Reads an array of bytes from the given start position in the MiRF registers.
void mirf_read_register(uint8_t reg, uint8_t *value, uint8_t len) {
  mirf_CSN_lo;
  SPI.transfer(R_REGISTER | (REGISTER_MASK & reg));
  spi_read_data(value, len);
  mirf_CSN_hi;
}

// Writes an array of bytes into inte the MiRF registers
void mirf_write_register(uint8_t reg, uint8_t *value, uint8_t len) {
  mirf_CSN_lo;
  SPI.transfer(W_REGISTER | (REGISTER_MASK & reg));
  spi_write_data(value, len);
  mirf_CSN_hi;
}

void mirf_send(uint8_t *value, uint8_t len) {

	PMODE = TXMODE; // Set to transmitter mode

	TX_POWERUP; // Power up
	_delay_ms(5); // wait for transmitter to come up



	mirf_CSN_lo; // Pull down chip select

	SPI.transfer(FLUSH_TX);  // Write cmd to flush tx fifo

	mirf_CSN_hi; // Pull up chip select



	mirf_CSN_lo;  // Pull down chip select

	SPI.transfer(W_TX_PAYLOAD); // Write cmd to write payload

	spi_write_data(value, len); // Write payload

	mirf_CSN_hi; // Pull up chip select



	mirf_CE_hi; // Start transmission

	_delay_us(15);

	mirf_CE_lo;

}
