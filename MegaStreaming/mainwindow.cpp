#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "crypt.h"

const QString UNDEFINED = "undef";
const QString FILE_CONF = QDir::currentPath() + "\\defconf.msdc";
const QString FILE_CONF_UNIX = QDir::currentPath() + "/defconf.msdc";
const QString FILE_PASS = QDir::currentPath() + "\\tmppwd.mspi";
const QString FILE_PASS_UNIX = QDir::currentPath() + "/tmppwd.mspi";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    on_actionNew_triggered();

    // setting column's width
    ui->accountList->setColumnWidth(0, 210);
    ui->accountList->setColumnWidth(1, 110);
    ui->accountList->horizontalHeader()->setStretchLastSection(1);

    ui->treeWidget->setColumnWidth(0, 120);
    ui->treeWidget->header()->stretchLastSection();

    // retrieving OS
    myos = UNDEFINED;

    #ifdef Q_OS_WIN32
        myos = "win32";
    #else
        #ifdef Q_OS_ANDROID
            myos = "android";
        #else
            myos = "unix";
        #endif
    #endif

    alphabet = "qazwsxedcrfvtgbyhnujmikolp.@1234567890"; // valid characters for e-mail

    // setting path exe
    if (myos=="win32")
    {
        megals = ".\\megatools-1.9.91-win32\\megals.exe";
        player = ".\\VLCPortable\\VLCPortable.exe";
        openConf(FILE_CONF);
        if (boolAutoLoadPassword) openPass(FILE_PASS);
    }

    // unix separator /, not \. Needs WINE per Windows' programs
    if (myos == "unix") {
        if (!QFile::exists("/usr/bin/wine")) QMessageBox::information(this, "Info","To use MEGAstream on a unix system you need to have wine installed. If you want to use the VLC player installed in your system, just write 'vlc' in the 'Player' text field.");
        megals = "./megatools-1.9.91-win32/megals.exe";
        player = "./VLCPortable/VLCPortable.exe";
        openConf(FILE_CONF_UNIX);
        if (boolAutoLoadPassword==true) openPass(FILE_PASS);
    }

    if (myos == "android") {
        QMessageBox::information(this, "Info","To use MEGAstream on a Android device, you must install megals (all the megatools suite) and a layer (for example VLC) into the system. So you need to have root privileges, and possibly the linux-standard-installer app.");
        megals = "megals";
        player = "vlc";
        openConf(FILE_CONF_UNIX);
        if (boolAutoLoadPassword==true) openPass(QDir::currentPath()+"/tmppwd.mspl");
    }

    ui->txtMegals->setText(megals);
    ui->txtPlayer->setText(player);
    ui->autopwd->setChecked(boolAutoLoadPassword);
    on_hidepwd_clicked(ui->hidepwd->isChecked());
}

MainWindow::~MainWindow()
{
    delete ui;
}

