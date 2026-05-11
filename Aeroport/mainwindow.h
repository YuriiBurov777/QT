#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "dbmanager.h"

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
    void on_airportComboBox_currentIndexChanged(int index);
    void on_departureRadioButton_toggled(bool checked);
    void on_arrivalRadioButton_toggled(bool checked);
    void on_showFlightsButton_clicked();
    void on_showStatsButton_clicked();
    void on_dateEdit_dateChanged(const QDate &date);
    void attemptReconnection();

private:
    void setupConnections();
    void loadAirports();
    void updateFlightTable();
    void setControlsEnabled(bool enabled);
    void updateStatusLabel();

    Ui::MainWindow *ui;
    DBManager *m_dbManager;
    QTimer *m_reconnectTimer;
    bool m_firstConnectionAttempt;
};

#endif // MAINWINDOW_H
