#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QColor>
#include <QFileDialog>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QFile>
#include <QHeaderView>
#include <QStandardItem>
#include <algorithm>

namespace {

const QColor kErrorCellBg(255, 204, 204);

int columnForErrorField(const QString &field)
{
    if (field == QStringLiteral("название"))
        return 0;
    if (field == QStringLiteral("описание"))
        return 1;
    if (field == QStringLiteral("кол-во слотов"))
        return 2;
    if (field == QStringLiteral("максимальный вес"))
        return 3;
    return -1;
}

QStandardItem *makeCell(const QString &text, bool highlightError)
{
    auto *cell = new QStandardItem(text);
    if (highlightError)
        cell->setData(kErrorCellBg, Qt::BackgroundRole);
    return cell;
}

} // namespace

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->tablesHorizontalLayout->setStretch(0, 1);
    ui->tablesHorizontalLayout->setStretch(1, 1);

    validModel = new QStandardItemModel(this);
    validModel->setColumnCount(4);
    validModel->setHorizontalHeaderLabels({"название", "описание", "кол-во слотов", "максимальный вес"});
    ui->validTableView->setModel(validModel);

    invalidModel = new QStandardItemModel(this);
    invalidModel->setColumnCount(4);
    invalidModel->setHorizontalHeaderLabels({"название", "описание", "кол-во слотов", "максимальный вес"});
    ui->invalidTableView->setModel(invalidModel);

    ui->validTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->invalidTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_loadButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "Выберите JSON файл",
        "",
        "JSON Files (*.json);;All Files (*)"
        );

    if (filePath.isEmpty()) {
        return;
    }

    loadJsonFile(filePath);
}

void MainWindow::loadJsonFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть файл: " + filePath);
        return;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        QMessageBox::critical(this, "Ошибка", "Ошибка парсинга JSON: " + parseError.errorString());
        return;
    }

    allItems.clear();
    validItems.clear();
    invalidItems.clear();

    auto processObject = [this](const QJsonObject &obj) {
        BagItem item;

        if (obj.contains("название") && obj["название"].isString()) {
            item.name = obj["название"].toString();
        }

        if (obj.contains("описание") && obj["описание"].isString()) {
            item.description = obj["описание"].toString();
        }

        if (obj.contains("кол-во слотов")) {
            QJsonValue val = obj["кол-во слотов"];
            if (val.isDouble()) {
                item.slotCount = val.toInt();
            } else if (val.isString()) {
                bool ok = false;
                item.slotCount = val.toString().toInt(&ok);
                if (!ok)
                    item.slotCount = 0;
            }
        }

        if (obj.contains("максимальный вес")) {
            QJsonValue val = obj["максимальный вес"];
            if (val.isDouble()) {
                item.maxWeight = val.toDouble();
            } else if (val.isString()) {
                bool ok = false;
                item.maxWeight = val.toString().toDouble(&ok);
                if (!ok)
                    item.maxWeight = 0.0;
            }
        }

        item.validate();
        allItems.append(item);

        if (item.isValid) {
            validItems.append(item);
        } else {
            invalidItems.append(item);
        }
    };

    if (doc.isArray()) {
        QJsonArray arr = doc.array();
        for (const QJsonValue &val : arr) {
            if (val.isObject()) {
                processObject(val.toObject());
            }
        }
    } else if (doc.isObject()) {
        processObject(doc.object());
    }

    std::sort(validItems.begin(), validItems.end(), [](const BagItem &a, const BagItem &b) {
        return a.name.toLower() > b.name.toLower();
    });

    displayValidItems();
    displayInvalidItems();

    QFileInfo fi(filePath);
    const QString stem = fi.completeBaseName();
    const QString dir = fi.absolutePath();
    const QString validPath = dir + QLatin1Char('/') + stem + QStringLiteral("_valid.json");
    const QString errorsPath = dir + QLatin1Char('/') + stem + QStringLiteral("_errors.json");

    if (!validItems.isEmpty())
        saveValidItems(validPath);
    if (!invalidItems.isEmpty())
        saveInvalidItems(errorsPath);
}

void MainWindow::displayValidItems()
{
    validModel->removeRows(0, validModel->rowCount());

    for (const BagItem &item : validItems) {
        QList<QStandardItem*> row;
        row.append(new QStandardItem(item.name));
        row.append(new QStandardItem(item.description));
        row.append(new QStandardItem(QString::number(item.slotCount)));
        row.append(new QStandardItem(QString::number(item.maxWeight)));
        validModel->appendRow(row);
    }
}

void MainWindow::displayInvalidItems()
{
    invalidModel->removeRows(0, invalidModel->rowCount());

    for (const BagItem &item : invalidItems) {
        const int errCol = columnForErrorField(item.errorField);
        QList<QStandardItem*> row;
        row.append(makeCell(item.name.isEmpty() ? QStringLiteral("ПУСТО") : item.name, errCol == 0));
        row.append(makeCell(item.description.isEmpty() ? QStringLiteral("ПУСТО") : item.description,
                            errCol == 1));
        row.append(makeCell(item.slotCount <= 0 ? QStringLiteral("НЕКОРР") : QString::number(item.slotCount),
                            errCol == 2));
        row.append(makeCell(item.maxWeight <= 0.0 ? QStringLiteral("НЕКОРР")
                                                  : QString::number(item.maxWeight),
                            errCol == 3));
        invalidModel->appendRow(row);
    }
}

void MainWindow::saveValidItems(const QString &filePath)
{
    QFile out(filePath);
    if (!out.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, QStringLiteral("Предупреждение"),
                             QStringLiteral("Не удалось сохранить файл: ") + filePath);
        return;
    }

    QJsonArray arr;
    for (const BagItem &item : validItems) {
        QJsonObject obj;
        obj[QStringLiteral("название")] = item.name;
        obj[QStringLiteral("описание")] = item.description;
        obj[QStringLiteral("кол-во слотов")] = item.slotCount;
        obj[QStringLiteral("максимальный вес")] = item.maxWeight;
        arr.append(obj);
    }

    QJsonDocument doc(arr);
    out.write(doc.toJson(QJsonDocument::Indented));
    out.close();
}

void MainWindow::saveInvalidItems(const QString &filePath)
{
    QFile out(filePath);
    if (!out.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, QStringLiteral("Предупреждение"),
                             QStringLiteral("Не удалось сохранить файл: ") + filePath);
        return;
    }

    QJsonArray arr;
    for (const BagItem &item : invalidItems) {
        QJsonObject obj;
        obj[QStringLiteral("название")] = item.name;
        obj[QStringLiteral("описание")] = item.description;
        obj[QStringLiteral("кол-во слотов")] = item.slotCount;
        obj[QStringLiteral("максимальный вес")] = item.maxWeight;
        obj[QStringLiteral("ошибка")] = item.errorField;
        arr.append(obj);
    }

    QJsonDocument doc(arr);
    out.write(doc.toJson(QJsonDocument::Indented));
    out.close();
}
