#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static inline double ms(clock_t a, clock_t b) {
    return 1000.0 * (double)(b - a) / CLOCKS_PER_SEC;
}

// Funções de ordenação
void ordenar_decrescente(int *v, int n){
    for (int i = 0; i < n; i++){
        for (int j = 0; j < n - 1; j++){
            if (v[j] < v[j + 1]){
                int aux = v[j];
                v[j] = v[j + 1];
                v[j + 1] = aux;
            }
        }
    }
}

void ordenar_asc(int *v, int n){
    for (int i = 0; i < n; i++){
        for (int j = 0; j < n - 1; j++){
            if (v[j] > v[j + 1]){
                int aux = v[j];
                v[j] = v[j + 1];
                v[j + 1] = aux;
            }
        }
    }
}

int comparacoes = 0;

void bubble_sort(int *v, int n){
    int trocou;
    do{
        trocou = 0;
        for (int i = 0; i < n - 1; i++){
            comparacoes++;
            if (v[i] > v[i + 1]){
                int aux = v[i];
                v[i] = v[i + 1];
                v[i + 1] = aux;
                trocou = 1;
            }
        }
    } while (trocou);
}

void insertion_sort(int *v, int n){
    for (int i = 1; i < n; i++){
        int j = i;
        comparacoes++;
        while (j > 0 && v[j - 1] > v[j]){
            comparacoes++;
            int temp = v[j];
            v[j] = v[j - 1];
            v[j - 1] = temp;
            j--;
        }
    }
}

// Funções para qsort com contagem de comparações
typedef unsigned long long u64;
static u64 QSORT_COMPS = 0;

static int cmp_int_asc_count(const void *a, const void *b) {
    QSORT_COMPS++;
    int A = *(const int*)a, B = *(const int*)b;
    return (A > B) - (A < B);
}

void qsort_asc_wrapped(int *v, int n, u64 *comps) {
    QSORT_COMPS = 0;
    qsort(v, (size_t)n, sizeof(int), cmp_int_asc_count);
    *comps = QSORT_COMPS;
}

// Função auxiliar para ordenar 5 valores (mediana)
void sort5(double *arr) {
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 4; j++) {
            if (arr[j] > arr[j+1]) {
                double tmp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = tmp;
            }
        }
    }
}

// Gera vetor quase ordenado (~10% perturbado)
void quase_ordenado(int *v, int n) {
    ordenar_asc(v, n);
    int trocas = n / 10; // 10%
    for (int i = 0; i < trocas; i++) {
        int i1 = rand() % n;
        int i2 = rand() % n;
        int tmp = v[i1];
        v[i1] = v[i2];
        v[i2] = tmp;
    }
}

// Função para salvar resultados no CSV
void salvar_csv(const char *nome_arquivo, const char *algoritmo, const char *estado, double tempo_mediana, double comps_media) {
    FILE *f = fopen(nome_arquivo, "a");
    if (!f) {
        printf("Erro ao abrir arquivo %s\n", nome_arquivo);
        return;
    }
    fprintf(f, "%s,%s,%.2f,%.0f\n", algoritmo, estado, tempo_mediana, comps_media);
    fclose(f);
}

// Executa 5 repetições e calcula mediana/média
void executar_repeticoes(
    const char *nome,
    void (*algoritmo)(int*, int),
    int *dados_orig, int n,
    int ordenar_estado // 0 = aleatório, 1 = ordenado, -1 = decrescente, 2 = quase ordenado
) {
    double tempos[5];
    long long soma_comps = 0;

    for (int r = 0; r < 5; r++) {
        int *dados = (int*)malloc(n * sizeof(int));
        memcpy(dados, dados_orig, n * sizeof(int));

        if (ordenar_estado == 1) ordenar_asc(dados, n);
        else if (ordenar_estado == -1) ordenar_decrescente(dados, n);
        else if (ordenar_estado == 2) quase_ordenado(dados, n);

        comparacoes = 0;
        clock_t start = clock();
        algoritmo(dados, n);
        clock_t end = clock();

        tempos[r] = ms(start, end);
        soma_comps += comparacoes;

        free(dados);
    }

    sort5(tempos);
    double mediana = tempos[2];
    double media_comps = soma_comps / 5.0;

    const char *estado;
    if (ordenar_estado == 0) estado = "Aleatório";
    else if (ordenar_estado == 1) estado = "Ordenado";
    else if (ordenar_estado == -1) estado = "Decrescente";
    else estado = "Quase Ordenado";

    printf("[%s - %s] Comparações (média): %.0f | Tempo (mediana): %.2f ms\n",
           nome, estado, media_comps, mediana);

    // Salva no CSV
    salvar_csv("resultados.csv", nome, estado, mediana, media_comps);
}

