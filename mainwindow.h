#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "audio.h"
#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

//signals:
//    void fullScreenChanged(bool fullScreen);

private slots:
    void appendText(QByteArray data);

    void startTimer();
    void stopTimer();
    void savePCM();

private:
    void setupUI();

    QByteArray *pcm;
    QTimer *timer;
    QTextEdit *txtEdit;
    QPushButton *btnPlay;
    QPushButton *btnSave;
    bool record;
    Audio audio;
};

#endif // MAINWINDOW_H
