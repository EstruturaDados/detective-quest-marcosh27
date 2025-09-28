#include <stdio.h>   // Para funções de entrada e saída (printf, scanf).
#include <stdlib.h>  // Para alocação dinâmica de memória (malloc, free) e exit.
#include <string.h>  // Para manipulação de strings (strcpy, strcmp, strlen).

// --- Definição das Estruturas de Dados ---

typedef struct PistaNode {
    char pista[150];               // Texto da pista coletada.
    struct PistaNode* esquerda;    // Ponteiro para pistas alfabeticamente menores.
    struct PistaNode* direita;     // Ponteiro para pistas alfabeticamente maiores.
} PistaNode;

typedef struct Sala {
    char nome[50];                 // Nome do cômodo.
    char pista[150];               // Pista encontrada neste cômodo. Se vazia, não há pista.
    struct Sala* esquerda;         // Ponteiro para a sala à esquerda.
    struct Sala* direita;          // Ponteiro para a sala à direita.
} Sala;


// --- Protótipos das Funções ---
Sala* criarSala(const char* nome, const char* pista);
PistaNode* inserirPista(PistaNode* raiz, const char* pistaTexto);
void exibirPistas(PistaNode* raiz);
void liberarMapa(Sala* raiz);
void liberarPistas(PistaNode* raizPistas);

int main() {
    // A raiz do mapa da mansão.
    Sala* hall = criarSala("Hall de Entrada", ""); // "" indica sem pista.

    // A raiz da árvore de pistas começa vazia.
    PistaNode* arvorePistas = NULL;

    // --- Montagem do Mapa da Mansão com Pistas ---
    hall->esquerda = criarSala("Sala de Estar", "");
    hall->direita = criarSala("Sala de Jantar", "");

    hall->esquerda->esquerda = criarSala("Biblioteca", "Um recibo de uma loja de venenos, datado de ontem.");
    hall->esquerda->direita = criarSala("Jardim de Inverno", "Pegadas de sapatos masculinos tamanho 42 na terra macia.");

    hall->direita->direita = criarSala("Cozinha", "Uma faca de chef esta faltando no conjunto.");
    hall->direita->direita->esquerda = criarSala("Despensa", "Um cheiro fraco de amendoas amargas no ar.");

    // --- Loop Principal de Exploração ---
    Sala* salaAtual = hall; // O jogador começa no Hall.
    char escolha;

    printf("--- Detective Quest: O Mistério da Mansão ---\n");
    printf("Explore os comodos, colete pistas e resolva o caso.\n");
    printf("Use 'e' (esquerda), 'd' (direita) ou 's' para sair e ver seu relatorio.\n\n");

    while (1) { // O loop continua até o jogador escolher 's'.
        printf("----------------------------------------\n");
        printf("Voce esta em: %s\n", salaAtual->nome);

        // Verifica se há uma pista na sala atual e se ela já não foi coletada.
        if (strlen(salaAtual->pista) > 0) {
            printf(">> Voce encontrou uma pista: %s\n", salaAtual->pista);
            // Insere a pista na BST.
            arvorePistas = inserirPista(arvorePistas, salaAtual->pista);
            // "Remove" a pista da sala para não ser coletada novamente.
            strcpy(salaAtual->pista, "");
        }

        printf("Escolha seu caminho (e/d/s): ");
        scanf(" %c", &escolha);

        if (escolha == 's') {
            printf("\nVoce decidiu encerrar a investigacao por enquanto.\n");
            break; // Sai do loop while.
        } else if (escolha == 'e') {
            if (salaAtual->esquerda != NULL) {
                salaAtual = salaAtual->esquerda; // Move o jogador para a esquerda.
            } else {
                printf(">> Nao ha caminho para a esquerda.\n");
            }
        } else if (escolha == 'd') {
            if (salaAtual->direita != NULL) {
                salaAtual = salaAtual->direita; // Move o jogador para a direita.
            } else {
                printf(">> Nao ha caminho para a direita.\n");
            }
        } else {
            printf(">> Opcao invalida!\n");
        }
    }

    // --- Exibição do Relatório de Pistas ---
    exibirPistas(arvorePistas);

    // --- Limpeza da Memória ---
    liberarMapa(hall);
    liberarPistas(arvorePistas);
    printf("\nO jogo terminou e a memoria foi liberada com sucesso.\n");

    return 0;
}

Sala* criarSala(const char* nome, const char* pista) {
    Sala* novaSala = (Sala*) malloc(sizeof(Sala));
    if (novaSala == NULL) {
        printf("Erro: Falha ao alocar memoria para uma nova sala.\n");
        exit(1);
    }
    strcpy(novaSala->nome, nome);
    if (pista != NULL) {
        strcpy(novaSala->pista, pista);
    } else {
        strcpy(novaSala->pista, ""); // Garante que o campo não terá lixo de memória.
    }
    novaSala->esquerda = NULL;
    novaSala->direita = NULL;
    return novaSala;
}

PistaNode* inserirPista(PistaNode* raiz, const char* pistaTexto) {
    // Caso base 1: A árvore (ou sub-árvore) está vazia.
    if (raiz == NULL) {
        PistaNode* novoNode = (PistaNode*) malloc(sizeof(PistaNode));
        if(novoNode == NULL) {
            printf("Erro: Falha ao alocar memoria para nova pista.\n");
            exit(1);
        }
        strcpy(novoNode->pista, pistaTexto);
        novoNode->esquerda = novoNode->direita = NULL;
        return novoNode; // Retorna o novo nó como a nova raiz.
    }

    // Compara a nova pista com a pista na raiz atual para decidir para onde ir.
    int comparacao = strcmp(pistaTexto, raiz->pista);

    if (comparacao < 0) { // A nova pista vem antes alfabeticamente.
        raiz->esquerda = inserirPista(raiz->esquerda, pistaTexto);
    } else if (comparacao > 0) { // A nova pista vem depois alfabeticamente.
        raiz->direita = inserirPista(raiz->direita, pistaTexto);
    }
    // Se comparacao == 0, a pista já existe, então não fazemos nada.

    return raiz; // Retorna a raiz original (ou a nova, no caso base).
}

void exibirPistas(PistaNode* raiz) {
    printf("\n--- RELATORIO DE PISTAS COLETADAS ---\n");

    if (raiz == NULL) {
        printf("Nenhuma pista foi encontrada.\n");
        return;
    }
    // Função auxiliar recursiva para fazer o percurso em ordem.
    void percursoEmOrdem(PistaNode* node) {
        if (node == NULL) {
            return;
        }
        percursoEmOrdem(node->esquerda);       // 1. Visita a sub-árvore esquerda.
        printf("- %s\n", node->pista);         // 2. Visita o nó atual.
        percursoEmOrdem(node->direita);        // 3. Visita a sub-árvore direita.
    }
    percursoEmOrdem(raiz);
}

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