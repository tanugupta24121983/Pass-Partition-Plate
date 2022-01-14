#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <map>
#include <QCryptographicHash>
#include <QMessageBox>
#include <QFile>
#include <cmath>
#include <QDebug>
#include <QFileDialog>

#include <QMap>
#include <QtMath>
#include <QCompleter>
#include <QSettings>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QPixmap pixmap(":/images/Isgec-logo.jpg");
    ui->stackedWidget->setCurrentIndex(0);
    int w = ui->label_isgec_logo->width();
    int h = ui->label_isgec_logo->height();
    ui->label_isgec_logo->setPixmap(pixmap.scaled(w,h,Qt::KeepAspectRatio));

    QPixmap pixmap1(":/images/PP-Dimesnsion.png");
    w = ui->lb_dimanesion_table->width();
    h = ui->lb_dimanesion_table->height();
    ui->lb_dimanesion_table->setPixmap(pixmap1);

    QPixmap pixmap2(":/images/PP-Dimesnsion2.png");
    w = ui->lb_dimanesion_table_2->width();
    h = ui->lb_dimanesion_table_2->height();
    ui->lb_dimanesion_table_2->setPixmap(pixmap2);

    ui->le_inside_diameter->setText(QString::number(0));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_le_inside_diameter_editingFinished()
{
     calculate_thickness(ui->cb_material->currentText());
}


void MainWindow::on_cb_material_activated(const QString &arg1)
{
    calculate_thickness(arg1);
}





void MainWindow::calculate_thickness(QString arg1) {

    if(arg1 == "Carbon Steel")
    {
        if(ui->le_inside_diameter->text().toInt() >= 0 && ui->le_inside_diameter->text().toInt() < 610)
        {
            ui->le_minimun_thickness_t1->setText(QString::number(9.5));
        }
        else if(ui->le_inside_diameter->text().toInt() >= 610 && ui->le_inside_diameter->text().toInt() < 1524)
        {
            ui->le_minimun_thickness_t1->setText(QString::number(12.7));
        }
        else if(ui->le_inside_diameter->text().toInt() >= 1549 && ui->le_inside_diameter->text().toInt() < 2540)
        {
            ui->le_minimun_thickness_t1->setText(QString::number(15.9));
        }
        else {
            ui->le_minimun_thickness_t1->setText(QString::number(-1));
        }
    }
    else if(arg1 == "Alloy Material")
    {
        if(ui->le_inside_diameter->text().toInt() >= 0 && ui->le_inside_diameter->text().toInt() < 610)
        {
            ui->le_minimun_thickness_t1->setText(QString::number(6.4));
        }
        else if(ui->le_inside_diameter->text().toInt() >= 610 && ui->le_inside_diameter->text().toInt() < 1524)
        {
            ui->le_minimun_thickness_t1->setText(QString::number(9.5));
        }
        else if(ui->le_inside_diameter->text().toInt() >= 1549 && ui->le_inside_diameter->text().toInt() < 2540)
        {
            ui->le_minimun_thickness_t1->setText(QString::number(12.7));
        }
        else {
            ui->le_minimun_thickness_t1->setText(QString::number(-1));
        }
    }
    else
    {
        ui->le_minimun_thickness_t1->setText(QString::number(-1));
    }
}


void MainWindow::on_cb_material_currentIndexChanged(const QString &arg1)
{
   //s calculate_thickness(arg1);
}


void MainWindow::on_cb_material_editTextChanged(const QString &arg1)
{
     calculate_thickness(arg1);
}


void MainWindow::on_cb_material_currentTextChanged(const QString &arg1)
{
    calculate_thickness(arg1);
}



