/* =========================================================
 * indexador.c  –  Módulo de indexação e busca
 *
 * Responsabilidades:
 *   - Normalização de palavras (minúsculas, sem acentos, sem pontuação)
 *   - Leitura recursiva de pasta e indexação de arquivos .txt
 *   - Seleção da AVL correta pela letra inicial
 *   - Busca e exibição de ocorrências
 *   - Estatísticas do índice
 * ========================================================= */

/* _POSIX_C_SOURCE habilita S_ISREG e dirent no modo c99 */
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>     /* opendir/readdir/closedir (POSIX)       */
#include <sys/stat.h>   /* stat() para verificar tipo de entrada  */

#include "avl.h"
#include "indexador.h"

/* ----------------------------------------------------------
 * Variáveis globais
 * ---------------------------------------------------------- */

/* Vetor de 26 raízes de AVL, uma por letra do alfabeto */
AVLNode *indice[26];

/* Contador global de ocorrências (incrementado durante indexação) */
int total_ocorrencias = 0;

/* ----------------------------------------------------------
 * Tabela de conversão de caracteres acentuados para ASCII puro
 * Cobre Latin-1 / UTF-8 de dois bytes mais comuns no português
 * ---------------------------------------------------------- */

/*
 * normalizar_char: converte um byte de entrada para o caractere
 * ASCII correspondente (minúsculo, sem acento).
 * Retorna '\0' para caracteres que devem ser descartados.
 *
 * Como o terminal em UTF-8 envia acentos em múltiplos bytes,
 * esta função é chamada dentro de um parser de sequências UTF-8
 * para tratar os casos mais comuns do português.
 */
char normalizar_char(unsigned char c) {
    /* Letras minúsculas diretas */
    if (c >= 'a' && c <= 'z') return (char)c;
    /* Letras maiúsculas → minúsculas */
    if (c >= 'A' && c <= 'Z') return (char)(c + 32);
    /* Dígitos são mantidos */
    if (c >= '0' && c <= '9') return (char)c;
    /* Todo o resto (pontuação, espaços, etc.) é descartado */
    return '\0';
}

/*
 * normalizar_sequencia_utf8: recebe um codepoint Unicode (calculado
 * a partir dos bytes UTF-8) e retorna o caractere ASCII base.
 * Cobre os blocos Latin Extended usados no português.
 */
static char codepoint_para_ascii(unsigned int cp) {
    /* Mapeamento: codepoint → letra base ASCII minúscula */
    switch (cp) {
        /* á à â ã ä å */
        case 0xE0: case 0xE1: case 0xE2: case 0xE3:
        case 0xC0: case 0xC1: case 0xC2: case 0xC3:
            return 'a';
        /* é è ê ë */
        case 0xE8: case 0xE9: case 0xEA: case 0xEB:
        case 0xC8: case 0xC9: case 0xCA: case 0xCB:
            return 'e';
        /* í ì î ï */
        case 0xEC: case 0xED: case 0xEE: case 0xEF:
        case 0xCC: case 0xCD: case 0xCE: case 0xCF:
            return 'i';
        /* ó ò ô õ ö */
        case 0xF2: case 0xF3: case 0xF4: case 0xF5: case 0xF6:
        case 0xD2: case 0xD3: case 0xD4: case 0xD5: case 0xD6:
            return 'o';
        /* ú ù û ü */
        case 0xF9: case 0xFA: case 0xFB: case 0xFC:
        case 0xD9: case 0xDA: case 0xDB: case 0xDC:
            return 'u';
        /* ç Ç */
        case 0xE7: case 0xC7:
            return 'c';
        /* ñ Ñ */
        case 0xF1: case 0xD1:
            return 'n';
        default:
            return '\0';
    }
}

/*
 * normalizar_palavra: percorre a string src byte a byte,
 * interpretando sequências UTF-8 de 1 e 2 bytes,
 * e produz em dest a palavra normalizada (só [a-z0-9]).
 */
