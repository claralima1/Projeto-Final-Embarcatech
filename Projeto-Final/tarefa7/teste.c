#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "hardware/i2c.h"
#include "hardware/timer.h"
#include "ssd1306.h"
#include "stdlib.h"

// Pinos I2C
#define I2C_PORT i2c1  // Define o pino I2C para o primeiro barramento
#define PINO_SCL 14     // Pino SCL (clock) do barramento I2C
#define PINO_SDA 15     // Pino SDA (dados) do barramento I2C

// Pinos do LED RGB
#define LED_R_PIN 13    // Pino do LED vermelho
#define LED_G_PIN 11    // Pino do LED verde
#define LED_B_PIN 12    // Pino do LED azul

// Configuração do buzzer
#define BUZZER_PIN 21    // Pino do buzzer
#define BUZZER_FREQUENCY 4000  // Frequência do som do buzzer (4kHz)


// Definições de tempo
#define INTERVALO_PISCA 500  // Intervalo para piscar LEDs e buzzer (em milissegundos)
#define TEMPO_ALERTA 5000     // Tempo total de alerta (5 segundos)
#define LIMIAR_PRESSAO 500    // Limite para detectar movimento no sensor de pressão

// Declaração de variáveis globais
ssd1306_t disp;  // Variável para o display OLED
struct repeating_timer timer_sensor;  // Timer para simular atualização do sensor
struct repeating_timer timer_pisca;  // Timer para piscar LED e buzzer
int valor_pressao = 0;  // Valor do sensor de pressão
bool estado_alerta = false;  // Estado do alerta (LED e buzzer ligados/desligados)
bool alerta_ativo = false;  // Flag para verificar se o alerta está ativo

// Inicializa o PWM do buzzer
void pwm_init_buzzer(uint pin) {
    gpio_set_function(pin, GPIO_FUNC_PWM);  // Configura o pino como PWM
    uint slice_num = pwm_gpio_to_slice_num(pin);  // Obtém o número do "slice" do PWM
    pwm_config config = pwm_get_default_config();  // Obtém a configuração padrão do PWM
    pwm_config_set_clkdiv(&config, 4.0);  // Ajusta o divisor de clock (para controlar a velocidade do PWM)
    pwm_init(slice_num, &config, true);  // Inicializa o PWM com a configuração
    pwm_set_gpio_level(pin, 0);  // Inicializa o nível do PWM como 0 (sem som)
}

// Simula o sensor de pressão (agora a cada 5 segundos)
bool atualiza_sensor_callback(struct repeating_timer *t) {
    valor_pressao = rand() % 1024;  // Simula um valor de pressão aleatório (entre 0 e 1023)
    printf("Sensor de pressão: %d\n", valor_pressao);  // Exibe o valor no console
    return true;  // Retorna verdadeiro para que o timer continue executando
}

// Callback para piscar LED e buzzer
bool pisca_led_buzzer_callback(struct repeating_timer *t) {
    estado_alerta = !estado_alerta;  // Alterna o estado do alerta (ligar/desligar)
    gpio_put(LED_R_PIN, estado_alerta);  // Acende o LED vermelho dependendo do estado
    pwm_set_gpio_level(BUZZER_PIN, estado_alerta ? 2048 : 0);  // Aciona o buzzer se o alerta estiver ativo
    return true;  // Retorna verdadeiro para que o timer continue executando
}

