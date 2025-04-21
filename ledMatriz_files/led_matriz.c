#include "./led_matriz.h"

// Converte valores RGB (0.0 a 1.0) para formato de 32 bits (0xGGRRBB00)
uint32_t matriz_rgb(double r, double g, double b) {
    unsigned char R = r * 255;
    unsigned char G = g * 255;
    unsigned char B = b * 255;
    return (G << 24) | (R << 16) | (B << 8);
}

// Limpa todos os LEDs
void limpar_todos_leds(PIO pio, uint sm) {
    for (int i = 0; i < num_pixels; i++) {
        pio_sm_put_blocking(pio, sm, matriz_rgb(0.0, 0.0, 0.0));
    }
}

// Acende todos os LEDs
void acender_leds(PIO pio, uint sm, double r, double g, double b) {
    for (int i = 0; i < num_pixels; i++) {
        pio_sm_put_blocking(pio, sm, matriz_rgb(b, r, g));
    }
}

// --- FUNÇÕES PARA CONTROLAR AS BORDAS DA MATRIZ 5x5 ---

// Acende a borda SUPERIOR (primeira linha)
void acender_borda_superior(PIO pio, uint sm, double r, double g, double b) {
    for (int col = 0; col < 5; col++) {
        int pos = col; // Posição na primeira linha (0 a 4)
        pio_sm_put_blocking(pio, sm, matriz_rgb(r, g, b));
    }
}

// Acende a borda INFERIOR (última linha)
void acender_borda_inferior(PIO pio, uint sm, double r, double g, double b) {
    for (int col = 0; col < 5; col++) {
        int pos = 20 + col; // Última linha (posições 20 a 24 em uma matriz 5x5)
        pio_sm_put_blocking(pio, sm, matriz_rgb(r, g, b));
    }
}

// Acende a borda ESQUERDA (primeira coluna)
void acender_borda_esquerda(PIO pio, uint sm, double r, double g, double b) {
    for (int linha = 0; linha < 5; linha++) {
        int pos = linha * 5; // Primeira coluna (0, 5, 10, 15, 20)
        pio_sm_put_blocking(pio, sm, matriz_rgb(r, g, b));
    }
}

// Acende a borda DIREITA (última coluna)
void acender_borda_direita(PIO pio, uint sm, double r, double g, double b) {
    for (int linha = 0; linha < 5; linha++) {
        int pos = (linha * 5) + 4; // Última coluna (4, 9, 14, 19, 24)
        pio_sm_put_blocking(pio, sm, matriz_rgb(r, g, b));
    }
}