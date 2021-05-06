﻿#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

QString xend = "<br />";
QString dend = "</div>";
QString xl = "，。“”？：！…‘’~—*（）《》【】·；、_";
QString pre = "上一章";
QString las = "下一章";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), fd_booklist(nullptr), fd_bookmenu(nullptr), fd(nullptr) {
    ui->setupUi(this);
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::Getessay);
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::Getessay);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::Getessay_pre);
}

void MainWindow::Getessay_pre() {
    QTextCodec *tc = QTextCodec::codecForName("GBK");
    QString str = tc->toUnicode(fd->downloadedData());
    Getpreurl(str);
    str = "https://www.hongyeshuzhai.com/" + str;
    QUrl url(str);
    if(fd != nullptr)   delete fd;
    fd = new FileDownloader(url, this);
    connect(fd, SIGNAL (downloaded()), this, SLOT (loadText()));
}

void MainWindow::Getpreurl(QString &str) {
    int pos = str.indexOf(pre);
    str.truncate(pos);
    str = str.right(35);
    int start = str.indexOf("/");
    int end = str.length() - 2;
    str = str.mid(start, end - start);
}

void MainWindow::Getessay_las() {
    QTextCodec *tc = QTextCodec::codecForName("GBK");
    QString str = tc->toUnicode(fd->downloadedData());
    Getlasurl(str);
    str = "https://www.hongyeshuzhai.com/" + str;
    QUrl url(str);
    if(fd != nullptr)   delete fd;
    fd = new FileDownloader(url, this);
    connect(fd, SIGNAL (downloaded()), this, SLOT (loadText()));
}

void MainWindow::Getlasurl(QString &str) {
    int pos = str.indexOf(las);
    str.truncate(pos);
    str = str.right(35);
    int start = str.indexOf("/");
    int end = str.length() - 2;
    str = str.mid(start, end - start);
}

void MainWindow::Getessay() {
    name = ui->textEdit->toPlainText();
    QString page_str = ui->textEdit_2->toPlainText();
    double page_tmp = page_str.toDouble();
    page = number_Transfer_BigChinese(page_tmp);
    find_essay_page();
}

void MainWindow::find_essay_page() {
    QUrl url("https://www.hongyeshuzhai.com/xiaoshuodaquan/");
    if(fd_booklist != nullptr)   delete fd_booklist;
    fd_booklist = new FileDownloader(url, this);
    connect(fd_booklist, SIGNAL (downloaded()), this, SLOT (loadText_booklist(name, page)));
}

//小说大全页面查找指定书目
void MainWindow::loadText_booklist() {
    QTextCodec *tc = QTextCodec::codecForName("GBK");
    QString str = tc->toUnicode(fd_booklist->downloadedData());
    QString str_bookmenu = Translation_book(str);
    QUrl url(str_bookmenu);
    if(fd_bookmenu != nullptr)   delete fd_bookmenu;
    fd_bookmenu = new FileDownloader(url, this);
    connect(fd_bookmenu, SIGNAL (downloaded()), this, SLOT (loadText_bookmenu(page)));
}

QString MainWindow::Translation_book(QString str) {
    QString s;
    int index2 = str.indexOf(name) - 2;
    int index1 = index2 - 44;//////网址位数不确定
    s = str.mid(index1, 44);
    return s;
}

//指定小说页面查找指定章节
void MainWindow::loadText_bookmenu() {
    QTextCodec *tc = QTextCodec::codecForName("GBK");
    QString str = tc->toUnicode(fd_bookmenu->downloadedData());
    QString str_chapter = Translation_chapter(str);
    QUrl url(str_chapter);
    if(fd != nullptr)   delete fd;
    fd = new FileDownloader(url, this);
    connect(fd, SIGNAL (downloaded()), this, SLOT (loadText()));
}

QString MainWindow::Translation_chapter(QString str) {
    QString s;
    int index2 = str.indexOf(page) - 3;
    int index1 = index2 - 28;//////网址位数不确定
    s = str.mid(index1, 28);
    s = "https://www.hongyeshuzhai.com/" + s;
    return s;
}

MainWindow::~MainWindow() {
    delete ui;
}

//分析文章正文
void MainWindow::loadText() {
    QTextCodec *tc = QTextCodec::codecForName("GBK");
    QString str = tc->toUnicode(fd->downloadedData());
    QString str_change = Translation_text(str);
    ui->textBrowser->setPlainText(str_change);
}

