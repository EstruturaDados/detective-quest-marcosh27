#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Definição de Constantes e Estruturas ---

// Define o tamanho da nossa tabela hash. Um número primo geralmente é uma boa escolha.
#define HASH_TABLE_SIZE 11

typedef struct HashNode {
    char pista[150];
    char suspeito[50];
    struct HashNode* next;
} HashNode;

typedef struct PistaNode {
    char pista[150];
    struct PistaNode* esquerda;
    struct PistaNode* direita;
} PistaNode;

typedef struct Sala {
    char nome[50];
    char pista[150];
    struct Sala* esquerda;
    struct Sala* direita;
} Sala;


// --- Protótipos das Funções ---

// Funções do Mapa
Sala* criarSala(const char* nome, const char* pista);
void liberarMapa(Sala* sala);

// Funções da Árvore de Pistas (BST)
PistaNode* adicionarPista(PistaNode* raiz, const char* pistaTexto);
void exibirPistas(PistaNode* raiz);
void liberarPistas(PistaNode* raizPistas);

// Funções da Tabela Hash
unsigned int hashFunction(const char* str);
HashNode** criarTabelaHash();
void inserirNaHash(HashNode** tabela, const char* pista, const char* suspeito);
const char* encontrarSuspeito(HashNode** tabela, const char* pista);
void liberarTabelaHash(HashNode** tabela);

// Funções de Lógica do Jogo
void verificarSuspeitoFinal(PistaNode* arvorePistas, HashNode** tabelaHash);
void contarPistasParaSuspeito(PistaNode* noPista, HashNode** tabela, const char* suspeitoAcusado, int* contador);

int main() {
    // --- Inicialização das Estruturas ---
    Sala* hall = criarSala("Hall de Entrada", "");
    PistaNode* arvorePistas = NULL;
    HashNode** tabelaHash = criarTabelaHash();

    // --- Montagem do Mapa da Mansão ---
    // (A estrutura é a mesma, mas as pistas foram atualizadas)
    hall->esquerda = criarSala("Sala de Estar", "");
    hall->direita = criarSala("Sala de Jantar", "");
    hall->esquerda->esquerda = criarSala("Biblioteca", "Um recibo de uma loja de venenos, datado de ontem.");
    hall->esquerda->direita = criarSala("Jardim de Inverno", "Pegadas de sapatos masculinos tamanho 42 na terra macia.");
    hall->direita->direita = criarSala("Cozinha", "Uma faca de chef esta faltando no conjunto.");
    hall->direita->direita->esquerda = criarSala("Despensa", "Um cheiro fraco de amendoas amargas no ar, similar a cianeto.");

    // --- Populando a Tabela Hash (Conhecimento do Detetive) ---
    inserirNaHash(tabelaHash, "Um recibo de uma loja de venenos, datado de ontem.", "Mordomo");
    inserirNaHash(tabelaHash, "Pegadas de sapatos masculinos tamanho 42 na terra macia.", "Jardineiro");
    inserirNaHash(tabelaHash, "Uma faca de chef esta faltando no conjunto.", "Governanta");
    inserirNaHash(tabelaHash, "Um cheiro fraco de amendoas amargas no ar, similar a cianeto.", "Mordomo");

    // --- Loop Principal de Exploração (explorarSalas) ---
    Sala* salaAtual = hall;
    char escolha;

    printf("--- Detective Quest: O Julgamento Final ---\n");
    printf("Explore, colete pistas, conecte-as a um suspeito e faca sua acusacao.\n\n");

    while (1) {
        printf("----------------------------------------\n");
        printf("Voce esta em: %s\n", salaAtual->nome);

        // Se houver pista na sala, coleta e adiciona na BST.
        if (strlen(salaAtual->pista) > 0) {
            printf(">> Pista encontrada: %s\n", salaAtual->pista);
            arvorePistas = adicionarPista(arvorePistas, salaAtual->pista);
            strcpy(salaAtual->pista, ""); // Marca como coletada
        }

        printf("Escolha seu caminho (e/d/s para sair e julgar): ");
        scanf(" %c", &escolha);

        if (escolha == 's') {
            printf("\nVoce reuniu pistas suficientes e se dirige ao salao principal para o julgamento.\n");
            break;
        } else if (escolha == 'e') {
            if (salaAtual->esquerda != NULL) salaAtual = salaAtual->esquerda;
            else printf(">> Caminho bloqueado.\n");
        } else if (escolha == 'd') {
            if (salaAtual->direita != NULL) salaAtual = salaAtual->direita;
            else printf(">> Caminho bloqueado.\n");
        } else {
            printf(">> Opcao invalida!\n");
        }
    }

    // --- Fase Final: Julgamento ---
    exibirPistas(arvorePistas);
    verificarSuspeitoFinal(arvorePistas, tabelaHash);

    // --- Limpeza Final da Memória ---
    liberarMapa(hall);
    liberarPistas(arvorePistas);
    liberarTabelaHash(tabelaHash);
    printf("\nO caso foi encerrado. A memoria foi liberada.\n");

    return 0;
}

// --- Funções da Tabela Hash ---

unsigned int hashFunction(const char* str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash % HASH_TABLE_SIZE;
}

