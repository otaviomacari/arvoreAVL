#ifndef INDEXADOR_H
#define INDEXADOR_H

/* =========================================================
 * indexador.h  –  Interface do módulo de indexação de textos
 * ========================================================= */

#include "avl.h"

/* Vetor global de 26 AVLs (uma por letra inicial) */
extern AVLNode *indice[26];

/* Total global de ocorrências (atualizado durante indexação) */
extern int total_ocorrencias;

/* ----------------------------------------------------------
 * Normalização
 * ---------------------------------------------------------- */

/* Converte c para minúsculo, remove acento → retorna char ASCII puro
 * Retorna '\0' para caracteres que não devem ser incluídos em palavras */
char normalizar_char(unsigned char c);

/* Normaliza uma string inteira; resultado em dest (tamanho max dest_size) */
void normalizar_palavra(const char *src, char *dest, int dest_size);

/* ----------------------------------------------------------
 * Indexação
 * ---------------------------------------------------------- */

/* Indexa todos os arquivos .txt dentro da pasta path */
void indexar_pasta(const char *path);

/* Indexa um único arquivo */
void indexar_arquivo(const char *caminho_arquivo, const char *nome_arquivo);

/* ----------------------------------------------------------
 * Busca
 * ---------------------------------------------------------- */

/* Normaliza o termo, seleciona a AVL pela 1ª letra e executa busca */
void buscar_palavra(const char *termo);

/* ----------------------------------------------------------
 * Estatísticas
 * ---------------------------------------------------------- */
void exibir_estatisticas(void);

/* ----------------------------------------------------------
 * Liberação de memória
 * ---------------------------------------------------------- */
void liberar_indice(void);

#endif /* INDEXADOR_H */
