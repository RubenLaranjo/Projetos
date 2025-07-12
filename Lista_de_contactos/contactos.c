#include <stdio.h>
#include <string.h>

typedef struct {
        char nome[100];
        int idade;
        char n_telefone[10];

}Info;

int main(){

    char continuar = 's';
    char op;

    Info contacto;

        do{
        printf("Pretende procurar, escrever um contacto ou sair do programa (p/e/s)\n");
        scanf(" %c",&op);

        switch (op)
        {
        case 'e' :
        {
            FILE *ficheiro = fopen("C:\\Users\\LARANJO\\Desktop\\Lista_de_contactos\\lista.csv", "a");

            if (ficheiro == NULL) {
            perror("Erro ao abrir o ficheiro\n");
            return 1;
            }            

            while(continuar == 's'){
                printf("Por favor insira o nome do contacto: \n");
                scanf(" %[^\n]", contacto.nome);

                printf("Por favor insira o idade do contacto: \n");
                scanf(" %d", &contacto.idade);
                
                printf("Por favor insira o numero de telefone do contacto: \n");
                scanf(" %s", contacto.n_telefone);

                if(strlen(contacto.n_telefone) > 9 ){
                    printf("O seu numero de telefone tem mais que 9 digitos confirma ou pretende alterar: (s/n)\n");
                    char alterar;
                    scanf(" %c",&alterar);
                    if(alterar == 's'){
                        printf("Por favor insira o novo numero de telefone: \n");
                        scanf(" %s", contacto.n_telefone);  
                    }

                }

                fprintf(ficheiro,"%s,%d,%s\n",contacto.nome,contacto.idade,contacto.n_telefone);                

                printf("Deseja continuar a escrever? (s/n): \n");
                scanf(" %c", &continuar);       

            }

        fclose(ficheiro);

        }
        break;
        case 'p':
        {
            char procura;
            char loop = 's';
            printf("Pretende procurar atraves de nome, idade ou numero de telefone (n/i/t)\n");
            scanf(" %c",&procura);
                
            FILE *ficheiro = fopen("C:\\Users\\LARANJO\\Desktop\\Lista_de_contactos\\lista.csv", "r");
            if (ficheiro == NULL) {
                perror("Erro ao abrir o ficheiro\n");
                return 1;
            }

            while(loop == 's'){
            do{
                char leitura[256];

                switch (procura)
                {
                case 'n' :
                {
                    rewind(ficheiro);

                    char nome_p[100];
                    printf("Digite o nome que pretende encontrar\n");
                    scanf(" %[^\n]", nome_p);
                    

                    int nome_p_encont = 0;

                    while (fgets(leitura, sizeof(leitura), ficheiro))
                    {
                        Info l_lida;

                        sscanf(leitura, "%[^,],%d,%s", l_lida.nome, &l_lida.idade, l_lida.n_telefone);

                        if(strstr(l_lida.nome, nome_p) != NULL){

                            printf("Nome: %s, Idade: %d, Telefone: %s\n", l_lida.nome, l_lida.idade, l_lida.n_telefone);
                            nome_p_encont = 1;
                        }

                    }

                        printf("Pretende fazer outra pesquisa?\n");
                        scanf(" %c", &loop);

                        if (nome_p_encont == 0){
                            printf("Não foi encontrada correspondencia para o nome: %s\n", nome_p);
                        }
                    break;
                }
                    

                case 'i' :
                    rewind(ficheiro);

                    int idade_p;
                    printf("Digite idade que pretende encontrar\n");
                    scanf(" %d", &idade_p);
                    

                    int idade_p_encont = 0;

                    while (fgets(leitura, sizeof(leitura), ficheiro))
                    {
                        Info l_lida;

                        sscanf(leitura, "%[^,],%d,%s", l_lida.nome, &l_lida.idade, l_lida.n_telefone);

                        if(l_lida.idade == idade_p){

                            printf("Nome: %s, Idade: %d, Telefone: %s\n", l_lida.nome, l_lida.idade, l_lida.n_telefone);
                            idade_p_encont = 1;
                        }

                    }

                        printf("Pretende fazer outra pesquisa?\n");
                        scanf(" %c", &loop);

                        if (idade_p_encont == 0){
                            printf("Não foi encontrada correspondencia para a idade: %d\n", idade_p);
                        }                    
                    break;                    
                case 't' :

                    rewind(ficheiro);

                    char tele_p[10];
                    printf("Digite o telemovel que pretende encontrar\n");
                    scanf(" %s", tele_p);
                    
                    int tele_p_encont = 0;

                    while (fgets(leitura, sizeof(leitura), ficheiro))
                    {
                        Info l_lida;

                        sscanf(leitura, "%[^,],%d,%s", l_lida.nome, &l_lida.idade, l_lida.n_telefone);

                        if(strstr(l_lida.n_telefone, tele_p) != NULL){

                            printf("Nome: %s, Idade: %d, Telefone: %s\n", l_lida.nome, l_lida.idade, l_lida.n_telefone);
                            tele_p_encont = 1;
                        }

                    }

                        printf("Pretende fazer outra pesquisa?\n");
                        scanf(" %c", &loop);

                        if (tele_p_encont == 0){
                            printf("Não foi encontrada correspondencia para o numero de telemovel: %s\n", tele_p);
                        }
                    
                    break;                

                default:
                    printf("Escolha inválida, tente novamente\n");
                    printf("Pretende procurar atraves de nome, idade ou numero de telefone (n/i/t): \n");
                    scanf(" %c", &procura);
                    break;
                }
            }while(procura != 'n' && procura != 'i' && procura != 't');
            }
            fclose(ficheiro);
        }
        break;
        }
        }while(op != 's');
        return 0;
}