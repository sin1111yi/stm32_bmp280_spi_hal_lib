/* declare SPI NCS GPIO Structure */
typedef struct __NCS_IO
{
    GPIO_TypeDef *port;
    uint16_t pin;
} ncs_io;

/* declare databuf and loop control var */

__attribute((aligned(8))) uint8_t spiDataBuf[SPI_DATABUF_SIZE] = {0}; // all read data will store in this array
static uint16_t i = 0;                                                // loop control

/*** basic spi operate ***/
/*** I want to make these functions can be reused ***/
/*
 * @brief   write a byte through SPI and read feedback
 * @param   byte: byte to write
 * @return  received byte
 * */
uint8_t spi_wr_byte(SPI_HandleTypeDef *hspi, uint8_t byte)
{
    uint8_t feedback = 0;

    // wait SPI serial free
    while (HAL_SPI_GetState(hspi) == HAL_SPI_STATE_BUSY_TX_RX)
        ;

    if (HAL_SPI_TransmitReceive(hspi, &byte, &feedback, 1, 0x0100) != HAL_OK)
    {
        return 0xff;
    }

    return feedback;
}
/*
 * @brief   write several bytes through SPI
 * @param   address: address of the first reg
 * @param   bytes: bytes to write
 * @param   num: number of bytes
 * */
void spi_w_bytes(SPI_HandleTypeDef *hspi, uint8_t address, uint8_t *bytes,
                 uint16_t num, ncs_io cs)
{

    HAL_GPIO_WritePin(cs.port, cs.pin, GPIO_PIN_RESET);

    spi_wr_byte(hspi, address);
    for (i = 0; i < num; i++)
        spi_wr_byte(hspi, bytes[i]);

    HAL_GPIO_WritePin(cs.port, cs.pin, GPIO_PIN_SET);
}

/*
 * @brief   read several bytes through SPI
 *          to get the data, check spiDataBuf[0...num-1]
 * @param   address: address of the first reg
 * @param   num: number of bytes to read, number < DATABUF_SIZ
 * @return  data read array
 * */
void spi_r_bytes(SPI_HandleTypeDef *hspi, uint8_t address, uint8_t num,
                 ncs_io cs)
{
    uint8_t _address = address | 0x80;

    HAL_GPIO_WritePin(cs.port, cs.pin, GPIO_PIN_RESET);

    // may be can use HAL_SPI_TransmitReceive()
    HAL_SPI_Transmit(hspi, &_address, 1, 0x0100);
    HAL_SPI_Receive(hspi, spiDataBuf, num, 0x0100); // store read data to spiDataBuf

    HAL_GPIO_WritePin(cs.port, cs.pin, GPIO_PIN_SET);
}

/*
 * @brief   read a byte through SPI
 * @param   address: address of the first reg
 * */
void spi_w_byte(SPI_HandleTypeDef *hspi, uint8_t address, uint8_t byte,
                ncs_io cs)
{

    HAL_GPIO_WritePin(cs.port, cs.pin, GPIO_PIN_RESET);

    spi_wr_byte(hspi, address);
    spi_wr_byte(hspi, byte);

    HAL_GPIO_WritePin(cs.port, cs.pin, GPIO_PIN_SET);
}

/*
 * @brief   read a byte through SPI
 * @param   address: address of the first reg
 * @return  data read out
 * */
uint8_t spi_r_byte(SPI_HandleTypeDef *hspi, uint8_t address, ncs_io cs)
{
    uint8_t _address = address | 0x80;
    HAL_GPIO_WritePin(cs.port, cs.pin, GPIO_PIN_RESET);

    HAL_SPI_Transmit(hspi, &_address, 1, 0x0100);
    HAL_SPI_Receive(hspi, spiDataBuf, 1, 0x0100); // store read data to spiDataBuf

    HAL_GPIO_WritePin(cs.port, cs.pin, GPIO_PIN_SET);
    return spiDataBuf[0];
}