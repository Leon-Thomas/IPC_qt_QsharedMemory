#include "mainwindow.h"
#include <QSharedMemory>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QPixmap>
#include <QBuffer>
#include <QFileDialog>


const char *KEY_SHARED_MEMORY = "shared";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      sharedMemory(new QSharedMemory(KEY_SHARED_MEMORY, this))
{
    QWidget *widget = new QWidget(this);
    this->setCentralWidget(widget);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    QPushButton *saveBtn    = new QPushButton(tr("Save"),this);
    QLabel      *picLabel   = new QLabel(tr("图片"), this);
    QPushButton *loadBtn    = new QPushButton(tr("Load"),this);

    mainLayout->addWidget(saveBtn);
    mainLayout->addWidget(picLabel);
    mainLayout->addWidget(loadBtn);

    widget->setLayout(mainLayout);

    QObject::connect(saveBtn, &QPushButton::clicked, [=]() {
        if(sharedMemory->isAttached()){
            sharedMemory->detach();
        }
        QString filename = QFileDialog::getOpenFileName(this, tr("Picture File"),
                                                        tr("/home"),
                                                        tr("picture(*.png *.jpg)"));
        QPixmap pixmap(filename);
        picLabel->setPixmap(pixmap);
        QBuffer buffer;
        QDataStream out(&buffer);
        buffer.open(QIODevice::ReadWrite);
        out << pixmap;


    });
}

MainWindow::~MainWindow()
{

}
