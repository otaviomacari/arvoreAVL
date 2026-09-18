# Indexador de Textos com Árvore AVL

Este projeto implementa um **sistema de indexação e busca de palavras em arquivos de texto**, desenvolvido em linguagem C e utilizando **Árvores AVL** como principal estrutura de dados.

O sistema percorre uma pasta contendo arquivos `.txt`, identifica e normaliza as palavras encontradas, armazena cada palavra em uma estrutura AVL e registra todas as ocorrências da palavra, incluindo o arquivo, a linha e a coluna em que ela apareceu.

O projeto também disponibiliza um menu interativo que permite realizar a indexação de uma pasta, pesquisar palavras e consultar estatísticas sobre o índice criado.

O principal objetivo é demonstrar, na prática, a utilização de **Árvores AVL, listas encadeadas, busca binária, recursividade, ponteiros, alocação dinâmica e manipulação de arquivos e diretórios em C**.

---

# Objetivos

O sistema foi desenvolvido para:

* Indexar arquivos de texto com extensão `.txt`;
* Percorrer uma pasta e identificar os arquivos disponíveis;
* Extrair palavras dos arquivos;
* Normalizar as palavras para facilitar as buscas;
* Converter letras maiúsculas para minúsculas;
* Remover acentos das palavras;
* Remover pontuação e outros caracteres não utilizados;
* Organizar as palavras em Árvores AVL;
* Separar as palavras em 26 árvores, de acordo com a primeira letra;
* Registrar todas as ocorrências de cada palavra;
* Informar o arquivo, linha e coluna de cada ocorrência;
* Realizar buscas utilizando a propriedade de busca binária da AVL;
* Exibir estatísticas do índice;
* Liberar corretamente a memória alocada.

---

# Funcionamento geral

O funcionamento do sistema pode ser resumido da seguinte forma:

```text
                 Arquivos .txt
                       |
                       v
                Leitura dos arquivos
                       |
                       v
                 Extração das palavras
                       |
                       v
                  Normalização
                       |
                       v
              Identificação da 1ª letra
                       |
          +------------+------------+
          |            |            |
          v            v            v
        AVL A        AVL B        ... AVL Z
          |            |              |
          +------------+--------------+
                       |
                       v
              Busca de uma palavra
                       |
                       v
             Localização das ocorrências
                       |
                       v
              Arquivo / Linha / Coluna
```

O sistema utiliza **26 árvores AVL independentes**, uma para cada letra do alfabeto.

Por exemplo:

```text
indice[0] → palavras iniciadas com A
indice[1] → palavras iniciadas com B
indice[2] → palavras iniciadas com C
...
indice[25] → palavras iniciadas com Z
```

Essa divisão reduz o espaço de busca necessário para uma palavra específica.

---

# Estrutura do projeto

O projeto é dividido em quatro arquivos principais:

```text
.
├── avl.c
├── avl.h
├── indexador.c
├── indexador.h
└── main.c
```

Cada arquivo possui uma responsabilidade específica.

| Arquivo       | Responsabilidade                              |
| ------------- | --------------------------------------------- |
| `avl.c`       | Implementação da Árvore AVL                   |
| `avl.h`       | Estruturas e protótipos da AVL                |
| `indexador.c` | Indexação, normalização, busca e estatísticas |
| `indexador.h` | Interface do módulo de indexação              |
| `main.c`      | Menu e interação com o usuário                |

---

# 1. Estrutura `Ocorrencia`

A estrutura `Ocorrencia` está definida em `avl.h`:

```c
typedef struct Ocorrencia {
    char arquivo[256];
    int  linha;
    int  coluna;
    struct Ocorrencia *prox;
} Ocorrencia;
```

Ela representa uma ocorrência de uma determinada palavra dentro dos arquivos indexados.

Cada ocorrência armazena:

* Nome do arquivo;
* Número da linha;
* Número da coluna;
* Ponteiro para a próxima ocorrência.

Por exemplo, se a palavra `computador` aparecer várias vezes:

```text
computador
├── arquivo: texto1.txt
│   ├── linha: 2
│   └── coluna: 10
│
├── arquivo: texto1.txt
│   ├── linha: 8
│   └── coluna: 5
│
└── arquivo: texto2.txt
    ├── linha: 3
    └── coluna: 15
```

Essas ocorrências são armazenadas em uma **lista encadeada**.

---

# 2. Estrutura `AVLNode`

