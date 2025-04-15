#include "mainwindow.h"

void drawHeatmapFromFloat(QCustomPlot *customPlot, QCPColorMap *colorMap, std::complex<float> * data, int rows, int cols) {
    // 1. 创建 ColorMap 并设置尺寸
    if (colorMap == NULL) {
        colorMap = new QCPColorMap(customPlot->xAxis, customPlot->yAxis);
        colorMap->data()->setSize(cols, rows); // 注意列是x，行是y

        // 2. 设置坐标范围（你可以自定义）
        colorMap->data()->setRange(QCPRange(0, cols), QCPRange(0, rows)); // xRange, yRange
    }

    // 3. 把你的 float 数据放进去
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            float abs_data = sqrt(data[x + y * cols].real() * data[x + y * cols].real() +
                    data[x + y * cols].imag() * data[x + y * cols].imag());
            colorMap->data()->setCell(x, y, abs_data);  // 注意 data[y][x]
        }
    }

    // 4. 设置颜色梯度
    colorMap->setGradient(QCPColorGradient::gpJet);

    // 5. 可选：添加颜色条图例
    /* QCPColorScale *colorScale = new QCPColorScale(customPlot);
    customPlot->plotLayout()->addElement(0, 1, colorScale);
    colorMap->setColorScale(colorScale);
    colorScale->setType(QCPAxis::atRight);
    colorScale->axis()->setLabel("Value"); */
    colorMap->rescaleDataRange(true);

    // 6. 更新坐标轴 & 重绘
    customPlot->rescaleAxes();
    customPlot->replot();
}

void readData(std::string realname, std::string imagname, std::complex<float> * data, int rows, int cols) {
    std::ifstream rfile(realname), ifile(imagname);
    if (!rfile.is_open() || !ifile.is_open()) {
        std::cerr << "Failed to open file" << std::endl;
        return;
    }

    for (int ii = 0; ii < rows; ii++) {
        for (int jj = 0; jj < cols; jj++) {
            float rpart = 0.0, ipart = 0.0;
            rfile >> rpart;
            ifile >> ipart;
            std::complex<float> num = {rpart, ipart};
            data[ii * cols + jj] = num;
        }
    }
}

void MainWindow::updateUDPData() {
    mod->receiveData();
    std::complex<float> * tdata = mod->getData();
    if (tdata == NULL) return;
    memcpy(data, tdata, _ROWS_ * _COLS_ * sizeof (std::complex<float>));
    drawHeatmapFromFloat(customPlot, colorMap, data, _ROWS_, _COLS_);
}

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    timer  = new QTimer();
    layout = new QVBoxLayout(this);
    customPlot = new QCustomPlot(); customPlot->setOpenGl(true);
    mod        = new UDPmodule();
    // mod        = new TCPmodule();
    layout->addWidget(customPlot);
    // readData(rfile, ifile, data, _ROWS_, _COLS_);

    this->setLayout(layout);
    this->setGeometry(100, 100, 1280, 800);

    timer->start(1); // 1ms update 1000Hz
    connect(this->timer, SIGNAL(timeout()), this, SLOT(updateUDPData()));
}

MainWindow::~MainWindow()
{

}

