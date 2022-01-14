#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QAxObject>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_le_inside_diameter_editingFinished();

    void on_cb_material_activated(const QString &arg1);
    void calculate_thickness(QString arg1);


    void on_cb_material_currentIndexChanged(const QString &arg1);

    void on_cb_material_editTextChanged(const QString &arg1);

    void on_cb_material_currentTextChanged(const QString &arg1);


    void on_le_plate_thickness_b_editingFinished();

    void on_le_plate_thickness_a_editingFinished();

    void on_pb_submit_clicked();

    void on_le_press_drop_editingFinished();

    void on_le_factor_editingFinished();

    void on_le_allowable_stress_editingFinished();

    void on_le_minimun_thickness_t1_editingFinished();

    void on_le_provided_thickness_t_editingFinished();

    void on_le_minimun_thickness_t1_textChanged(const QString &arg1);

    void on_le_min_reqd_thickness_textChanged(const QString &arg1);

    void on_uimin_reqd_thickness_as_per_2_textChanged(const QString &arg1);

    void on_le_CA_editingFinished();

    void on_pb_calculate_clicked();

    void on_pb_generate_report_clicked();

private:
    Ui::MainWindow *ui;
    QAxObject * excel;
    QAxObject * workbooks;
    QAxObject * workbook;
    QAxObject * sheets;
    QAxObject * sheet;


};
#endif // MAINWINDOW_H