Cada palavra distinta é armazenada em um nó da AVL:

```c
typedef struct AVLNode {
    char palavra[100];
    int altura;
    Ocorrencia *ocorrencias;
    Ocorrencia *ultima_ocorrencia;
    struct AVLNode *esq;
    struct AVLNode *dir;
} AVLNode;
```

Os principais campos são:

| Campo               | Função                            |
| ------------------- | --------------------------------- |
| `palavra`           | Palavra armazenada no nó          |
| `altura`            | Altura utilizada no balanceamento |
| `ocorrencias`       | Início da lista de ocorrências    |
| `ultima_ocorrencia` | Final da lista de ocorrências     |
| `esq`               | Filho esquerdo                    |
| `dir`               | Filho direito                     |

A palavra funciona como a **chave de ordenação da árvore**.

A comparação entre palavras é realizada através de:

```c
strcmp()
```

---

# 3. Lista de ocorrências

Um dos diferenciais deste projeto é que a AVL não armazena apenas a existência de uma palavra.

Ela também armazena todas as vezes em que aquela palavra apareceu.

Por exemplo:

```text
                "dados"
                   |
             AVLNode
                   |
                   v
             Ocorrência 1
                   |
                   v
             Ocorrência 2
                   |
                   v
             Ocorrência 3
                   |
                  NULL
```

Dessa forma, uma única palavra pode possuir diversas ocorrências em diferentes arquivos, linhas e colunas.

---

# 4. Ponteiro para a última ocorrência

A estrutura possui dois ponteiros:

```c
Ocorrencia *ocorrencias;
Ocorrencia *ultima_ocorrencia;
```

O primeiro aponta para o início da lista.

O segundo aponta diretamente para o último elemento.

Isso permite inserir uma nova ocorrência no final da lista em tempo constante:

```text
O(1)
```

Sem o ponteiro `ultima_ocorrencia`, seria necessário percorrer toda a lista até encontrar seu último elemento.

---

# 5. Altura da árvore

A função:

```c
int avl_altura(AVLNode *no)
```

retorna a altura de um determinado nó.

Quando o nó é `NULL`, a função retorna:

```c
-1
```

Para um nó folha:

```text
altura = 0
```

A altura é fundamental para determinar se a árvore está balanceada.

---

# 6. Fator de balanceamento

O fator de balanceamento é calculado através da função:

```c
int avl_fator(AVLNode *no)
```

A fórmula utilizada é:

```text
Fator = altura(esquerda) - altura(direita)
```

Assim:

```text
Fator > 0 → mais pesada à esquerda
Fator < 0 → mais pesada à direita
Fator = 0 → alturas iguais
```

Uma árvore AVL deve manter o fator de balanceamento entre:

```text
-1 ≤ fator ≤ 1
```

Quando esse limite é ultrapassado, é necessário realizar uma rotação.

---

# 7. Atualização da altura

Sempre que uma inserção modifica a estrutura da árvore, a altura dos nós envolvidos precisa ser atualizada.

A função responsável por isso é:

```c
static void atualizar_altura(AVLNode *no)
```

A fórmula utilizada é:

```text
altura = 1 + max(altura(esquerda), altura(direita))
```

No código:

```c
no->altura =
    1 + max(avl_altura(no->esq),
            avl_altura(no->dir));
```

---

# 8. Rotações da AVL

Quando uma árvore AVL fica desbalanceada, são utilizadas rotações para restaurar seu equilíbrio.

O projeto implementa quatro situações:

* Esquerda-Esquerda;
* Esquerda-Direita;
* Direita-Direita;
* Direita-Esquerda.

---

# 9. Rotação simples à direita

A rotação à direita é utilizada principalmente no caso **Esquerda-Esquerda (LL)**.

Antes:

```text
       y
      / \
     x   T3
    / \
   T1  T2
```

Depois:

```text
       x
      / \
     T1  y
        / \
       T2  T3
```

No código:

```c
static AVLNode *rotacao_direita(AVLNode *y)
```

A operação principal é:

```c
x->dir = y;
y->esq = T2;
```

Depois das alterações, as alturas são atualizadas.

---

# 10. Rotação simples à esquerda

A rotação à esquerda é utilizada principalmente no caso **Direita-Direita (RR)**.

Antes:

```text
     x
    / \
   T1  y
      / \
     T2  T3
```

Depois:

```text
       y
      / \
     x   T3
    / \
   T1  T2
```