void MainWindow::on_le_plate_thickness_b_editingFinished()
{
    float ration = ui->le_plate_thickness_a->text().toFloat()/ui->le_plate_thickness_b->text().toFloat();
    ui->le_ratio->setText(QString::number(ration,'f', 4));
    if(ui->cb_condition->currentText() == "Three sides fixed - One side simply supported")
    {
       if(ration == 0.25) {
           ui->le_factor->setText(QString::number(0.020,'f', 4));
       }
       else if((ration > 0.25)&&(ration < 0.50)){
           float N37 = 0.25;
           float P37 = 0.5;
           float N38 = 0.020;
           float P38 = 0.081;
           float O37 = ration;
           float mul1 = (N37 - O37)/(N37 - P37);
           float mul2 = N38 - P38;
           float value = N38 - (mul1 * mul2);
           ui->le_factor->setText(QString::number(value,'f', 4));
       }
       else if(ration == 0.50) {
            ui->le_factor->setText(QString::number(0.081,'f', 4));
       }
       else if((ration >  0.50)&&(ration < 0.75)){
           float N37 = 0.50;
           float P37 = 0.75;
           float N38 = 0.081;
           float P38 = 0.173;
           float O37 = ration;
           float mul1 = (N37 - O37)/(N37 - P37);
           float mul2 = N38 - P38;
           float value = N38 - (mul1 * mul2);
           ui->le_factor->setText(QString::number(value,'f', 4));
       }
       else if(ration == 0.75) {
            ui->le_factor->setText(QString::number(0.173,'f', 4));
       }
       else if((ration >  0.75)&&(ration < 1.0)){
            float N37 = 0.75;
            float P37 = 1.0;
            float N38 = 0.173;
            float P38 = .307;
            float O37 = ration;
            float mul1 = (N37 - O37)/(N37 - P37);
            float mul2 = N38 - P38;
            float value = N38 - (mul1 * mul2);
            ui->le_factor->setText(QString::number(value,'f', 4));
       }
       else {
           ui->le_factor->setText(QString::number(0,'f', 4));
       }
    }
    else if(ui->cb_condition->currentText() == "Long sides fixed Short sides simply supported")
    {
        if(ration == 1.0) {
            ui->le_factor->setText(QString::number(.4182,'f', 4));
        }
        else if((ration > 1.0)&&(ration < 1.2)){
            float N37 = 1.0;
            float P37 = 1.2;
            float N38 = .4182;
            float P38 = .4626;
            float O37 = ration;
            float mul1 = (N37 - O37)/(N37 - P37);
            float mul2 = N38 - P38;
            float value = N38 - (mul1 * mul2);
            ui->le_factor->setText(QString::number(value,'f', 4));
        }
        else if(ration == 1.2) {
             ui->le_factor->setText(QString::number(.4182,'f', 4));
        }
        else if((ration > 1.2)&&(ration < 1.4)){
            float N37 = 1.2;
            float P37 = 1.4;
            float N38 = .4626;
            float P38 = .4860;
            float O37 = ration;
            float mul1 = (N37 - O37)/(N37 - P37);
            float mul2 = N38 - P38;
            float value = N38 - (mul1 * mul2);
            ui->le_factor->setText(QString::number(value,'f', 4));
        }
        else if(ration == 1.4) {
             ui->le_factor->setText(QString::number(.4860,'f', 4));
        }
        else if((ration > 1.4)&&(ration < 1.6)){
            float N37 = 1.4;
            float P37 = 1.6;
            float N38 = .4860;
            float P38 = .4968;
            float O37 = ration;
            float mul1 = (N37 - O37)/(N37 - P37);
            float mul2 = N38 - P38;
            float value = N38 - (mul1 * mul2);
            ui->le_factor->setText(QString::number(value,'f', 4));
        }
        else if(ration == 1.6) {
             ui->le_factor->setText(QString::number(.4968,'f', 4));
        }
        else if((ration > 1.6)&&(ration < 1.8)){
            float N37 = 1.6;
            float P37 = 1.8;
            float N38 = .4968;
            float P38 = .4971;
            float O37 = ration;
            float mul1 = (N37 - O37)/(N37 - P37);
            float mul2 = N38 - P38;
            float value = N38 - (mul1 * mul2);
            ui->le_factor->setText(QString::number(value,'f', 4));
        }
        else if(ration == 1.8) {
             ui->le_factor->setText(QString::number(.4971,'f', 4));
        }
        else if((ration > 1.8)&&(ration < 2.0)){
            float N37 = 1.8;
            float P37 = 2.0;
            float N38 = .4971;
            float P38 = .4973;
            float O37 = ration;
            float mul1 = (N37 - O37)/(N37 - P37);
            float mul2 = N38 - P38;
            float value = N38 - (mul1 * mul2);
            ui->le_factor->setText(QString::number(value,'f', 4));
        }
        else if(ration == 2.0) {
             ui->le_factor->setText(QString::number(.4973,'f', 4));
        }
        else {
            ui->le_factor->setText(QString::number(0.5000,'f', 4));
        }
    }
    else if(ui->cb_condition->currentText() == "Short sides fixed Long sides simply supported")
    {
        if(ration == 1.0) {
            ui->le_factor->setText(QString::number(.4182,'f', 4));
        }
        else if((ration > 1.0)&&(ration < 1.2)){
            float N37 = 1.0;
            float P37 = 1.2;
            float N38 = .4182;
            float P38 = .5208;
            float O37 = ration;
            float mul1 = (N37 - O37)/(N37 - P37);
            float mul2 = N38 - P38;
            float value = N38 - (mul1 * mul2);
            ui->le_factor->setText(QString::number(value,'f', 4));
        }
        else if(ration == 1.2) {
             ui->le_factor->setText(QString::number(.5208,'f', 4));
        }
        else if((ration > 1.2)&&(ration < 1.4)){
            float N37 = 1.2;
            float P37 = 1.4;
            float N38 = .5208;
            float P38 = .5988;
            float O37 = ration;
            float mul1 = (N37 - O37)/(N37 - P37);
            float mul2 = N38 - P38;
            float value = N38 - (mul1 * mul2);
            ui->le_factor->setText(QString::number(value,'f', 4));
        }
        else if(ration == 1.4) {
             ui->le_factor->setText(QString::number(.5988,'f', 4));
        }
        else if((ration > 1.4)&&(ration < 1.6)){
            float N37 = 1.4;
            float P37 = 1.6;
            float N38 = .5988;
            float P38 = .6540;
            float O37 = ration;
            float mul1 = (N37 - O37)/(N37 - P37);
            float mul2 = N38 - P38;
            float value = N38 - (mul1 * mul2);
            ui->le_factor->setText(QString::number(value,'f', 4));
        }
        else if(ration == 1.6) {
             ui->le_factor->setText(QString::number(.6540,'f', 4));
        }
        else if((ration > 1.6)&&(ration < 1.8)){
            float N37 = 1.6;
            float P37 = 1.8;
            float N38 = .6540;
            float P38 = .6912;
            float O37 = ration;
            float mul1 = (N37 - O37)/(N37 - P37);
            float mul2 = N38 - P38;
            float value = N38 - (mul1 * mul2);
            ui->le_factor->setText(QString::number(value,'f', 4));
        }
        else if(ration == 1.8) {
             ui->le_factor->setText(QString::number(.6912,'f', 4));
        }
        else if((ration > 1.8)&&(ration < 2.0)){
            float N37 = 1.8;
            float P37 = 2.0;
            float N38 = .6912;
            float P38 = .7146;
            float O37 = ration;
            float mul1 = (N37 - O37)/(N37 - P37);
            float mul2 = N38 - P38;
            float value = N38 - (mul1 * mul2);
            ui->le_factor->setText(QString::number(value,'f', 4));
        }
        else if(ration == 2.0) {
             ui->le_factor->setText(QString::number(.7146,'f', 4));
        }
        else {
            ui->le_factor->setText(QString::number(0.7500,'f', 4));
        }
    }
    else {
        QMessageBox::warning(this, tr("My Application"),
                             tr("Invalid Selection"));
    }

    // Calculate plate thickness
    double H36 = ui->le_plate_thickness_b->text().toDouble();
    double H16 = ui->le_press_drop->text().toDouble();
    double H44 = ui->le_factor->text().toDouble();
    double H14 = ui->le_allowable_stress->text().toDouble();
    double P8 = ui->le_minimun_thickness_t1->text().toDouble();
    double minReqdThickness = (H36*sqrt(H16*H44/(1.5*H14)));
    ui->le_min_reqd_thickness->setText(QString::number(minReqdThickness,'f', 4));

}


