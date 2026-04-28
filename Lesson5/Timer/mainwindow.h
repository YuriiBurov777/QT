
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "stopwatch.h"

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
    void on_startStopButton_clicked();
    void on_clearButton_clicked();
    void on_lapButton_clicked();

    void updateTimeDisplay(const QString &time);
    //void addLapToBrowser(const QString &lapInfo);


private:
    Ui::MainWindow *ui;
    Stopwatch *_StopWatch;
    bool m_isRunning;
};

#endif
