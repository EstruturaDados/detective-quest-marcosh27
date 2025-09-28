#include <stdio.h>   // Para funções de entrada e saída, como printf e scanf.
#include <stdlib.h>  // Para alocação dinâmica de memória, como malloc e free.
#include <string.h>  // Para manipulação de strings, como strcpy.

// --- Definição da Estrutura de Dados ---

typedef struct Sala {
    char nome[50];                // Nome do cômodo (ex: "Cozinha").
    struct Sala* esquerda;        // Ponteiro para a sala à esquerda. Se for NULL, não há caminho.
    struct Sala* direita;         // Ponteiro para a sala à direita. Se for NULL, não há caminho.
} Sala;


// --- Protótipos das Funções ---
// Declarar as funções antes de usá-las é uma boa prática em C.
Sala* criarSala(const char* nome);
void explorarSalas(Sala* salaAtual);
void liberarMapa(Sala* raiz);

int main() {
    // --- Montagem do Mapa da Mansão ---
    // A raiz da nossa árvore é o Hall de Entrada.
    Sala* hall = criarSala("Hall de Entrada");

    // Montando o primeiro andar (nível 1 da árvore)
    hall->esquerda = criarSala("Sala de Estar");
    hall->direita = criarSala("Sala de Jantar");

    // Montando os cômodos conectados à Sala de Estar (nível 2)
    hall->esquerda->esquerda = criarSala("Biblioteca");
    hall->esquerda->direita = criarSala("Jardim de Inverno");

    // Montando os cômodos conectados à Sala de Jantar (nível 2)
    hall->direita->direita = criarSala("Cozinha");
    
    // Montando o cômodo conectado à Cozinha (nível 3)
    hall->direita->direita->esquerda = criarSala("Despensa");

    // --- Início da Exploração ---
    printf("--- Bem-vindo ao Detective Quest! ---\n");
    printf("Você está na mansão e precisa investigar os cômodos.\n");
    printf("Use 'e' para ir para a esquerda e 'd' para ir para a direita.\n\n");

    // A exploração começa na raiz da árvore (o Hall).
    explorarSalas(hall);

    // --- Limpeza da Memória ---
    // Após o fim da exploração, liberamos a memória de todas as salas criadas.
    liberarMapa(hall);
    printf("\nO jogo terminou e a memória foi liberada com sucesso.\n");

    return 0; // Indica que o programa terminou sem erros.
}

Sala* criarSala(const char* nome) {
    // 1. Aloca memória do tamanho de uma struct Sala.
    Sala* novaSala = (Sala*) malloc(sizeof(Sala));

    // 2. Verifica se a alocação de memória foi bem-sucedida.
    if (novaSala == NULL) {
        printf("Erro: Falha ao alocar memória para uma nova sala.\n");
        exit(1); // Encerra o programa se não houver memória disponível.
    }

    // 3. Copia o nome recebido para o campo 'nome' da nova sala.
    strcpy(novaSala->nome, nome);

    // 4. Inicializa os caminhos como nulos (sem saída).
    novaSala->esquerda = NULL;
    novaSala->direita = NULL;

    // 5. Retorna o endereço da sala criada.
    return novaSala;
}

void explorarSalas(Sala* salaAtual) {
    // Condição de segurança: se por algum motivo a sala for nula, apenas retorna.
    if (salaAtual == NULL) {
        return;
    }

    // Exibe a localização atual do jogador.
    printf("Você está em: %s\n", salaAtual->nome);

    // Verifica se é um nó folha (não tem caminhos para esquerda nem para direita).
    if (salaAtual->esquerda == NULL && salaAtual->direita == NULL) {
        printf("Este cômodo não tem mais saídas. Fim da exploração neste caminho!\n");
        return; // Fim da recursão para este caminho.
    }

    // Loop para garantir que o jogador faça uma escolha válida
    while (1) {
        char escolha;
        printf("Escolha seu caminho (e/d): ");
        // O espaço antes de "%c" é importante para consumir quebras de linha ou espaços
        // que possam ter ficado no buffer de entrada.
        scanf(" %c", &escolha);

        if (escolha == 'e') {
            if (salaAtual->esquerda != NULL) {
                explorarSalas(salaAtual->esquerda); // Chamada recursiva para a sala da esquerda.
                break; // Sai do loop while após explorar o caminho.
            } else {
                printf("Não há caminho para a esquerda. Tente outra direção.\n");
            }
        } else if (escolha == 'd') {
            if (salaAtual->direita != NULL) {
                explorarSalas(salaAtual->direita); // Chamada recursiva para a sala da direita.
                break; // Sai do loop while após explorar o caminho.
            } else {
                printf("Não há caminho para a direita. Tente outra direção.\n");
            }
        } else {
            printf("Opção inválida! Escolha 'e' para esquerda ou 'd' para direita.\n");
        }
    }
}

void liberarMapa(Sala* sala) {
    // Condição de parada da recursão: se o nó é nulo, não há nada a fazer.
    if (sala == NULL) {
        return;
    }

    // Libera a sub-árvore da esquerda.
    liberarMapa(sala->esquerda);
    // Libera a sub-árvore da direita.
    liberarMapa(sala->direita);
    // Libera o nó atual.
    free(sala);
}