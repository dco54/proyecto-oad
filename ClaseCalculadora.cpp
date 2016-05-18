#include<iostream>
#include "calculadora.h"
using namespace std;

int Calculadora::suma(){
    if (respuesta >=0){
      return (a+b)%mod;
    else {
      respuesta=respuesta+mod;
      return respuesta;
}
int Calculadora::resta(){
    return (a-b)%mod;
}
int Calculadora::multiplicar(){
    return (a*b)%mod;
}