// --- PUBLIC ---
void MainWindow::getAccountFiles(QString email, QString password)
{
    QTemporaryFile file;
    QString myfile;
    QString myMegals;

    if(file.open())
    {
        qDebug() << file.fileName();
        myfile = file.fileName()+QString("n");
        file.close();
    }

    megals = ui->txtMegals->text();

    myMegals = megals;

    // getting absolute path
    if (megals.mid(0,1) == ".\\") myMegals =  QDir::currentPath() + megals.mid(1);
    if (megals.mid(0,1) == "./") myMegals =  QDir::currentPath() + megals.mid(1);

    if (myos == "unix") {
        if (myMegals.right(4)==".exe") {
            myMegals =  "wine " + myMegals;
        }
    }

    // command line
    QString program = myMegals + " -e -u " + email +" -p " + password + " >> " + myfile;

    QByteArray ba = program.toLocal8Bit();
    const char *temppr = ba.data();
    system(temppr);

    QTreeWidgetItem *thisuser = new QTreeWidgetItem(ui->treeWidget);
       thisuser->setText(0, email);

    int count = 0;
    QFile inputFile(myfile);
    if (inputFile.open(QIODevice::ReadOnly))
    {
       QTextStream in(&inputFile);
       while ( !in.atEnd() )
       {
           QString line = in.readLine();
           if(line[0]==QChar('h')) {
               qDebug() << line.split(" ", QString::SkipEmptyParts).at(1);
               qDebug() << line.split(" ", QString::SkipEmptyParts).at(0);
               QTreeWidgetItem *fileItem = new QTreeWidgetItem(thisuser);
               fileItem->setText(0, line.mid(line.indexOf(" ")+1));
               fileItem->setText(1, line.split(" ", QString::SkipEmptyParts).at(0));
               count++;
           }
       }
    }

    if (count == 0) QMessageBox::critical(this,"Error","It was not possible to retrieve the list of " + email + "'s account files. There are three possible reasons: email or password are wrong, you are using a proxy to connect to internet, you do not have files uploaded into your account. Please take note that it is not possible to stream files if you are using a proxy and it's not possible to stream files that have not been uploaded to you account but have been shared from some other account.");
    inputFile.close();
    //remove file now
    QFile::remove(myfile);
    thisuser->setExpanded(true);
    ui->treeWidget->header()->resizeSections(QHeaderView::ResizeToContents);
}

// --- PRIVATE ---
void MainWindow::on_actionNew_triggered()
{
    ui->accountList->setRowCount(10);

    QTableWidgetItem *item_1;
    QTableWidgetItem *item_2;
    for (int var = 0; var < ui->accountList->rowCount(); ++var) {
        item_1 = new QTableWidgetItem();
        item_2 = new QTableWidgetItem();
        item_1->setText("");
        item_2->setText("");
        ui->accountList->setItem(var, 0, item_1);
        ui->accountList->setItem(var, 1, item_2);
    }
}

void MainWindow::on_hidepwd_clicked(bool state)
{
    ui->accountList->setColumnHidden(1, state);
}

void MainWindow::openConf(QString filename)
{
    QFile inputFile(filename);
    if (inputFile.open(QIODevice::ReadOnly))
    {
        int i = 0;
        QTextStream in(&inputFile);

        while (!in.atEnd())
        {
            QString line = in.readLine();
            if (i==0) player = line;
            if (i==1 && line=="true") boolAutoLoadPassword = true;
            if (i==1 && line=="false") boolAutoLoadPassword = false;
            if (i==2) megals = line;
            if (i==3 && line=="true") ui->hidepwd->setChecked(true);
            if (i==3 && line=="false") ui->hidepwd->setChecked(false);
        }
    }
}

void MainWindow::saveConf(QString fileName)
{
    QString tempco = "";
    tempco = tempco + player + QString("\n"); //player
    if (boolAutoLoadPassword == true) tempco = tempco + "true" + QString("\n");     //autoloadpwd
    if (boolAutoLoadPassword == false) tempco = tempco + "false" + QString("\n");    //autoloadpwd
    tempco = tempco + megals + QString("\n"); //megals
    if (ui->hidepwd->isChecked() == true) tempco = tempco + "true" + QString("\n");     //hidepwd
    if (ui->hidepwd->isChecked() == false) tempco = tempco + "false" + QString("\n");    //hidepwd
    if (!(fileName.isEmpty())) {
        //if (fileName.right(5) != ".mspl") fileName = fileName + ".mspl";
        QByteArray ba = tempco.toLatin1();
        char *strsave = ba.data();
        QByteArray bac = fileName.toLatin1();
        char *filec = bac.data();

        std::ofstream out(filec);
        if (!out) QMessageBox::critical(this,"Error","Impossible to save the file.") ;
        out << strsave;
        out.close();
    }
}