No código:

```c
static AVLNode *rotacao_esquerda(AVLNode *x)
```

A operação principal é:

```c
y->esq = x;
x->dir = T2;
```

Depois, as alturas são atualizadas.

---

# 11. Rotação dupla Esquerda-Direita

O caso **Esquerda-Direita (LR)** acontece quando:

```text
fator > 1
```

e o filho esquerdo está pesado para a direita.

A solução consiste em duas rotações:

```text
1. Rotação à esquerda no filho esquerdo
2. Rotação à direita no nó desbalanceado
```

No código:

```c
static AVLNode *rotacao_esquerda_direita(AVLNode *z) {
    z->esq = rotacao_esquerda(z->esq);
    return rotacao_direita(z);
}
```

---

# 12. Rotação dupla Direita-Esquerda

O caso **Direita-Esquerda (RL)** acontece quando:

```text
fator < -1
```

e o filho direito está pesado para a esquerda.

A solução consiste em:

```text
1. Rotação à direita no filho direito
2. Rotação à esquerda no nó desbalanceado
```

No código:

```c
static AVLNode *rotacao_direita_esquerda(AVLNode *z) {
    z->dir = rotacao_direita(z->dir);
    return rotacao_esquerda(z);
}
```

---

# 13. Rebalanceamento automático

A função:

```c
static AVLNode *rebalancear(AVLNode *no)
```

analisa o fator de balanceamento e decide qual rotação deve ser realizada.

Os quatro casos são:

```text
Fator > 1
    |
    +-- filho esquerdo >= 0 → Rotação à direita
    |
    +-- filho esquerdo < 0  → Rotação Esquerda-Direita

Fator < -1
    |
    +-- filho direito <= 0 → Rotação à esquerda
    |
    +-- filho direito > 0  → Rotação Direita-Esquerda
```

Dessa maneira, o balanceamento da árvore acontece automaticamente após as inserções.

---

# 14. Criação de um nó AVL

A função:

```c
AVLNode *avl_criar_no(
    const char *palavra,
    const char *arquivo,
    int linha,
    int coluna
)
```

cria um novo nó da AVL.

Primeiramente, é feita a alocação dinâmica:

```c
AVLNode *no =
    (AVLNode *)malloc(sizeof(AVLNode));
```

Depois, a palavra é copiada para o nó:

```c
strncpy(no->palavra,
        palavra,
        sizeof(no->palavra) - 1);
```

O nó começa com altura `0` e sem filhos:

```c
no->altura = 0;
no->esq = NULL;
no->dir = NULL;
```

Também é criada a primeira ocorrência da palavra.

---

# 15. Inserção na AVL

A função:

```c
AVLNode *avl_inserir(...)
```

realiza a inserção de uma palavra na árvore.

A comparação é feita utilizando:

```c
int cmp = strcmp(palavra, raiz->palavra);
```

Se:

```text
cmp < 0
```

a palavra é menor e deve ser inserida na subárvore esquerda.

Se:

```text
cmp > 0
```

a palavra é maior e deve ser inserida na subárvore direita.

Se:

```text
cmp == 0
```

a palavra já existe.

Nesse caso, não é criado um novo nó AVL.

Em vez disso, apenas uma nova ocorrência é adicionada à lista encadeada.

---

# 16. Identificação de palavra nova

A função de inserção recebe:

```c
int *nova_palavra
```

Esse parâmetro funciona como uma informação de saída.

Quando uma nova palavra é criada:

```c
*nova_palavra = 1;
```

Quando a palavra já existe:

```c
*nova_palavra = 0;
```

Isso permite que o código que chama a função saiba se encontrou uma palavra inédita ou apenas uma nova ocorrência de uma palavra já existente.

---

# 17. Busca binária na AVL

A função:

```c
AVLNode *avl_buscar(
    AVLNode *raiz,
    const char *palavra
)
```

realiza uma busca binária na árvore.

Primeiramente, a palavra procurada é comparada com a palavra armazenada no nó atual.

```c
int cmp = strcmp(palavra, raiz->palavra);
```

Se forem iguais:

```c
return raiz;
```

Se a palavra procurada for menor:

```c
return avl_buscar(raiz->esq, palavra);
```

Caso seja maior:

```c
return avl_buscar(raiz->dir, palavra);
```

Como a árvore é balanceada, a busca possui complexidade:

```text
O(log n)
```

em condições de balanceamento da AVL.

---

