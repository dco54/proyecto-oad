
#include "tabulador.h"
tabulador::tabulador()
{
   arrays_x = new double[1050];
   arrays_y  = new double[1050];
}
tabulador::~tabulador()
{
  delete[] arrays_x;
  delete[] arrays_y;
}

void tabulador::reconoce_funcion(std::string funcion){
      automata *automata_funciones;
      automata_funciones = new automata;
      if(automata_funciones->reconoce_funcion_constante(funcion)==true){
        constante=true;
        this->valor_C=automata_funciones->getVariable_C();
        this->funcion_constante();
      }
      else{
          if(automata_funciones->reconoce_funcion_lineal(funcion)==true){
             lineal=true;
             this->valor_M=automata_funciones->getVariable_M();
             this->valor_C=automata_funciones->getVariable_C();
             this->funcion_lineal();
          }
            else if(automata_funciones->reconoce_funcion_cuadratica(funcion)==true){
                  cuadratica=true;
                  this->valor_A=automata_funciones->getVariable_A();
                  this->valor_B=automata_funciones->getVariable_B();
                  this->valor_C=automata_funciones->getVariable_C();
                  this->funcion_cuadratica();
              }
                  else{
                    this->no_existe=true;
                  }
        }
}
void tabulador::funcion_constante(){
    if(constante==true){
    for (int i=0; i<1001; ++i)
    {
        if((i/10.0)-50>range_positive)
            break;
        if(((i/10.0)-50>=range_negative)){
                *(arrays_x+i)=(i/10.0)-50;
                *(arrays_y+i)=valor_C;
        }
    }
    }
}
void tabulador::funcion_lineal(){
    if(lineal==true){
         for (int i=0; i<1001; ++i)
    {
        if((i/10.0)-50>range_positive)
            break;
        if(((i/10.0)-50>=range_negative)){
           *(arrays_x+i)=(i/10.0)-50;
           *(arrays_y+i)=valor_M*(*(arrays_x+i))+valor_C;
        }
    }
    }
}
void tabulador::funcion_cuadratica(){
    double valor_x_cuadratico=0;
    double valor_x=0;
    if(cuadratica==true){
         for (int i=0; i<1001; ++i)
            {
                if((i/10.0)-50>range_positive)
                    break;
                if(((i/10.0)-50>=range_negative)){
                   *(arrays_x+i)=(i/10.0)-50;
                   valor_x=*(arrays_x+i);
                   valor_x_cuadratico=valor_x*valor_x;
                   *(arrays_y+i)=(valor_A*valor_x_cuadratico)+(valor_B*valor_x)+valor_C;
                }
            }
    }
}
