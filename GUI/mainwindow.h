#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <vector>
#include <fstream>
#include <QTimer>
#include <QMainWindow>
#include <QVBoxLayout>
#include "udpmodule.h"
#include "tcpmodule.h"
#include "qcustomplot.h"

void drawHeatmapFromFloat(QCustomPlot *customPlot, QCPColorMap *colorMap, std::complex<float> * data, int rows, int cols);
void readData(std::string realname, std::string imagname, std::complex<float> * data, int rows, int cols);

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    QCustomPlot *customPlot = NULL;
    QLayout * layout = NULL;
    QTimer  * timer  = NULL;
    UDPmodule * mod  = NULL;
    QCPColorMap * colorMap = NULL;
    // TCPmodule * mod  = NULL;

    std::string rfile = "C:/Users/laoxi/Desktop/UDP/build-UDP_Define-Desktop_Qt_5_12_12_MSVC2017_64bit-Debug/debug/rraw.txt";
    std::string ifile = "C:/Users/laoxi/Desktop/UDP/build-UDP_Define-Desktop_Qt_5_12_12_MSVC2017_64bit-Debug/debug/iraw.txt";
    std::complex<float> * data = new std::complex<float> [_ROWS_ * _COLS_];

    ~MainWindow();
public slots:
    void updateUDPData();
};
#endif // MAINWINDOW_H
