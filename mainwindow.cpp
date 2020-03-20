#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDialog>
#include <QFileDialog>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

void draw (QTableWidget *table, QJsonObject obj, QCustomPlot *plot, QLineEdit *pdk, QString name) {
    QJsonValue value = obj.value(QString(name));
    // qWarning() << "QJsonValue: " << value;
    QJsonArray item = value.toArray();
    int length = item.count();
    // qWarning() << "QJsonArray: " << item;
    double arr[length];
    for (int i = 0; i < length; i++) {
        arr[i] = item[i].toDouble();
    }

    for (int k = 0; k < length; k++) {
        QString text = QString::number(arr[k],'g', 6);
        QTableWidgetItem *item = table->item(k, 0);
        if(item && item != nullptr)
            item->setText(text);
        else
        {
            table->setItem(k, 0, (new QTableWidgetItem(text)));
           // ui->tableWidget_7->item(k, 0)->setText(text);
        }
    }

    double min = 1;
    double max = length;
    int n = length;

    QVector <double> x(n), y(n);

    int i = 0;
    for (double X = min; X <= max; X++) {
        x[i] = X;
        QTableWidgetItem *item = table->item(i, 0);
        if (item && item != nullptr)
            y[i] = table->item(i, 0)->text().toDouble();
        else
            y[i] = 0;
        i++;
    }

    plot->xAxis->setLabel("Месяцы");
    plot->yAxis->setLabel("Концентрация");

    plot->addGraph();
    plot->graph(0)->setData(x, y);

    plot->xAxis->setRange(min, max);
    plot->xAxis->setTicker(QSharedPointer<QCPAxisTickerFixed>(new QCPAxisTickerFixed));

    double minY = y[0], maxY = y[0];
    for (int i = 1; i < n; i++)
    {
        if (y[i]<minY) minY = y[i];
        if (y[i]>maxY) maxY = y[i];
    }

    pdk->setText(QString::number(maxY, 'g', 6));

    QVector <double> max_x(2), max_y(2);
    max_x[0] = min;
    max_x[1] = max;
    max_y[0] = maxY;
    max_y[1] = maxY;

    plot->addGraph();
    plot->graph(1)->setData(max_x, max_y);
    plot->graph(1)->setPen(QColor(255, 0, 0, 255));

    maxY = maxY * 1.1;
    minY = minY / 1.1;

    plot->yAxis->setRange(minY, maxY);

    plot->replot();
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_2_clicked()
{
    QString file = QFileDialog::getOpenFileName(nullptr, "Выберите файл", "", "*.json");
    ui->lineEdit->setText(file);
}

void MainWindow::on_button_1_clicked()
{
    QString filename = ui->lineEdit->text();

    QFile file;
    QString val;
    QFileInfo checkFile(filename);
    if(checkFile.exists() && checkFile.isFile()) {
        file.setFileName(filename);
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        val = file.readAll();
        file.close();
        // qWarning() << val;
        QJsonDocument d = QJsonDocument::fromJson(val.toUtf8());
        QJsonObject sett2 = d.object();

        draw(ui->tableWidget_7, sett2, ui->widget_7, ui->lineEdit_8, "Фенолы");
        draw(ui->tableWidget_6, sett2, ui->widget_6, ui->lineEdit_9, "Фтор");
        draw(ui->tableWidget_5, sett2, ui->widget_5, ui->lineEdit_10, "Нефтяные углеводороды");
        draw(ui->tableWidget_4, sett2, ui->widget_4, ui->lineEdit_11, "Нитраты");
        draw(ui->tableWidget_8, sett2, ui->widget_3, ui->lineEdit_12, "Магний");
        draw(ui->tableWidget_3, sett2, ui->widget_2, ui->lineEdit_13, "Хлориды");
        draw(ui->tableWidget,   sett2, ui->widget,   ui->lineEdit_14, "Сульфаты");
   }
}

void forecast (QTableWidget *table, QCustomPlot *plot) {
    int rows = table->rowCount();
    double l[100];
    double t[100];
    double k = 0.8;
    double k1 = 0.2;
    double b = 0.9;

    double forecast[12];

    for (int i = 0; i < rows; i++) {
        double yt = table->item(i, 1)->text().toDouble();
        if (i == 0) {
            l[i+1] = yt;
            t[i+1] = 0;
        }
        else {
            l[i+1] = k * yt + k1 * (l[i-1] + t[i-1]);
            t[i+1] = b * (l[i+1] - l[i]) + 0.1 * t[i];
        }
    }

    for (int i = 1; i <= 12; i++) {
        forecast[i] = l[rows] + i * t[rows];
    }
}

void MainWindow::on_pushButton_7_clicked()
{

}
