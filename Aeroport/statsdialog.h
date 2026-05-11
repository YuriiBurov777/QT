#ifndef STATSDIALOG_H
#define STATSDIALOG_H

#include <QDialog>
#include "dbmanager.h"

class QCustomPlot;
class QComboBox;
class QTabWidget;
class QLabel;

namespace Ui {
class StatsDialog;
}

class StatsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StatsDialog(const QString& airportCode,
                        const QString& airportName,
                        DBManager* dbManager,
                        QWidget *parent = nullptr);
    ~StatsDialog();

private slots:
    void onMonthComboBoxChanged(int index);

private:
    void setupUI();
    void loadYearlyStats();
    void loadMonthlyStats(int month);

    Ui::StatsDialog *ui;
    QString m_airportCode;
    QString m_airportName;
    DBManager* m_dbManager;

    QCustomPlot* m_yearlyPlot;
    QCustomPlot* m_monthlyPlot;
    QComboBox* m_monthComboBox;
    QTabWidget* m_tabWidget;
    QLabel* m_airportLabel;
};

#endif // STATSDIALOG_H
