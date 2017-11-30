#include <stdio.h>

typedef struct {
  int chave;
  char nome[21];
  int idade;
  int next;
  int previous;
} registro;

#define TAMANHO_ARQUIVO 11
#define D 100
#define L 108

FILE *fptr;
char filename[6];
int num;
char type, action;
registro r;
registro t[TAMANHO_ARQUIVO];
int lastEmpty;

registro emptyRegister() {
  registro empty;
  empty.chave = -1;
  empty.idade = -1;
  empty.next = -1;
  empty.previous = -1;
  return empty;
}

/**
 * Escreve um registro no arquivo
 * @param r registro a ser inserido
 * @param i posição a inserir
 */
void insertLine(registro r, int i) {
  fptr = fopen(filename, "r+");
  fseek(fptr, i*sizeof(r), SEEK_SET);
  fwrite(&r, sizeof(registro), 1, fptr);
  fclose(fptr);
}

/**
 * Imprime um registro
 * @param p   registro a ser impresso
 */
void printLine(registro p) {
  printf("chave: %d\n", p.chave);
  printf("%s\n", p.nome);
  printf("%d\n", p.idade);
}

/**
 * Lê um registro no arquivo
 * @param  i posição para leitura
 * @return   registro
 */
registro readLine(int i) {
  registro n;
  fptr = fopen(filename, "r+");

  fseek(fptr, i*sizeof(r), SEEK_SET);
  fread(&n, sizeof(registro), 1, fptr);
  return n;
}

/**
 * Identifica qual é a última posição vazia
 * @param starts ponto inicial de busca
 */
void indentifyLastEmpty(int starts) {
  int j;
  registro n;
  for (j = starts; j >= 0; j--) {
    n = readLine(j);
    if (n.chave == -1) {
      lastEmpty = j;
      return;
    }
  }
}

/**
 * Imprime conteúdo do arquivo
 */
void printFile() {
  int i;
  registro p;

  for (i = 0; i < TAMANHO_ARQUIVO; i++) {
    p = readLine(i);
    if (p.chave != - 1) {
      if (type == D) {
        printf("%d: %d %s %d\n", i, p.chave, p.nome, p.idade);
      } else if (type == L) {
        if (p.next == -1)
          printf("%d: %d %s %d nulo %d\n", i, p.chave, p.nome, p.idade, p.previous);
        else
          printf("%d: %d %s %d %d %d\n", i, p.chave, p.nome, p.idade, p.next, p.previous);
      }
    } else {
      printf("%d: vazio\n", i);
    }
  }
}

/**
 * Busca por uma chave
 * @param  key chave a ser encontrada
 * @return     posição da chave ou -1
 */
int searchForKey(int key) {
  int i = 0;
  registro c;
  for (; i < TAMANHO_ARQUIVO; i++) {
    c = readLine(i);
    if (c.chave == key) return i;
  }
  return -1;
}

/**
 * Calcula a primeira função de hash
 * @param  chave  valor da chave para hashing
 * @return       valor do hashing
 */
int h1(int chave) {
  return chave % TAMANHO_ARQUIVO;
}

/**
 * Calcula a segunda função de hash
 * @param  chave  valor da chave para hashing
 * @return       valor do hashing
 */
int h2(int chave) {
  int result = (chave/TAMANHO_ARQUIVO) % TAMANHO_ARQUIVO;
  return (result == 0)? 1: result;
}

/**
 * Calcula função de hash para solução
 * de colisões com hash duplo
 * @param  chave  valor da chave para hashing
 * @param  i      número de chamadas da função
 * @return        valor do hashing
 */
int doubleHash(int chave, int i) {
  return ((h1(chave) + i * h2(chave)) % TAMANHO_ARQUIVO);
}

/**
 * Método de inserção para hashing com encadeamento
 */
void lHashing() {
  int count = 0, f, j;
  registro a, b, c;
  f = h1(r.chave);
  a = readLine(f);

  if (a.chave == -1) {
    insertLine(r, f);
    indentifyLastEmpty(lastEmpty);
    return;
  } else if (f != h1(a.chave)) {
    insertLine(r, f);
    insertLine(a, lastEmpty);
    if (a.previous != -1) {
      b = readLine(a.previous);
      b.next = lastEmpty;
      insertLine(b, a.previous);
    }
    if (a.next != -1) {
      c = readLine(a.next);
      c.previous = lastEmpty;
      insertLine(c, a.next);
    }
    indentifyLastEmpty(lastEmpty);
    return;
  }

  while (a.next != -1) {
    f = a.next;
    a = readLine(f);
  }
  a.next = lastEmpty;
  insertLine(a, f);
  r.previous = f;
  insertLine(r, lastEmpty);
  indentifyLastEmpty(lastEmpty);
}

/**
 * Método de inserção para hashing duplo
 */
void dHashing() {
  int count = 0, f;
  for (f = h1(r.chave);;) {
    if (readLine(f).chave == -1) {
      insertLine(r, f);
      count = 0;
      break;
    }
    count++;
    f = doubleHash(r.chave, count);
  }
}

/**
 * Lê o registroda entrada e define
 * o método de inserção no arquivo
 */
void insert() {
  scanf("%d", &r.chave);
  scanf("%20s", &r.nome);
  scanf("%d", &r.idade);
  r.next = -1;
  r.previous = -1;
  if (searchForKey(r.chave) == -1) {
    if (type == L) {
      lHashing();
    } else if (type == D) {
      dHashing();
    }
  } else {
    printf("chave ja existente: %d\n", r.chave);
  }
}

