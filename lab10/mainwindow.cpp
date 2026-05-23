#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QButtonGroup>
#include <QFileDialog>
#include <QMessageBox>
#include <QRegularExpression>
#include <QDir>
#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>
#include <QStandardPaths>
#include <QCryptographicHash>
#include <QTextStream>
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QButtonGroup *serversGroup = new QButtonGroup(this);

    serversGroup->setExclusive(true);

    serversGroup->addButton(ui->euCheck);
    serversGroup->addButton(ui->asiaCheck);
    serversGroup->addButton(ui->usaCheck);

    setWindowTitle("Регистрация");

    setStyleSheet(
        "QMainWindow { background-color: #2E3440; }"
        "QLabel { color: green; font-size: 14px; }"
        "QLineEdit { background-color: white; color: black; border: 1px solid gray; }"
        "QPushButton { background-color: #5E81AC; color: white; font-size: 14px; }"
        "QCheckBox { color: red; }"
    );

    ui->passwordEdit->setEchoMode(QLineEdit::Normal);

    photoPath = "resources/default.png";
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_uploadButton_clicked()
{
    QString file = QFileDialog::getOpenFileName(
        this,
        "Выбрать фото",
        "",
        "Images (*.png *.jpg *.jpeg)"
    );

    if(!file.isEmpty())
    {
        photoPath = file;

        QPixmap pix(file);

        ui->photoLabel->setPixmap(
            pix.scaled(
                ui->photoLabel->size(),
                Qt::KeepAspectRatio
            )
        );
    }
}

bool MainWindow::validateName()
{
    QRegularExpression regex(
        QStringLiteral("^[A-Za-zА-Яа-я]{2,20}$")
    );

    if(!regex.match(ui->nameEdit->text()).hasMatch())
    {
        QMessageBox::warning(
            this,
            "Ошибка",
            "В имени не должно быть цифр!"
        );

        return false;
    }

    return true;
}

bool MainWindow::validateSurname()
{
    QRegularExpression regex(
        QStringLiteral("^[A-Za-zА-Яа-я]{2,20}$")
    );

    if(!regex.match(ui->surnameEdit->text()).hasMatch())
    {
        QMessageBox::warning(
            this,
            "Ошибка",
            "Фамилия должна содержать только буквы!"
        );

        return false;
    }

    return true;
}

bool MainWindow::validateNickname()
{
    QRegularExpression regex(
        QStringLiteral("^[A-Za-z0-9_]{4,16}$")
    );

    if(!regex.match(ui->nicknameEdit->text()).hasMatch())
    {
        QMessageBox::warning(
            this,
            "Ошибка",
            "Никнейм должен быть от 4 до 16 символов!"
        );

        return false;
    }

    return true;
}

bool MainWindow::validateMail()
{
    QRegularExpression regex(
        QStringLiteral(R"(^[\w\.]+@\w+\.\w+$)")
    );

    if(!regex.match(ui->mailEdit->text()).hasMatch())
    {
        QMessageBox::warning(
            this,
            "Ошибка",
            "Неверный формат почты!"
        );

        return false;
    }

    return true;
}

bool MainWindow::validatePassword()
{
    QRegularExpression regex(
        QStringLiteral("^.{6,20}$")
    );

    if(!regex.match(ui->passwordEdit->text()).hasMatch())
    {
        QMessageBox::warning(
            this,
            "Ошибка",
            "Пароль должен быть от 6 до 20 символов!"
        );

        return false;
    }

    return true;
}

bool MainWindow::validateID()
{
    QRegularExpression regex(
        QStringLiteral("^[A-Z]{1}[0-9]{3}-[0-9]{2}[A-Z]{1}-[A-Z]{1}$")
    );

    if(!regex.match(ui->idEdit->text()).hasMatch())
    {
        QMessageBox::warning(
            this,
            "Ошибка",
            "ID должен быть в формате A111-11A-A"
        );

        return false;
    }

    return true;
}

bool MainWindow::validateServer()
{
    if(!ui->euCheck->isChecked() && !ui->asiaCheck->isChecked() && !ui->usaCheck->isChecked())
    {
        QMessageBox::warning(
            this,
            "Ошибка",
            "Сервер должен быть выбран"
            );

        return false;

    }
    return true;
}

bool MainWindow::validatePhoto()
{
    if(photoPath == "resutces/default.png")
    {
        QMessageBox::warning(
            this,
            "Ошибка",
            "Загрузите фотографию!"
            );
        return false;
    }
    return true;
}

bool MainWindow::validateInputs()
{
    return validateName() &&
           validateSurname() &&
           validateNickname() &&
           validateMail() &&
           validatePassword() &&
           validateID() && validateServer() && validatePhoto();

}

bool MainWindow::isNicknameTaken(const QString &nickname)
{
    QString desktop =
        QStandardPaths::writableLocation(
            QStandardPaths::DesktopLocation
        );

    QString path =
        desktop + "/Accounts/" + nickname;

    return QDir(path).exists();
}

QString MainWindow::encryptPassword(const QString &password)
{
    QByteArray hash =
        QCryptographicHash::hash(
            password.toUtf8(),
            QCryptographicHash::Sha256
        );

    return hash.toHex();
}

void MainWindow::on_registerButton_clicked()
{
    if(!validateInputs())
        return;

    QString nickname =
        ui->nicknameEdit->text();

    if(isNicknameTaken(nickname))
    {
        QMessageBox::warning(
            this,
            "Ошибка",
            "Такой логин уже существует!"
        );

        return;
    }

    QString desktop =
        QStandardPaths::writableLocation(
            QStandardPaths::DesktopLocation
        );

    QString userFolder =
        desktop + "/Accounts/" + nickname;

    QDir().mkpath(userFolder);

    QString imageDest =
        userFolder + "/photo.png";

    QFile::copy(photoPath, imageDest);

    QStringList servers;

    if(ui->euCheck->isChecked())
        servers << "Europe";

    if(ui->asiaCheck->isChecked())
        servers << "Asia";

    if(ui->usaCheck->isChecked())
        servers << "America";

    QJsonObject obj;

    obj["name"] = ui->nameEdit->text();
    obj["surname"] = ui->surnameEdit->text();
    obj["nickname"] = nickname;
    obj["email"] = ui->mailEdit->text();
    obj["id_key"] = ui->idEdit->text();
    obj["servers"] = servers.join(", ");

    QFile jsonFile(userFolder + "/data.json");

    if(jsonFile.open(QIODevice::WriteOnly))
    {
        jsonFile.write(
            QJsonDocument(obj).toJson()
        );

        jsonFile.close();
    }

    QFile txtFile(userFolder + "/info.txt");

    if(txtFile.open(QIODevice::WriteOnly))
    {
        QTextStream out(&txtFile);

        out << ui->mailEdit->text()
            << "/"
            << encryptPassword(
                   ui->passwordEdit->text()
               );

        txtFile.close();
    }

    QMessageBox::information(
        this,
        "Успех",
        "Регистрация успешно завершена!"
    );
}
