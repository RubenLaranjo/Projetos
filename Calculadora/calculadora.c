#include <stdio.h>
#include <math.h>
#include <string.h>

float aritmetica(float num1, float num2, char operacao);
float op_esp(float num1, char op[10]);

int main()
{   
    char continuar = 's';
    char choise = 's';
    float num1, num2, resultado, ans = NAN;
    char operacao, opcao;
    char op[10];

    while (continuar == 's')
    {

    printf("Deseja realizar uma conta com 2 numeros ou apenas realizar uma operacao com um numero? (s ou n) \n");
    scanf(" %c", &opcao);
    
    if(opcao == 's'){
    printf("Insira o valor que pretende calcular primeiro:\n");
    scanf("%f", &num1);
    
    printf("Diga qual a operacao que pretende executar (+, -, *, /):\n");
    scanf(" %c", &operacao);
    
    printf("Insira o valor que pretende calcular em segundo:\n");
    scanf("%f", &num2);

    
    resultado = aritmetica(num1, num2, operacao);
    ans = resultado;
    printf("O resultado é: %f\n", resultado);
    printf("Pretende continuar a conta?\n");
    scanf(" %c",&continuar);
    
    }else{

    printf("Insira o valor que pretende calcular:\n");
    scanf("%f", &num1);
    
    printf("Diga qual a operacao que pretende executar (log; ln; sqrt; pot;):\n");
    scanf("%s", op);


    resultado = op_esp(num1,op);
    printf("O resultado é: %f\n", resultado);

    printf("Deseja efetuar alguma conta com este valor? (s/n)\n");
    scanf(" %c", &choise);

     
    }

    while(!isnan(ans) && continuar == 's' && choise == 's'){
    char especial;
    
    printf("Pretende algo simpels ou complexo: (s/c)\n");
    scanf(" %c", &especial);

    if(especial == 's'){
    printf("Diga qual a operacao que pretende executar (+, -, *, /):\n");
    scanf(" %c", &operacao);
    printf("Insira o valor que pretende calcular em segundo:\n");
    scanf("%f", &num2);
    resultado = aritmetica(resultado, num2, operacao);

    }else{
    printf("Diga qual a operacao que pretende executar (log; ln; sqrt; pot;):\n");
    scanf("%s", op);
    resultado = op_esp(resultado,op);
    
    
    }

    printf("O resultado é: %f\n", resultado);
    printf("Pretende continuar a conta?\n");
    scanf(" %c",&continuar);
    }

    }

    return 0;
}

float aritmetica(float num1, float num2, char operacao){
    switch (operacao) {
        case '+': return num1 + num2; break;
        case '-': return num1 - num2; break;
        case '*': return num1 * num2; break;
        case '/':
            while (num2 == 0)
            {
                printf("Por favor insira outro valor diferente de 0\n");
                scanf("%f", &num2);
            }
        return num1 / num2; break;

        default:
        printf("Operacao ivalida");
        return 0; break;
    }
}

float op_esp(float num1, char op[10]){

    if(strcmp(op, "log") == 0){
            while (num1 <= 0)
            {
                printf("Por favor insira outro valor diferente de 0\n");
                scanf("%f", &num1);
            }
        return log10(num1);
    }
    if(strcmp(op, "ln") == 0){
            while (num1 <= 0)
            {
                printf("Por favor insira outro valor diferente de 0\n");
                scanf("%f", &num1);
            }
        return log(num1);
    }
    if(strcmp(op, "sqrt") == 0){
            while (num1 < 0)
            {
                printf("Por favor insira outro valor diferente de 0\n");
                scanf("%f", &num1);
            }
        return sqrt(num1);
    }

    if(strcmp(op, "pot") == 0){
        float pot;
        printf("Qual o valor de potencia pretende elevar o valor anterios\n");
        scanf("%f", &pot);
        return pow(num1,pot);
    }
     return 0;
}   
