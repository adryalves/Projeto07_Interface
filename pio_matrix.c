#include <stdio.h>
#include "pico/stdlib.h"

#include <math.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
#include "pico/bootrom.h"

#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include "inc/font.h"

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C
// arquivo .pio
#include "pio_matrix.pio.h"

// número de LEDs
#define NUM_PIXELS 25

// pino de saída
#define OUT_PIN 7

// botão de interupção
const uint button_A = 5;
const uint button_B = 6;

const uint led_Verde = 11;
const uint led_Azul = 12;
const uint ledA_pin = 13; // Red => GPIO13

static volatile uint numero_atual = 0;
static volatile uint32_t last_time = 0; // Armazena o tempo do último evento (em microssegundos)

static void gpio_irq_handler(uint gpio, uint32_t events);

// vetores pra criação dos numeros
double numero0[NUM_PIXELS] = {0.7, 0.7, 0.7, 0.7, 0.7,
                              0.7, 0.0, 0.0, 0.0, 0.7,
                              0.7, 0.0, 0.0, 0.0, 0.7,
                              0.7, 0.0, 0.0, 0.0, 0.7,
                              0.7, 0.7, 0.7, 0.7, 0.7};

double numero1[NUM_PIXELS] = {0.0, 0.0, 0.7, 0.0, 0.0,
                              0.0, 0.0, 0.7, 0.7, 0.0,
                              0.7, 0.0, 0.7, 0.0, 0.0,
                              0.0, 0.0, 0.7, 0.0, 0.0,
                              0.7, 0.7, 0.7, 0.7, 0.7};

double numero2[NUM_PIXELS] = {0.7, 0.7, 0.7, 0.7, 0.7,
                              0.7, 0.0, 0.0, 0.0, 0.0,
                              0.7, 0.7, 0.7, 0.7, 0.7,
                              0.0, 0.0, 0.0, 0.0, 0.7,
                              0.7, 0.7, 0.7, 0.7, 0.7};

double numero3[NUM_PIXELS] = {0.7, 0.7, 0.7, 0.7, 0.7,
                              0.7, 0.0, 0.0, 0.0, 0.0,
                              0.7, 0.7, 0.7, 0.7, 0.7,
                              0.7, 0.0, 0.0, 0.0, 0.0,
                              0.7, 0.7, 0.7, 0.7, 0.7};

double numero4[NUM_PIXELS] = {0.7, 0.0, 0.0, 0.0, 0.7,
                              0.7, 0.0, 0.0, 0.0, 0.7,
                              0.7, 0.7, 0.7, 0.7, 0.7,
                              0.7, 0.0, 0.0, 0.0, 0.0,
                              0.0, 0.0, 0.0, 0.0, 0.7};

double numero5[NUM_PIXELS] = {0.7, 0.7, 0.7, 0.7, 0.7,
                              0.0, 0.0, 0.0, 0.0, 0.7,
                              0.7, 0.7, 0.7, 0.7, 0.7,
                              0.7, 0.0, 0.0, 0.0, 0.0,
                              0.7, 0.7, 0.7, 0.7, 0.7};

double numero6[NUM_PIXELS] = {0.7, 0.0, 0.0, 0.0, 0.0,
                              0.0, 0.0, 0.0, 0.0, 0.7,
                              0.7, 0.7, 0.7, 0.7, 0.7,
                              0.7, 0.0, 0.0, 0.0, 0.7,
                              0.7, 0.7, 0.7, 0.7, 0.7};

double numero7[NUM_PIXELS] = {0.7, 0.7, 0.7, 0.7, 0.7,
                              0.7, 0.0, 0.0, 0.0, 0.0,
                              0.0, 0.0, 0.0, 0.0, 0.7,
                              0.7, 0.0, 0.0, 0.0, 0.0,
                              0.0, 0.0, 0.0, 0.0, 0.7};

double numero8[NUM_PIXELS] = {0.7, 0.7, 0.7, 0.7, 0.7,
                              0.7, 0.0, 0.0, 0.0, 0.7,
                              0.7, 0.7, 0.7, 0.7, 0.7,
                              0.7, 0.0, 0.0, 0.0, 0.7,
                              0.7, 0.7, 0.7, 0.7, 0.7};

double numero9[NUM_PIXELS] = {0.7, 0.7, 0.7, 0.7, 0.7,
                              0.7, 0.0, 0.0, 0.0, 0.7,
                              0.7, 0.7, 0.7, 0.7, 0.7,
                              0.7, 0.0, 0.0, 0.0, 0.0,
                              0.7, 0.7, 0.7, 0.7, 0.7};

// rotina para definição da intensidade de cores do led
uint32_t matrix_rgb(double b, double r, double g)
{
    unsigned char R, G, B;
    R = r * 255;
    G = g * 255;
    B = b * 255;
    return (G << 24) | (R << 16) | (B << 8);
}

// rotina para acionar a matrix de leds - ws2812b
void desenho_pio(double *desenho, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b)
{

    for (int16_t i = 0; i < NUM_PIXELS; i++)
    {
        if (i % 2 == 0)
        {
            valor_led = matrix_rgb(desenho[24 - i], r = 0.0, g = 0.0);
            pio_sm_put_blocking(pio, sm, valor_led);
        }
        else
        {
            valor_led = matrix_rgb(b = 0.0, desenho[24 - i], g = 0.0);
            pio_sm_put_blocking(pio, sm, valor_led);
        }
    }
}