QString MainWindow::Translation_text(QString str) {
    QString s;
    int index1 = str.indexOf("<h1>") + 4;
    int index2 = str.indexOf("</h1>");
    s += str.mid(index1, index2 - index1);
    s += '\n';  s += '\n';
    int it = str.indexOf("<div id=\"content\">") + 18;
    while(!dcheck(it, str)) {
        if((str[it] >= 0x4E00 && str[it] <= 0x9FA5) || bcheck(str[it]))
            s += str[it];
        if(xcheck(it, str)) s += '\n';
        it ++;
    }
    return s;
}

bool MainWindow::bcheck(QCharRef a) {
    for(int i = 0; i < 23; i ++)
        if(a == xl[i])  return 1;
    return 0;
}

bool MainWindow::xcheck(int i, QString &str) {
    for(int it = 0; it < 6; it ++)
        if(str[i + it] != xend[it])   return 0;
    return 1;
}

bool MainWindow::dcheck(int i, QString &str) {
    for(int it = 0; it < 6; it ++)
        if(str[i + it] != dend[it])   return 0;
    return 1;
}

QString MainWindow::number_Transfer_BigChinese(double Fnumber)
{

    if(qAbs(Fnumber)<0.01)   //保留2位小数时，近似看作零的情况
        return "零";


    //判断正负号
    QString numberSign;//存储符号
    if(Fnumber<0)
        numberSign = "（负数）";
   //将数据的绝对值 转换成字符串，如-58 转成 “58.00”
   QString number = QString::number(qAbs(Fnumber),'f',2);//qAbs绝对值 ，保留两位小数
   QString Left_of_Point;//整数部分
   int length =number.length()-3;//整数部分的长度，（精确度为2，去除小数和小数点）
   if(length>12)
   {
        //qDebug()<<"输入的数值超过范围！"
        return "输入的数值超过范围！";
   }

   QStringList numerical_unit   =   {"","拾","佰","仟","万","拾", "佰", "仟", "亿", "拾", "佰", "仟"};
   QStringList numerical_value  =   {"零","壹","贰","叁","肆","伍","陆","柒","捌","玖"};

   //数数整数低位多少个连零
   int counter_0=0;
   for(int i =length-1;i>=0;i--)
   {
       if((number[i].toLatin1()-'0' )==0) //从个位开始，向高位走
           counter_0++;
       else
           break;

   }
   if(length==1 && counter_0==1) //0.x
       counter_0=0; //不进行过滤

   //1400  0.2

   for(int i=0,flag=1;i<length-counter_0;i++)
   {
        //5     8       1    2   3
        //伍 拾 捌       壹佰 贰拾 叁
       if((number[i].toLatin1()-'0')==0)
       {
           if((flag!=0 && (length-1-i)%4 != 0) || length ==1) //flag！=0  表示前一个数值 不为0
           Left_of_Point+="零"; //后面不用添加 单位
           if((length-1-i)%4 == 0) //如果0处于分段处，后面需添加单位
           Left_of_Point+=numerical_unit[length-1-i];//添加数值单位

           flag =0; //标记

       }
       else
       {
        flag =1;
        Left_of_Point+=numerical_value[number[i].toLatin1()-'0']; //'5'-'0'==5
        Left_of_Point+=numerical_unit[length-1-i];//添加数值单位

       }
   }
    //QString Right_of_Point;//小数点右侧，小数部分(保留两位)  xxxx.yy
    int totalLength = number.length();
    if(number[totalLength-2]=='0'&&number[totalLength-1]=='0')
    {
        QString Bigcn=numberSign+Left_of_Point+"吨";
        return Bigcn;
    }
    else if(number[totalLength-2]!='0'&&number[totalLength-1]=='0')
    {
        QString Bigcn=numberSign+Left_of_Point+"点"+numerical_value[number[totalLength-2].toLatin1()-'0']+"吨";
        return Bigcn;
    }
    else
    {
        QString Bigcn=numberSign+Left_of_Point+"点"+numerical_value[number[totalLength-2].toLatin1()-'0']+numerical_value[number[totalLength-1].toLatin1()-'0']+"吨";
        return Bigcn;
    }


    return "异常情况";
}