void MainWindow::on_le_plate_thickness_a_editingFinished()
{
    float ration = ui->le_plate_thickness_a->text().toFloat()/ui->le_plate_thickness_b->text().toFloat();
    ui->le_ratio->setText(QString::number(ration,'f', 4));
    if(ui->cb_condition->currentText() == "Three sides fixed - One side simply supported")
    {
       if(ration == 0.25) {
           ui->le_factor->setText(QString::number(0.020,'f', 4));
       }
       else if((ration > 0.25)&&(ration < 0.50)){
           float N37 = 0.25;
           float P37 = 0.5;
           float N38 = 0.020;
           float P38 = 0.081;
           float O37 = ration;
           float mul1 = (N37 - O37)/(N37 - P37);
           float mul2 = N38 - P38;
           float value = N38 - (mul1 * mul2);
           ui->le_factor->setText(QString::number(value,'f', 4));
       }
       else if(ration == 0.50) {
            ui->le_factor->setText(QString::number(0.081,'f', 4));
       }
       else if((ration >  0.50)&&(ration < 0.75)){
           float N37 = 0.50;
           float P37 = 0.75;
           float N38 = 0.081;
           float P38 = 0.173;
           float O37 = ration;
           float mul1 = (N37 - O37)/(N37 - P37);
           float mul2 = N38 - P38;
           float value = N38 - (mul1 * mul2);
           ui->le_factor->setText(QString::number(value,'f', 4));
       }
       else if(ration == 0.75) {
            ui->le_factor->setText(QString::number(0.173,'f', 4));
       }
       else if((ration >  0.75)&&(ration < 1.0)){
            float N37 = 0.75;
            float P37 = 1.0;
            float N38 = 0.173;
            float P38 = .307;
            float O37 = ration;
            float mul1 = (N37 - O37)/(N37 - P37);
            float mul2 = N38 - P38;
            float value = N38 - (mul1 * mul2);
            ui->le_factor->setText(QString::number(value,'f', 4));
       }
       else {
           ui->le_factor->setText(QString::number(0,'f', 4));
       }
    }
    else if(ui->cb_condition->currentText() == "Long sides fixed Short sides simply supported")
    {
        if(ration == 1.0) {
            ui->le_factor->setText(QString::number(.4182,'f', 4));
        }
        else if((ration > 1.0)&&(ration < 1.2)){
            float N37 = 1.0;
            float P37 = 1.2;
            float N38 = .4182;
            float P38 = .4626;
            float O37 = ration;
            float mul1 = (N37 - O37)/(N37 - P37);
            float mul2 = N38 - P38;
            float value = N38 - (mul1 * mul2);
            ui->le_factor->setText(QString::number(value,'f', 4));
        }
        else if(ration == 1.2) {
             ui->le_factor->setText(QString::number(.4182,'f', 4));
        }
        else if((ration > 1.2)&&(ration < 1.4)){
            float N37 = 1.2;
            float P37 = 1.4;
            float N38 = .4626;
            float P38 = .4860;
            float O37 = ration;
            float mul1 = (N37 - O37)/(N37 - P37);
            float mul2 = N38 - P38;
            float value = N38 - (mul1 * mul2);
            ui->le_factor->setText(QString::number(value,'f', 4));
        }
        else if(ration == 1.4) {
             ui->le_factor->setText(QString::number(.4860,'f', 4));
        }
        else if((ration > 1.4)&&(ration < 1.6)){
            float N37 = 1.4;
            float P37 = 1.6;
            float N38 = .4860;
            float P38 = .4968;
            float O37 = ration;
            float mul1 = (N37 - O37)/(N37 - P37);
            float mul2 = N38 - P38;
            float value = N38 - (mul1 * mul2);
            ui->le_factor->setText(QString::number(value,'f', 4));
        }
        else if(ration == 1.6) {
             ui->le_factor->setText(QString::number(.4968,'f', 4));
        }
        else if((ration > 1.6)&&(ration < 1.8)){
            float N37 = 1.6;
            float P37 = 1.8;
            float N38 = .4968;
            float P38 = .4971;
            float O37 = ration;
            float mul1 = (N37 - O37)/(N37 - P37);
            float mul2 = N38 - P38;
            float value = N38 - (mul1 * mul2);
            ui->le_factor->setText(QString::number(value,'f', 4));
        }
        else if(ration == 1.8) {
             ui->le_factor->setText(QString::number(.4971,'f', 4));
        }
        else if((ration > 1.8)&&(ration < 2.0)){
            float N37 = 1.8;
            float P37 = 2.0;
            float N38 = .4971;
            float P38 = .4973;
            float O37 = ration;
            float mul1 = (N37 - O37)/(N37 - P37);
            float mul2 = N38 - P38;
            float value = N38 - (mul1 * mul2);
            ui->le_factor->setText(QString::number(value,'f', 4));
        }
        else if(ration == 2.0) {
             ui->le_factor->setText(QString::number(.4973,'f', 4));
        }
        else {
            ui->le_factor->setText(QString::number(0.5000,'f', 4));
        }
    }
    else if(ui->cb_condition->currentText() == "Short sides fixed Long sides simply supported")
    {
        if(ration == 1.0) {
            ui->le_factor->setText(QString::number(.4182,'f', 4));
        }
        else if((ration > 1.0)&&(ration < 1.2)){
            float N37 = 1.0;
            float P37 = 1.2;
            float N38 = .4182;
            float P38 = .5208;
            float O37 = ration;
            float mul1 = (N37 - O37)/(N37 - P37);
            float mul2 = N38 - P38;
            float value = N38 - (mul1 * mul2);
            ui->le_factor->setText(QString::number(value,'f', 4));
        }
        else if(ration == 1.2) {
             ui->le_factor->setText(QString::number(.5208,'f', 4));
        }
        else if((ration > 1.2)&&(ration < 1.4)){
            float N37 = 1.2;
            float P37 = 1.4;
            float N38 = .5208;
            float P38 = .5988;
            float O37 = ration;
            float mul1 = (N37 - O37)/(N37 - P37);
            float mul2 = N38 - P38;
            float value = N38 - (mul1 * mul2);
            ui->le_factor->setText(QString::number(value,'f', 4));
        }
        else if(ration == 1.4) {
             ui->le_factor->setText(QString::number(.5988,'f', 4));
        }
        else if((ration > 1.4)&&(ration < 1.6)){
            float N37 = 1.4;
            float P37 = 1.6;
            float N38 = .5988;
            float P38 = .6540;
            float O37 = ration;
            float mul1 = (N37 - O37)/(N37 - P37);
            float mul2 = N38 - P38;
            float value = N38 - (mul1 * mul2);
            ui->le_factor->setText(QString::number(value,'f', 4));
        }
        else if(ration == 1.6) {
             ui->le_factor->setText(QString::number(.6540,'f', 4));
        }
        else if((ration > 1.6)&&(ration < 1.8)){
            float N37 = 1.6;
            float P37 = 1.8;
            float N38 = .6540;
            float P38 = .6912;
            float O37 = ration;
            float mul1 = (N37 - O37)/(N37 - P37);
            float mul2 = N38 - P38;
            float value = N38 - (mul1 * mul2);
            ui->le_factor->setText(QString::number(value,'f', 4));
        }
        else if(ration == 1.8) {
             ui->le_factor->setText(QString::number(.6912,'f', 4));
        }
        else if((ration > 1.8)&&(ration < 2.0)){
            float N37 = 1.8;
            float P37 = 2.0;
            float N38 = .6912;
            float P38 = .7146;
            float O37 = ration;
            float mul1 = (N37 - O37)/(N37 - P37);
            float mul2 = N38 - P38;
            float value = N38 - (mul1 * mul2);
            ui->le_factor->setText(QString::number(value,'f', 4));
        }
        else if(ration == 2.0) {
             ui->le_factor->setText(QString::number(.7146,'f', 4));
        }
        else {
            ui->le_factor->setText(QString::number(0.7500,'f', 4));
        }
    }
    else {
        QMessageBox::warning(this, tr("My Application"),
                             tr("Invalid Selection"));
    }

    // Calculate plate thickness
    double H36 = ui->le_plate_thickness_b->text().toDouble();
    double H16 = ui->le_press_drop->text().toDouble();
    double H44 = ui->le_factor->text().toDouble();
    double H14 = ui->le_allowable_stress->text().toDouble();
    double P8 = ui->le_minimun_thickness_t1->text().toDouble();
    double minReqdThickness = (H36*sqrt(H16*H44/(1.5*H14)));
    ui->le_min_reqd_thickness->setText(QString::number(minReqdThickness,'f', 4));
}


