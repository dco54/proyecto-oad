#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qcustomplot.h"


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