void MainWindow::openPass(QString fileName)
{
    QByteArray bac = fileName.toLatin1();
    char *filec = bac.data();
    std::ifstream texto(filec);
    if (!texto) return;

    if (!(fileName.isEmpty())) {
        QString key = QInputDialog::getText(this,"Passphrase","Please insert the encription key for the password file:");
        if (key.isNull() || key.isEmpty()) key = alphabet.at(0);
        if (texto) {
            on_actionNew_triggered();
            QString tempyval;
            char tmpchr;
            int i = 0;
            int tablea = 0;
            do {
                texto >> tmpchr;
                if (tmpchr!='|') tempyval = tempyval + tmpchr;
                if (tmpchr=='|') {
                    if ((tablea==1) and (tempyval != QString("table1"))  and (tempyval != QString("function")) and (tempyval != QString("result")) ) {
                        if ((i%2)!=0) {
                            QTableWidgetItem *titemo = ui->accountList->item((i-1)/2,1) ;
                            if (titemo) titemo->setText(decrypt(tempyval, key, alphabet)) ;
                        } else {
                            QTableWidgetItem *titem = ui->accountList->item((i/2),0) ;
                            if (titem) titem->setText(decrypt(tempyval, key, alphabet)) ;
                        }
                        i++;
                    }
                    if (tempyval == QString("table1")) {
                        tablea=1;
                    }

                    tempyval = "";
                }

            } while (!texto.eof());
            texto.close();
        }
    }
}

void MainWindow::savepass(QString fileName)
{
    if (fileName.isEmpty()) {
        return;
    }
    //save all the cells values
    if (fileName.right(5) != ".mspl") fileName = fileName + ".mspl";

    QString key = QInputDialog::getText(this,"Passphrase","Please insert the encription key for the password file:");
    if (key.isNull() || key.isEmpty()) key = alphabet.at(0);
    QString tempyval;
    tempyval = "table1|";
    for (int i=0; i<ui->accountList->rowCount() ; i++) {
        QTableWidgetItem *titem = ui->accountList->item(i,0) ;
        QTableWidgetItem *titemo = ui->accountList->item(i,1) ;
        if (!titem || titem->text().isEmpty()) {
            break;
        } else {
            QString yvaluerq = titemo->data(Qt::DisplayRole).toString() ;
            QString valuerq = titem->data(Qt::DisplayRole).toString() ;
            tempyval = tempyval + QString("\n") + crypt(valuerq, key, alphabet) + QString("|\n") + crypt(yvaluerq, key, alphabet) + QString("|");
        }
    }

    if (!(fileName.isEmpty())) {
        QByteArray ba = tempyval.toLatin1();
        char *strsave = ba.data();
        QByteArray bac = fileName.toLatin1();
        char *filec = bac.data();

        std::ofstream out(filec);
        if (!out) QMessageBox::critical(this,"Error","Impossible to save the file.") ;
        out << strsave;
        out.close();
    }
}

QString MainWindow::crypt(QString text, QString key, QString alphabet)
{
    return objCrypt.crypt(text, key, alphabet);
}

QString MainWindow::decrypt(QString text, QString key, QString alphabet)
{
    return objCrypt.decrypt(text, key, alphabet);
}

// --- EVENTS ---
void MainWindow::on_ffilter_textChanged(const QString &arg1)
{
    if (ui->treeWidget->topLevelItemCount()!=0) {
            for(int i=0;i<ui->treeWidget->topLevelItemCount();++i) {
                if (ui->treeWidget->topLevelItem(i)->childCount()!=0) {
                    for(int j=0;j<ui->treeWidget->topLevelItem(i)->childCount(); ++j) {
                        if (ui->treeWidget->topLevelItem(i)->child(j)->text(0).contains(ui->ffilter->text(), Qt::CaseInsensitive) || ui->ffilter->text().isEmpty()) {
                            qDebug() << ui->treeWidget->topLevelItem(i)->child(j)->text(0);
                            ui->treeWidget->topLevelItem(i)->child(j)->setHidden(false);
                        } else {
                            ui->treeWidget->topLevelItem(i)->child(j)->setHidden(true);
                        }
                    }
                }
            }
    }
}