void MainWindow::on_pb_submit_clicked()
{
    QString password = QString("%1").arg(QString(QCryptographicHash::hash(ui->le_password->text().toUtf8(),QCryptographicHash::Md5).toHex()));
    QString username = ui->le_username->text();
    QString filename="\\\\MANISH\\Users\\MANISH\\Downloads\\Test-Deploy\\Data.txt";
    QFile file( filename );
    std::map<QString, QString> usermap;
    if ( !file.open(QIODevice::ReadOnly) ){
        QMessageBox::warning(this, tr("My Application"),
                             tr("Unable to locate the Database File"));
    }
    QString Complete_Data = file.readAll();
    QStringList userdetails;
    if(!Complete_Data.isEmpty())
        userdetails = Complete_Data.split("\n");

    foreach(QString user, userdetails) {
        if(user.isEmpty())
            continue;
        QString username = user.split(" ").at(0);
        QString password = user.split("\n").at(0).split(" ").at(1).split("\r").at(0);
        usermap.insert({username, password});
    }
    if(auto it = usermap.find(username); it != usermap.end())
    {
        if(password == it->second) {
            ui->stackedWidget->setCurrentIndex(0);
        }
        else {
            QMessageBox::warning(this, tr("My Application"),
                                 tr("Password incorrect"));
        }
    }
    else {
        QMessageBox::warning(this, tr("My Application"),
                             tr("User Does Not Exists"));
    }

}


