/* =========================================================
 * avl.c  –  Implementação completa da Árvore AVL
 *
 * Operações implementadas:
 *   - Inserção com rebalanceamento automático
 *   - Busca binária
 *   - Rotação simples à esquerda
 *   - Rotação simples à direita
 *   - Rotação dupla esquerda-direita (Left-Right)
 *   - Rotação dupla direita-esquerda (Right-Left)
 *   - Atualização de altura
 *   - Cálculo do fator de balanceamento
 * ========================================================= */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "avl.h"

/* ----------------------------------------------------------
 * Utilitários internos
 * ---------------------------------------------------------- */

/* Retorna o maior entre dois inteiros */
static int max(int a, int b) {
    return (a > b) ? a : b;
}

/* Retorna a altura de um nó (trata NULL como -1) */
int avl_altura(AVLNode *no) {
    if (no == NULL) return -1;
    return no->altura;
}

/* Atualiza a altura de um nó com base nos filhos */
static void atualizar_altura(AVLNode *no) {
    if (no != NULL)
        no->altura = 1 + max(avl_altura(no->esq), avl_altura(no->dir));
}

/* Fator de balanceamento = altura(esq) - altura(dir)
 * > 0 → pesado à esquerda
 * < 0 → pesado à direita */
int avl_fator(AVLNode *no) {
    if (no == NULL) return 0;
    return avl_altura(no->esq) - avl_altura(no->dir);
}

/* ----------------------------------------------------------
 * Rotações
 * ---------------------------------------------------------- */

/*
 * Rotação simples à DIREITA (Right Rotation)
 * Usada quando o nó está desbalanceado à esquerda (fator > 1)
 * e o filho esquerdo também está pesado à esquerda (fator >= 0).
 *
 *       y                x
 *      / \    →        /   \
 *     x   T3          T1    y
 *    / \                   / \
 *   T1  T2               T2   T3
 */
static AVLNode *rotacao_direita(AVLNode *y) {
    AVLNode *x  = y->esq;
    AVLNode *T2 = x->dir;

    /* Realiza a rotação */
    x->dir = y;
    y->esq = T2;

    /* Atualiza alturas (y primeiro, pois agora é filho de x) */
    atualizar_altura(y);
    atualizar_altura(x);

    return x; /* nova raiz da subárvore */
}

/*
 * Rotação simples à ESQUERDA (Left Rotation)
 * Usada quando o nó está desbalanceado à direita (fator < -1)
 * e o filho direito também está pesado à direita (fator <= 0).
 *
 *     x                  y
 *    / \       →        / \
 *   T1   y             x   T3
 *       / \           / \
 *      T2  T3        T1  T2
 */
static AVLNode *rotacao_esquerda(AVLNode *x) {
    AVLNode *y  = x->dir;
    AVLNode *T2 = y->esq;

    /* Realiza a rotação */
    y->esq = x;
    x->dir = T2;

    /* Atualiza alturas (x primeiro) */
    atualizar_altura(x);
    atualizar_altura(y);

    return y; /* nova raiz da subárvore */
}

/*
 * Rotação dupla ESQUERDA-DIREITA (Left-Right Rotation)
 * Usada quando fator > 1 (pesado à esquerda) mas o filho
 * esquerdo está pesado à direita (fator < 0).
 * Solução: rotaciona filho esquerdo para a esquerda, depois y para direita.
 *
 *     z              z             x
 *    / \            / \           / \
 *   y   T4  →     x   T4  →    y     z
 *  / \           / \           / \   / \
 * T1   x        y  T3         T1 T2 T3 T4
 *     / \      / \
 *    T2  T3   T1  T2
 */
static AVLNode *rotacao_esquerda_direita(AVLNode *z) {
    z->esq = rotacao_esquerda(z->esq); /* 1ª: rotação esquerda no filho */
    return rotacao_direita(z);          /* 2ª: rotação direita na raiz   */
}

/*
 * Rotação dupla DIREITA-ESQUERDA (Right-Left Rotation)
 * Usada quando fator < -1 (pesado à direita) mas o filho
 * direito está pesado à esquerda (fator > 0).
 * Solução: rotaciona filho direito para a direita, depois z para a esquerda.
 *
 *   z                z               x
 *  / \              / \             / \
 * T1   y    →     T1   x    →    z     y
 *     / \             / \       / \   / \
 *    x  T4           T2  y     T1 T2 T3 T4
 *   / \                 / \
 *  T2  T3              T3  T4
 */
static AVLNode *rotacao_direita_esquerda(AVLNode *z) {
    z->dir = rotacao_direita(z->dir); /* 1ª: rotação direita no filho  */
    return rotacao_esquerda(z);        /* 2ª: rotação esquerda na raiz  */
}

/* ----------------------------------------------------------
 * Rebalanceamento
 * Chamado após inserção para corrigir desequilíbrios.
 * ---------------------------------------------------------- */
static AVLNode *rebalancear(AVLNode *no) {
    int fator = avl_fator(no);

    /* --- Caso Esquerda-Esquerda: rotação simples à direita --- */
    if (fator > 1 && avl_fator(no->esq) >= 0)
        return rotacao_direita(no);

    /* --- Caso Esquerda-Direita: rotação dupla LE → RI --- */
    if (fator > 1 && avl_fator(no->esq) < 0)
        return rotacao_esquerda_direita(no);

    /* --- Caso Direita-Direita: rotação simples à esquerda --- */
    if (fator < -1 && avl_fator(no->dir) <= 0)
        return rotacao_esquerda(no);

    /* --- Caso Direita-Esquerda: rotação dupla RI → LE --- */
    if (fator < -1 && avl_fator(no->dir) > 0)
        return rotacao_direita_esquerda(no);

    return no; /* já balanceado */
}

