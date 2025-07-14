#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "bib/ssd1306.h"
#include "hardware/i2c.h"
#include "hardware/timer.h"
#include "taskcnt.h"

// Definição dos pinos do display
#define I2C_SDA 14
#define I2C_SCL 15

// Definições dos pinos dos botões
#define BUTTON_A 5  // Botão A
#define BUTTON_B 6  // Botão B

// Definição do pino do LED RGB
#define LED_R 13  // LED vermelho
#define LED_G 11  // LED verde
#define LED_B 12  // LED azul


// Processo de configuração de LED RGB
void setup_led_rgb() {

    // LED Vermelho
    gpio_init(LED_R);
    gpio_set_dir(LED_R, GPIO_OUT);
    gpio_put(LED_R, 0);  // Inicialmente desligado

    // LED Verde
    gpio_init(LED_G);
    gpio_set_dir(LED_G, GPIO_OUT);
    gpio_put(LED_G, 0);  // Inicialmente desligado

    // LED Azul
    gpio_init(LED_B);
    gpio_set_dir(LED_B, GPIO_OUT);
    gpio_put(LED_B, 0);  // Inicialmente desligado
}

// Processo de configuração dos botões
void setup_buttons() {

    // Botão A
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);  // Habilita o resistor de pull-up

    // Botão B
    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);  // Habilita o resistor de pull-up
}

/*----------------------------------------------------------------------------------------------------------------------------*/

// Exibe o texto inicial
void initial_txt(u_int8_t *ssd, struct render_area frame_area) {
    
    char *text[] = {
        
        "      Ola", 
        "   aperte A",
        "  para contar", 
        "  suas tarefas",
        " depois aperte",
        " B para seguir"
    };

    // Desenha o texto no display
    int y = 0;
    for (uint i = 0; i < sizeof(text) / sizeof(text[0]); i++) {
        ssd1306_draw_string(ssd, 5, y, text[i]);
        y += 8;
    }

    render_on_display(ssd, &frame_area);

};

// Exibe a quantidade de tarefas
void task_txt(u_int8_t *ssd, struct render_area frame_area, uint8_t task_inc){

    memset(ssd, 0, ssd1306_buffer_length); // Limpeza do buffer

    char task_inc_str[5];  // Buffer para armazenar o número convertido

    sprintf(task_inc_str, "%d", task_inc);  // Converte o número para string

    // Array de strings com um espaço reservado para o número
    char text_num[6][20];  

    // Copiando as mensagens fixas para o array
    snprintf(text_num[0], 20, "   Voce tem");
    snprintf(text_num[1], 20, "      %s", task_inc_str);  // Espaço para o número de tarefas
    snprintf(text_num[2], 20, " tarefas hoje");
    snprintf(text_num[3], 20, "   aperte o");
    snprintf(text_num[4], 20, "   botao  B");
    snprintf(text_num[5], 20, " para iniciar");

    // Desenha o texto no display
    int y = 0;
    for (uint i = 0; i < 6; i++) {
        ssd1306_draw_string(ssd, 5, y, text_num[i]);
        y += 8;
    }

    render_on_display(ssd, &frame_area);

}

// Atualiza o cronômetro
bool timer_callback(struct repeating_timer *t) {
    struct time_data *time = (struct time_data *)t->user_data;

    time->sec++;
    if (time->sec >= 60) {
        time->sec = 0;
        time->min++;
        if (time->min >= 60) {
            time->min = 0;
            time->hrs++;
        }
    }

    return true; 
};