# 18. Normalização das palavras

Antes de uma palavra ser armazenada ou pesquisada, ela passa por um processo de normalização.

A função principal é:

```c
void normalizar_palavra(
    const char *src,
    char *dest,
    int dest_size
)
```

A normalização transforma as palavras para uma forma padronizada.

Por exemplo:

```text
"Computação" → "computacao"
"COMPUTADOR" → "computador"
"árvore"     → "arvore"
"ÁRVORE"     → "arvore"
```

Isso permite que diferentes formas de escrita sejam tratadas como a mesma palavra.

---

# 19. Conversão de maiúsculas e minúsculas

A função:

```c
char normalizar_char(unsigned char c)
```

trata caracteres ASCII.

Letras maiúsculas são convertidas para minúsculas:

```c
if (c >= 'A' && c <= 'Z')
    return (char)(c + 32);
```

Letras minúsculas são mantidas.

Os números também são mantidos:

```c
if (c >= '0' && c <= '9')
    return (char)c;
```

Outros caracteres são descartados.

---

# 20. Tratamento de acentos

O sistema também possui uma função para converter caracteres acentuados para suas respectivas letras básicas:

```c
static char codepoint_para_ascii(unsigned int cp)
```

Por exemplo:

```text
á → a
à → a
â → a
ã → a

é → e
ê → e

í → i

ó → o
ô → o
õ → o

ú → u
ü → u

ç → c
```

Dessa forma:

```text
"ação"
```

é normalizada para:

```text
"acao"
```

---

# 21. Processamento de UTF-8

Como os arquivos podem utilizar caracteres UTF-8, a função `normalizar_palavra()` verifica o tamanho das sequências de bytes.

O código trata:

```text
ASCII          → 1 byte
UTF-8 comum    → 2 bytes
Outras UTF-8   → descartadas
```

Para caracteres acentuados comuns do português, o código calcula o *codepoint* e utiliza `codepoint_para_ascii()` para obter a letra equivalente.

---

# 22. Indexação de arquivos

A função:

```c
void indexar_arquivo(
    const char *caminho_arquivo,
    const char *nome_arquivo
)
```

abre um arquivo utilizando:

```c
fopen(caminho_arquivo, "r");
```

O conteúdo é lido linha por linha através de:

```c
fgets()
```

Para cada linha, o programa mantém:

```text
num_linha
col
```

que representam a linha e a coluna atuais.

Cada palavra encontrada é normalizada e posteriormente inserida na AVL correspondente.

---

# 23. Identificação da AVL

Depois da normalização, a primeira letra da palavra determina em qual árvore ela será armazenada.

O índice é calculado através de:

```c
int idx = palavra[0] - 'a';
```

Exemplo:

```text
"arvore" → 'a' - 'a' = 0
"banana" → 'b' - 'a' = 1
"computador" → 'c' - 'a' = 2
```

Assim:

```text
indice[0] → A
indice[1] → B
indice[2] → C
...
indice[25] → Z
```

---

# 24. Indexação de uma pasta

A função:

```c
void indexar_pasta(const char *path)
```

é responsável por procurar os arquivos `.txt` dentro de uma determinada pasta.

Para acessar os diretórios, são utilizadas funções POSIX:

```c
opendir()
readdir()
closedir()
```

O código ignora:

```text
.
..
```

e verifica se a extensão do arquivo é `.txt`.

A comparação é feita de forma *case-insensitive*, permitindo extensões como:

```text
.txt
.TXT
.Txt
```

---

# 25. Verificação de arquivos regulares

Depois de encontrar um arquivo com extensão `.txt`, o programa utiliza:

```c
stat()
```

para verificar se a entrada realmente corresponde a um arquivo regular.

A verificação é feita através de:

```c
S_ISREG(st.st_mode)
```

Isso evita tentar indexar diretórios ou outros tipos de entrada.

---

# 26. Busca de uma palavra

A busca é realizada através da função:

```c
void buscar_palavra(const char *termo)
```

Primeiramente, o termo é normalizado.

Por exemplo:

```text
"ÁRVORE"
```

torna-se:

```text
"arvore"
```

Depois, a primeira letra determina qual das 26 árvores deve ser consultada.

Por exemplo:

```text
"computador"
```

começa com `c`, portanto:

```c
indice[2]
```

será utilizada.

A busca é então realizada através de:

```c
avl_buscar(indice[idx], normalizado);
```

---

