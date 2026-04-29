#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QtConcurrent>
#include <QSqlTableModel>
#include <QSqlQueryModel>
#include "database.h"
#include "dbdata.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void ReceiveStatusConnectionToDB(bool status);
    void onReceiveTableModel(QSqlTableModel* model, int typeR);
    void onReceiveQueryModel(QSqlQueryModel* model, int typeR);
    void ScreenDataFromDB(const QTableWidget *widget, int typeRequest);

private slots:
    void on_act_addData_triggered();
    void on_act_connect_triggered();
    void on_pb_request_clicked();
    void on_pb_clear_clicked();
    void on_cb_category_currentIndexChanged(int index);

    void on_pushButton_clicked();



signals:
    void sig_RequestToDb(QString request);

private:
    void setupTableViews();

    QVector<QString> dataForConnect; //Данные для подключения к БД.
    Ui::MainWindow *ui;
    DbData *dataDb;
    DataBase* dataBase;
    QMessageBox* msg;

    int currentRequestType;
};

#endif // MAINWINDOW_H
