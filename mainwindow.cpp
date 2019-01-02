#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QPixmap>
#include "MyBitmap.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Should verify that this connection worked as SIGNAL and SLOT doesn't check for typos
    QObject::connect(ui->frontImageOpenButton, SIGNAL(clicked()), this, SLOT(onOpenFrontButtonclicked()));
    QObject::connect(ui->backImageOpenButton, SIGNAL(clicked()), this, SLOT(onOpenBackButtonClicked()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onOpenFrontButtonclicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Image"), "", tr("Image Files (*.yuv *.png *.jpg *.bmp)"));
    ui->frontImageFileLabel->setText(fileName);

    QPixmap myImage;

    bool loaded = false;
    if (fileName.contains(".yuv"))
    {
        CMyBitmap yuv2Bmp(fileName, 640, 480);
        myImage = yuv2Bmp._pixmap;
        loaded = true;
    }
    else
    {
        loaded = myImage.load(fileName);
    }

    if (loaded)
    {

        if (myImage.width() > maxImageDisplayWidth || myImage.height() > maxImageDisplayHeight)
        {
            myImage = myImage.scaled(maxImageDisplayWidth, maxImageDisplayHeight, Qt::AspectRatioMode::KeepAspectRatio);
        }
        const int bitmapFileHeaderSize = 14;
        const int dibHeaderSize = 40;
        myImage.data_ptr();
        ui->frontImageLabel->setPixmap(myImage);
    }
}

void MainWindow::onOpenBackButtonClicked()
{
    //ui->backImageLabel->setPixmap(myBmp._pixmap);

    ui->backImageFileLabel->setText(tr("Back File name"));
}