# 27. Exibição das ocorrências

Quando a palavra é encontrada, o sistema percorre sua lista de ocorrências:

```c
Ocorrencia *oc = no->ocorrencias;

while (oc) {
    ...
    oc = oc->prox;
}
```

Para cada ocorrência, são exibidos:

```text
Arquivo
Linha
Coluna
```

Um resultado pode ser apresentado desta forma:

```text
Palavra encontrada: computador

  Arquivo: texto1.txt
  Linha:   4
  Coluna:  12

  Arquivo: texto2.txt
  Linha:   8
  Coluna:  3
```

---

# 28. Estatísticas do índice

A função:

```c
void exibir_estatisticas(void)
```

apresenta informações sobre cada uma das 26 árvores.

Para cada letra, são calculados:

* Número de palavras distintas;
* Número total de ocorrências;
* Altura da AVL.

Por exemplo:

```text
Letra 'a':   15 palavras |    42 ocorrências | altura AVL: 4
Letra 'b':   10 palavras |    31 ocorrências | altura AVL: 3
Letra 'c':   18 palavras |    57 ocorrências | altura AVL: 5
```

Ao final, são apresentados os totais:

```text
Total de palavras distintas : ...
Total de ocorrências        : ...
```

---

# 29. Contagem de palavras distintas

A função:

```c
int avl_contar_nos(AVLNode *raiz)
```

conta quantos nós existem em uma determinada árvore.

Como cada nó representa uma palavra diferente, essa função representa a quantidade de **palavras distintas**.

A implementação é recursiva:

```c
return 1
     + avl_contar_nos(raiz->esq)
     + avl_contar_nos(raiz->dir);
```

---

# 30. Contagem de ocorrências

A função:

```c
int avl_contar_ocorrencias(AVLNode *raiz)
```

percorre cada nó da AVL e, para cada palavra, percorre sua lista de ocorrências.

Dessa maneira, é possível obter o número total de vezes que as palavras apareceram nos arquivos.

A contagem considera:

```text
Palavras distintas
        +
Todas as ocorrências dessas palavras
```

---

# 31. Impressão em ordem

A função:

```c
void avl_imprimir_em_ordem(AVLNode *raiz)
```

realiza um percurso **em ordem**:

```text
Esquerda
   ↓
Raiz
   ↓
Direita
```

Como a AVL é uma árvore de busca binária, o resultado aparece em ordem lexicográfica:

```text
algoritmo
arvore
arquivo
banana
busca
computador
...
```

Essa função é principalmente útil para depuração e visualização das palavras armazenadas.

---

# 32. Liberação da memória

O projeto utiliza alocação dinâmica através de `malloc()`.

Por isso, toda a memória precisa ser liberada ao final da execução.

A função:

```c
void avl_liberar(AVLNode *raiz)
```

utiliza um percurso em pós-ordem:

```text
1. Libera subárvore esquerda
2. Libera subárvore direita
3. Libera o nó atual
```

Depois disso, também libera a lista de ocorrências associada à palavra.

---

# 33. Liberação do índice completo

A função:

```c
void liberar_indice(void)
```

percorre as 26 árvores:

```c
for (int i = 0; i < 26; i++) {
    avl_liberar(indice[i]);
    indice[i] = NULL;
}
```

Dessa forma, toda a estrutura criada durante a indexação é liberada.

O contador global também é reiniciado:

```c
total_ocorrencias = 0;
```

---

# 34. Interface do sistema

O arquivo `main.c` funciona como ponto de entrada da aplicação.

Inicialmente, as 26 árvores são configuradas como vazias:

```c
for (int i = 0; i < 26; i++)
    indice[i] = NULL;
```

Depois, o sistema apresenta um menu interativo.

```text
┌─────────────────────────────┐
│  1 - Indexar pasta          │
│  2 - Buscar palavra         │
│  3 - Exibir estatísticas    │
│  4 - Sair                   │
└─────────────────────────────┘
```

---

# 35. Opção 1 — Indexar pasta

Ao escolher:

```text
1 - Indexar pasta
```

o sistema solicita o caminho da pasta.

Se o usuário simplesmente pressionar `Enter`, é utilizada a pasta padrão:

```text
./textos_exemplo
```

Caso já exista um índice, o programa pergunta se o usuário deseja limpar o índice anterior e realizar uma nova indexação.

---

# 36. Opção 2 — Buscar palavra

A opção:

```text
2 - Buscar palavra
```