void normalizar_palavra(const char *src, char *dest, int dest_size) {
    int di = 0;                       /* índice de escrita em dest */
    int si = 0;                       /* índice de leitura em src  */
    int len = (int)strlen(src);

    while (si < len && di < dest_size - 1) {
        unsigned char b = (unsigned char)src[si];

        if (b < 0x80) {
            /* Byte ASCII puro */
            char c = normalizar_char(b);
            if (c != '\0') dest[di++] = c;
            si++;
        } else if ((b & 0xE0) == 0xC0 && si + 1 < len) {
            /* Sequência UTF-8 de 2 bytes: 110xxxxx 10xxxxxx */
            unsigned char b2 = (unsigned char)src[si + 1];
            unsigned int  cp = ((b & 0x1F) << 6) | (b2 & 0x3F);
            char c = codepoint_para_ascii(cp);
            if (c != '\0') dest[di++] = c;
            si += 2;
        } else if ((b & 0xF0) == 0xE0 && si + 2 < len) {
            /* Sequência UTF-8 de 3 bytes (e.g. caracteres CJK – descartar) */
            si += 3;
        } else if ((b & 0xF8) == 0xF0 && si + 3 < len) {
            /* Sequência UTF-8 de 4 bytes – descartar */
            si += 4;
        } else {
            si++; /* byte inválido ou isolado – ignora */
        }
    }
    dest[di] = '\0';
}

/* ----------------------------------------------------------
 * Indexação de um único arquivo
 * ---------------------------------------------------------- */
void indexar_arquivo(const char *caminho_arquivo, const char *nome_arquivo) {
    FILE *fp = fopen(caminho_arquivo, "r");
    if (!fp) {
        fprintf(stderr, "Aviso: não foi possível abrir '%s'.\n",
                caminho_arquivo);
        return;
    }

    printf("  Indexando: %s\n", nome_arquivo);

    char linha_buf[4096];   /* buffer para uma linha inteira          */
    int  num_linha = 1;     /* contador de linhas (1-based)           */

    while (fgets(linha_buf, sizeof(linha_buf), fp)) {
        int  col    = 1;    /* coluna atual (1-based)                 */
        int  i      = 0;
        int  len    = (int)strlen(linha_buf);

        while (i < len) {
            /* Pula separadores (espaços, tabs, pontuação básica) */
            unsigned char b = (unsigned char)linha_buf[i];

            /* Detecta início de token: byte >= 'A' ou byte UTF-8 alto */
            int eh_inicio = (isalpha(b) || b > 0x7F);

            if (eh_inicio) {
                /* Acumula o token */
                char token[512];
                int  ti    = 0;
                int  col_inicio = col; /* coluna de início da palavra */

                while (i < len && ti < (int)sizeof(token) - 1) {
                    unsigned char tb = (unsigned char)linha_buf[i];

                    if (isalpha(tb) || tb > 0x7F || isdigit(tb)) {
                        /* Estima avanço de colunas por bytes (simplificado) */
                        token[ti++] = (char)tb;
                        i++;
                        col++;
                    } else {
                        break;
                    }
                }
                token[ti] = '\0';

                /* Normaliza o token extraído */
                char palavra[256];
                normalizar_palavra(token, palavra, sizeof(palavra));

                /* Só indexa se tiver pelo menos 1 caractere e iniciar com letra */
                if (strlen(palavra) > 0 && palavra[0] >= 'a' && palavra[0] <= 'z') {
                    int idx = palavra[0] - 'a'; /* índice no vetor (0–25) */
                    int nova = 0;

                    indice[idx] = avl_inserir(indice[idx], palavra,
                                              nome_arquivo, num_linha,
                                              col_inicio, &nova);
                    total_ocorrencias++;
                }
            } else {
                /* Avança sobre separadores ASCII */
                i++;
                col++;
            }
        }

        num_linha++;
    }

    fclose(fp);
}

/* ----------------------------------------------------------
 * Indexação de uma pasta (todos os .txt)
 * ---------------------------------------------------------- */
