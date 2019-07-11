#include "window.h"
#include <QPushButton>
#include <iostream>
#include <sstream>
#include <string>
#include <stdio.h>
#include <fstream>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <algorithm>
#include <QLabel>
#include <QFileDialog>
#include <QGraphicsView>
#include <QGroupBox>
#include <QRect>
#include <QMessageBox>
#include <QTextEdit>

using namespace std;
Window::Window(QWidget *parent) : QWidget(parent)
{
    setFixedSize(800, 250);
    image_select = new QLabel(this);

    image_select->setText("Select an image file to use: ");
    //image_select->setGeometry(0, 0, 400, 200);
    image_select->show();
    num_states_in_partition = 0;
    current_partition_size = 0;
    browse_button = new QPushButton("Browse Files", this);
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(image_select);
    layout->addWidget(browse_button);
    QGroupBox *group = new QGroupBox(this);
    group->setLayout(layout);
    group->setGeometry(0, -25, 650, 200);
    connect(browse_button, SIGNAL (clicked()), this, SLOT (browseButtonClicked()));
}

void Window::slotButtonClicked(bool clicked){
    int i , j;
    //Determine which button sent the signal, if it has not been checked, highlight it and add it
    //to the partition, if it has been checked, unhighlight it and remove it from the partition.
    for(i = 0; i < r; i++){
        for(j = 0; j < c; j++){
            if(buttons[i][j] == sender()){
                if(clicked){
                    if(!in_partition[i][j]){
                        QPalette pal = buttons[i][j]->palette();
                        pal.setColor(QPalette::Button, QColor(Qt::blue));
                        buttons[i][j]->setAutoFillBackground(true);
                        buttons[i][j]->setPalette(pal);
                        buttons[i][j]->update();
                        std::string s = buttons[i][j]->text().toUtf8().constData();
                        stringstream ss(s);
                        int pos = 0;
                        ss >> pos;
                        clicked_buttons.push_back(pos);
                        current_partition_size++;
                        in_partition[i][j] = true;
                    }
                    else{
                        QMessageBox messageBox;
                        messageBox.warning(0,"Error","State is already in a partition");
                    }
                }
                else{
                    QPalette pal = buttons[i][j]->palette();
                    buttons[i][j]->setAutoFillBackground(false);
                    buttons[i][j]->setPalette(pal);
                    buttons[i][j]->update();
                    std::string s = buttons[i][j]->text().toUtf8().constData();
                    stringstream ss(s);
                    int pos = 0;
                    ss >> pos;
                    in_partition[i][j] = false;
                    clicked_buttons.erase(std::remove(clicked_buttons.begin(), clicked_buttons.end(), pos), clicked_buttons.end());
                    current_partition_size--;
                }
            }
        }
    }
}

void Window::slotEnterButtonClicked(bool clicked){
    int i;
    std::string out_string;
    std::stringstream ss;
    std::stringstream new_ss;
    ss << num_partitions;
    out_string = ss.str();
    *outfile << "Partition " + out_string + " states: ";
    num_partitions++;
    new_ss << num_partitions;
    out_string = new_ss.str();
    for(i = 0; i < clicked_buttons.size(); i++){
        //write to text file
        *outfile << clicked_buttons[i] << " ";
    }
    *outfile << "\n";
    //Clear selected buttons
    clicked_buttons.clear();
    clearButtons();
    std::string new_button_text("Enter Partition " + out_string);
    QString qstr = QString::fromStdString(new_button_text);
    enter_button->setText(qstr);
    num_states_in_partition += current_partition_size;
    current_partition_size = 0;
}

void Window::clearButtons(){
    int i, j;
    for(i = 0; i < r; i++){
        for(j = 0; j < c; j++){
            if(buttons[i][j]->isChecked()){
                QPalette button_palette;
                button_palette.setColor(QPalette::ButtonText, Qt::red);
                buttons[i][j]->setChecked(false);
                buttons[i][j]->setAutoFillBackground(false);
                buttons[i][j]->setPalette(button_palette);

                buttons[i][j]->update();
            }
        }
    }
}

void Window::closeEvent(QCloseEvent *bar){
    outfile->close();
}