void desenharNumero(uint numero, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b)
{

    switch (numero)
    {
    case 0:
        desenho_pio(numero0, valor_led, pio, sm, r, g, b);
        break;
    case 1:
        desenho_pio(numero1, valor_led, pio, sm, r, g, b);
        break;
    case 2:
        desenho_pio(numero2, valor_led, pio, sm, r, g, b);
        break;
    case 3:
        desenho_pio(numero3, valor_led, pio, sm, r, g, b);
        break;
        ;
    case 4:
        desenho_pio(numero4, valor_led, pio, sm, r, g, b);
        break;
    case 5:
        desenho_pio(numero5, valor_led, pio, sm, r, g, b);
        break;
    case 6:
        desenho_pio(numero6, valor_led, pio, sm, r, g, b);
        break;
    case 7:
        desenho_pio(numero7, valor_led, pio, sm, r, g, b);
        break;
    case 8:
        desenho_pio(numero8, valor_led, pio, sm, r, g, b);
        break;
    case 9:
        desenho_pio(numero9, valor_led, pio, sm, r, g, b);
        break;
    }
}

void atualizar_display(char *mensagem)
{
    i2c_init(I2C_PORT, 400 * 1000);

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);                    // Set the GPIO pin function to I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);                    // Set the GPIO pin function to I2C
    gpio_pull_up(I2C_SDA);                                        // Pull up the data line
    gpio_pull_up(I2C_SCL);                                        // Pull up the clock line
    ssd1306_t ssd;                                                // Inicializa a estrutura do display
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
    ssd1306_config(&ssd);                                         // Configura o display
    ssd1306_send_data(&ssd);                                      // Envia os dados para o display

    // Limpa o display. O display inicia com todos os pixels apagados.
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    ssd1306_draw_string(&ssd, mensagem, 8, 10);
    ssd1306_send_data(&ssd); // Atualiza o display
}

int main()
{
    stdio_init_all();
    char input[2]; // Buffer para armazenar a entrada do usuário
    PIO pio = pio0;
    bool ok;
    uint16_t i;
    uint32_t valor_led;
    double r = 0.0, b = 0.0, g = 0.0;

    // coloca a frequência de clock para 128 MHz, facilitando a divisão pelo clock
    ok = set_sys_clock_khz(128000, false);

    printf("iniciando a transmissão PIO");
    if (ok)
        printf("clock set to %ld\n", clock_get_hz(clk_sys));

    // configurações da PIO
    uint offset = pio_add_program(pio, &pio_matrix_program);
    uint sm = pio_claim_unused_sm(pio, true);
    pio_matrix_program_init(pio, sm, offset, OUT_PIN);

    // inicializar o botão de interrupção - GPIO5
    gpio_init(button_A);
    gpio_set_dir(button_A, GPIO_IN);
    gpio_pull_up(button_A);

    // inicializar o botão de interrupção - GPIO5
    gpio_init(button_B);
    gpio_set_dir(button_B, GPIO_IN);
    gpio_pull_up(button_B);

    // interrupção da gpio habilitada
    gpio_set_irq_enabled_with_callback(button_A, GPIO_IRQ_EDGE_FALL, 1, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(button_B, GPIO_IRQ_EDGE_FALL, 1, &gpio_irq_handler);

    // inicializar o LED verde - GPIO13
    gpio_init(led_Verde);              // Inicializa o pino do LED
    gpio_set_dir(led_Verde, GPIO_OUT); // Configura o pino como saída

    // inicializar o LED verde - GPIO13
    gpio_init(led_Azul);              // Inicializa o pino do LED
    gpio_set_dir(led_Azul, GPIO_OUT); // Configura o pino como saída

    // inicializacao do display

    i2c_init(I2C_PORT, 400 * 1000);

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);                    // Set the GPIO pin function to I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);                    // Set the GPIO pin function to I2C
    gpio_pull_up(I2C_SDA);                                        // Pull up the data line
    gpio_pull_up(I2C_SCL);                                        // Pull up the clock line
    ssd1306_t ssd;                                                // Inicializa a estrutura do display
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
    ssd1306_config(&ssd);                                         // Configura o display
    ssd1306_send_data(&ssd);                                      // Envia os dados para o display

    // Limpa o display. O display inicia com todos os pixels apagados.
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    while (true)
    {

        int c = getchar_timeout_us(100000);
        if (c != PICO_ERROR_TIMEOUT)
        {
            ssd1306_draw_char(&ssd, c, 8, 10);
            ssd1306_send_data(&ssd);

            if (c >= '0' && c <= '9')
            {
                uint numero = c - '0';
                desenharNumero(numero, valor_led, pio, sm, r, g, b);
            }
        }
        sleep_ms(100);
    }
}

// rotina da interrupção
static void gpio_irq_handler(uint gpio, uint32_t events)
{
    uint32_t current_time = to_us_since_boot(get_absolute_time());

    if (current_time - last_time > 200000) // 200 ms de debouncing
    {
        last_time = current_time;
        if (gpio == button_A) // Se for o primeiro botão
        {
            bool estado_atual = gpio_get(led_Verde); // Obtém o estado atual
            gpio_put(led_Verde, !estado_atual);      // Alterna o estado
            char msg[32];
            sprintf(msg, "LED Verde: %s", estado_atual ? "Desligado" : "Ligado");
            atualizar_display(msg);
            printf("Botão A pressionado. LED Verde %s\n", estado_atual ? "desligado" : "ligado");
        }
        else if (gpio == button_B) // Se for o segundo botão
        {
            bool estado_atual = gpio_get(led_Azul); // Obtém o estado atual
            gpio_put(led_Azul, !estado_atual);      // Alterna o estado
            char msg[32];
            sprintf(msg, "LED Azul: %s", estado_atual ? "Desligado" : "Ligado");
            atualizar_display(msg);
            printf("Botão B pressionado. LED Azul %s\n", estado_atual ? "desligado" : "ligado");
        }
    }
}