void MainWindow::on_le_press_drop_editingFinished()
{

    // Calculate plate thickness
    double H36 = ui->le_plate_thickness_b->text().toDouble();
    double H16 = ui->le_press_drop->text().toDouble();
    double H44 = ui->le_factor->text().toDouble();
    double H14 = ui->le_allowable_stress->text().toDouble();
    double P8 = ui->le_minimun_thickness_t1->text().toDouble();
    double minReqdThickness = (H36*sqrt(H16*H44/(1.5*H14)));
    ui->le_min_reqd_thickness->setText(QString::number(minReqdThickness,'f', 4));
}


void MainWindow::on_le_factor_editingFinished()
{
    // Calculate plate thickness
    double H36 = ui->le_plate_thickness_b->text().toDouble();
    double H16 = ui->le_press_drop->text().toDouble();
    double H44 = ui->le_factor->text().toDouble();
    double H14 = ui->le_allowable_stress->text().toDouble();
    double P8 = ui->le_minimun_thickness_t1->text().toDouble();
    double minReqdThickness = (H36*sqrt(H16*H44/(1.5*H14)));
    ui->le_min_reqd_thickness->setText(QString::number(minReqdThickness,'f', 4));
}


void MainWindow::on_le_allowable_stress_editingFinished()
{
    // Calculate plate thickness
    double H36 = ui->le_plate_thickness_b->text().toDouble();
    double H16 = ui->le_press_drop->text().toDouble();
    double H44 = ui->le_factor->text().toDouble();
    double H14 = ui->le_allowable_stress->text().toDouble();
    double P8 = ui->le_minimun_thickness_t1->text().toDouble();
    double minReqdThickness = (H36*sqrt(H16*H44/(1.5*H14)));
    ui->le_min_reqd_thickness->setText(QString::number(minReqdThickness,'f', 4));
}


