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


QMap<QString, QMap<QString, QMap<QString, QList<QString>>>> m_map;
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

    ui->fh_cb_lifting_ug_materia_2->setInsertPolicy(QComboBox::NoInsert);
    ui->fh_cb_lifting_ug_materia_2->completer()->setCompletionMode(QCompleter::PopupCompletion);
    ui->fh_cb_lifting_ug_materia_2->completer()->setFilterMode(Qt::MatchContains);
    ui->le_inside_diameter->setText(QString::number(0));
    connect(ui->fh_cb_lifting_ug_materia_2,SIGNAL(textActivated(QString)), this, SLOT (on_fh_cb_lifting_ug_materia_2_activated(QString)));
    connect(ui->fh_cb_grade_2,SIGNAL(textActivated(QString)), this, SLOT (on_fh_cb_grade_2_activated(QString)));
    connect(ui->cb_thickness,SIGNAL(textActivated(QString)), this, SLOT (on_cb_thickness_activated(QString)));
    load_temprature_data();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::load_temprature_data()
{
    QString fileName = "C:/ISGEC-TOOLS/ALLOWABLE STRESS TABLE.xlsx";
    QFile file(fileName);
    try {
        if(file.open(QIODevice::ReadOnly)) {
            qDebug() << "File opened succesfully";
            auto excel     = new QAxObject("Excel.Application");
            auto workbooks = excel->querySubObject("Workbooks");
            auto workbook  = workbooks->querySubObject("Open(const QString&)",fileName);
            auto sheets    = workbook->querySubObject("Worksheets");
            auto sheet     = sheets->querySubObject("Item(int)", 1);

            QVariant var;
            if (sheet != NULL && ! sheet->isNull())
            {
                QAxObject *usedRange = sheet->querySubObject("UsedRange");

                var = usedRange->dynamicCall("Value");
                delete usedRange;
            }

            workbook->dynamicCall("Close()");
            excel->dynamicCall("Quit()");
            delete excel;

            QVariantList varRows = var.toList();
            const int rowCount = varRows.size();
            QVariantList rowData;

            for(int i=1;i<rowCount;++i)
            {
                rowData = varRows[i].toList();
                auto specno =rowData[2].toString();
                specno = specno.trimmed();
                if(specno == "") continue;
                if(!m_map.contains(specno)) {
                    auto grade= rowData[3].toString();
                    m_map.insert(specno,{});
                    QMap<QString, QMap<QString, QList<QString>>> &it = m_map.find(specno).value();
                    it.insert(grade,{});
                    QMap<QString, QList<QString>> &it_grade = it.find(grade).value();
                    auto thickness = rowData[4].toString();
                    QList<QString> temp_list;
                    auto composition = rowData[1].toString();
                    temp_list.append(composition);
                    for(int i = 0; i < 37; i++)
                    {
                        auto temprature = rowData[5 + i].toString();
                        temp_list.append(temprature);
                    }

                    it_grade.insert(thickness,temp_list);
                }

                else {
                    QMap<QString, QMap<QString, QList<QString>>> &it = m_map.find(specno).value();
                    auto grade = rowData[3].toString();
                    grade = grade.trimmed();
                    QList<QString> temp_list;
                    auto composition = rowData[1].toString();
                    temp_list.append(composition);
                    for(int i = 0; i < 37; i++)
                    {
                        auto temprature = rowData[5 + i].toString();
                        temp_list.append(temprature);
                    }
                    if(!it.contains(grade)) {
                        it.insert(grade,{});
                        QMap<QString, QList<QString>> &it_grade = it.find(grade).value();
                        auto thickness = rowData[4].toString();
                        it_grade.insert(thickness,temp_list);
                    }
                    else {
                        QMap<QString, QList<QString>> &it_grade = it.find(grade).value();
                        auto thickness = rowData[4].toString();
                        it_grade.insert(thickness,temp_list);
                    }
                }
            }

        }
    }
    catch(...) {
        file.close();
    }
    file.close();
    QMapIterator<QString,QMap<QString, QMap<QString, QList<QString>>>> i(m_map);
    while (i.hasNext()) {
        i.next();
        ui->fh_cb_lifting_ug_materia_2->addItem(i.key());
    }
    ui->fh_cb_lifting_ug_materia_2->setCurrentIndex(0);
    on_fh_cb_lifting_ug_materia_2_activated(ui->fh_cb_lifting_ug_materia_2->currentText());
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
            QMessageBox::warning(this, tr("My Application"),
                                 tr("Diameter Out Of Range"));
            ui->le_minimun_thickness_t1->setText(QString::number(0));
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
            QMessageBox::warning(this, tr("My Application"),
                                 tr("Diameter Out Of Range"));
            ui->le_minimun_thickness_t1->setText(QString::number(0));
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
    if(!(ui->le_plate_thickness_b->text().isEmpty() || ui->le_plate_thickness_a->text().isEmpty()))
    {
        calculate_B_Factor();
    }
}