/**
 * Atualiza o antecessor de um registro
 * de índice `current` para `previous`
 * @param  current  Índice do registro
 * @param  previous índice do antecessor
 * @return          registro modificado
 */
registro updatePrevious(int current, int previous) {
  registro n;
  n = readLine(current);
  n.previous = previous;
  insertLine(n, current);
  return n;
}

/**
 * Apaga uma posição do arquivo
 * @param i posição a ser apagada
 */
void clearPosition(int i) {
  insertLine(emptyRegister(), i);
  indentifyLastEmpty(TAMANHO_ARQUIVO);
  return;
}

/**
 * [removeLine description]
 */
void removeLine() {
  int key, i;
  scanf("%d", &key);

  if (type == D) {
    i = searchForKey(key);
    if (i != -1) {
      insertLine(emptyRegister(), i);
    } else {
      printf("chave nao encontrada: %d\n", key);
    }
  } else if (type == L) {
    registro o, c, n;

    i = searchForKey(key);

    if (i != -1) {
      o = readLine(i);

      //  Átomo
      if (o.previous == -1 && o.next == -1) {
        clearPosition(i);
        return;
      }
      // Tem próximo
      if (o.next != -1) {
        n = updatePrevious(o.next, o.previous);
        if (n.next != -1) {
          updatePrevious(n.next, i);
        }
        // É o primeiro elemento;
        if (o.previous == -1) {
          clearPosition(i);
          clearPosition(o.next);
          insertLine(n, i);
          return;
        }
      }

      //Tem antecessor
      if (o.previous != -1) {
        c = readLine(o.previous);
        c.next = o.next;
        insertLine(c, o.previous);
        clearPosition(i);
        return;
      }
    } else {
      printf("chave nao encontrada: %d\n", key);
      return;
    }
  }
}

/**
 * Cria arquivo se não existir
 */
void createFileIfNotExists() {
  fptr = fopen(filename, "rb+");
  if (fptr == NULL) fptr = fopen(filename, "wb");
  fclose(fptr);
}

/**
 * Lê o tipo de hashing da entrada
 */
void scanType() {
  scanf("%c", &type);
  if (type == D) {
    filename[0] = 'D';
  } else if (type == L) {
    filename[0] = 'L';
  }
  filename[1] = '.';
  filename[2] = 'b';
  filename[3] = 'i';
  filename[4] = 'n';
  filename[5] = '\0';
}

/**
 * Verifica se uma determinada
 * chave foi armazenada no arquivo
 */
void consultLine() {
  int key, c;
  scanf("%d", &key);
  c = searchForKey(key);
  if (c != -1) {
    printLine(readLine(c));
  } else {
    printf("chave nao encontrada: %d\n", key);
  }
}

float lMedia() {
  int i = 0, f, items = 0, count = 0, h_count = 0;
  float it, co;
  registro s;
  for (; i < TAMANHO_ARQUIVO; i++) {
    s = readLine(i);
    if (s.chave != -1) {
      for (f = h1(s.chave), h_count = 0; f != i; h_count++) {
        if (s.previous == -1) break;

        f = s.previous;
        s = readLine(f);
        count++;
      }
      count += (h_count == 0)? 1 : h_count;
      items++;
    }
  }
  printf("items: %d count: %d\n", items, count);
  it = items;
  co = count;

  return co/it;
}

float dMedia() {
  int i = 0, f, items = 0, count = 0, h_count = 0;
  float it, co;
  registro s;
  for (; i < TAMANHO_ARQUIVO; i++) {
    s = readLine(i);
    if (s.chave != -1) {
      for (f = h1(s.chave), h_count = 0; f != i; h_count++) {
        f = doubleHash(s.chave, h_count);
      }
      count += (h_count == 0)? 1 : h_count;
      items++;
    }
  }
  it = items;
  co = count;

  return co/it;
}

/**
 * Define o método de cálculo de média e imprime na tela
 */
void showMedia() {
  float result;
  if (type == D) {
    result = dMedia();
  } else if (type == L) {
    result = lMedia();
  }
  printf("%.1f\n", result);
}

/**
 * Gerencia as opções de interação do sistema
 */
void callAction() {
  while(scanf("%c", &action)) {
    if (action == 'e') break;
    switch (action) {
      case 'i':
        insert();
        break;
      case 'c':
        consultLine();
        break;
      case 'r':
        removeLine();
        break;
      case 'p':
        printFile();
        break;
      case 'm':
        showMedia();
        break;
    }
  }
}

/**
 * Verifica se um arquivo não está vazio
 * @return tamanho do arquivo
 */
int notEmptyFile() {
  int size;
  fptr = fopen(filename, "r+");
  fseek(fptr, 0, SEEK_END);
  size = ftell(fptr);
  fclose(fptr);
  return size;
}

/**
 * Zera todo o arquivo
 */
void zeroFill() {
  int i = 0;
  for(; i < TAMANHO_ARQUIVO; i++) {
    insertLine(emptyRegister(), i);
  }
}

int main() {
  scanType();
  createFileIfNotExists();
  if (!notEmptyFile()) {
    zeroFill();
  }
  if (type == L) {
    indentifyLastEmpty(TAMANHO_ARQUIVO);
  }
  callAction();
  return 0;
}