/* ----------------------------------------------------------
 * Criação de nó
 * ---------------------------------------------------------- */
AVLNode *avl_criar_no(const char *palavra, const char *arquivo,
                      int linha, int coluna) {
    AVLNode *no = (AVLNode *)malloc(sizeof(AVLNode));
    if (!no) {
        fprintf(stderr, "Erro: falha ao alocar nó AVL.\n");
        exit(EXIT_FAILURE);
    }

    strncpy(no->palavra, palavra, sizeof(no->palavra) - 1);
    no->palavra[sizeof(no->palavra) - 1] = '\0';
    no->altura = 0;
    no->esq    = NULL;
    no->dir    = NULL;

    /* Cria a primeira ocorrência */
    Ocorrencia *oc = (Ocorrencia *)malloc(sizeof(Ocorrencia));
    if (!oc) {
        fprintf(stderr, "Erro: falha ao alocar ocorrência.\n");
        exit(EXIT_FAILURE);
    }
    strncpy(oc->arquivo, arquivo, sizeof(oc->arquivo) - 1);
    oc->arquivo[sizeof(oc->arquivo) - 1] = '\0';
    oc->linha  = linha;
    oc->coluna = coluna;
    oc->prox   = NULL;

    no->ocorrencias       = oc;
    no->ultima_ocorrencia = oc; /* tail pointer para inserção O(1) */

    return no;
}

/* ----------------------------------------------------------
 * Inserção na AVL
 * nova_palavra: flag de saída — 1 se palavra nova, 0 se já existia
 * ---------------------------------------------------------- */
AVLNode *avl_inserir(AVLNode *raiz, const char *palavra,
                     const char *arquivo, int linha, int coluna,
                     int *nova_palavra) {
    /* --- Caso base: posição vazia → cria nó --- */
    if (raiz == NULL) {
        *nova_palavra = 1;
        return avl_criar_no(palavra, arquivo, linha, coluna);
    }

    int cmp = strcmp(palavra, raiz->palavra);

    if (cmp < 0) {
        /* Vai para a subárvore esquerda */
        raiz->esq = avl_inserir(raiz->esq, palavra, arquivo,
                                linha, coluna, nova_palavra);
    } else if (cmp > 0) {
        /* Vai para a subárvore direita */
        raiz->dir = avl_inserir(raiz->dir, palavra, arquivo,
                                linha, coluna, nova_palavra);
    } else {
        /* Palavra já existe: apenas adiciona nova ocorrência à lista */
        *nova_palavra = 0;

        Ocorrencia *oc = (Ocorrencia *)malloc(sizeof(Ocorrencia));
        if (!oc) {
            fprintf(stderr, "Erro: falha ao alocar ocorrência.\n");
            exit(EXIT_FAILURE);
        }
        strncpy(oc->arquivo, arquivo, sizeof(oc->arquivo) - 1);
        oc->arquivo[sizeof(oc->arquivo) - 1] = '\0';
        oc->linha  = linha;
        oc->coluna = coluna;
        oc->prox   = NULL;

        /* Insere no final via tail pointer (O(1)) */
        raiz->ultima_ocorrencia->prox = oc;
        raiz->ultima_ocorrencia       = oc;

        return raiz; /* nenhum rebalanceamento necessário */
    }

    /* Atualiza altura e rebalanceia no caminho de volta */
    atualizar_altura(raiz);
    return rebalancear(raiz);
}

/* ----------------------------------------------------------
 * Busca na AVL (busca binária padrão)
 * ---------------------------------------------------------- */
AVLNode *avl_buscar(AVLNode *raiz, const char *palavra) {
    if (raiz == NULL) return NULL;

    int cmp = strcmp(palavra, raiz->palavra);

    if (cmp == 0) return raiz;           /* encontrado */
    if (cmp < 0)  return avl_buscar(raiz->esq, palavra);
    return avl_buscar(raiz->dir, palavra);
}

/* ----------------------------------------------------------
 * Contagem de nós (palavras distintas na subárvore)
 * ---------------------------------------------------------- */
int avl_contar_nos(AVLNode *raiz) {
    if (raiz == NULL) return 0;
    return 1 + avl_contar_nos(raiz->esq) + avl_contar_nos(raiz->dir);
}

/* ----------------------------------------------------------
 * Contagem de ocorrências totais na subárvore
 * ---------------------------------------------------------- */
int avl_contar_ocorrencias(AVLNode *raiz) {
    if (raiz == NULL) return 0;

    int cnt = 0;
    Ocorrencia *oc = raiz->ocorrencias;
    while (oc) { cnt++; oc = oc->prox; }

    return cnt + avl_contar_ocorrencias(raiz->esq)
               + avl_contar_ocorrencias(raiz->dir);
}

/* ----------------------------------------------------------
 * Impressão em ordem (debug / listagem)
 * ---------------------------------------------------------- */
void avl_imprimir_em_ordem(AVLNode *raiz) {
    if (raiz == NULL) return;
    avl_imprimir_em_ordem(raiz->esq);
    printf("  [%s]\n", raiz->palavra);
    avl_imprimir_em_ordem(raiz->dir);
}

/* ----------------------------------------------------------
 * Liberação de memória
 * ---------------------------------------------------------- */
void avl_liberar(AVLNode *raiz) {
    if (raiz == NULL) return;

    /* Percurso pós-ordem para liberar filhos antes do pai */
    avl_liberar(raiz->esq);
    avl_liberar(raiz->dir);

    /* Libera a lista de ocorrências */
    Ocorrencia *oc = raiz->ocorrencias;
    while (oc) {
        Ocorrencia *tmp = oc->prox;
        free(oc);
        oc = tmp;
    }

    free(raiz);
}
