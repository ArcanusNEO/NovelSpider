#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkAddressEntry>
#include <QNetworkReply>
#include <QByteArray>
#include "filedownloader.h"
#include <QTextCodec>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    QString Translation(QString str);
    bool dcheck(int it, QString &str);
    bool xcheck(int it, QString &str);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    FileDownloader* fd;

public slots:
    void Getessay();
    void loadText();
};
#endif // MAINWINDOW_H
