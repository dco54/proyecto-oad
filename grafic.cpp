#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qcustomplot.h"
#include "tabulador.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    graficadora = new tabulador;
    graficadora->range_negative=range_x_ne;
    graficadora->range_positive=range_x;
    
    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    qDebug() << "Valor RANGO X NEGATIVO:" <<this->range_x_ne<<"Valor RANGO X POSITIVO:"<<this->range_x;
}

MainWindow::~MainWindow()
{
    delete graficadora;
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QVector<double> x(tam_x), y(tam_y); 
    qDebug() << "TABULADOR DE LA GRAFICADORA";
    graficadora->reconoce_funcion(ui->line_edit_funcion->text().toStdString());
    if(graficadora->getEstado()==false){
    for (int i=0; i<1001; ++i)
    {
        if((i/10.0)-50>range_x)
            break;
        if(((i/10.0)-50>=range_x_ne)){
                x[i] = *(graficadora->arrays_x + i);
                y[i] = *(graficadora->arrays_y + i);
                qDebug() << "Valor eje x:" <<x[i]<<"Valor eje y :"<<y[i];
        }
    }
    ui->customPlot->addGraph();
    ui->customPlot->graph(0)->setData(x,y);
    ui->customPlot->xAxis->setLabel("x");
    ui->customPlot->yAxis->setLabel("y");
    ui->customPlot->xAxis->setRange(range_x_ne, range_x);
    ui->customPlot->yAxis->setRange(range_x_ne, range_x);
    ui->customPlot->replot();
    qDebug() << "FIN DEL TABULADOR";
    }
}

void MainWindow::on_grabar_clicked()
{
    bool ok;
    double valor_range_x=ui->lineX->text().toDouble(&ok);
    double valor_range_x_ne=ui->lineX_ne->text().toDouble(&ok);
    this->range_x=valor_range_x;
    this->range_x_ne=valor_range_x_ne;
    graficadora->range_negative=range_x_ne;
    graficadora->range_positive=range_x;
    qDebug() << "Valores seteados";
    qDebug() << "Valor RANGO X NEGATIVO:" <<this->range_x_ne<<"Valor RANGO X POSITIVO:"<<this->range_x;
}

void MainWindow::on_pushExit_clicked()
{
   this->close();
}

void MainWindow::on_line_edit_funcion_cursorPositionChanged(int arg1, int arg2)

# Tabulador 

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
double tabulador::redondear(double Num, int nDec)
{
  return ((float)((int)(Num * pow(10, nDec) + 0.5))) / pow(10, nDec);
}
