// #include <stdio.h>
// void main()
// {
//     int a = 2;

//     if (a == 0){
//         // Operações();
//     }
//     else if (a == 1){
//         // Operações();
//     }
//     else if (a == 2){
//         // Operações();
//     }
//     else if (a == 3){
//         // Operações();
//     }
//     else{
//         // Operações();
//     }
// }


// #include <stdio.h>
// void main(){
//     int a = 2;
//     switch (a)    {
//     case 1:
//         // Operações();
//         break;
//     case 2:
//         // Operações();
//         break;
//     case 3:
//         // Operações();
//         break;
//     default:
//         // Operações();
//         break;
//     }
// }

#include <stdio.h>
void main()
{
    int a1, a2;
    int b = 2;
    int c = 3; 
    int d = 4;
    //Estrutura IF/ELSE
    if(b>2){
        a1=c;
    }
    else{
        a1=d;
    }
    //Operador Ternário
    a2 = b>2 ? c : d;

    printf(a1);
    printf(a2);
}