void MainWindow::on_le_minimun_thickness_t1_editingFinished()
{
    // Calculate plate thickness
    double H36 = ui->le_plate_thickness_b->text().toDouble();
    double H16 = ui->le_press_drop->text().toDouble();
    double H44 = ui->le_factor->text().toDouble();
    double H14 = ui->le_allowable_stress->text().toDouble();
    double P8 = ui->le_minimun_thickness_t1->text().toDouble();
    double minReqdThickness = (H36*sqrt(H16*H44/(1.5*H14)));
    ui->le_min_reqd_thickness->setText(QString::number(minReqdThickness,'f', 4));
}


void MainWindow::on_le_provided_thickness_t_editingFinished()
{
    double filletsize = ui->le_provided_thickness_t->text().toDouble()* 0.75;
    ui->le_FilletSize->setText(QString::number(filletsize,'f', 4));
}


void MainWindow::on_le_minimun_thickness_t1_textChanged(const QString &arg1)
{
    if(arg1.toDouble() < ui->le_min_reqd_thickness->text().toDouble()) {
        ui->uimin_reqd_thickness_as_per_2->setText(QString::number(ui->le_min_reqd_thickness->text().toDouble(),'f', 4));
    }
    else {
        ui->uimin_reqd_thickness_as_per_2->setText(QString::number(arg1.toDouble(),'f', 4));
    }
}


