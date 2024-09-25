#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include "algorithm.h"
#include "base.h"
#include "res.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QStringList MainWindow::getFiles()
{

    //定义文件对话框类
     QFileDialog *fileDialog = new QFileDialog(this);

     //定义文件对话框标题
     fileDialog->setWindowTitle(QStringLiteral("选择文件"));

     //设置打开的文件路径
     fileDialog->setDirectory("./");

     //设置文件过滤器,只显示.ui .cpp 文件,多个过滤文件使用空格隔开
     fileDialog->setNameFilter(tr("File(*.*)"));

     //设置可以选择多个文件,默认为只能选择一个文件QFileDialog::ExistingFiles
     fileDialog->setFileMode(QFileDialog::ExistingFiles);

     //设置视图模式
     fileDialog->setViewMode(QFileDialog::Detail);

     //获取选择的文件的路径
     QStringList fileNames;
     if (fileDialog->exec()) {
         fileNames = fileDialog->selectedFiles();
     }

     return fileNames;
}

void MainWindow::on_APP_File_Select_clicked()
{
    QStringList files = getFiles();
    ui->APP_File_TextEdit->setText(files.join(","));

}


void MainWindow::on_EMV_File_Select_clicked()
{
    QStringList files = getFiles();

    ui->EMV_File_TextEdit->setText(files.join(","));
}

#define APP_ADDR_TAG ("FILE_APP_ADDR=0x")
#define EMV_ADDR_TAG ("FILE_EMV_ADDR=0x")
#define I_APP_MAX_SIZE (800 * 1024)
#define I_EMV_MAX_SIZE (320 * 1024)

typedef struct _dsp_tms_head_stru_
{
    int tms_emv_addr;
    int tms_emv_len;
    int tms_app_addr;
    int tms_app_len;
    int resv_addr_1;
    int resv_len_1;
    int resv_addr_2;
    int resv_len_2;
    int tms_file_sign_len;
    unsigned char  tms_file_sign_data[512];
}dsp_tms_head_stru;

