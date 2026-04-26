#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

        // Имена для RadioButton
        ui->radioButton->setText("Первый вариант");
        ui->radioButton->setChecked(true);
        ui->radioButton_2->setText("Второй вариант");

        // Заполнение выпадающего списка (ComboBox)
        ui->comboBox->addItem("Пункт 1");
        ui->comboBox->addItem("Пункт 2");
        ui->comboBox->addItem("Пункт 3");
        ui->comboBox->addItem("Пункт 4");
        ui->comboBox->addItem("Пункт 5");

        // Изменение надписи на кнопке
        ui->pushButton->setText("Нажми меня");

        // Изменение типа кнопки на Toggle (Checkable)
        ui->pushButton->setCheckable(true);

        //Настройка ProgressBar
        ui->progressBar->setMinimum(0);   // Минимальное значение (0%)
        ui->progressBar->setMaximum(100); // Максимальное значение (100%)
        ui->progressBar->setValue(0);     // Текущее значение (0%)
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_pressed()
{
    // Получаем текущее значение прогресс-бара
       int currentValue = ui->progressBar->value();

    // Увеличиваем на 10
       currentValue += 10;

    // Проверяем: если значение превышает максимум (100) или больше 100%
       if (currentValue > ui->progressBar->maximum()) {
          // Сбрасываем на 0
            ui->progressBar->setValue(0);
        } else {
           // Иначе устанавливаем новое значение
            ui->progressBar->setValue(currentValue);
        }
}
