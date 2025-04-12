#include <stdio.h>
#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define ATRASO 10000 // 10 milissegundos de atraso
#define NUMERO_DE_PECAS 16
#define ALTURA_DA_PECA 3
#define LARGURA_DA_PECA 5
#define TAMANHO_DO_QUEBRA_CABECA 4

// Definição do tamanho da janela do jogo
#define ALTURA_DA_JANELA_DO_JOGO (TAMANHO_DO_QUEBRA_CABECA * ALTURA_DA_PECA)
#define LARGURA_DA_JANELA_DO_JOGO (TAMANHO_DO_QUEBRA_CABECA * LARGURA_DA_PECA)

typedef struct {
    int numero;
    int linha, coluna;
} PecaDoQuebraCabeca; //struct para armazenar as peças do quebra-cabeça

// Função para inicializar o quebra-cabeça
void inicializarQuebraCabeca(PecaDoQuebraCabeca *quebraCabeca) {
    // Inicializa o array de números das peças
    int numeros[NUMERO_DE_PECAS];
    for (int i = 0; i < NUMERO_DE_PECAS; i++) {
        numeros[i] = i;
    }

    // Embaralha os números das peças
    for (int i = NUMERO_DE_PECAS - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = numeros[i];
        numeros[i] = numeros[j];
        numeros[j] = temp;
    }

    // Distribui os números embaralhados nas peças
    for (int i = 0; i < NUMERO_DE_PECAS; i++) {
        quebraCabeca[i].numero = numeros[i];
        quebraCabeca[i].linha = i / TAMANHO_DO_QUEBRA_CABECA;
        quebraCabeca[i].coluna = i % TAMANHO_DO_QUEBRA_CABECA;
    }
}

// Função para desenhar uma peça na tela
void desenharPeca(WINDOW *win, int linha, int coluna, int numero) {
    int inicioY = linha * ALTURA_DA_PECA;
    int inicioX = coluna * LARGURA_DA_PECA;
    int fimY = (linha + 1) * ALTURA_DA_PECA - 1;
    int fimX = (coluna + 1) * LARGURA_DA_PECA - 1;
    int meioY = inicioY + ALTURA_DA_PECA / 2;
    int meioX = inicioX + LARGURA_DA_PECA / 2;

    // Desenha as bordas da peça
    for (int i = inicioY + 1; i < fimY; i++) {
        mvwaddch(win, i, inicioX, '|');
        mvwaddch(win, i, fimX, '|');
    }

    for (int i = inicioX + 1; i < fimX; i++) {
        mvwaddch(win, inicioY, i, '-');
        mvwaddch(win, fimY, i, '-');
    }

    // Desenha o número no centro da peça
    if (numero != 0) {
        mvwprintw(win, meioY, meioX, "%d", numero);
    }
}

// Função para desenhar todo o quebra-cabeça na tela
void desenharQuebraCabeca(WINDOW *win, PecaDoQuebraCabeca *quebraCabeca) {
    wclear(win);

    for (int i = 0; i < NUMERO_DE_PECAS; i++) {
        int linha = quebraCabeca[i].linha;
        int coluna = quebraCabeca[i].coluna;
        desenharPeca(win, linha, coluna, quebraCabeca[i].numero);
    }

    wrefresh(win);
}

// Função para verificar se o quebra-cabeça foi resolvido
int quebraCabecaResolvido(PecaDoQuebraCabeca *quebraCabeca) {
    int contagem = 1;
    for (int i = 0; i < NUMERO_DE_PECAS; i++) {
        if (quebraCabeca[i].numero == 0) {
            continue;
        }
        if (quebraCabeca[i].numero != contagem) {
            return 0;
        }
        contagem++;
    }
    return 1;
}

// Função para mover uma peça do quebra-cabeça
void moverPecaDoQuebraCabeca(PecaDoQuebraCabeca *quebraCabeca, int *x, int *y, int dx, int dy) {
    if (*y + dy >= 0 && *y + dy < TAMANHO_DO_QUEBRA_CABECA && *x + dx >= 0 && *x + dx < TAMANHO_DO_QUEBRA_CABECA) {
        quebraCabeca[*y * TAMANHO_DO_QUEBRA_CABECA + *x].numero = quebraCabeca[(*y + dy) * TAMANHO_DO_QUEBRA_CABECA + *x + dx].numero;
        quebraCabeca[(*y + dy) * TAMANHO_DO_QUEBRA_CABECA + *x + dx].numero = 0;
        *y += dy;
        *x += dx;
    }
}