void MainWindow::on_le_plate_thickness_a_editingFinished()
{
    if(!(ui->le_plate_thickness_b->text().isEmpty() || ui->le_plate_thickness_a->text().isEmpty()))
    {
        calculate_B_Factor();
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
    int filletsize_int = filletsize;
    if(double(filletsize_int) == filletsize)
    {
        ui->le_FilletSize->setText(QString::number(filletsize_int));
    }
    else
    {
        ui->le_FilletSize->setText(QString::number(filletsize_int + 1));
    }
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
    QString fileName_pdf = "C:/ISGEC-TOOLS/Pass_partition_plate";
    QFile::remove(fileName_pdf + ".pdf");
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
    workbook->dynamicCall("ExportAsFixedFormat(int, const QString&, int, BOOL, BOOL)", 0, fileName_pdf, 0, false, true);
    workbook->dynamicCall("Close()");
    workbook->dynamicCall("Quit()");
    excel->dynamicCall("Quit()");
    file_write.close();

    QMessageBox msgBox;
    msgBox.setText("Report has been generated... ");
    msgBox.exec();
}


void MainWindow::on_fh_cb_lifting_ug_materia_2_activated(const QString &arg1)
{
    QMap<QString, QMap<QString, QList<QString>>> &it_grade = m_map.find(arg1).value();
    ui->fh_cb_grade_2->clear();
    if(!it_grade.empty()) {
        QMapIterator<QString, QMap<QString, QList<QString>>> i(it_grade);
        while (i.hasNext()) {
            i.next();
            QString key = i.key();
            ui->fh_cb_grade_2->addItem(key);
            ui->fh_cb_grade_2->setCurrentIndex(-1);
        }
    }
    ui->fh_cb_grade_2->setCurrentIndex(0);
    if(ui->fh_cb_grade_2->count())
        on_cb_grade_activated_custom(ui->fh_cb_grade_2->currentText());
}

void MainWindow::on_cb_grade_activated_custom(const QString &arg1)
{
    QMap<QString, QList<QString>> &ref = m_map.find(ui->fh_cb_lifting_ug_materia_2->currentText()).value().find(arg1).value();
    QMapIterator<QString, QList<QString>> i_thickness(ref);
    ui->cb_thickness->clear();
    ui->cb_thickness->setCurrentIndex(-1);
    while (i_thickness.hasNext()) {
        i_thickness.next();
        ui->cb_thickness->addItem(i_thickness.key());
    }
    ui->cb_thickness->setCurrentIndex(0);
    if(ui->cb_thickness->count())
        on_cb_thickness_activated_custom(ui->cb_thickness->currentText());
}

void MainWindow::on_fh_cb_grade_2_activated(const QString &arg1)
{
    QMap<QString, QList<QString>> &ref = m_map.find(ui->fh_cb_lifting_ug_materia_2->currentText()).value().find(arg1).value();
    QMapIterator<QString, QList<QString>> i_thickness(ref);
    ui->cb_thickness->clear();
    ui->cb_thickness->setCurrentIndex(-1);
    while (i_thickness.hasNext()) {
        i_thickness.next();
        ui->cb_thickness->addItem(i_thickness.key());
    }
    ui->cb_thickness->setCurrentIndex(0);
    if(ui->cb_thickness->count())
        on_cb_thickness_activated_custom(ui->cb_thickness->currentText());
}

void MainWindow::on_cb_thickness_activated_custom(const QString &arg1)
{
    QList<QString> &ref = m_map.find(ui->fh_cb_lifting_ug_materia_2->currentText()).value().find(ui->fh_cb_grade_2->currentText()).value().find(arg1).value();
    if(!ref.isEmpty()) {
        ui->le_temprature->setText("40");
        ui->le_composition->setText(ref.at(0));
        ui->le_uns_no->setText(ref.at(1));
        ui->le_product_type->setText(ref.at(2));
        ui->le_allowable_stress->setText(QString::number( 10.197162 * ref.at(3).toDouble()));
    }
}

void MainWindow::on_cb_thickness_activated(const QString &arg1)
{
    QList<QString> &ref = m_map.find(ui->fh_cb_lifting_ug_materia_2->currentText()).value().find(ui->fh_cb_grade_2->currentText()).value().find(arg1).value();
    if(!ref.isEmpty()) {
        ui->le_temprature->setText("40");
        ui->le_composition->setText(ref.at(0));
        ui->le_uns_no->setText(ref.at(1));
        ui->le_product_type->setText(ref.at(2));
        ui->le_allowable_stress->setText(QString::number( 10.197162 * ref.at(3).toDouble()));
    }

}


void MainWindow::on_le_temprature_editingFinished()
{
    int temp = ui->le_temprature->text().toInt();
    QList<QString> &ref = m_map.find(ui->fh_cb_lifting_ug_materia_2->currentText()).value().find(ui->fh_cb_grade_2->currentText()).value().find(ui->cb_thickness->currentText()).value();
    QString allowableStress ;
    if(40 >= temp)
    {
        allowableStress = QString::number( ref.at(3).toDouble());
    }
    else if(temp > 40 && temp < 65)
    {
        int lowerindex = 3;
        int higherindex = 4;
        float x = 40;
        float z = 65;
        float y = temp;
        float m = ref.at(lowerindex).toDouble();
        float o = ref.at(higherindex).toDouble();
        allowableStress = QString::number(((o-m)/(z-x) * (y - x)) + m);
    }
    else if(temp == 65)
    {
        allowableStress = QString::number( ref.at(4).toDouble());
    }
    else if(temp > 65 && temp < 100)
    {
        int lowerindex = 4;
        int higherindex = 5;
        float x = 65;
        float z = 100;
        float y = temp;
        float m = ref.at(lowerindex).toDouble();
        float o = ref.at(higherindex).toDouble();
        allowableStress = QString::number(((o-m)/(z-x) * (y - x)) + m);
    }
    else if(temp - 100 >= 0 && temp %25 == 0)
    {
        int newtemp = temp -100;
        int index = newtemp / 25;
        QString temp = ref.at(index + 5);
        if(temp.contains("NA",Qt::CaseInsensitive) )
        {
            QMessageBox::warning(this, tr("My Application"),
                                 tr("Invalid Temprature"));
            ui->le_allowable_stress->setText("0");
            return;

        }
        allowableStress = QString::number( ref.at(5 + index).toDouble());
    }

    else if(temp > 900)
    {
        QMessageBox::warning(this, tr("My Application"),
                             tr("Invalid Temprature"));
        ui->le_allowable_stress->setText("0");
        return;

    }
    else
    {
        int newtemp = temp -100;
        int index = newtemp / 25;
        int lowerindex = 5 + index;
        int higherindex = lowerindex + 1;
        float x = 100 + index * 25;
        float z = x + 25;
        float y = temp;
        if(ref.at(lowerindex).contains("NA",Qt::CaseInsensitive) || ref.at(higherindex).contains("NA",Qt::CaseInsensitive))
        {
            QMessageBox::warning(this, tr("My Application"),
                                 tr("Invalid Temprature"));
            ui->le_allowable_stress->setText("0");
            return;

        }
        float m = ref.at(lowerindex).toDouble();
        float o = ref.at(higherindex).toDouble();
        allowableStress = QString::number(m -((m-o)/(z-x) * (y - x)));
    }
    QString Final_value = QString::number(10.197162 * allowableStress.toFloat());
    ui->le_allowable_stress->setText(Final_value);
}


void MainWindow::on_cb_condition_currentIndexChanged(const QString &arg1)
{
    if(!(ui->le_plate_thickness_b->text().isEmpty() || ui->le_plate_thickness_a->text().isEmpty()))
    {
        calculate_B_Factor();
    }
}

void MainWindow::calculate_B_Factor()
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
       else if(ration == 1.0) {
            ui->le_factor->setText(QString::number(0.307,'f', 4));
       }
       else if((ration >  1.0)&&(ration < 1.5)){
            float N37 = 1.0;
            float P37 = 1.5;
            float N38 = .307;
            float P38 = .539;
            float O37 = ration;
            float mul1 = (N37 - O37)/(N37 - P37);
            float mul2 = N38 - P38;
            float value = N38 - (mul1 * mul2);
            ui->le_factor->setText(QString::number(value,'f', 4));
       }
       else if(ration == 1.5) {
            ui->le_factor->setText(QString::number(0.539,'f', 4));
       }
       else if((ration >  1.5)&&(ration < 2.0)){
            float N37 = 1.5;
            float P37 = 2.0;
            float N38 = .539;
            float P38 = .657;
            float O37 = ration;
            float mul1 = (N37 - O37)/(N37 - P37);
            float mul2 = N38 - P38;
            float value = N38 - (mul1 * mul2);
            ui->le_factor->setText(QString::number(value,'f', 4));
       }
       else if(ration == 2.0) {
            ui->le_factor->setText(QString::number(0.657,'f', 4));
       }
       else if((ration >  2.0)&&(ration < 3.0)){
            float N37 = 2.0;
            float P37 = 3.0;
            float N38 = .657;
            float P38 = .718;
            float O37 = ration;
            float mul1 = (N37 - O37)/(N37 - P37);
            float mul2 = N38 - P38;
            float value = N38 - (mul1 * mul2);
            ui->le_factor->setText(QString::number(value,'f', 4));
       }
       else if(ration == 3.0) {
            ui->le_factor->setText(QString::number(0.718,'f', 4));
       }
       else {
           ui->le_factor->setText(QString::number(0,'f', 4));
           QMessageBox::warning(this, tr("My Application"),
                                tr("Invalid a/b ratio"));
       }
    }
    else if(ui->cb_condition->currentText() == "Long sides fixed Short sides simply supported")
    {
        if(ration < 1.0)
        {
            ui->le_factor->setText(QString::number(0,'f', 4));
            QMessageBox::warning(this, tr("My Application"),
                                 tr("Invalid a/b ratio"));
        }
        else if(ration == 1.0) {
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
        if(ration < 1.0)
        {
            ui->le_factor->setText(QString::number(0,'f', 4));
            QMessageBox::warning(this, tr("My Application"),
                                 tr("Invalid a/b ratio"));
        }
        else if(ration == 1.0) {
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


void MainWindow::on_cb_condition_currentTextChanged(const QString &arg1)
{
    if(!(ui->le_plate_thickness_b->text().isEmpty() || ui->le_plate_thickness_a->text().isEmpty()))
    {
        calculate_B_Factor();
    }
}