void Window::submitButtonClicked(bool clicked){

    std::string row_s = enter_cols->text().toUtf8().constData();
    stringstream ssCols(row_s);
    ssCols >> c;
    std::string col_s = enter_rows->text().toUtf8().constData();
    stringstream ssRows(col_s);
    ssRows >> r;
    num_partitions = 1;
    if(!(is_number(row_s) && is_number(col_s))){
        QMessageBox messageBox;
        messageBox.warning(0,"Error","Please enter a valid number");
        messageBox.setFixedSize(500,200);
    }
    else{
        enter_cols->close();
        enter_rows->close();
        submit_button->close();
        setFixedSize(imageObject->width(), imageObject->height() + 2 * imageObject->height()/4);
        // Create and position the buttons
        int i, j;
        int count = r * c - 1;
        float width = imageObject->width()/c;
        float height = imageObject->height()/r;
        buttons.resize(r, vector<QPushButton *>(c, new QPushButton()));
        in_partition.resize(r, vector<bool>(c, false));
        //display partition selection buttons over map
        for(i = 0; i < r; i++){
            for(j = 0; j < c; j++){
                QString s = QString::number(count);
                buttons[i][c - j - 1] = new QPushButton(s , this);
                buttons[i][c - j - 1]->setCheckable(true);
                buttons[i][c - j - 1]->setStyleSheet("QPushButton{background: transparent;}");
                //adjust button size and position to account for # of rows and columns
                buttons[i][c - j - 1]->setGeometry((c - j - 1)*width, i*height, width, height);
                buttons[i][c - j - 1]->show();
                connect(buttons[i][c - j - 1], SIGNAL (clicked(bool)), this, SLOT (slotButtonClicked(bool)));
                count--;
            }
        }

        enter_button = new QPushButton("Enter Partition 1", this);
        enter_button->setGeometry(0, imageObject->height(), imageObject->width(), imageObject->height()/4);
        connect(enter_button, SIGNAL (clicked(bool)), this, SLOT (slotEnterButtonClicked(bool)));
        finish_button = new QPushButton("Finish", this);
        int new_height = imageObject->height() + imageObject->height()/4;
        finish_button->setGeometry(0, new_height, imageObject->width(), imageObject->height()/4);
        connect(finish_button, SIGNAL (clicked()), this, SLOT (slotFinishButtonClicked()));
        enter_button->show();
        finish_button->show();
    }

}

void Window::browseButtonClicked(){
    browse_button->close();
    image_select->close();
    //Obtain image file and store the path in imagePath
    QString imagePath = QFileDialog::getOpenFileName(
                this,
                tr("Open File"),
                "",
                tr("JPEG (*.jpg *.jpeg);;PNG (*.png)" )
                );

    //Obtain the desired save location and store it in filename
    QFileDialog dialog(this);
    QStringList filename;
    dialog.setFileMode(QFileDialog::Directory);

    if(dialog.exec()){
        filename = dialog.selectedFiles();
    }
    imageObject = new QImage();
    imageObject->load(imagePath);
    QImage resized_image = imageObject->scaled(imageObject->width(), imageObject->height());
    image = QPixmap::fromImage(resized_image);

    scene = new QGraphicsScene(this);
    scene->addPixmap(image);;
    view = new QGraphicsView(this);
    view->setGeometry(0, 0, imageObject->width(),  imageObject->height());
    view->setScene(scene);

    setFixedSize(imageObject->width(), imageObject->height() + 200);
    enter_rows = new QLineEdit;
    enter_cols = new QLineEdit;
    submit_button = new QPushButton("Submit dimensions", this);
    enter_rows->setPlaceholderText("Number of rows");
    enter_cols->setPlaceholderText("Number of cols");
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(enter_rows);
    layout->addWidget(enter_cols);
    layout->addWidget(submit_button);
    QGroupBox *group = new QGroupBox(this);
    group->setLayout(layout);
    group->setGeometry(0, imageObject->height(), imageObject->width(), 200);
    connect(submit_button, SIGNAL (clicked(bool)), this, SLOT (submitButtonClicked(bool)));

    //create and open text file to write to
    std::string path = filename[0].toStdString() + "/states.txt";
    outfile = new std::ofstream (path.c_str());
    view->show();
    group->show();

}

void Window::slotFinishButtonClicked()
{
    if(num_states_in_partition == r * c){
        outfile->close();
        this->close();
    }
    else{
        QMessageBox messageBox;
        messageBox.warning(0,"Error","All states must be added to a partition");
    }
}

bool Window::is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}