void MainWindow::on_le_min_reqd_thickness_textChanged(const QString &arg2)
{
    QString arg= ui->le_minimun_thickness_t1->text();
    if(arg.toDouble() < ui->le_min_reqd_thickness->text().toDouble()) {
        ui->uimin_reqd_thickness_as_per_2->setText(QString::number(ui->le_min_reqd_thickness->text().toDouble(),'f', 4));
    }
    else {
        ui->uimin_reqd_thickness_as_per_2->setText(QString::number(arg.toDouble(),'f', 4));
    }
}


void MainWindow::on_uimin_reqd_thickness_as_per_2_textChanged(const QString &arg1)
{
    double val = ui->uimin_reqd_thickness_as_per_2->text().toDouble() + ui->le_CA->text().toDouble();
    ui->le_CA_PLUS_thickness->setText(QString::number(val,'f', 4));
}


void MainWindow::on_le_CA_editingFinished()
{
    double val = ui->uimin_reqd_thickness_as_per_2->text().toDouble() + ui->le_CA->text().toDouble();
    ui->le_CA_PLUS_thickness->setText(QString::number(val,'f', 4));
}


void MainWindow::on_pb_calculate_clicked()
{
    if(ui->le_provided_thickness_t->text().toDouble() > ui->le_CA_PLUS_thickness->text().toDouble()) {
        ui->cb_result->setText("Provided thickness is sufficient");
        ui->cb_result->setChecked(true);
    }
    else {
        ui->cb_result->setText("Increase thickness");
        ui->cb_result->setChecked(false);
    }
}