void MainWindow::on_pushButton_clicked()
{
//    char * pTest = "4174C4CE723F9C84AFF05634F82360E0DC19E9182FEF10D60E495F381BEA0E868BC9DE9CE03080EA881632A28C2C5EECCD31D337D147B779E32C4361C31AC9F1AE4D6EF3AB9175AB792F181F62AC32FC5046E8E34044D8FC3A224FAB975780C1619655DF712F5C894E16BD24815B753266AB9883A83B1B2AF6122779F16B821286068AB4E4B2B6F20761FB02B1E76F5CDDD155F7567620CC33FB442799189879B49253CD574402DC961B8E4F12BE17AA5B41C740C36D3240696B8441CFBE4B4195DBB59CA2A4C054DBD285D1A1D460A7C8C6A4AA468CFE1E077B4883288CA3E91AC189EB29A683340D119669061C5F2D7715AD76A490306963B73315BF49E75A";
//    char pPrint[1024];
//    char pPrint2[1024];

//    std_asc_2_bcd(pTest, 512, pPrint);

//    for(int ai = 0;ai<256;ai++){
//        //sprintf(&pPrint2[ai], "%02x",*(pPrint+ai));
//        qDebug("x%02x,",pPrint[ai]&0xFF);
//    }

//    return;

    QFile file_app;//创建对象
    QFile file_emv;//创建对象
    QFile file_cfg;//创建对象
    dsp_tms_head_stru file_head;
    QString NewFileNmae = "emv.asc";
    QString CfgFileNmae = "bin.cfg";

    const QString app_file_path = ui->APP_File_TextEdit->toPlainText();
    const QString emv_file_path = ui->EMV_File_TextEdit->toPlainText();
    int len = 0;
    bool isok;

    file_head.tms_emv_addr = 0;
    file_head.tms_emv_len = 0;
    file_head.tms_app_addr = 0;
    file_head.tms_app_len = 0;
    file_head.resv_addr_1 = 0;
    file_head.resv_len_1 = 0;
    file_head.resv_addr_2 = 0;
    file_head.resv_len_2 = 0;
    file_head.tms_file_sign_len = 0;

    char tepBuf[1024] = {0};

    if(app_file_path == "" && emv_file_path == "")
    {
        QMessageBox::critical(this,"error info","please choose app bin file and emv bin file");
        return;
    }

    /************open emv file*******************/
    if(emv_file_path != ""){
        file_emv.setFileName(emv_file_path);
        isok = file_emv.open(QIODevice::ReadOnly);
        if(isok != true)
        {
            QMessageBox::critical(this,"error info","open emv file error");
            return;
        }

        file_head.tms_emv_len = file_emv.size();
    }


    /************open app file*******************/
    if(app_file_path != ""){

        file_app.setFileName(app_file_path);
        isok = file_app.open(QIODevice::ReadOnly);
        if(isok != true)
        {
            QMessageBox::critical(this,"error info","open app file error");
            return;
        }

        file_head.tms_app_len = file_app.size();

    }

    char c_errormsg[256] = {0};
    if(file_head.tms_app_len>I_APP_MAX_SIZE){
        memset(c_errormsg, 0, sizeof(c_errormsg));
        sprintf(c_errormsg, "APP File size exceeds %d KB", (file_head.tms_emv_len-I_EMV_MAX_SIZE));
        //QString errmsg = c_errormsg;
        QMessageBox::critical(this,"error info", c_errormsg);
        return;
    }

    if(file_head.tms_emv_len> I_EMV_MAX_SIZE){

        memset(c_errormsg, 0, sizeof(c_errormsg));
        sprintf(c_errormsg, "EMV File size exceeds %d KB", (file_head.tms_emv_len-I_EMV_MAX_SIZE));
        //QString errmsg = c_errormsg;
        QMessageBox::critical(this,"error info", c_errormsg);
        return;
    }

    /************create new file*******************/
    QString remove_filename = QApplication::applicationDirPath()+"/"+NewFileNmae;
    remove(remove_filename.toUtf8().data());
    QFile file_new(QApplication::applicationDirPath()+"/"+NewFileNmae);

    isok = file_new.open(QIODevice::ReadWrite);
    if(!isok){
        QMessageBox::critical(this,"error info","create file error");
        return;
    }


    file_cfg.setFileName(QApplication::applicationDirPath()+"/"+CfgFileNmae);
    file_cfg.open(QIODevice::ReadOnly);

    char cfg_buf[1024*2];
    len = file_cfg.size();

    while(len>0){
        memset(cfg_buf, 0, 1024*2);
        len -= file_cfg.readLine(cfg_buf, 1024*2);
        qDebug()<<"cfg_buf :"<<cfg_buf;
        if(!memcmp(APP_ADDR_TAG, cfg_buf, strlen(APP_ADDR_TAG))){
            file_head.tms_app_addr = strtol(cfg_buf+strlen(APP_ADDR_TAG), NULL, 16);
        }
        else if(!memcmp(EMV_ADDR_TAG, cfg_buf, strlen(EMV_ADDR_TAG))){
            file_head.tms_emv_addr = strtol(cfg_buf+strlen(EMV_ADDR_TAG), NULL, 16);
        }

    }

    qDebug("file_app_addr=%08x", file_head.tms_app_addr);
    qDebug("file_emv_addr=%08x", file_head.tms_emv_addr);


    memset(tepBuf, 0, 32);
    sprintf(tepBuf, "%08x", file_head.tms_emv_addr);
    std_asc_2_bcd(tepBuf, 8, &tepBuf[16]);
    std_char_opposite_endian(&tepBuf[16],4);
    file_new.write(&tepBuf[16], 4);

    memset(tepBuf, 0, 32);
    sprintf(tepBuf, "%08x", file_head.tms_emv_len);
    std_asc_2_bcd(tepBuf, 8, &tepBuf[16]);
    //trace_value("emv file len ",&tepBuf[16], 4);
    std_char_opposite_endian(&tepBuf[16],4);
    file_new.write(&tepBuf[16], 4);

    memset(tepBuf, 0, 32);
    sprintf(tepBuf, "%08x", file_head.tms_app_addr);
    std_asc_2_bcd(tepBuf, 8, &tepBuf[16]);
    std_char_opposite_endian(&tepBuf[16],4);
    file_new.write(&tepBuf[16], 4);

    memset(tepBuf, 0, 32);
    sprintf(tepBuf, "%08x", file_head.tms_app_len);
    std_asc_2_bcd(tepBuf, 8, &tepBuf[16]);
    std_char_opposite_endian(&tepBuf[16],4);
    file_new.write(&tepBuf[16], 4);

    file_new.write("\x00\x00\x00\x00", 4);//for future
    file_new.write("\x00\x00\x00\x00", 4);//for future
    file_new.write("\x00\x00\x00\x00", 4);//for future
    file_new.write("\x00\x00\x00\x00", 4);//for future

    memset(tepBuf, 0, sizeof(file_head.tms_file_sign_len)+sizeof(file_head.tms_file_sign_data));
    file_new.write(tepBuf, sizeof(file_head.tms_file_sign_len)+sizeof(file_head.tms_file_sign_data));

    int file_len = file_head.tms_emv_len;
    int file_w_len = 1024*16;
    int file_rw_len = 0;
    char buf[1024*16]={0};
    while(file_len>0){
        file_w_len = file_w_len>file_len?file_len:file_w_len;
        file_rw_len = file_emv.read(buf, file_w_len);
        file_new.write(buf, file_rw_len);
        file_len -= file_w_len;
    }

    file_len = file_head.tms_app_len;
    file_rw_len = 0;
    file_w_len = 1024*16;

    while(file_len>0){
        file_w_len = file_w_len>file_len?file_len:file_w_len;
        file_rw_len = file_app.read(buf, file_w_len);
        file_new.write(buf, file_rw_len);
        file_len -= file_w_len;
    }

    file_app.close();
    file_new.close();
    file_emv.close();

    algorithm alg;
    QString file_md5_data = fileMd5(remove_filename, sizeof(file_head));
    QString file_signed_data;
    file_md5_data = file_md5_data.toUpper();
    file_md5_data += "tms_ota";

    alg.RSASignAction(file_md5_data, file_signed_data, priKey_file);
    //alg.RSAVerAction(file_md5_data, file_signed_data, pubKey_file);
    file_new.open(QIODevice::ReadWrite);
    if(isok != true)
    {
        QMessageBox::critical(this,"error info","open new file error");
        return;
    }
    file_new.seek(sizeof(file_head)-sizeof(file_head.tms_file_sign_len)-sizeof(file_head.tms_file_sign_data));
    memset(tepBuf, 0, 32);
    sprintf(tepBuf, "%08x", file_signed_data.length()/2);
    std_asc_2_bcd(tepBuf, 8, &tepBuf[16]);
    std_char_opposite_endian(&tepBuf[16],4);
    file_new.write(&tepBuf[16], 4);

    memset(tepBuf, 0, 512);
    std_asc_2_bcd(file_signed_data.toUtf8().data(), file_signed_data.length(), tepBuf);
    trace_value("file_signed_data ",tepBuf, file_signed_data.length()/2);
    file_new.write(tepBuf, file_signed_data.length()/2);
    file_new.close();

    return;

}