int main() {
    srand(time(NULL)); // Inicializa o gerador de números aleatórios

    PecaDoQuebraCabeca quebraCabeca[NUMERO_DE_PECAS];
    inicializarQuebraCabeca(quebraCabeca); // Inicializa o quebra-cabeça

    int x, y;
    for (int i = 0; i < NUMERO_DE_PECAS; i++) {
        if (quebraCabeca[i].numero == 0) {
            y = quebraCabeca[i].linha;
            x = quebraCabeca[i].coluna;
            break;
        }
    }

    initscr(); // Inicializa a tela do ncurses
    noecho(); // Não mostra as teclas digitadas na tela
    curs_set(FALSE); // Não mostra o cursor na tela

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x); // Obtém o tamanho da tela

    // Cria uma nova janela para o jogo no centro da tela
    WINDOW *janela_do_jogo = newwin(ALTURA_DA_JANELA_DO_JOGO, LARGURA_DA_JANELA_DO_JOGO, (max_y - ALTURA_DA_JANELA_DO_JOGO) / 2, (max_x - LARGURA_DA_JANELA_DO_JOGO) / 2);
    keypad(janela_do_jogo, TRUE); // Permite o uso das teclas especiais
    nodelay(janela_do_jogo, TRUE); // Faz com que o wgetch não aguarde a entrada do usuário

    // Cria uma nova janela para a mensagem abaixo da janela do jogo
    WINDOW *janela_da_mensagem = newwin(1, LARGURA_DA_JANELA_DO_JOGO, (max_y - ALTURA_DA_JANELA_DO_JOGO) / 2 + ALTURA_DA_JANELA_DO_JOGO, (max_x - LARGURA_DA_JANELA_DO_JOGO) / 2);

    desenharQuebraCabeca(janela_do_jogo, quebraCabeca); // Desenha o quebra-cabeça na tela

    int ch;
    int movimentoFeito = 0;
    while ((ch = wgetch(janela_do_jogo)) != 'q') { // Loop principal do jogo
        switch(ch) {
            case KEY_UP:
                moverPecaDoQuebraCabeca(quebraCabeca, &x, &y, 0, -1); // Move a peça para cima
                movimentoFeito = 1;
                break;
            case KEY_DOWN:
                moverPecaDoQuebraCabeca(quebraCabeca, &x, &y, 0, 1); // Move a peça para baixo
                movimentoFeito = 1;
                break;
            case KEY_LEFT:
                moverPecaDoQuebraCabeca(quebraCabeca, &x, &y, -1, 0); // Move a peça para a esquerda
                movimentoFeito = 1;
                break;
            case KEY_RIGHT:
                moverPecaDoQuebraCabeca(quebraCabeca, &x, &y, 1, 0); // Move a peça para a direita
                movimentoFeito = 1;
                break;
        }

        if (movimentoFeito) {
            desenharQuebraCabeca(janela_do_jogo, quebraCabeca); // Redesenha o quebra-cabeça na tela
            movimentoFeito = 0;
        }

        if (quebraCabecaResolvido(quebraCabeca)) { // Verifica se o quebra-cabeça foi resolvido
            // Calcula a posição para a mensagem ser centralizada na janela da mensagem
            int msg_x = (LARGURA_DA_JANELA_DO_JOGO - 17) / 2; // 17 é o comprimento da string "Parabéns!!!"

            // Imprime a mensagem na janela da mensagem
            mvwprintw(janela_da_mensagem, 0, msg_x, "Parabéns!!!");
            wrefresh(janela_da_mensagem);
            usleep(10000000); // Espera 10 segundos
            break;
        }

        usleep(ATRASO); // Espera um curto período de tempo antes de verificar o próximo movimento
    }

    delwin(janela_do_jogo); // Deleta a janela do jogo
    delwin(janela_da_mensagem); // Deleta a janela da mensagem
    endwin(); // Encerra a tela do ncurses
    return 0;
}
