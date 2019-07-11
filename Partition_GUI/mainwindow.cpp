#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPushButton>
#include <window.h>
#include <QFileDialog>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QPushButton *new_button = new QPushButton("hello", this);
    new_button->setGeometry(0, 0, 50 , 50);
}
