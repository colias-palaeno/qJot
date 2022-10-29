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
    void create_button(QString& name);
    void refresh_button_list();

private slots:
    void on_actionSave_triggered();
    void on_actionNew_Note_triggered();

private:
    Ui::MainWindow* ui;
};
#endif // MAINWINDOW_H
