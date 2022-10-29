#include "mainwindow.h"
#include "ui_mainwindow.h"
					
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QPushButton>
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

QString current_file;

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
        if (current_file != "") // if a file has been selected previously...
            findChild<QPushButton*>("tab_" + current_file)->setEnabled(true); // enable the tab button for it

        else if
        ((ui->bodyText->toPlainText() != "Click here to type a note." || ui->titleText->text() != "New Note")
                 &&
            QMessageBox::question(
                this,
                tr("qJot"),
                tr("If you view this note, the current note will be discarded. Proceed?"),
                QMessageBox::Yes | QMessageBox::No
            ) == QMessageBox::No
        )
            return;
	
        btn->setDisabled(true);
        current_file = btn->text();
        ui->titleText->setText(current_file);

        QFile file(file_path(current_file));

        file.open(QIODevice::ReadOnly);
        ui->bodyText->setPlainText(QTextStream(&file).readAll());
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

	refresh_button_list();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionSave_triggered()
{
    auto title = ui->titleText->text();
    QFile file(
        file_path((current_file != "") ?
            current_file : title)
    );
		
    if (current_file == "") { // if no file is selected (saving a new file)
        if (!QDir(qJot_path()).exists())
            QDir(app_data_path()).mkdir("qJot");

        else if (file.exists() && QMessageBox::question(
            this,
            tr("qJot"),
            tr("A note with that name already exists. Do you wish to overwrite it?"),
            QMessageBox::Yes | QMessageBox::No
        ) == QMessageBox::No)

            return; // abort!
    }

    else if (title != current_file) // if a file is selected, and the title differs from the current filename
            file.rename(file_path(title)); // rename the old file, and give it the new title

    if (file.open(QIODevice::WriteOnly)) { // if the file opened successfully ...
        current_file = title;
        file.resize(0);

        QTextStream(&file) << ui->bodyText->toPlainText();

        for (auto* btn : findChildren<QPushButton*>())
            delete btn;

        refresh_button_list();
        findChild<QPushButton*>("tab_" + title)->setDisabled(true); /* disables the current button if it isn't already disabled -
        if a new button was created from a new file, it wouldn't have been disabled before */
    }
}

void MainWindow::on_actionNew_Note_triggered()
{
    MainWindow::on_actionSave_triggered();
    ui->titleText->setText("New Note");
    ui->bodyText->setPlainText("Click here to type a note.");

    if (current_file != "") {
        findChild<QPushButton*>("tab_" + current_file)->setEnabled(true);
        current_file = "";
    }
}