// Exibe o cronômetro 
void timer_txt(uint8_t *ssd, struct render_area frame_area, struct time_data *time) {
    // Inicialização do cronômetro
    time->hrs = 0;
    time->min = 0;
    time->sec = 0;

    // Configura o timer para chamar a callback a cada segundo (1.000.000 microssegundos)
    struct repeating_timer timer;
    add_repeating_timer_us(1000000, timer_callback, time, &timer);

    while (1) {

        memset(ssd, 0, ssd1306_buffer_length); // Limpeza do buffer

        char buffer_time[20];    // Buffer para armazenar o cronômetro
        char text_time[5][20];  // Array de strings com um espaço reservado para o cronômetro

        // Copiando as mensagens fixas para o array
        snprintf(text_time[0], 20, "Tempo decorrido");
        snprintf(text_time[1], 20, "   %02d:%02d:%02d", time->hrs, time->min, time->sec);  // Espaço pata o cronômetro HH:MM:SS
        snprintf(text_time[2], 20, "   aperte o");
        snprintf(text_time[3], 20, "   botao  A");
        snprintf(text_time[4], 20, " para finalizar");

        // Desenha o texto no display
        int y = 0;
        for (uint i = 0; i < 5; i++) {
            ssd1306_draw_string(ssd, 5, y, text_time[i]);
            y += 8;
        }

        render_on_display(ssd, &frame_area);

        // Verifica se o botão B foi pressionado para finalizar o cronômetro
        if (gpio_get(BUTTON_A) == 0) {
            cancel_repeating_timer(&timer); // Para o timer
            memset(ssd, 0, ssd1306_buffer_length); // Limpeza do buffer 
            break; // Sai do loop
        }

        sleep_ms(100); // Pequeno delay para evitar leitura excessiva do botão
    }
};


// Exibe o texto final
void final_txt(u_int8_t *ssd, struct render_area frame_area){

    memset(ssd, 0, ssd1306_buffer_length); // Limpeza do buffer

    char *text_end[] = {
        
        " Parabens suas",
        " tarefas foram",
        "  finalizadas"
    };

    // Desenha o texto no display
    int y = 0;
    for (uint i = 0; i < sizeof(text_end) / sizeof(text_end[0]); i++) {
        ssd1306_draw_string(ssd, 5, y, text_end[i]);
        y += 8;
    }

    render_on_display(ssd, &frame_area);
};

int main() {
    stdio_init_all();

    // Inicialização dos pinos e da comunicação I2C
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Inicialização completa do display SSD1306
    ssd1306_init();

    // Configuração da área de renderização
    struct render_area frame_area = {
        start_column : 0, 
        end_column : ssd1306_width - 1,
        start_page : 0,
        end_page : ssd1306_n_pages - 1
    };

    calculate_render_area_buffer_length(&frame_area);

    // Zerar o display
    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);
    render_on_display(ssd, &frame_area);

    // Configuração de LEDs e Botões    
    setup_led_rgb();
    setup_buttons();

    
    struct time_data time = {0, 0, 0};
    uint8_t task_inc = 0;

    while (1) {

        initial_txt(ssd, frame_area);

        // Loop pra contabilizar as tarefas
        while (1) {

            if (gpio_get(BUTTON_A) == 0){
                sleep_ms(200); // Pequeno delay para evitar leitura excessiva
                gpio_put(LED_B, 1); // Liga o LED azul
                sleep_ms(500); // Debounce do botão
                task_inc++; // Incrementa a quantidade de tarefas
                gpio_put(LED_B, 0); // Desliga o LED azul
            }
            
            if(gpio_get(BUTTON_B) == 0){
                gpio_put(LED_G, 1); // Liga o LED verde
                sleep_ms(500); // Debounce do botão
                gpio_put(LED_G, 0); // Desliga o LED verde
                break;
            }
           
        }
        
        // Loop para contabilizar realização das tarefas 
        while (task_inc != 0){
        
            task_txt(ssd, frame_area, task_inc);
            
            // Se o botão B for pressionado irá iniciar o cronômetro
            if (gpio_get(BUTTON_B) == 0) {
                sleep_ms(200); // Pequeno delay para evitar leitura excessiva
                timer_txt(ssd, frame_area, &time);
                task_inc--;
            }
        }

        break;
   
    }

    final_txt(ssd, frame_area);
    sleep_ms(50000);
}