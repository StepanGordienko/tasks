#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QVector>
#include "bagitem.h"

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
    void on_loadButton_clicked();

private:
    void loadJsonFile(const QString &filePath);
    void displayValidItems();
    void displayInvalidItems();
    void saveValidItems(const QString &filePath);
    void saveInvalidItems(const QString &filePath);

    Ui::MainWindow *ui;
    QStandardItemModel *validModel;
    QStandardItemModel *invalidModel;
    QVector<BagItem> allItems;
    QVector<BagItem> validItems;
    QVector<BagItem> invalidItems;
};

#endif // MAINWINDOW_H