void MainWindow::on_refresh_clicked()
{
    if (!( (!ui->accountList->item(0,0)) || ui->accountList->item(0,0)->text().isEmpty() || ui->accountList->item(0,0)->text().isNull() )) {
        if (ui->treeWidget->topLevelItemCount()!=0) {
            for(int i=0;i<ui->treeWidget->topLevelItemCount();++i) {
                delete ui->treeWidget->topLevelItem(i);
            }
        }

        for (int i = 0; i < ui->accountList->rowCount()-1; i++) {
            if (!( (!ui->accountList->item(i,0)) || ui->accountList->item(i,0)->text().isEmpty() || ui->accountList->item(i,0)->text().isNull() )) {
                getAccountFiles(ui->accountList->item(i,0)->text(),ui->accountList->item(i,1)->text());
            } else  {
                break;
            }
        }
    }
}

void MainWindow::on_play_clicked()
{
    if (ui->treeWidget->selectedItems().count()!=0)      on_treeWidget_itemDoubleClicked(ui->treeWidget->selectedItems().at(0), 0);
}

void MainWindow::on_EmptyAccount_clicked()
{
    on_actionNew_triggered();
}

void MainWindow::on_deleteAccount_clicked()
{
    if (ui->accountList->selectedItems().count()!=0){
        ui->accountList->removeRow(ui->accountList->selectedItems().at(0)->row());
    }
}

void MainWindow::on_hidepwd_stateChanged(int arg1)
{
    on_hidepwd_stateChanged(ui->hidepwd->isChecked());
}

void MainWindow::on_saveconf_clicked()
{
    if (myos == "win32") {
        saveConf(FILE_CONF);
        if (boolAutoLoadPassword==true) savepass(FILE_PASS);
    }
    if (myos == "unix") {
        saveConf(FILE_CONF_UNIX);
        if (boolAutoLoadPassword==true) savepass(FILE_PASS_UNIX);
    }
}

void MainWindow::on_save_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Open password file", QDir::currentPath(), "MEGAstream password list (*.mspl)");
    savepass(fileName);
}

void MainWindow::on_load_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open password file", QDir::currentPath(), "MEGAstream password list (*.mspl)");
    openPass(fileName);
}

void MainWindow::on_autopwd_stateChanged(bool checked)
{
    boolAutoLoadPassword = checked;
}

void MainWindow::on_txtMegals_textChanged(const QString &arg1)
{
    megals = arg1;
}

void MainWindow::on_txtPlayer_textChanged(const QString &arg1)
{
    player = arg1;
}

void MainWindow::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    QString url1 = item->text(1);
    qDebug() << url1;
    QString url2 = "http://stream.mega-stream.me/stream.php/stream.php?ph=";
    int n = url1.indexOf("!");
    url2.append(url1.mid(n+1,(url1.indexOf("!", n+1)-(n+1))));
    url2.append("&key=");
    url2.append(url1.mid((url1.indexOf("!", n+1)+1)));
    qDebug() << url2;

    QString program = player;
    if (player.mid(0,1) == ".\\") program =  QDir::currentPath() + player.mid(1);
    if (player.mid(0,1) == "./") program =  QDir::currentPath() + player.mid(1);

    QStringList arguments;

    if (myos == "unix") {
        if (program.right(4)==".exe") {
            arguments << program;
            program =  "wine";
        }
    }

    arguments << url2;

    QProcess *myProcesso = new QProcess();
    myProcesso->start(program, arguments);

    if (myProcesso->waitForStarted(1000) == false) {
        qDebug() << "Error starting external program";
        QMessageBox::critical(this,"Error","Error launching external player.");
    }  else  {

        qDebug() << "External player running";
    }
    myProcesso->waitForReadyRead(100);
    myProcesso->waitForFinished(100);
    qDebug() << myProcesso->readAllStandardOutput();
}