void MainWindow::on_pb_generate_report_clicked()
{
    QString fileName = "C:/ISGEC-TOOLS/Pass partition plate.xlsx";
    QFile file_write(fileName);
    if(file_write.open(QIODevice::ReadWrite)) {
       excel     = new QAxObject("Excel.Application");
       workbooks = excel->querySubObject("Workbooks");
       workbook  = workbooks->querySubObject("Open(const QString&)",fileName);
       sheets    = workbook->querySubObject("Worksheets");
       sheet     = sheets->querySubObject("Item(int)", 1);
    }
    else {

        QMessageBox::warning(this, tr("My Application"),
                             tr("Can not open file to write please check if file is closed"));
    }
    file_write.close();
    // on_pb_check_for_shear_clicked ////////////////////////////////////////////////////////////////
    if(ui->le_provided_thickness_t->text().toDouble() > ui->le_CA_PLUS_thickness->text().toDouble()) {
        auto cell = sheet->querySubObject("Cells(int,int)", 56,8);
        cell->setProperty("Value", "SAFE");
    }
    else {
        auto cell = sheet->querySubObject("Cells(int,int)", 56,8);
        cell->setProperty("Value", "UNSAFE");
    }

    auto cell = sheet->querySubObject("Cells(int,int)", 5,3);
    cell->setProperty("Value", ui->le_designed_by->text());

    cell = sheet->querySubObject("Cells(int,int)", 1,6);
    cell->setProperty("Value", ui->le_client->text());

    cell = sheet->querySubObject("Cells(int,int)", 3,6);
    cell->setProperty("Value", ui->le_eqpt->text());

    cell = sheet->querySubObject("Cells(int,int)", 5,6);
    cell->setProperty("Value", ui->le_job_no->text());

    cell = sheet->querySubObject("Cells(int,int)", 5,8);
    cell->setProperty("Value", ui->le_dr_no->text());

    cell = sheet->querySubObject("Cells(int,int)", 6,6);
    cell->setProperty("Value", ui->le_doc_no->text());

    cell = sheet->querySubObject("Cells(int,int)", 5,11);
    cell->setProperty("Value", ui->le_rev->text());


    cell = sheet->querySubObject("Cells(int,int)", 14,8);
    cell->setProperty("Value", ui->le_allowable_stress->text());

    cell = sheet->querySubObject("Cells(int,int)", 16,8);
    cell->setProperty("Value", ui->le_press_drop->text());

    cell = sheet->querySubObject("Cells(int,int)", 18,8);
    cell->setProperty("Value", ui->le_inside_diameter->text());


    cell = sheet->querySubObject("Cells(int,int)", 20,8);
    cell->setProperty("Value", ui->le_minimun_thickness_t1->text());

    cell = sheet->querySubObject("Cells(int,int)", 36,8);
    cell->setProperty("Value", ui->le_plate_thickness_b->text());

    cell = sheet->querySubObject("Cells(int,int)", 38,8);
    cell->setProperty("Value", ui->le_plate_thickness_a->text());


    cell = sheet->querySubObject("Cells(int,int)", 40,6);
    cell->setProperty("Value", ui->cb_condition->currentText());

    cell = sheet->querySubObject("Cells(int,int)", 42,8);
    cell->setProperty("Value", ui->le_ratio->text());

    cell = sheet->querySubObject("Cells(int,int)", 44,8);
    cell->setProperty("Value", ui->le_factor->text());

    cell = sheet->querySubObject("Cells(int,int)", 46,8);
    cell->setProperty("Value", ui->le_min_reqd_thickness->text());

    cell = sheet->querySubObject("Cells(int,int)", 48,8);
    cell->setProperty("Value", ui->uimin_reqd_thickness_as_per_2->text());

    cell = sheet->querySubObject("Cells(int,int)", 50,8);
    cell->setProperty("Value", ui->le_CA->text());

    cell = sheet->querySubObject("Cells(int,int)", 52,8);
    cell->setProperty("Value", ui->le_CA_PLUS_thickness->text());

    cell = sheet->querySubObject("Cells(int,int)", 54,8);
    cell->setProperty("Value", ui->le_provided_thickness_t->text());

    cell = sheet->querySubObject("Cells(int,int)", 56,8);
    cell->setProperty("Value", ui->le_FilletSize->text());

    cell = sheet->querySubObject("Cells(int,int)", 58,6);
    cell->setProperty("Value", ui->cb_result->text());

    workbook->dynamicCall("Save()");
    workbook->dynamicCall("Close()");
    workbook->dynamicCall("Quit()");
    excel->dynamicCall("Quit()");
    file_write.close();

    QMessageBox msgBox;
    msgBox.setText("Report has been generated... ");
    msgBox.exec();
}

