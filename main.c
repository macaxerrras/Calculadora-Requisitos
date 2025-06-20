#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "expressao.h"  // ✅ OK — apenas o .h

int main() {
    char entrada[512];
    int opcao;

    while (1) {
        printf("\n--- Avaliador de Expressoes ---\n");
        printf("1. Converter Infixa -> Pos-fixada\n");
        printf("2. Converter Pos-fixada -> Infixa\n");
        printf("3. Avaliar expressao Pos-fixada\n");
        printf("4. Avaliar expressao Infixa\n");
        printf("0. Desligar\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);
        getchar();

        if (opcao == 0) break;

        printf("Digite a expressao: ");
        fgets(entrada, sizeof(entrada), stdin);
        entrada[strcspn(entrada, "\n")] = '\0';

        switch (opcao) {
            case 1: {
                char *copia = strdup(entrada);
                char *pos = getFormaPosFixa(copia);
                printf("Pos-fixada: %s\n", pos);
                free(pos);
                free(copia);
                break;
            }
            case 2: {
                char *copia = strdup(entrada);
                char *inf = getFormaInFixa(copia);
                printf("Infixa: %s\n", inf);
                free(inf);
                free(copia);
                break;
            }
            case 3: {
                char *copia = strdup(entrada);
                float v = getValorPosFixa(copia);
                printf("Valor: %.2f\n", v);
                free(copia);
                break;
            }
            case 4: {
                float v = getValorInFixa(entrada);
                printf("Valor: %.2f\n", v);
                break;
            }
            default:
                printf("Opcao inválida.\n");
        }
    }

    return 0;
}