HashNode** criarTabelaHash() {
    HashNode** tabela = (HashNode**) calloc(HASH_TABLE_SIZE, sizeof(HashNode*));
    if(!tabela) {
        printf("Erro ao alocar tabela hash.\n");
        exit(1);
    }
    return tabela;
}

void inserirNaHash(HashNode** tabela, const char* pista, const char* suspeito) {
    unsigned int index = hashFunction(pista);
    HashNode* newNode = (HashNode*) malloc(sizeof(HashNode));
    if(!newNode) {
        printf("Erro ao alocar no da hash.\n");
        exit(1);
    }
    strcpy(newNode->pista, pista);
    strcpy(newNode->suspeito, suspeito);
    newNode->next = tabela[index]; // O novo nó aponta para o antigo início da lista.
    tabela[index] = newNode;       // O início da lista agora é o novo nó.
}

const char* encontrarSuspeito(HashNode** tabela, const char* pista) {
    unsigned int index = hashFunction(pista);
    HashNode* current = tabela[index];
    while (current != NULL) {
        if (strcmp(current->pista, pista) == 0) {
            return current->suspeito;
        }
        current = current->next;
    }
    return NULL; // Não encontrado.
}

void liberarTabelaHash(HashNode** tabela) {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        HashNode* current = tabela[i];
        while (current != NULL) {
            HashNode* temp = current;
            current = current->next;
            free(temp);
        }
    }
    free(tabela);
}

// --- Lógica do Julgamento Final ---

void verificarSuspeitoFinal(PistaNode* arvorePistas, HashNode** tabelaHash) {
    char acusado[50];
    printf("\nCom base nas pistas, quem voce acusa? (Mordomo, Jardineiro, Governanta): ");
    scanf("%s", acusado);

    int contagemDePistas = 0;
    // Usa a função recursiva para contar as pistas para o acusado.
    contarPistasParaSuspeito(arvorePistas, tabelaHash, acusado, &contagemDePistas);

    printf("\n--- O VEREDITO ---\n");
    printf("Voce acusa %s. A acusacao e sustentada por %d pista(s).\n", acusado, contagemDePistas);

    // A condição de vitória é ter pelo menos 2 pistas contra o acusado.
    if (contagemDePistas >= 2) {
        printf("Evidencias contundentes! O suspeito confessa. Caso encerrado!\n");
        printf("PARABENS, DETETIVE!\n");
    } else {
        printf("As evidencias sao insuficientes. O suspeito e liberado por falta de provas.\n");
        printf("Voce falhou em resolver o caso.\n");
    }
}

void contarPistasParaSuspeito(PistaNode* noPista, HashNode** tabela, const char* suspeitoAcusado, int* contador) {
    if (noPista == NULL) return;

    // Para cada pista na árvore, busca o suspeito na tabela hash.
    const char* suspeitoDaPista = encontrarSuspeito(tabela, noPista->pista);
    if (suspeitoDaPista != NULL && strcmp(suspeitoDaPista, suspeitoAcusado) == 0) {
        (*contador)++; // Incrementa o contador se os nomes baterem.
    }

    // Continua a busca recursivamente nas sub-árvores.
    contarPistasParaSuspeito(noPista->esquerda, tabela, suspeitoAcusado, contador);
    contarPistasParaSuspeito(noPista->direita, tabela, suspeitoAcusado, contador);
}


// --- Funções auxiliares já conhecidas (com nomes atualizados conforme solicitado) ---

Sala* criarSala(const char* nome, const char* pista) {
    Sala* novaSala = (Sala*) malloc(sizeof(Sala));
    if (!novaSala) { printf("Erro de memoria.\n"); exit(1); }
    strcpy(novaSala->nome, nome);
    strcpy(novaSala->pista, pista);
    novaSala->esquerda = novaSala->direita = NULL;
    return novaSala;
}

PistaNode* adicionarPista(PistaNode* raiz, const char* pistaTexto) {
    if (raiz == NULL) {
        PistaNode* novoNode = (PistaNode*) malloc(sizeof(PistaNode));
        if (!novoNode) { printf("Erro de memoria.\n"); exit(1); }
        strcpy(novoNode->pista, pistaTexto);
        novoNode->esquerda = novoNode->direita = NULL;
        return novoNode;
    }
    int cmp = strcmp(pistaTexto, raiz->pista);
    if (cmp < 0) raiz->esquerda = adicionarPista(raiz->esquerda, pistaTexto);
    else if (cmp > 0) raiz->direita = adicionarPista(raiz->direita, pistaTexto);
    return raiz;
}

void exibirPistas(PistaNode* raiz) {
    printf("\n--- PISTAS COLETADAS ---\n");
    if (raiz == NULL) {
        printf("Nenhuma pista foi encontrada.\n");
        return;
    }
    void percursoEmOrdem(PistaNode* node) {
        if (node == NULL) return;
        percursoEmOrdem(node->esquerda);
        printf("- %s\n", node->pista);
        percursoEmOrdem(node->direita);
    }
    percursoEmOrdem(raiz);
}

// Funções de liberação de memória
void liberarMapa(Sala* sala) {
    if (sala == NULL) return;
    liberarMapa(sala->esquerda);
    liberarMapa(sala->direita);
    free(sala);
}

void liberarPistas(PistaNode* raizPistas) {
    if (raizPistas == NULL) return;
    liberarPistas(raizPistas->esquerda);
    liberarPistas(raizPistas->direita);
    free(raizPistas);
}