permite ao usuário informar um termo.

O sistema verifica se uma pasta já foi indexada.

Depois:

```text
Termo digitado
      ↓
Normalização
      ↓
Identificação da primeira letra
      ↓
Seleção da AVL
      ↓
Busca binária
      ↓
Lista de ocorrências
```

---

# 37. Opção 3 — Estatísticas

A opção:

```text
3 - Exibir estatísticas
```

apresenta informações sobre o índice atual.

São exibidos dados por letra e também os totais gerais.

Isso permite analisar a distribuição das palavras entre as 26 árvores AVL.

---

# 38. Opção 4 — Sair

Ao escolher:

```text
4 - Sair
```

o programa libera toda a memória utilizada:

```c
liberar_indice();
```

e encerra a execução.

Essa etapa é importante para evitar vazamentos de memória.

---

# Complexidade

Considerando `n` como o número de palavras distintas armazenadas em uma AVL:

## Inserção

A inserção em uma AVL possui complexidade:

```text
O(log n)
```

pois a árvore permanece balanceada.

## Busca

A busca também possui complexidade:

```text
O(log n)
```

em uma AVL balanceada.

## Rotação

Cada rotação possui custo:

```text
O(1)
```

pois envolve uma quantidade constante de alterações de ponteiros.

## Inserção de ocorrência

Quando uma palavra já existe, a nova ocorrência é adicionada utilizando o ponteiro:

```c
ultima_ocorrencia
```

Portanto, essa operação possui custo:

```text
O(1)
```

## Percurso para estatísticas

A contagem dos nós percorre toda a árvore:

```text
O(n)
```

A contagem das ocorrências depende também da quantidade total de ocorrências armazenadas.

---

# Organização das estruturas

A arquitetura do projeto pode ser visualizada da seguinte maneira:

```text
                       main.c
                         |
                         v
                  indexador.h
                         |
                         v
                  indexador.c
                    /        \
                   /          \
                  v            v
              AVL Tree      Arquivos
                  |
                  v
               avl.h
                  |
                  v
               avl.c
             /         \
            v           v
        AVLNode     Ocorrencia
                      |
                      v
                Lista encadeada
```

O projeto separa as responsabilidades entre os módulos, facilitando a manutenção e organização do código.

---

# Estruturas de Dados utilizadas

O projeto combina diferentes estruturas de dados.

## Árvore AVL

Utilizada para armazenar e pesquisar palavras de maneira eficiente.

## Lista encadeada

Utilizada para armazenar todas as ocorrências de uma palavra.

## Vetor

Utilizado para armazenar as 26 raízes das árvores:

```c
AVLNode *indice[26];
```

## Ponteiros

Utilizados para conectar:

* Nós da AVL;
* Filhos esquerdo e direito;
* Elementos da lista de ocorrências.

## Alocação dinâmica

Utilizada para criar:

* Nós AVL;
* Nós de ocorrência.

A memória é gerenciada utilizando:

```c
malloc()
free()
```

---

# Principais funções

## Módulo AVL

| Função                     | Responsabilidade                 |
| -------------------------- | -------------------------------- |
| `avl_criar_no()`           | Cria um novo nó AVL              |
| `avl_inserir()`            | Insere palavra ou ocorrência     |
| `avl_buscar()`             | Realiza busca binária            |
| `avl_altura()`             | Retorna a altura                 |
| `avl_fator()`              | Calcula o fator de balanceamento |
| `avl_contar_nos()`         | Conta palavras distintas         |
| `avl_contar_ocorrencias()` | Conta ocorrências                |
| `avl_imprimir_em_ordem()`  | Imprime palavras em ordem        |
| `avl_liberar()`            | Libera a árvore e ocorrências    |

## Módulo Indexador

| Função                  | Responsabilidade                    |
| ----------------------- | ----------------------------------- |
| `normalizar_char()`     | Normaliza caracteres ASCII          |
| `normalizar_palavra()`  | Normaliza palavras completas        |
| `indexar_arquivo()`     | Indexa um arquivo                   |
| `indexar_pasta()`       | Indexa arquivos `.txt` de uma pasta |
| `buscar_palavra()`      | Procura uma palavra no índice       |
| `exibir_estatisticas()` | Exibe estatísticas                  |
| `liberar_indice()`      | Libera todas as árvores             |

---

# Como executar

O projeto utiliza recursos POSIX, como:

```c
dirent.h
sys/stat.h
```

