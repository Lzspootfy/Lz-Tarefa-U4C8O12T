

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "lib/ssd1306.h"
#include "lib/font.h"
#include "hardware/pwm.h"
#include "pico/bootrom.h"
#define I2C_PORT i2c1
#define I2C_SDA 14 //display
#define I2C_SCL 15 //display
#define endereco 0x3C
#define JOYSTICK_X_PIN 26  // GPIO para eixo X
#define JOYSTICK_Y_PIN 27  // GPIO para eixo Y
#define JOYSTICK_PB 22 // GPIO para botão do Joystick
#define Botao_A 5 // GPIO para botão A
#define LED_BLUE 12 //gpio 12 para led vermelho
#define LED_RED 13 //led vermelho
#define LED_GREEN 11
#define WRAP 4096
#define DIV 16
uint16_t led_b_level = 0, led_r_level = 0;   // Inicialização dos níveis de PWM para os LEDs
uint slice_led_b, slice_led_r;           // Variáveis para armazenar os slices de PWM correspondentes aos LEDs
static uint sliceRED;
static uint sliceBLUE;
bool  enableSlices;
static volatile uint32_t last_time = 0;
bool leds_enabled = true;                // Variável para controlar o estado dos LEDs
bool cor = true;
ssd1306_t ssd; // Inicializa a estrutura do display
uint32_t current_time; //para o debounce

void gpio_irq_handler(uint gpio, uint32_t events); //funçao da interrupção


void setup_pwm_led(uint led, uint *slice, uint16_t level) //configuraçõe de pwm
{
  gpio_set_function(led, GPIO_FUNC_PWM);
  *slice = pwm_gpio_to_slice_num(led);
  pwm_set_clkdiv(*slice, DIV);
  pwm_set_wrap(*slice, WRAP);
  pwm_set_enabled(*slice, true);
  pwm_set_gpio_level(led, level);
   
  sliceRED = pwm_gpio_to_slice_num(LED_RED);
  sliceBLUE = pwm_gpio_to_slice_num(LED_BLUE);
  gpio_set_function(LED_RED, GPIO_FUNC_PWM);
  gpio_set_function(LED_BLUE, GPIO_FUNC_PWM);
  pwm_set_clkdiv(sliceRED, DIV);
  pwm_set_wrap(sliceRED, WRAP);
  pwm_set_clkdiv(sliceBLUE, DIV);
  pwm_set_wrap(sliceBLUE, WRAP);

  pwm_set_enabled(sliceRED, true);
  pwm_set_enabled(sliceBLUE, true);
  
  enableSlices= true;
}



 void init_gpio_buttons(){
  gpio_init(LED_GREEN);
  gpio_set_dir(LED_GREEN,GPIO_OUT);
  gpio_init(LED_BLUE);
  gpio_set_dir(LED_BLUE,GPIO_OUT);
  gpio_init(LED_RED);
  gpio_set_dir(LED_RED,GPIO_OUT);

  gpio_init(Botao_A);
  gpio_set_dir(Botao_A, GPIO_IN);
  gpio_pull_up(Botao_A);
} //inicializa o joystick, button_a

void setup_joystick(){ //inicializando o botao de clique do joy stick
  adc_init();
  adc_gpio_init(JOYSTICK_X_PIN);
  adc_gpio_init(JOYSTICK_Y_PIN);
  gpio_init(JOYSTICK_PB);
  gpio_set_dir(JOYSTICK_PB, GPIO_IN);
  gpio_pull_up(JOYSTICK_PB); 


}


