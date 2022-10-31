#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QStandardPaths>
#include <QTextStream>

QString app_data_path()
{
    return QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
}

QString qJot_path()
{
    return app_data_path()  + "/qJot/";
}

QString file_path(QString name)
{
    return qJot_path() + name + ".txt";
}

QString current_filename; // string that stores the current filename (not including the '.txt' extension)

QPushButton* MainWindow::current_btn()
{
    return findChild<QPushButton*>("tab_" + current_filename);
}

void MainWindow::create_button(QString& name)
{
    QPushButton* btn;

    {
        auto clean_name = name.remove(".txt");
        btn = new QPushButton(clean_name);
        btn->setObjectName("tab_" + clean_name);
    }

    ui->verticalLayout->addWidget(btn);
    btn->setFlat(true);

    connect(btn, &QPushButton::pressed, this, [=]
    {
        auto do_not_discard = [this] {
            return QMessageBox::question(
                this,
                tr("qJot"),
                tr("You have edited the current note, but have not saved your changes. Do you wish to discard your changes and view another note?"),
                QMessageBox::Yes | QMessageBox::No
            ) == QMessageBox::No;
        };

        if (current_filename != "") { // if a file is currently selected...
            current_btn()->setEnabled(true); // enable the button for it

            QFile current_file(file_path(current_filename));
            current_file.open(QIODevice::ReadOnly);
            if (QTextStream(&current_file).readAll() != ui->bodyText->toPlainText() && do_not_discard())
                return;
        } else if ((ui->bodyText->toPlainText() != "Click here to type a note." || ui->titleText->text() != "New Note") && do_not_discard())
            return;

        btn->setDisabled(true);
        current_filename = btn->text();
        ui->titleText->setText(current_filename);

        QFile new_file(file_path(current_filename));
        new_file.open(QIODevice::ReadOnly);
        ui->bodyText->setPlainText(QTextStream(&new_file).readAll());
    });
}

void MainWindow::refresh_button_list()
{
    QStringList files;

    {
        auto dir = QDir(qJot_path());
        dir.setNameFilters({"*.txt"});
        files = dir.entryList(QDir::NoFilter, QDir::Time);
    }

    for (QString file : files)
        create_button(file);
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->verticalLayout->setAlignment(Qt::AlignTop);

    refresh_button_list();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionSave_triggered()
{
    auto title = ui->titleText->text();
    QFile file(file_path((current_filename != "") ? current_filename : title));

    if (current_filename == "") { // if no file is selected (saving a new file)
        if (!QDir(qJot_path()).exists())
            QDir(app_data_path()).mkdir("qJot");
        else if (file.exists() && QMessageBox::question(
            this,
            tr("qJot"),
            tr("A note with that name already exists. Do you wish to overwrite it?"),
            QMessageBox::Yes | QMessageBox::No
        ) == QMessageBox::No)
            return;
    } else if (title != current_filename) // if a file is selected, and the title differs from the current filename
        file.rename(file_path(title)); // rename the old file, and give it the new title

    if (file.open(QIODevice::WriteOnly)) { // if the file opened successfully ...
        current_filename = title;
        file.resize(0);

        QTextStream(&file) << ui->bodyText->toPlainText();

        for (auto* btn : findChildren<QPushButton*>())
            delete btn;

        refresh_button_list();
        current_btn()->setDisabled(true); /* disables the current button if it isn't already disabled -
        if a new button was created from a new file, it wouldn't have been disabled before */
    }
}

void MainWindow::on_actionNew_Note_triggered()
{
    MainWindow::on_actionSave_triggered();
    ui->titleText->setText("New Note");
    ui->bodyText->setPlainText("Click here to type a note.");

    if (current_filename != "") {
        current_btn()->setEnabled(true);
        current_filename = "";
    }
}
