#ifndef AVL_H
#define AVL_H

/* =========================================================
 * avl.h  –  Definições da Árvore AVL e da Lista de Ocorrências
 * ========================================================= */

/* ----------------------------------------------------------
 * Ocorrência: guarda onde a palavra apareceu no corpus
 * ---------------------------------------------------------- */
typedef struct Ocorrencia {
    char arquivo[256];   /* nome do arquivo                  */
    int  linha;          /* linha dentro do arquivo (1-based) */
    int  coluna;         /* coluna inicial da palavra (1-based)*/
    struct Ocorrencia *prox; /* próxima ocorrência (lista encadeada) */
} Ocorrencia;

/* ----------------------------------------------------------
 * Nó da AVL
 * ---------------------------------------------------------- */
typedef struct AVLNode {
    char      palavra[100];    /* chave: palavra normalizada        */
    int       altura;          /* altura do nó para balanceamento   */
    Ocorrencia *ocorrencias;   /* lista de ocorrências (head)       */
    Ocorrencia *ultima_ocorrencia; /* ponteiro para o fim da lista (inserção O(1)) */
    struct AVLNode *esq;       /* filho esquerdo                    */
    struct AVLNode *dir;       /* filho direito                     */
} AVLNode;

/* ----------------------------------------------------------
 * Protótipos
 * ---------------------------------------------------------- */

/* Cria um novo nó com a palavra e a primeira ocorrência */
AVLNode *avl_criar_no(const char *palavra, const char *arquivo,
                      int linha, int coluna);

/* Insere palavra na árvore; se já existir, só adiciona ocorrência */
AVLNode *avl_inserir(AVLNode *raiz, const char *palavra,
                     const char *arquivo, int linha, int coluna,
                     int *nova_palavra);

/* Busca uma palavra; retorna o nó ou NULL */
AVLNode *avl_buscar(AVLNode *raiz, const char *palavra);

/* Altura (segura para nó NULL) */
int avl_altura(AVLNode *no);

/* Fator de balanceamento de um nó */
int avl_fator(AVLNode *no);

/* Conta nós (palavras distintas) */
int avl_contar_nos(AVLNode *raiz);

/* Conta total de ocorrências na subárvore */
int avl_contar_ocorrencias(AVLNode *raiz);

/* Libera toda a árvore (nós + listas de ocorrências) */
void avl_liberar(AVLNode *raiz);

/* Imprime a árvore em ordem (debug) */
void avl_imprimir_em_ordem(AVLNode *raiz);

#endif /* AVL_H */
