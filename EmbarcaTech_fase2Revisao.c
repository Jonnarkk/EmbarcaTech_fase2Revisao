#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/irq.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "pico/bootrom.h"
#include "./display_files/ssd1306.h"
#include "./headers/joystick.h"
#include "./ledMatriz_files/led_matriz.h"
#include "pio_matriz.pio.h"

// Defines
#define joyX 27
#define joyY 26
#define I2C_LINK 0x3C
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define BOTAO_A 5
#define BOTAO_B 6
#define BOTAO_J 22
#define LED_G 11
#define LED_B 12
#define LED_R 13
#define BUZZER 10
#define DIV 25.0
#define WRAP 1800

// Variáveis globais
uint DUTY_C = 0;
uint sliceB; // Variável para o slice do LED azul
uint sliceR; // Variável para o slice do LED vermelho
uint CONTADOR = 0; // Contador para mudar o estilo da borda
uint32_t last_print_time = 0;
uint32_t last_timeA = 0; // Guarda a última vez que o botão A foi pressionado
uint32_t last_timeJ = 0; // Guarda a última vez que o botão Joystick foi pressionado
ssd1306_t ssd;
bool cor = true;
volatile bool pwm_function = true; // Variável que permite o joystick controlar intensidade dos LEDs
volatile bool ledg_state = false; // Indica estado atual do LED verde
volatile bool botao_state = false; // Estado do botão do joystick

void gpio_irq_handler(uint gpio, uint32_t events) {
    // Variáveis referentes a matriz de LED's
    PIO pio = pio0;
    uint sm = 0;
    uint offset = pio_add_program(pio, &pio_matriz_program);
    pio_matriz_program_init(pio, sm, offset, pino_matriz);

    
    if (gpio == BOTAO_B) {
        printf("Reiniciando a placa para modo de gravação...\n");
        reset_usb_boot(0, 0);
    }

    uint32_t current_time = to_us_since_boot(get_absolute_time());

    if (current_time - last_timeA > 200000) { // Debounce de 200ms
        last_timeA = current_time;

        if (gpio == BOTAO_A) {
            pwm_function = !pwm_function;
            pwm_set_enabled(sliceB, pwm_function);
            pwm_set_enabled(sliceR, pwm_function);
            printf("ESTADO: %s\n", pwm_function ? "PWM ATIVADO" : "PWM DESATIVADO");
            limpar_todos_leds(pio, sm);
        }
    }

    if (current_time - last_timeJ > 200000) { // Debounce de 200ms
        last_timeJ = current_time;

        if (gpio == BOTAO_J) {
            botao_state = !botao_state; // Alterna o estado do botão
            ledg_state = botao_state; // Sincroniza o estado do LED com o botão
            gpio_put(LED_G, ledg_state); // Atualiza o LED verde

            // Incrementa o contador e garante que não passa de 3
            CONTADOR = (CONTADOR + 1) % 3;

            printf("ESTADO: %s | BORDA: %d\n", ledg_state ? "LED VERDE ATIVADO" : "LED VERDE DESATIVADO", CONTADOR);

            limpar_todos_leds(pio, sm);
        }
    }

}