void indexar_pasta(const char *path) {
    DIR *dir = opendir(path);
    if (!dir) {
        fprintf(stderr, "Erro: não foi possível abrir a pasta '%s'.\n", path);
        return;
    }

    printf("\nIndexando pasta: %s\n", path);
    printf("-------------------------------\n");

    int arquivos_indexados = 0;
    struct dirent *entrada;

    while ((entrada = readdir(dir)) != NULL) {
        /* Ignora . e .. */
        if (strcmp(entrada->d_name, ".") == 0 ||
            strcmp(entrada->d_name, "..") == 0)
            continue;

        /* Verifica se termina em .txt (case-insensitive) */
        const char *nome = entrada->d_name;
        int nlen = (int)strlen(nome);
        if (nlen < 5) continue; /* mínimo: "a.txt" */

        /* Compara extensão de forma case-insensitive sem strcasecmp */
        const char *ext = nome + nlen - 4;
        char ext_lower[5];
        for (int k = 0; k < 4; k++)
            ext_lower[k] = (char)tolower((unsigned char)ext[k]);
        ext_lower[4] = '\0';
        if (strcmp(ext_lower, ".txt") != 0) continue;

        /* Monta caminho completo */
        char caminho[1024];
        snprintf(caminho, sizeof(caminho), "%s/%s", path, nome);

        /* Verifica que é um arquivo regular */
        struct stat st;
        if (stat(caminho, &st) != 0 || !S_ISREG(st.st_mode)) continue;

        indexar_arquivo(caminho, nome);
        arquivos_indexados++;
    }

    closedir(dir);

    if (arquivos_indexados == 0)
        printf("Nenhum arquivo .txt encontrado na pasta.\n");
    else
        printf("-------------------------------\n");

    printf("Arquivos indexados: %d\n\n", arquivos_indexados);
}

/* ----------------------------------------------------------
 * Busca de palavra
 * ---------------------------------------------------------- */
void buscar_palavra(const char *termo) {
    /* Normaliza o termo de busca da mesma forma que as palavras indexadas */
    char normalizado[256];
    normalizar_palavra(termo, normalizado, sizeof(normalizado));

    if (strlen(normalizado) == 0 ||
        normalizado[0] < 'a' || normalizado[0] > 'z') {
        printf("Termo inválido após normalização.\n");
        return;
    }

    int idx = normalizado[0] - 'a';
    AVLNode *no = avl_buscar(indice[idx], normalizado);

    if (!no) {
        printf("Palavra não encontrada.\n\n");
        return;
    }

    printf("Palavra encontrada: %s\n\n", no->palavra);

    Ocorrencia *oc = no->ocorrencias;
    while (oc) {
        printf("  Arquivo: %s\n", oc->arquivo);
        printf("  Linha:   %d\n", oc->linha);
        printf("  Coluna:  %d\n\n", oc->coluna);
        oc = oc->prox;
    }
}

/* ----------------------------------------------------------
 * Estatísticas do índice
 * ---------------------------------------------------------- */
void exibir_estatisticas(void) {
    printf("\n========== ESTATÍSTICAS ==========\n\n");

    int total_distintas = 0;
    int total_ocs       = 0;

    for (int i = 0; i < 26; i++) {
        int nd = avl_contar_nos(indice[i]);
        int no = avl_contar_ocorrencias(indice[i]);
        int h  = avl_altura(indice[i]);

        total_distintas += nd;
        total_ocs       += no;

        if (nd > 0) {
            printf("  Letra '%c': %4d palavras | %5d ocorrências | altura AVL: %d\n",
                   'a' + i, nd, no, h);
        }
    }

    printf("\n  Total de palavras distintas : %d\n", total_distintas);
    printf("  Total de ocorrências        : %d\n", total_ocs);
    printf("\n==================================\n\n");
}

/* ----------------------------------------------------------
 * Liberação de toda a memória do índice
 * ---------------------------------------------------------- */
void liberar_indice(void) {
    for (int i = 0; i < 26; i++) {
        avl_liberar(indice[i]);
        indice[i] = NULL;
    }
    total_ocorrencias = 0;
}
