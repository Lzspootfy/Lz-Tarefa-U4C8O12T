# Projeto com Joystick, LEDs e Display SSD1306

Este projeto utiliza uma placa Raspberry Pi Pico com um joystick analógico, LEDs PWM e um display SSD1306 I2C para criar uma interface interativa. A posição do joystick controla o brilho dos LEDs, e o display exibe uma animação de retângulos e um marcador que se move conforme o joystick é manipulado. Além disso, o código inclui o controle de LEDs com interrupções e debounce.

▶️Video do YouTube https://youtu.be/Radfk8Z0A-Q▶️

## Componentes Utilizados

- **Raspberry Pi Pico**: Placa de desenvolvimento com microcontrolador RP2040.
- **Joystick Analógico**: Para controle de movimento (eixos X e Y) e um botão de clique.
- **LEDs RGB**: LEDs controlados por PWM para simular iluminação com base na posição do joystick.
- **Display SSD1306**: Display OLED de 128x64 pixels, com comunicação I2C.
- **Botões (Botão A e Botão do Joystick)**: Para interação com o sistema.

## Funcionalidade

- **Joystick**: O eixo X e Y do joystick ajustam o brilho dos LEDs vermelhos e azuis, respectivamente. O botão do joystick alterna o estado de um LED verde e modifica o conteúdo do display.
- **Botão A**: O botão A alterna a ativação dos LEDs vermelhos e azuis.
- **Display SSD1306**: Desenha retângulos e um marcador que se move conforme a posição do joystick. O estado do display também muda ao pressionar o botão do joystick.

## Requisitos

- Raspberry Pi Pico (ou outra placa compatível com o RP2040).
- Display OLED SSD1306 (I2C).
- Joystick analógico com um botão de clique.
- LEDs RGB (com controle PWM).
- Biblioteca `ssd1306` e `pico/stdlib` instaladas.

## Como Usar

### Configuração do Hardware

1. **Joystick**:
   - Pino X do joystick: GPIO 26 (entrada analógica).
   - Pino Y do joystick: GPIO 27 (entrada analógica).
   - Botão de clique do joystick: GPIO 22 (entrada digital).

2. **LEDs**:
   - LED Azul: GPIO 12.
   - LED Vermelho: GPIO 13.
   - LED Verde: GPIO 11.

3. **Botão A**: GPIO 5 (botão adicional para controle).

4. **Display SSD1306**:
   - SDA: GPIO 14.
   - SCL: GPIO 15.
   - Endereço I2C: 0x3C.

### Compilação e Execução

1. Instale a biblioteca `ssd1306` para o display.
2. Compile o código e carregue no seu Raspberry Pi Pico.
3. Conecte os componentes conforme as descrições de pinos acima.
4. Abra o terminal serial para monitoramento (opcional).
5. Execute o código e interaja com o sistema utilizando o joystick e os botões.

### Funções Principais

- **`gpio_irq_handler(uint gpio, uint32_t events)`**: Função de interrupção que lida com os eventos dos botões (Joystick e Botão A).
- **`setup_pwm_led(uint led, uint *slice, uint16_t level)`**: Configura os LEDs para controle via PWM.
- **`setup_joystick()`**: Inicializa os pinos do joystick e do botão de clique.
- **`init_gpio_buttons()`**: Inicializa os pinos dos LEDs e botões.
- **`main()`**: Loop principal que controla a leitura do joystick, atualização dos LEDs e do display.