void setup_inicial(){
    // Inicializar ADC no joystick
    joystick_adc_init(joyX, joyY);

    // Inicializa o I2C, configura a estrutura do display e limpa o display
    ssd1306_init_config_clean(&ssd, I2C_SCL, I2C_SDA, I2C_PORT, I2C_LINK);

    // Configura o PWM dos LED's vermelho e azul
    sliceB = gpio_pwm_config(LED_B, DIV, WRAP, DUTY_C);
    sliceR = gpio_pwm_config(LED_R, DIV, WRAP, DUTY_C);
    pwm_set_enabled(sliceB, pwm_function);
    pwm_set_enabled(sliceR, pwm_function);

    // Inicializa o PWM do buzzer
    gpio_init(BUZZER);
    gpio_set_function(BUZZER, GPIO_FUNC_PWM);

    // Inicializa LED verde
    gpio_init(LED_G);
    gpio_set_dir(LED_G, GPIO_OUT);

    // Inicializa botões e interrupções
    gpio_init(BOTAO_A);
    gpio_set_dir(BOTAO_A, GPIO_IN);
    gpio_pull_up(BOTAO_A);
    gpio_set_irq_enabled_with_callback(BOTAO_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    gpio_init(BOTAO_B);
    gpio_set_dir(BOTAO_B, GPIO_IN);
    gpio_pull_up(BOTAO_B);
    gpio_set_irq_enabled_with_callback(BOTAO_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    gpio_init(BOTAO_J);
    gpio_set_dir(BOTAO_J, GPIO_IN);
    gpio_pull_up(BOTAO_J);
    gpio_set_irq_enabled_with_callback(BOTAO_J, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
}


// Função para soar o alarme
void sirene(uint freq_grave, uint freq_agudo, uint duration) {
    uint slice_num = pwm_gpio_to_slice_num(BUZZER);
    pwm_set_enabled(slice_num, true);
    uint clock = 125000000; // Frequência do clock do PWM (125 MHz)
    uint wrap_value_grave = clock / freq_grave; // Valor de wrap para a frequência grave
    uint wrap_value_agudo = clock / freq_agudo; // Valor de wrap para a frequência "aguda"

    absolute_time_t start_time = get_absolute_time();
    while (absolute_time_diff_us(start_time, get_absolute_time()) < duration * 1000) {
        // Toca a frequência grave (PEN)
        pwm_set_wrap(slice_num, wrap_value_grave);
        pwm_set_gpio_level(BUZZER, wrap_value_grave/3); // Ciclo de trabalho de 50%
        sleep_ms(200); // Tempo de cada "PEN"

        // Toca a frequência "aguda" (PEN)
        pwm_set_wrap(slice_num, wrap_value_agudo);
        pwm_set_gpio_level(BUZZER, wrap_value_agudo/3); // Ciclo de trabalho de 50%
        sleep_ms(200); // Tempo de cada "PEN"
    }

    // Desliga o BUZZER
    pwm_set_gpio_level(BUZZER, 0);
}


int main(){

    // Variáveis referentes a matriz de LED's
    PIO pio = pio0;
    uint sm = 0;
    uint offset = pio_add_program(pio, &pio_matriz_program);
    pio_matriz_program_init(pio, sm, offset, pino_matriz);

    stdio_init_all();
    setup_inicial();

    while (true) {
        adc_select_input(0); // Lê eixo Y do analógico
        uint16_t joyY_valor = adc_read();
        adc_select_input(1); // Lê eixo X do analógico
        uint16_t joyX_valor = adc_read();

        ssd1306_fill(&ssd, !cor); // Limpa o display

        // Faz a atualização da pos. do quadrado no display
        uint posX = (joyX_valor * 115 / 4095) + 4; // Armazena o valor da pos. x
        uint posY = 52 - (joyY_valor * 48 / 4095); // Armazena o valor da pos. y
        ssd1306_rect(&ssd, posY, posX, 8, 8, cor, cor); // Atualiza com a nova pos.
        ssd1306_send_data(&ssd); // Atualiza o conteúdo do display

        if (pwm_function) {
            if (joyX_valor >= 1800 && joyX_valor <= 2300) { // Situação em que o quadrado está no centro
                pwm_set_gpio_level(LED_R, 0);
            } else if (joyX_valor > 2300) { // Situação em que o quadrado vai para direita
                pwm_set_gpio_level(LED_R, joyX_valor - 2300);
            } else if (joyX_valor < 1800) { // Situação em que o quadrado vai para esquerda
                pwm_set_gpio_level(LED_R, 1800 - joyX_valor);
            }

            if (joyY_valor >= 1800 && joyY_valor <= 2300) { // Situação em que o quadrado está no centro
                pwm_set_gpio_level(LED_B, 0);
            } else if (joyY_valor > 2300) { // Situação em que o quadrado vai para baixo
                pwm_set_gpio_level(LED_B, joyY_valor - 2300);
            } else if (joyY_valor < 1800) { // Situação em que o quadrado vai para cima
                pwm_set_gpio_level(LED_B, 1800 - joyY_valor);
            }

            if(joyX_valor > 2300) // Acende o a matriz de LED's quando o joystick vai para a direita
                acender_leds(pio, sm, 0.1, 0.1, 0.1);
            else if(joyX_valor < 1800)  // Acende o a matriz de LED's quando o joystick vai para a esquerda
                acender_leds(pio, sm, 0.0, 0.1, 0.1); 
            else if(joyY_valor > 2300)  // Acende o a matriz de LED's quando o joystick vai para cima
                acender_leds(pio, sm, 0.1, 0, 0.1);
            else if(joyY_valor < 1800)  // Acende o a matriz de LED's quando o joystick vai para baixo
                acender_leds(pio, sm, 0.1, 0.1, 0);
        }
        else{
            if(joyX_valor >= 4000) // Ativa o buzzer quando o joystick vai para a direita
                sirene(700, 300, 1000);
            else if (joyX_valor <= 20) // Ativa o buzzer quando o joystick vai para a esquerda
                sirene(100, 30, 1000);
            
            if(joyY_valor >= 4000) // Ativa o buzzer quando o joystick vai para cima
                sirene(10, 20, 1000);
            else if (joyY_valor <= 20) // Ativa o buzzer quando o joystick vai para baixo
                sirene(100, 2000, 100);
        }

        // Mostra informações do ADC no serial monitor
        uint32_t current_time = to_ms_since_boot(get_absolute_time());
        if ((current_time - last_print_time) >= 1000) {
            printf("Posicao X: %d | Posicao Y: %d\n", posX, posY);
            printf("VRX: %u | VRY: %u\n", joyX_valor, joyY_valor);
            last_print_time = current_time;
        }
    }
}
