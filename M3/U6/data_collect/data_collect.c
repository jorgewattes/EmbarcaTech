#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/dma.h"
#include "hardware/timer.h"
#include "hardware/uart.h"

// Data will be copied from src to dst
const char src[] = "Hello, world! (from DMA)";
char dst[count_of(src)];

int64_t alarm_callback(alarm_id_t id, void *user_data) {
    // Put your timeout handler code in here
    return 0;
}

// UART defines
// By default the stdout UART is `uart0`, so we will use the second one
#define UART_ID uart1
#define BAUD_RATE 115200

// Use pins 4 and 5 for UART1
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define UART_TX_PIN 4
#define UART_RX_PIN 5

#define CLOCK_DIV 12000
#define CAPTURE_CHANNEL 0
#define LED_PIN 25
#define NSAMP 10000

uint16_t capture_buf[NSAMP];
float sending_buf[NSAMP];
char base64_output[NSAMP * 4];

static const char base64_chars[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

void base64_encode(const uint8_t *input, size_t input_len, char *output) {
    size_t i, j;
    for (i = 0, j = 0; i < input_len;) {
        uint32_t octet_a = i < input_len ? input[i++] : 0;
        uint32_t octet_b = i < input_len ? input[i++] : 0;
        uint32_t octet_c = i < input_len ? input[i++] : 0;

        uint32_t triple = (octet_a << 16) | (octet_b << 8) | octet_c;

        output[j++] = base64_chars[(triple >> 18) & 0x3F];
        output[j++] = base64_chars[(triple >> 12) & 0x3F];
        output[j++] = (i > input_len + 1) ? '=' : base64_chars[(triple >> 6) & 0x3F];
        output[j++] = (i > input_len) ? '=' : base64_chars[triple & 0x3F];
    }
    output[j] = '\0';
}

int main() {
    stdio_init_all();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    adc_gpio_init(26 + CAPTURE_CHANNEL);

    adc_init();
    adc_select_input(CAPTURE_CHANNEL);
    adc_fifo_setup(true, true, 1, false, false);

    adc_set_clkdiv(CLOCK_DIV);

    sleep_ms(1000);

    uint dma_chan = dma_claim_unused_channel(true);
    dma_channel_config cfg = dma_channel_get_default_config(dma_chan);

    channel_config_set_transfer_data_size(&cfg, DMA_SIZE_16);
    channel_config_set_read_increment(&cfg, false);
    channel_config_set_write_increment(&cfg, true);
    channel_config_set_dreq(&cfg, DREQ_ADC);

    while (1) {
        adc_fifo_drain();
        adc_run(false);

        dma_channel_configure(dma_chan, &cfg,
                              capture_buf,
                              &adc_hw->fifo,
                              NSAMP,
                              true);

        gpio_put(LED_PIN, 1);
        adc_run(true);

        dma_channel_wait_for_finish_blocking(dma_chan);

        uint16_t min = 32768, max = 0;
        for (uint32_t i = 0; i < NSAMP; i++) {
            if (capture_buf[i] > max) max = capture_buf[i];
            if (capture_buf[i] < min) min = capture_buf[i];
        }

        for (uint32_t i = 0; i < NSAMP; i++) {
            sending_buf[i] = ((float)capture_buf[i] - min) / (max - min) * 2 - 1;
        }

        base64_encode((uint8_t *)sending_buf, sizeof(sending_buf), base64_output);
        printf("%s", base64_output);

        gpio_put(LED_PIN, 0);
    }
}