// Versão para qsort
void executar_repeticoes_qsort(
    const char *nome,
    int *dados_orig, int n,
    int ordenar_estado
) {
    double tempos[5];
    long long soma_comps = 0;

    for (int r = 0; r < 5; r++) {
        int *dados = (int*)malloc(n * sizeof(int));
        memcpy(dados, dados_orig, n * sizeof(int));

        if (ordenar_estado == 1) ordenar_asc(dados, n);
        else if (ordenar_estado == -1) ordenar_decrescente(dados, n);
        else if (ordenar_estado == 2) quase_ordenado(dados, n);

        u64 comps = 0;
        clock_t start = clock();
        qsort_asc_wrapped(dados, n, &comps);
        clock_t end = clock();

        tempos[r] = ms(start, end);
        soma_comps += comps;

        free(dados);
    }

    sort5(tempos);
    double mediana = tempos[2];
    double media_comps = soma_comps / 5.0;

    const char *estado;
    if (ordenar_estado == 0) estado = "Aleatório";
    else if (ordenar_estado == 1) estado = "Ordenado";
    else if (ordenar_estado == -1) estado = "Decrescente";
    else estado = "Quase Ordenado";

    printf("[%s - %s] Comparações (média): %.0f | Tempo (mediana): %.2f ms\n",
           nome, estado, media_comps, mediana);

    // Salva no CSV
    salvar_csv("resultados.csv", nome, estado, mediana, media_comps);
}

void executar_testes(int N) {
    printf("\n====================================================\n");
    printf("              TESTES COM %d ELEMENTOS\n", N);
    printf("====================================================\n");

    int *dados = (int*)malloc(N * sizeof(int));
    for (int i = 0; i < N; i++) dados[i] = rand() % N;

    // Bubble Sort
    executar_repeticoes("Bubble Sort", bubble_sort, dados, N, 0);
    executar_repeticoes("Bubble Sort", bubble_sort, dados, N, 1);
    executar_repeticoes("Bubble Sort", bubble_sort, dados, N, -1);
    executar_repeticoes("Bubble Sort", bubble_sort, dados, N, 2);

    // Insertion Sort
    executar_repeticoes("Insertion Sort", insertion_sort, dados, N, 0);
    executar_repeticoes("Insertion Sort", insertion_sort, dados, N, 1);
    executar_repeticoes("Insertion Sort", insertion_sort, dados, N, -1);
    executar_repeticoes("Insertion Sort", insertion_sort, dados, N, 2);

    // qsort
    executar_repeticoes_qsort("qsort", dados, N, 0);
    executar_repeticoes_qsort("qsort", dados, N, 1);
    executar_repeticoes_qsort("qsort", dados, N, -1);
    executar_repeticoes_qsort("qsort", dados, N, 2);

    free(dados);
}

int main(){
    srand(time(NULL));

    // Limpa CSV e adiciona cabeçalho
    FILE *f = fopen("resultados.csv", "w");
    if (f) {
        fprintf(f, "Algoritmo,Estado,TempoMediana(ms),ComparacoesMedia\n");
        fclose(f);
    }

    executar_testes(1000);
    executar_testes(5000);
    executar_testes(10000);

    return 0;
}
