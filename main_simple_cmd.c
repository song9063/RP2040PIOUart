
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "uart_tx.pio.h"
#include "uart_rx.pio.h"

#define PIO_TX_PIN 17
#define PIO_RX_PIN 16
#define RS485_EN 18

#if PIO_TX_PIN >= NUM_BANK0_GPIOS
#error Attempting to use a pin>=32 on a platform that does not support it
#endif

#if PIO_RX_PIN >= NUM_BANK0_GPIOS
#error Attempting to use a pin>=32 on a platform that does not support it
#endif

int main(){
    const uint SERIAL_BAUD = 115200;

    gpio_init(RS485_EN);
    gpio_set_dir(RS485_EN, GPIO_OUT);

    /* init PIO TX */
    PIO pio;
    uint sm;
    uint offset;
    bool success = pio_claim_free_sm_and_add_program_for_gpio_range(
        &uart_tx_program, &pio, &sm, &offset,
        PIO_TX_PIN, 1, true
    );
    hard_assert(success);
    uart_tx_program_init(pio, sm, offset, PIO_TX_PIN, SERIAL_BAUD);

    /* init PIO RX */
    PIO pioR;
    uint smR;
    uint offsetR;
    success = pio_claim_free_sm_and_add_program_for_gpio_range(
        &uart_rx_program, &pioR, &smR, &offsetR,
        PIO_RX_PIN, 1, true
    );
    hard_assert(success);
    uart_rx_program_init(pioR, smR, offsetR, PIO_RX_PIN, SERIAL_BAUD);

    while(true){
        char c = uart_rx_program_getc(pioR, smR);
        if(c == 'a'){
            gpio_put(RS485_EN, 1);
            sleep_ms(200);

            uart_tx_program_puts(pio, sm, "Hello, world! from PIO!\r\n");

            sleep_ms(200);
            gpio_put(RS485_EN, 0);
            sleep_ms(200);
        }else if(c == 'b'){
            gpio_put(RS485_EN, 1);
            sleep_ms(200);

            uart_tx_program_puts(pio, sm, "Hello, Linux!\r\n");

            sleep_ms(200);
            gpio_put(RS485_EN, 0);
            sleep_ms(200);
        }
        // sleep_ms(1000);
    }

    pio_remove_program_and_unclaim_sm(&uart_tx_program, pio, sm, offset);
}
