#include "mainwindow.h"
//#include "audio.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QDataStream>
#include <QRadioButton>
#include <QCheckBox>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
//    ui(new Ui::MainWindow)
{
//    ui->setupUi(this);
    setupUI();

    audio.initAudio();

    pcm = new QByteArray();
    connect(&audio, SIGNAL(collectData(QByteArray)), this, SLOT(appendText(QByteArray)));
    recording = false;

    timer = new QTimer(this);
    connect(btnPlay, SIGNAL(clicked(bool)), this, SLOT(playOrPause()));
//    connect(timer, SIGNAL(timeout()), &audio, SLOT(record()));
//    connect(btnPlay, SIGNAL(pressed()), this, SLOT(startTimer()));
//    connect(btnPlay, SIGNAL(released()), this, SLOT(stopTimer()));
    connect(btnSave, SIGNAL(clicked(bool)), this, SLOT(savePCM()));
}

MainWindow::~MainWindow()
{
    //    delete win;
    audio.uninitAudio();
}


void MainWindow::appendText(QByteArray data)
{
    txtEdit->append(data.mid(0, 20));
    pcm->append(data);
}

void MainWindow::playOrPause()
{
    if (recording) {
        audio.stopRecord();
    } else{
        audio.startRecord();
    }
    recording = !recording;
}

void MainWindow::startTimer()
{
    timer->start(500);
}

void MainWindow::stopTimer()
{
    timer->stop();
}

void MainWindow::savePCM()
{
    QFile f("D:/a.pcm");
    if (!f.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(this,tr("警告"),tr("打开文件失败"));
        return;
    }
    QDataStream out(&f);
    out.setVersion(QDataStream::Qt_5_6);
    out << *pcm;
    qDebug() << "pcm size:" << pcm->length();
//    f.(pcm->data(), pcm->length());
    f.close();
}

void MainWindow::setupUI()
{
    QWidget *win = new QWidget();
    txtEdit = new QTextEdit();
    btnPlay = new QPushButton("Capture");
    btnSave = new QPushButton("Save");

    QVBoxLayout *lay_main = new QVBoxLayout(win);
    lay_main->addWidget(btnPlay);
    lay_main->addWidget(txtEdit);
    lay_main->addStretch();

    QHBoxLayout *radio_lay = new QHBoxLayout;
    QCheckBox *radio_one = new QCheckBox("one");
    QCheckBox *radio_two = new QCheckBox("two");


    radio_lay->addWidget(radio_one);
    radio_lay->addWidget(radio_two);

    lay_main->addLayout(radio_lay);

    lay_main->addWidget(btnSave);

    setCentralWidget(win);
    this->setWindowTitle("Audio Capture");
    this->setFixedSize(300,600);



    QString qss;
   QFile qssFile(":/myQss.qss");
   qssFile.open(QFile::ReadOnly);
   if(qssFile.isOpen())
   {
     qss = QLatin1String(qssFile.readAll());
     this->setStyleSheet(qss);
     qssFile.close();
   }

//   setStyleSheet("");
}


