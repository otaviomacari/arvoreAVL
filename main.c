/* =========================================================
 * main.c  –  Ponto de entrada e menu interativo
 *
 * Menu:
 *   1 - Indexar pasta
 *   2 - Buscar palavra
 *   3 - Exibir estatísticas
 *   4 - Sair
 * ========================================================= */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "avl.h"
#include "indexador.h"

/* ----------------------------------------------------------
 * Leitura segura de linha do teclado
 * Remove o '\n' final e descarta resto do buffer se necessário.
 * ---------------------------------------------------------- */
static void ler_linha(char *buf, int tamanho) {
    if (!fgets(buf, tamanho, stdin)) {
        buf[0] = '\0';
        return;
    }
    /* Remove '\n' */
    int len = (int)strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    } else {
        /* Linha maior que o buffer: descarta o resto */
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    }
}

/* ----------------------------------------------------------
 * Banner de boas-vindas
 * ---------------------------------------------------------- */
static void exibir_banner(void) {
    printf("\n");
    printf("  ╔══════════════════════════════════════╗\n");
    printf("  ║    INDEXADOR DE TEXTOS COM AVL       ║\n");
    printf("  ║  Estrutura de Dados — PUC Minas      ║\n");
    printf("  ╚══════════════════════════════════════╝\n\n");
}

/* ----------------------------------------------------------
 * Menu principal
 * ---------------------------------------------------------- */
static void exibir_menu(void) {
    printf("  ┌─────────────────────────────┐\n");
    printf("  │  1 - Indexar pasta          │\n");
    printf("  │  2 - Buscar palavra         │\n");
    printf("  │  3 - Exibir estatísticas    │\n");
    printf("  │  4 - Sair                   │\n");
    printf("  └─────────────────────────────┘\n");
    printf("  Opção: ");
}

/* ----------------------------------------------------------
 * main
 * ---------------------------------------------------------- */
int main(void) {
    /* Inicializa o vetor de AVLs com NULL */
    for (int i = 0; i < 26; i++)
        indice[i] = NULL;

    exibir_banner();

    char input[512];
    int  opcao;
    int  indexado = 0; /* flag: já foi indexado ao menos uma vez? */

    while (1) {
        exibir_menu();
        ler_linha(input, sizeof(input));

        /* Converte para inteiro com validação */
        if (sscanf(input, "%d", &opcao) != 1) {
            printf("\n  Opção inválida. Tente novamente.\n\n");
            continue;
        }

        switch (opcao) {

            /* -----------------------------------------------
             * 1 – Indexar pasta
             * --------------------------------------------- */
            case 1: {
                printf("\n  Caminho da pasta (Enter para './textos_exemplo'): ");
                ler_linha(input, sizeof(input));

                /* Usa pasta padrão se vazio */
                const char *pasta = (strlen(input) == 0)
                                    ? "./textos_exemplo"
                                    : input;

                /* Pergunta se deseja re-indexar */
                if (indexado) {
                    printf("  Um índice já existe. Deseja limpar e re-indexar? (s/n): ");
                    char resp[8];
                    ler_linha(resp, sizeof(resp));
                    if (resp[0] == 's' || resp[0] == 'S') {
                        liberar_indice();
                        printf("  Índice anterior removido.\n");
                    } else {
                        printf("  Indexação cancelada.\n\n");
                        break;
                    }
                }

                indexar_pasta(pasta);
                indexado = 1;
                break;
            }

            /* -----------------------------------------------
             * 2 – Buscar palavra
             * --------------------------------------------- */
            case 2: {
                if (!indexado) {
                    printf("\n  Nenhuma pasta foi indexada ainda.\n");
                    printf("  Use a opção 1 primeiro.\n\n");
                    break;
                }
                printf("\n  Palavra a buscar: ");
                ler_linha(input, sizeof(input));

                if (strlen(input) == 0) {
                    printf("  Termo vazio.\n\n");
                    break;
                }

                printf("\n");
                buscar_palavra(input);
                break;
            }

            /* -----------------------------------------------
             * 3 – Estatísticas
             * --------------------------------------------- */
            case 3: {
                if (!indexado) {
                    printf("\n  Nenhuma pasta foi indexada ainda.\n\n");
                    break;
                }
                exibir_estatisticas();
                break;
            }

            /* -----------------------------------------------
             * 4 – Sair
             * --------------------------------------------- */
            case 4: {
                printf("\n  Liberando memória...\n");
                liberar_indice();
                printf("  Até logo!\n\n");
                return EXIT_SUCCESS;
            }

            default:
                printf("\n  Opção inválida. Escolha entre 1 e 4.\n\n");
        }
    }

    return EXIT_SUCCESS;
}
