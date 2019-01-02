#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onOpenFrontButtonclicked();
    void onOpenBackButtonClicked();

private:
    Ui::MainWindow *ui;
    const int maxImageDisplayWidth = 320;
    const int maxImageDisplayHeight = 240;
};

#endif // MAINWINDOW_H
