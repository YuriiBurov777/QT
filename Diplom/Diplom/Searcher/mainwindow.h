#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>

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
    //Обработчик нажатия кнопки поиска
    void onSearch();
    //Обработчик кнопки "Показать все документы"
    void onShowAllDocuments();
    //Обработчик изменения текста в поле поиска
    void onSearchTextChanged(const QString& text);
    
private:
    //Загрузка и отображение слов на вкладке "Словарь"
    void loadWordsTab();
    //Загрузка и отображение документов на вкладке "Документы"
    void loadDocumentsTab();
    //Выполнение поиска и отображение результатов
    void displaySearchResults();
    
    Ui::MainWindow *ui;
    QStringList m_lastSearchWords;
};

#endif // MAINWINDOW_H
