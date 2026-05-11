#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QTextStream>
#include <QStringConverter>

namespace {

QStringList nonEmptyLinesFromText(const QString &text)
{
    QStringList lines;
    const QStringList raw = text.split(QLatin1Char('\n'));
    for (const QString &line : raw) {
        const QString trimmed = line.trimmed();
        if (!trimmed.isEmpty())
            lines.append(trimmed);
    }
    return lines;
}

QJsonValue jsonValueForNumberOrText(const QString &s)
{
    bool ok = false;
    const double d = QString(s).trimmed().toDouble(&ok);
    if (ok)
        return d;
    return s.trimmed();
}

} // namespace

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->pushButtonFromTxt, &QPushButton::clicked, this, &MainWindow::loadFromTxt);
    connect(ui->pushButtonOk, &QPushButton::clicked, this, &MainWindow::saveToJson);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadFromTxt()
{
    const QString path = QFileDialog::getOpenFileName(
        this,
        tr("Импорт из текстового файла"),
        QString(),
        tr("Текстовые файлы (*.txt);;Все файлы (*.*)"));
    if (path.isEmpty())
        return;

    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Ошибка"), tr("Не удалось открыть файл."));
        return;
    }

    QTextStream ts(&f);
    ts.setEncoding(QStringConverter::Utf8);
    const QString content = ts.readAll();
    f.close();

    const QStringList lines = nonEmptyLinesFromText(content);
    if (lines.size() < 4) {
        QMessageBox::warning(
            this,
            tr("Ошибка"),
            tr("В файле должно быть не менее четырёх непустых строк в порядке:\n"
               "имя, фамилия, рост, вес."));
        return;
    }

    ui->lineEditName->setText(lines.at(0));
    ui->lineEditSurname->setText(lines.at(1));
    ui->lineEditHeight->setText(lines.at(2));
    ui->lineEditWeight->setText(lines.at(3));
}

void MainWindow::saveToJson()
{
    QString path = QFileDialog::getSaveFileName(
        this,
        tr("Сохранить в JSON"),
        QString(),
        tr("JSON (*.json);;Все файлы (*.*)"));
    if (path.isEmpty())
        return;

    if (!path.endsWith(QLatin1String(".json"), Qt::CaseInsensitive))
        path += QLatin1String(".json");

    QJsonArray array;
    QFile f(path);
    if (f.exists()) {
        if (!f.open(QIODevice::ReadOnly)) {
            QMessageBox::warning(this, tr("Ошибка"), tr("Не удалось прочитать существующий JSON."));
            return;
        }
        const QByteArray data = f.readAll();
        f.close();

        if (!data.trimmed().isEmpty()) {
            QJsonParseError err;
            const QJsonDocument doc = QJsonDocument::fromJson(data, &err);
            if (err.error != QJsonParseError::NoError || (!doc.isArray() && !doc.isObject())) {
                QMessageBox::warning(
                    this,
                    tr("Ошибка"),
                    tr("Файл не является допустимым JSON-массивом или объектом."));
                return;
            }
            if (doc.isArray())
                array = doc.array();
            else
                array.append(doc.object());
        }
    }

    QJsonObject obj;
    obj[QStringLiteral("имя")] = ui->lineEditName->text().trimmed();
    obj[QStringLiteral("фамилия")] = ui->lineEditSurname->text().trimmed();
    obj[QStringLiteral("рост")] = jsonValueForNumberOrText(ui->lineEditHeight->text());
    obj[QStringLiteral("вес")] = jsonValueForNumberOrText(ui->lineEditWeight->text());

    array.append(obj);

    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QMessageBox::warning(this, tr("Ошибка"), tr("Не удалось записать файл."));
        return;
    }
    const QJsonDocument out(array);
    f.write(out.toJson(QJsonDocument::Indented));
    f.close();

    QMessageBox::information(this, tr("Готово"), tr("Запись в JSON выполнена."));
}
