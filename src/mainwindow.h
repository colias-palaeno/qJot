#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    QPushButton* current_btn();
    void create_button(QString& name);
    void refresh_button_list();

    void on_actionSave_triggered();
    void on_actionNew_Note_triggered();

private:
    Ui::MainWindow* ui;
};
#endif // MAINWINDOW_H
