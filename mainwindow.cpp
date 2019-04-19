#include "mainwindow.h"
//#include "audio.h"
#include <QVBoxLayout>
#include <QDebug>
#include <QFile>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
//    ui(new Ui::MainWindow)
{
//    ui->setupUi(this);
    setupUI();

    audio.initAudio();

    pcm = new QByteArray();
    connect(&audio, SIGNAL(collectData(QByteArray)), this, SLOT(appendText(QByteArray)));
    record = false;

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), &audio, SLOT(record()));
    connect(btnPlay, SIGNAL(pressed()), this, SLOT(startTimer()));
    connect(btnPlay, SIGNAL(released()), this, SLOT(stopTimer()));
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
    QFile *f = new QFile("D:/a.pcm");
    f->writeData(pcm->data(), pcm->length());
    f->close();
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
    lay_main->addWidget(btnSave);

    setCentralWidget(win);
    this->setWindowTitle("Audio Capture");
    this->setFixedSize(300,600);

}


