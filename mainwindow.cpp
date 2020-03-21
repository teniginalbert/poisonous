#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDialog>
#include <QFileDialog>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

void draw (QTableWidget *table, QJsonObject sett2, QCustomPlot *plot, QLineEdit *pdk, QString name) {

    QJsonValue value = sett2.value(name);
    QJsonArray item = value.toArray();
    int length = item.count();
    double arr[length][3];

    for (int i = 0; i < length; i++) {
        arr[i][0] = item[i].toObject().value("concentration").toDouble();
        arr[i][1] = item[i].toObject().value("month").toDouble();
        arr[i][2] = item[i].toObject().value("year").toDouble();
    }

    for (int k = 0; k < length; k++) {
        QString concentration = QString::number(arr[k][0],'g', 6);
        QString year = QString::number(arr[k][2], 'g', 6);
        QString month = QString::number(arr[k][1], 'g', 6);

        QTableWidgetItem *item_concentration = table->item(k, 0);
        QTableWidgetItem *item_year = table->item(k, 2);
        QTableWidgetItem *item_month = table->item(k, 1);

        if(item_concentration && item_concentration != nullptr)
            item_concentration->setText(concentration);
        else
        {
            table->setItem(k, 0, (new QTableWidgetItem(concentration)));
           // ui->tableWidget_7->item(k, 0)->setText(text);
        }

        if(item_year && item_year != nullptr)
            item_year->setText(year);
        else
        {
            table->setItem(k, 2, (new QTableWidgetItem(year)));
           // ui->tableWidget_7->item(k, 0)->setText(text);
        }

        if(item_month && item_month != nullptr)
            item_month->setText(month);
        else
        {
            table->setItem(k, 1, (new QTableWidgetItem(month)));
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
            y[i] = item->text().toDouble();
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
        if (y[i] < minY) minY = y[i];
        if (y[i] > maxY) maxY = y[i];
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
    double l[rows];
    double t[rows];
    double k = 0.8;
    double k1 = 0.2;
    double b = 0.9;

    double final[rows + 12];

    for (int i = 0; i < rows; i++) {
        final[i] = table->item(i, 2)->text().toDouble();
        if (i == 0) {
            l[i+1] = final[i];
            t[i+1] = 0;
        }
        else {
            l[i+1] = k * final[i] + k1 * (l[i-1] + t[i-1]);
            t[i+1] = b * (l[i+1] - l[i]) + 0.1 * t[i];
        }
    }

    for (int i = 1; i <= 12; i++) {
        final[rows + i] = l[rows] + i * t[rows];
    }

}

void MainWindow::on_pushButton_7_clicked()
{
    forecast(ui->tableWidget_6, ui->widget_6);
}

void MainWindow::on_pushButton_8_clicked()
{
    forecast(ui->tableWidget_7, ui->widget_7);
}

void MainWindow::on_pushButton_clicked()
{
    forecast(ui->tableWidget, ui->widget);
}

void MainWindow::on_pushButton_3_clicked()
{
    forecast(ui->tableWidget_3, ui->widget_2);
}


void MainWindow::on_pushButton_4_clicked()
{
    forecast(ui->tableWidget_8, ui->widget_3);
}

void MainWindow::on_pushButton_5_clicked()
{
    forecast(ui->tableWidget_4, ui->widget_4);
}

void MainWindow::on_pushButton_6_clicked()
{
    forecast(ui->tableWidget_5, ui->widget_5);
}
