# Revisao para a Fase 2

## Descrição
Este projeto consiste em uma revisão para relembrar conceitos trabalhados na fase de capacitação do EmbarcaTech, de modo que grande maioria dos periféricos trabalhados
foram usados neste simples trabalho.

## Funcionalidades
- **Botao A**: Alterna entre os modos do projeto.
- **Botão B**: Faz com que a placa BitDogLab entre em modo bootsel.
- **Display**: Apresenta quadrado que muda proporcionalmente com o joystick.
- **Matriz de LED's**: Muda as cores presentes na matriz de acordo com o movimento do analógico.
- **Buzzer**: Emite sinais sonoros de diferentes graves, agudos e durações.

## Estrutura do Código
O código apresenta diversas funções, das quais vale a pena citar:

- `setup_inicial()`: Inicializa todos os pinos e periféricos necessários para o projeto.
- `sirene()`: Aciona o buzzer para alternar entre frequências de grave e agudo, simulando uma espécie de alarme.
- `acender_leds()`: Acende os LED's na matriz com cores diferentes.
- `limpar_todos_leds()`: Apaga todos os LED's na matriz.

## Estrutura dos arquivos
```
project/
│
├── display_files/
│   ├── font.h
│   ├── ssd1306.c
│   ├── ssd1306.h
│
├── ledMatriz_files/
│   ├── led_matriz.c
│   ├── led_matriz.h
├── EmbarcaTech_fase2Revisao.c
├── CMakeLists.txt
├── pio_matriz.pio
└── README.md
```
## Desenvolvedor 
Guilherme Miller Gama Cardoso

## Link com o vídeo explicativo
https://drive.google.com/file/d/1hyvJc1MzhWcv0xuFX458pzrDGGXuCeK1/view?usp=sharing