// Função para inicializar os periféricos
void inicializa() {
    stdio_init_all();  // Inicializa a comunicação serial para depuração
    
    // Inicializa I2C e o Display OLED
    i2c_init(I2C_PORT, 400 * 1000);  // Inicializa o barramento I2C com 400 kHz
    gpio_set_function(PINO_SCL, GPIO_FUNC_I2C);  // Configura o pino SCL como função I2C
    gpio_set_function(PINO_SDA, GPIO_FUNC_I2C);  // Configura o pino SDA como função I2C
    gpio_pull_up(PINO_SCL);  // Ativa o pull-up no pino SCL
    gpio_pull_up(PINO_SDA);  // Ativa o pull-up no pino SDA
    ssd1306_init(&disp, 128, 64, 0x3C, I2C_PORT);  // Inicializa o display OLED com resolução 128x64

    // Inicializa o Buzzer e LEDs
    gpio_init(BUZZER_PIN);  // Inicializa o pino do buzzer
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);  // Define o pino do buzzer como saída
    pwm_init_buzzer(BUZZER_PIN);  // Configura o buzzer para PWM

    gpio_init(LED_R_PIN);  // Inicializa o pino do LED vermelho
    gpio_init(LED_G_PIN);  // Inicializa o pino do LED verde
    gpio_init(LED_B_PIN);  // Inicializa o pino do LED azul
    gpio_set_dir(LED_R_PIN, GPIO_OUT);  // Define o pino do LED vermelho como saída
    gpio_set_dir(LED_G_PIN, GPIO_OUT);  // Define o pino do LED verde como saída
    gpio_set_dir(LED_B_PIN, GPIO_OUT);  // Define o pino do LED azul como saída

    // Inicializa o gerador de números aleatórios
    srand(time_us_64() % 1000000);  // Semente para números aleatórios

    // Inicia temporizador para atualizar o sensor de pressão a cada 5 segundos
    add_repeating_timer_ms(5000, atualiza_sensor_callback, NULL, &timer_sensor);  // Chama a função de atualização do sensor a cada 5 segundos
}

// Função para mostrar texto no display OLED
void mostrar_texto(int col, int lin, int tam, char *msg, bool limpar) {
    if (limpar) {
        ssd1306_clear(&disp);  // Limpa a tela se o parâmetro 'limpar' for verdadeiro
    }
    ssd1306_draw_string(&disp, col, lin, tam, msg);  // Desenha o texto no display na posição (col, lin)
    ssd1306_show(&disp);  // Atualiza o display para mostrar o texto
}

// Desliga LEDs e buzzer
void desliga_led_buzzer() {
    gpio_put(LED_R_PIN, 0);  // Apaga o LED vermelho
    gpio_put(LED_G_PIN, 0);  // Apaga o LED verde
    gpio_put(LED_B_PIN, 0);  // Apaga o LED azul
    pwm_set_gpio_level(BUZZER_PIN, 0);  // Desliga o buzzer
    alerta_ativo = false;  // Define o alerta como inativo
}

int main() {
    inicializa();  // Chama a função de inicialização

    char text[30];  // Variável para armazenar o texto a ser exibido no display

    while (true) {
        if (valor_pressao > LIMIAR_PRESSAO) {  // Se o valor da pressão for maior que o limiar, indica movimento
            if (!alerta_ativo) {  // Se o alerta não estiver ativo, aciona o alerta
                alerta_ativo = true;
                snprintf(text, sizeof(text), "Bebe Movimentando!");  // Define a mensagem
                mostrar_texto(5, 20, 1, text, true);  // Mostra a mensagem no display

                // Inicia o temporizador para piscar o LED e o buzzer
                add_repeating_timer_ms(INTERVALO_PISCA, pisca_led_buzzer_callback, NULL, &timer_pisca);
                sleep_ms(TEMPO_ALERTA);  // Mantém o alerta ativo por 5 segundos
                cancel_repeating_timer(&timer_pisca);  // Cancela o temporizador de pisca
                desliga_led_buzzer();  // Desliga LEDs e buzzer
            }
        } else {  // Se não houver movimento, exibe "Bebe Dormindo"
            snprintf(text, sizeof(text), "Bebe Dormindo!");
            mostrar_texto(5, 20, 1, text, true);
            sleep_ms(TEMPO_ALERTA);  // Exibe a mensagem por 5 segundos
        }
    }

    return 0;  // Finaliza o programa
}