int main()
{
  stdio_init_all();
  init_gpio_buttons();
  // I2C Initialisation. Using it at 400Khz.
  i2c_init(I2C_PORT, 400 * 1000);
  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
  gpio_pull_up(I2C_SDA); // Pull up the data line
  gpio_pull_up(I2C_SCL); // Pull up the clock line
  ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
  ssd1306_config(&ssd); // Configura o display
  ssd1306_send_data(&ssd); // Envia os dados para o display

  // Limpa o display. O display inicia com todos os pixels apagados.
  ssd1306_fill(&ssd, false);
  ssd1306_send_data(&ssd);
  
  setup_joystick();
  setup_pwm_led(LED_BLUE, &slice_led_b, led_b_level);
  setup_pwm_led(LED_RED, &slice_led_r, led_r_level);

  uint16_t adc_value_x;
  uint16_t adc_value_y;  
  char str_x[5];  // Buffer para armazenar a string
  char str_y[5];  // Buffer para armazenar a string  
  
  gpio_set_irq_enabled_with_callback(JOYSTICK_PB, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler); //interrupção
  gpio_set_irq_enabled_with_callback(Botao_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler); //interrupção

 
  uint32_t last_print_time = 0;
  bool last_sw_state = true;

  
  while (true)
  {
    adc_select_input(1); // Seleciona o ADC para eixo y. O pino 26 como entrada analógica
    adc_value_x = adc_read();
    adc_select_input(0); // Seleciona o ADC para eixo x. O pino 27 como entrada analógica
    adc_value_y = adc_read();    
    sprintf(str_x, "%d", adc_value_x);  // Converte o inteiro em string
    sprintf(str_y, "%d", adc_value_y);  // Converte o inteiro em string

      // Calcula a variação do joystick a partir do centro (2048)
      uint16_t led_brightness_x = abs((int)adc_value_x - 2048) * 2;
      uint16_t led_brightness_y = abs((int)adc_value_y - 2048) * 2;
      
      // Se o joystick estiver próximo do centro, apaga os LEDs
      if (led_brightness_x < 400) led_brightness_x = 0;
      if (led_brightness_y < 200) led_brightness_y = 0;
      
      pwm_set_gpio_level(LED_RED, led_brightness_x);
      pwm_set_gpio_level(LED_BLUE, led_brightness_y);
  
    //cor = !cor;
    // Atualiza o conteúdo do display com animações
    ssd1306_fill(&ssd, !cor); // Limpa o display

    //entra nessa condição se apertar no joystick
     
    ssd1306_rect(&ssd, 3, 3, 120, 57, cor, !cor); // Desenha um retângulo
    ssd1306_draw_string(&ssd, "+",  adc_value_x/35, adc_value_y/75); // Desenha uma string      
    ssd1306_send_data(&ssd); // Atualiza o display

    sleep_ms(100);
  }
}



void gpio_irq_handler(uint gpio, uint32_t events){
  
  if(gpio==JOYSTICK_PB){ //condição para caso o joy stick seja pressionado
   current_time = to_us_since_boot(get_absolute_time());

  // Verifica se passou tempo suficiente desde o último evento
  if (current_time - last_time > 200000) // 200 ms de debouncing
  {
      last_time = current_time; // Atualiza o tempo do último evento
      gpio_put(LED_GREEN,!gpio_get(LED_GREEN)); // Alterna o estado
      ssd1306_rect(&ssd, 3, 3, 121, 58, cor, !cor); // Desenha um retângulo
      ssd1306_rect(&ssd, 3, 3, 122, 59, cor, !cor); // Desenha um retângulo
      ssd1306_rect(&ssd, 3, 3, 120, 57, cor, !cor); // Desenha um retângulo
      ssd1306_rect(&ssd, 3, 3, 119, 56, cor, !cor); // Desenha um retângulo
      ssd1306_send_data(&ssd); // Atualiza o display

    
  }

 
  }

  ssd1306_send_data(&ssd); // Atualiza o display
  bool a=true;
  if(gpio==Botao_A){ //condição para caso o botão A seja pressionado.
    current_time = to_us_since_boot(get_absolute_time());
    if (current_time - last_time > 200000) // 200 ms de debouncing
    {
        last_time = current_time;
    if(enableSlices) {
      pwm_set_enabled(slice_led_b, enableSlices); // Desativa PWM do LED azul
      pwm_set_enabled(slice_led_r, enableSlices); // Desativa PWM do LED vermelho
      enableSlices = !enableSlices;
  }else{
    pwm_set_enabled(slice_led_b, enableSlices); // Desativa PWM do LED azul
    pwm_set_enabled(slice_led_r, enableSlices); // Desativa PWM do LED vermelho
    enableSlices = !enableSlices;

   }
  }
 }
}
