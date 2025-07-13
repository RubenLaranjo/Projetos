#include <stdio.h>

int main() {
    int n_alunos,opcao;
    char loop;
    float notas[100];
    float soma = 0.0;
    float maior_nota = 0.0;
    float menor_nota = 0.0;

    do {
    printf("Quantos alunos inscritos tem a cadeira?\n");
    scanf(" %d",&n_alunos);
    if(n_alunos <= 0 || n_alunos > 100){
        printf("Insira um numero valido de alunos\n");
    }
    }while(n_alunos <= 0 || n_alunos > 100);

    for (int i = 0; i < n_alunos; i++) {
    printf("Nota do aluno %d: ", i + 1);
    scanf("%f", &notas[i]);
    soma += notas[i];
    }

    maior_nota = menor_nota = notas[0];

    for (int i = 1; i < n_alunos; i++) {
    if (notas[i] > maior_nota) maior_nota = notas[i];
    if (notas[i] < menor_nota) menor_nota = notas[i];
    }
    
    do{
        printf("Pretende efetuar qual operacao?\n");
        printf("1 - Ver as notas de todos os alunos \n");
        printf("2 - Ver a melhor nota de todos os alunos \n");
        printf("3 - Ver a pior nota de todos os alunos \n");
        printf("4 - Ver a medias de notas dos alunos\n");
        scanf(" %d",&opcao);


        switch (opcao)
        {
        case 1:

            for (int i = 0; i < n_alunos; i++) {
            printf("Nota do aluno %d: %.2f \n",i+1,notas[i]);
            }        
            break;
        case 2:
            printf("A melhor nota foi: %.2f \n",maior_nota);
            break;
        case 3:
            printf("A pior nota foi: %.2f \n",menor_nota);        
            break;
        case 4:

            printf("A media das notas e: %.2f\n",soma/n_alunos);
            
            break;     


        }
        printf("Pretende efetuar mais alguma operacao? (s/n)\n");
        scanf(" %c",&loop);
    
    }while(loop != 'n');
    
}



