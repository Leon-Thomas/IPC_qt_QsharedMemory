#include "mainwindow.h"
#include <QSharedMemory>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QPixmap>
#include <QBuffer>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

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
        //picLabel->setPixmap(pixmap);
        QBuffer buffer;
        QDataStream out(&buffer);
        buffer.open(QIODevice::ReadWrite);
        out << pixmap;

        int size = buffer.size();
        if(!sharedMemory->create(size))
        {
            qDebug()<<tr("SharedMemory error: ")
                   <<sharedMemory->errorString();
        }
        else{
            sharedMemory->lock();
            char *to = static_cast<char *>(sharedMemory->data());
            const char * from = buffer.data().constData();
            memcpy(to, from, qMin(size, sharedMemory->size()));
            sharedMemory->unlock();
        }

    });

    QObject::connect(loadBtn, &QPushButton::clicked, [=]() {
       if(!sharedMemory->attach()) {
           if(sharedMemory->error() == QSharedMemory::NoError)
               qDebug()<<"No Error";
           else
               qDebug()<<tr("Attach error: ")
                      <<sharedMemory->errorString();
       }
       QBuffer buffer;
       QPixmap pixmap;
       QDataStream in(&buffer);

       sharedMemory->lock();
       buffer.setData(static_cast<char *>(sharedMemory->data()), sharedMemory->size());
       buffer.open(QIODevice::ReadWrite);
       in >> pixmap;
       sharedMemory->unlock();
       sharedMemory->detach();
       picLabel->setPixmap(pixmap);

    });
}

MainWindow::~MainWindow()
{

}
