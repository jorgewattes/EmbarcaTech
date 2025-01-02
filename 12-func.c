#include <stdio.h>
void main(){
	//Declaração
	int quadrado(int arg){
		int aux;
		aux=arg*arg;
		//Operações
		return aux;
	}


	//Chamada
	int a= quadrado(3);
	printf("a=%d\n",a); //a=9
}