Por isso, a execução é direcionada principalmente para ambientes Linux/macOS ou ambientes compatíveis com POSIX.

Compile todos os arquivos juntos:

```bash
gcc -std=c99 -Wall -Wextra -O2 main.c avl.c indexador.c -o indexador
```

Depois execute:

```bash
./indexador
```

Em sistemas Windows utilizando um ambiente compatível com GCC/POSIX, o comando de execução pode ser:

```bash
indexador.exe
```

---

# Estrutura esperada da pasta de textos

O programa pode utilizar uma pasta como:

```text
textos_exemplo/
├── texto1.txt
├── texto2.txt
├── historia.txt
└── computacao.txt
```

Ao selecionar a opção de indexação, o sistema percorre os arquivos `.txt` encontrados nessa pasta.

---

# Exemplo de utilização

Inicialmente, o programa apresenta:

```text
╔══════════════════════════════════════╗
║    INDEXADOR DE TEXTOS COM AVL       ║
║  Estrutura de Dados — PUC Minas      ║
╚══════════════════════════════════════╝
```

Depois:

```text
┌─────────────────────────────┐
│  1 - Indexar pasta          │
│  2 - Buscar palavra         │
│  3 - Exibir estatísticas    │
│  4 - Sair                   │
└─────────────────────────────┘
Opção:
```

O usuário pode selecionar a opção `1`, informar o caminho da pasta e aguardar a indexação.

Depois disso, pode selecionar:

```text
2 - Buscar palavra
```

e informar, por exemplo:

```text
computacao
```

O sistema procura a palavra na AVL correspondente à letra `C` e apresenta todas as ocorrências encontradas.

---

# Fluxo completo de uma palavra

Considere a palavra:

```text
"Computação,"
```

Durante a indexação:

```text
"Computação,"
       ↓
Extração do token
       ↓
"Computação"
       ↓
Normalização
       ↓
"computacao"
       ↓
Primeira letra = 'c'
       ↓
indice[2]
       ↓
Busca na AVL
       ↓
Palavra já existe?
     /          \
   Não           Sim
   |              |
Cria nó       Adiciona ocorrência
   |              |
   +-------> Lista de ocorrências
```

Esse fluxo permite que palavras com diferenças de capitalização, acentuação e pontuação sejam tratadas de maneira padronizada.

---

# Diferencial da implementação

Um dos principais aspectos deste projeto é a combinação de duas estruturas de dados:

```text
                 AVL
                  |
                  v
              Palavra
                  |
                  v
        Lista de ocorrências
```

A AVL é responsável por encontrar rapidamente a palavra.

A lista encadeada é responsável por armazenar todas as posições onde essa palavra apareceu.

Além disso, a divisão em 26 árvores:

```text
A → AVL
B → AVL
C → AVL
...
Z → AVL
```

permite restringir a busca à árvore correspondente à primeira letra da palavra.

---

# Conceitos de Estruturas de Dados aplicados

Este projeto trabalha diretamente com:

* Árvore Binária de Busca;
* Árvore AVL;
* Balanceamento de árvores;
* Rotações simples;
* Rotações duplas;
* Fator de balanceamento;
* Altura de árvores;
* Busca binária;
* Listas encadeadas;
* Vetores;
* Ponteiros;
* Recursividade;
* Alocação dinâmica;
* Gerenciamento de memória;
* Manipulação de arquivos;
* Manipulação de diretórios;
* Strings;
* Normalização de caracteres;
* Codificação UTF-8;
* Análise de complexidade.

---

# Conclusão

Este projeto apresenta uma aplicação prática de **Árvores AVL para indexação e busca de palavras em arquivos de texto**.

A utilização da AVL permite manter as palavras organizadas e realizar buscas de maneira eficiente. Cada palavra possui uma lista encadeada contendo todas as suas ocorrências, permitindo identificar exatamente em qual arquivo, linha e coluna o termo foi encontrado.

A utilização de 26 árvores, uma para cada letra inicial, acrescenta uma etapa de organização ao índice e restringe a busca à estrutura correspondente.

O projeto também demonstra conceitos importantes da linguagem C, como **ponteiros, structs, recursividade, alocação dinâmica, gerenciamento de memória, manipulação de arquivos e diretórios e tratamento de strings**.

Dessa forma, a implementação integra diferentes estruturas de dados e conceitos de programação em um sistema completo de **indexação, busca e análise de textos**.
