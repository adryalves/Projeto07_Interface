# Projeto BitDogLab - Controle de LEDs e Display SSD1306

## Link de vídeo de funcionamento:

## Objetivos
- Compreender e aplicar comunicação serial (UART e I2C).
- Controlar LEDs comuns e endereçáveis (WS2812).
- Implementar interrupções e debounce para botões.
- Exibir informações no display SSD1306 com suporte a caracteres maiúsculos e minúsculos.

## Componentes Utilizados
- Matriz 5x5 de LEDs WS2812 (GPIO 7).
- LED RGB (GPIOs 11, 12, 13).
- Botão A (GPIO 5) e Botão B (GPIO 6).
- Display SSD1306 (I2C, GPIOs 14 e 15).

## Funcionalidades

1. **Entrada de Caracteres via PC**:
   - Caracteres digitados no Serial Monitor são exibidos no display SSD1306.
   - Números de 0 a 9 acionam símbolos correspondentes na matriz de LEDs WS2812.

2. **Interação com Botões**:
   - **Botão A**: Alterna o estado do LED RGB Verde e exibe mensagens no display e no Serial Monitor.
   - **Botão B**: Alterna o estado do LED RGB Azul e exibe mensagens no display e no Serial Monitor.

3. **Uso de Interrupções e Debounce**:
   - Todas as interações com botões são tratadas via interrupções (IRQ).
   - Implementação de debounce via software para evitar leituras incorretas.

## Requisitos
- Uso de interrupções para botões.
- Implementação de debounce.
- Controle de LEDs comuns e WS2812.
- Utilização do display SSD1306 com suporte a fontes maiúsculas e minúsculas.
- Comunicação UART para envio de informações.
- Código bem estruturado e comentado.

## Estrutura do Código
- **`ssd1306.c` / `ssd1306.h`**: Controle do display SSD1306.
- **`font.h`**: Biblioteca de fontes com caracteres maiúsculos, minúsculos e números.
- **`main.c`**: Lógica principal do projeto, incluindo interrupções, debounce e controle de LEDs.
- **`ws2812.c` / `ws2812.h`**: Controle da matriz de LEDs WS2812.

## Como Executar
1. Conecte à placa BitDogLab no computador via cabo USB.
2. Compile o código.
3. Faça o BOOTSEL na placa e clique em run
4. Utilize o Serial Monitor do VS Code para interagir com o projeto.
5. Pressione os botões A e B para alternar os estados dos LEDs RGB e observe as mensagens no display e no Serial Monitor.

## Exemplo de Uso
- Digite um caractere no Serial Monitor para exibi-lo no display SSD1306.
- Pressione o Botão A para alternar o LED Verde e o Botão B para alternar o LED Azul.
