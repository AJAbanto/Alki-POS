#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "databasedialog.h"
#include <QtSql/QtSql>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlRelationalTableModel>
#include <QtCore>
#include <QDebug>
#include <QString>
#include <QCompleter>
#include <QMessageBox>
#include <QInputDialog>
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrinter>
#include <QPainter>
#include <QFontDatabase>




MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->progpage->setCurrentWidget(ui->Loginpage);
    ui->progpage->showMaximized();

                                                                               //creating connection
    if(createconnection()){
        qDebug()<< "Connected to server";
        setTables();
        setCompleters();                                                        //set completer for all search bars
    }

    int id = QFontDatabase::addApplicationFont(":/fonts/fre3of9x.ttf");             //setting barcode fonts from resourcefile
    QString family = QFontDatabase::applicationFontFamilies(id).at(0);
    BarFont.setFamily(family);


}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    QString Cname , Ctype , Cbill , CDel , Ccon;
    QStringList Clist;
    float mxdebt;
    QSqlQuery newC , newClist;
    QCompleter *custcompleter;

    Cname = ui->Cname->text();
    Ctype = ui->Ctype->currentText();
    Cbill = ui->Cbill->text();
    CDel = ui->Cdel->text();
    Ccon = ui->Ccon->text();
    mxdebt = ui->spinmxdbt->value();



    qDebug() << Cname << Ctype << Cbill << CDel << Ccon;
    newC.prepare("INSERT INTO tblcustomers ( Name ,Type , BillingAdd , DeliveryAdd,Contact , Maxdebt ) VALUES (? , ? , ? , ? , ?, ? );");
    newC.bindValue(0 ,Cname);
    newC.bindValue(1 ,Ctype);
    newC.bindValue(2 ,Cbill);
    newC.bindValue(3 ,CDel);
    newC.bindValue(4 ,Ccon);
    newC.bindValue(5, mxdebt);


    if(newC.exec()){

        ui->Cname->clear();
        ui->Cbill->clear();
        ui->Cdel->clear();
        ui->Ccon->clear();
        qDebug()<<"New row inserted";

        //-------------- re-populate table---------------

        custtbl->select();                 //populate table

        qDebug()<<custtbl->rowCount();

        ui->customertbl->setModel(custtbl);        //show table on tabl view
        ui->customertbl->show();


        //---------------- end repopulate -------------------------
    }

    if(newClist.exec("SELECT Name from tblcustomers;")){                    //filling up search bar completer
        for(;newClist.next();)
            Clist << newClist.value(0).toString();
        custcompleter = new QCompleter(Clist, this);
        custcompleter->setCaseSensitivity(Qt::CaseInsensitive);
        ui->txtsearchcust->setCompleter(custcompleter);

    }


}


void MainWindow::on_btncustsearch_clicked()
{
    QString scname = ui->txtsearchcust->text(); //read name
    ui->txtsearchcust->clear();                 //clear textbox

    if(scname.isEmpty()){
        custtbl->setFilter("");
      }else{
        QString sstring = "Name LIKE '"+ scname +"%'";            //search for names similar to search name
        custtbl->setFilter(sstring);
    }

}

void MainWindow::on_btn_addprod_clicked()
{
    QStringList prodinfo , plist;
    float prodvals[4] = {0};
    QSqlQuery newp , newplist;


    prodinfo<<ui->txtp_name->text() << ui->txtp_code->text() << ui->Pctgy->currentText() <<ui->Qtype->currentText(); //read lineedits


    for(int i = 0 ; i < 4 ; i++){
        switch(i){
            case 0:
                prodvals[i] = ui->spin_pcs->value();
                break;
            case 1:
                prodvals[i] = ui->spin_wgt->value();
                break;
            case 2:
                prodvals[i] = ui->spin_pprice->value();
                break;
            case 3:
                prodvals[i] = ui->spin_rprice->value();
                break;
        }
    }                               //read numerical values

    newp.prepare("INSERT INTO tblproducts(Name , Code , Category ,"
                 " Qty_Type , Pcs_per_Bndl , Wgt_per_Bndl ,Pur_price , Rtail_price)VALUES (? , ? , ? , ? , ? , ? , ? , ?);");

    for(int i = 0 ; i < 8 ; i++){                                       //bind values entered
        if(i < 4){
            newp.bindValue(i , prodinfo[i]);
            qDebug()<<prodinfo[i];

        }else if (i >= 4){
            newp.bindValue(i , prodvals[i-4]);
            qDebug()<<QString::number(prodvals[i-4]);
        }

    }

    if (newp.exec()){
        ui->txtp_code->clear();                         //clean up textboxes & spinboxes
        ui->txtp_name->clear();
        ui->Pctgy->setCurrentIndex(0);
        ui->Qtype->setCurrentIndex(0);
        ui->spin_pcs->clear();
        ui->spin_wgt->clear();
        ui->spin_pprice->clear();
        ui->spin_rprice->clear();

        prodtbl->select();                              //refresh table




        QSqlQuery prods;
        QStringList prodlist;

        if(prods.exec("SELECT Name from tblproducts;")){
            for(; prods.next() ;)
                prodlist << prods.value(0).toString();
            ui->prodstock->clear();
            ui->prodstock->addItems(prodlist);
        }
    }else{
            qDebug()<<"Insert Failed";
    }

    if(newplist.exec("SELECT Name from tblproducts;")){                 //filling product restock combo box
        for(;newplist.next();)                                          //Subject to change
            plist << newplist.value(0).toString();                      // centralize product list (make global)

        ui->prodstock->addItems(plist);
        QCompleter *prodcompleter = new QCompleter(plist, this );                          //updating product completer
        prodcompleter->setCaseSensitivity(Qt::CaseInsensitive);
        ui->txtsearchprod->setCompleter(prodcompleter);
    }

}

void MainWindow::on_btnprodsearch_clicked()
{
    QString spname = ui->txtsearchprod->text(); //read name
    ui->txtsearchprod->clear();                 //clear textbox

    if(spname.isEmpty()){
        prodtbl->setFilter("");
      }else{
        QString sstring = "Name LIKE '"+ spname +"%' OR Code LIKE '"+spname +"%'";            //search for names similar to search name
        prodtbl->setFilter(sstring);
    }
}

void MainWindow::on_pushButton_5_clicked()
{
    QSqlQuery getID , newstock ,checkstock, updatestock;
    int prodID , curstock, addstock;
    getID.prepare("SELECT ID FROM tblproducts WHERE Name = ?;");
    getID.bindValue(0, ui->prodstock->currentText());


    if(ui->stockqty->value() > 0){
        if(getID.exec()){
            getID.next();
            qDebug()<<"current text is:" << ui->prodstock->currentText() << "Return value is: "<<getID.value(0).toString();

            prodID = getID.value(0).toInt();

            checkstock.prepare("SELECT Quantity , Storage_ID  FROM tblinventory WHERE Product_ID = ? ;");
            checkstock.bindValue(0, prodID);

            if(checkstock.exec()){

                checkstock.next();                                                   //position on first record and check if null quantity

                if(!checkstock.isNull("Quantity")){
                                                                                    //update row if stock already exists
                    curstock = checkstock.value(0).toInt();
                    addstock = ui->stockqty->value();

                    updatestock.prepare("UPDATE tblinventory SET Quantity = ? WHERE Product_ID = ?;");
                    updatestock.bindValue(0 , addstock + curstock);
                    updatestock.bindValue(1, prodID);
                    updatestock.exec();

                }else{
                                                                                   //insert new row if stock does not exist
                    newstock.prepare("INSERT INTO tblinventory (Product_ID , Quantity , Loc_ID) VALUES (?,?,?)");
                    newstock.bindValue(0,prodID);
                    newstock.bindValue(1, ui->stockqty->value());
                    newstock.bindValue(2, ui->stockstorage->currentText());
                    newstock.exec();
                    //insert new record.
                }
            }
        }

        inventbl->select();
    }else {
        QMessageBox ZeroQty;
        ZeroQty.setText("Quantity to re-stock cannot be zero");
        ZeroQty.exec();
    }
}

void MainWindow::on_inventsearchbttn_clicked()
{
    QString invenname = ui->txtinventsearch->text(); //read name
    ui->txtinventsearch->clear();                 //clear textbox

    if(invenname.isEmpty()){
        inventbl->setFilter("");
      }else{
        QString sstring = "Name LIKE '"+ invenname +"%' OR Code LIKE '"+invenname +"%'";            //search for names similar to search name
        inventbl->setFilter(sstring);
    }


}

void MainWindow::on_txtinventsearch_returnPressed()
{
    MainWindow::on_inventsearchbttn_clicked();
}

void MainWindow::on_txtsearchprod_returnPressed()
{
    MainWindow::on_btnprodsearch_clicked();
}

void MainWindow::on_txtbarcode_returnPressed()
{
    int  cur_stock;
    QSqlQuery checkstock, getprodinfo , updatestock , recordtrans;
    QString  barcode = ui->txtbarcode->text();           //read barcode
    ui->txtbarcode->clear();                            //clear line edit
    int lastestrcpt = ui->latestrcpt2->value();         //get latest rcpt number

        getprodinfo.prepare("SELECT ID , Code , Rtail_price FROM tblproducts WHERE Code = ?;");       //retrieve product info from table
        getprodinfo.bindValue(0 , barcode);

        if(getprodinfo.exec()){
            getprodinfo.next();
            checkstock.prepare("SELECT Quantity FROM tblinventory WHERE Product_ID = ? ");          //get quantity instock
            checkstock.bindValue(0,getprodinfo.value(0).toInt());

            for(int i = 0 ; i< 3 ; i++)
                qDebug()<<getprodinfo.value(i).toString();

            if(checkstock.exec()){
                checkstock.next();
                if(!checkstock.isNull("Quantity") || !(checkstock.value(0).toInt() == 0)){     //check if item is in-stock
                    cur_stock = checkstock.value(0).toInt();

                    updatestock.prepare("UPDATE tblinventory SET Quantity = ? WHERE Product_ID = ?;");      //updating stock
                    updatestock.bindValue(0,cur_stock-1);
                    updatestock.bindValue(1, getprodinfo.value(0).toInt());

                    if(updatestock.exec()){
                        recordtrans.prepare("INSERT INTO tbltransactions (RecieptID , ProductID ,Rtail_price , Quantity , userID )VALUES( ?,? ,?, 1 , 1); ");
                        recordtrans.bindValue(0, lastestrcpt );
                        recordtrans.bindValue(1 ,getprodinfo.value(0).toInt());
                        recordtrans.bindValue(2, getprodinfo.value(2).toFloat());                         //appending new transaction

                        if(recordtrans.exec()){
                            qDebug()<<"Success!!!";
                            transtbl->select();
                        }else
                            qDebug()<<"Fail to append"<<recordtrans.lastError();
                    }

                    //--Important note--
                    // I assumed that no one customer may transact more than a few pcs
                    // of the same item
                }else{
                    QMessageBox nostock;                                                         //warning if no stock
                    nostock.setText("No stock available for scanned item, please scan again");
                    nostock.setIcon(QMessageBox::Warning);
                    nostock.exec();

                }


            }

        }

    getsubtotal();

}

void MainWindow::on_btn_cashout_clicked()
{

    //prompt payment Dialogue box
    bool accpt;

    double cashamount = QInputDialog::getDouble(this,tr("Payment"), tr("Cash Amount:"),0,0,500,1,&accpt);

    if(accpt){

        int currrecpt = ui->latestrcpt->value();

        printreceipt(cashamount);                                         //print receipt and reset table view

        QSqlQuery newrecpt;
        newrecpt.prepare("INSERT INTO tblreceipts (ID , Customer_ID) VALUES(?,5) ");
        newrecpt.bindValue(0,currrecpt++);

        if(newrecpt.exec()){
            qDebug()<<"bruh";
            ui->latestrcpt->setValue( ui->latestrcpt->value()+1) ;
            ui->latestrcpt2->setValue(ui->latestrcpt->value()) ;
        }
    }

}

void MainWindow::on_buttonBox_accepted()
{
    QString accesslevel,user = ui->txtusername->text(), pass = ui->txtpassword->text();
    QSqlQuery getLoginfo;

    getLoginfo.prepare("SELECT username , password , usertype FROM tblusers WHERE username = ?;");
    getLoginfo.bindValue(0,user);

    if(getLoginfo.exec()){
        getLoginfo.next();
        int check = QString::compare(getLoginfo.value(1).toString() , pass , Qt::CaseInsensitive);

        if(check == 0){
            accesslevel = getLoginfo.value(2).toString();
            int admin =QString::compare(accesslevel, "admin");
            int manager = QString::compare(accesslevel, "manager");
            int clerk = QString::compare(accesslevel,"clerk");

            if(admin == 0 ){
                ui->progpage->setCurrentWidget(ui->adminpage);
            }else if(manager == 0){
                ui->progpage->setCurrentWidget(ui->adminpage);
            }else if(clerk == 0){
                ui->progpage->setCurrentWidget(ui->Cashierpage);
            }
        }else{
            QMessageBox errlogin;
            errlogin.setText("Invalid username or Password");
            errlogin.setIcon(QMessageBox::Warning);
            errlogin.exec();
        }

    }
}

void MainWindow::on_pushButton_3_clicked()
{
    ui->progpage->setCurrentWidget(ui->Loginpage);
}

void MainWindow::on_txtpassword_returnPressed()
{
    on_buttonBox_accepted();
}

void MainWindow::on_txtbarcode2_returnPressed()
{

    qDebug()<<"Enter pressed";

    int  cur_stock;
    QSqlQuery checkstock, getprodinfo , updatestock , recordtrans;
    QString  barcode = ui->txtbarcode2->text();           //read barcode
    ui->txtbarcode2->clear();                            //clear line edit
    int lastestrcpt = ui->latestrcpt->value();          //get latest receipt number

        getprodinfo.prepare("SELECT ID , Code , Rtail_price FROM tblproducts WHERE Code = ?;");       //retrieve product info from table
        getprodinfo.bindValue(0 , barcode);

        if(getprodinfo.exec()){
            getprodinfo.next();
            checkstock.prepare("SELECT Quantity FROM tblinventory WHERE Product_ID = ? ");          //get quantity instock
            checkstock.bindValue(0,getprodinfo.value(0).toInt());

            for(int i = 0 ; i< 3 ; i++)
                qDebug()<<getprodinfo.value(i).toString();

            if(checkstock.exec()){
                checkstock.next();
                if(!checkstock.isNull("Quantity") || !(checkstock.value(0).toInt() == 0)){     //check if item is in-stock
                    cur_stock = checkstock.value(0).toInt();

                    updatestock.prepare("UPDATE tblinventory SET Quantity = ? WHERE Product_ID = ?;");      //updating stock
                    updatestock.bindValue(0,cur_stock-1);
                    updatestock.bindValue(1, getprodinfo.value(0).toInt());

                    if(updatestock.exec()){
                        recordtrans.prepare("INSERT INTO tbltransactions (RecieptID , ProductID ,Rtail_price , Quantity , userID )VALUES( ?,? ,?, 1 , 1); ");
                        recordtrans.bindValue(0, lastestrcpt );
                        recordtrans.bindValue(1 ,getprodinfo.value(0).toInt());
                        recordtrans.bindValue(2, getprodinfo.value(2).toFloat());                         //appending new transaction

                        if(recordtrans.exec()){
                            qDebug()<<"Success!!!";
                            transtbl->select();
                        }else
                            qDebug()<<"Fail to append"<<recordtrans.lastError();
                    }

                    //--Important note--
                    // I assumed that no one customer may transact more than a few pcs
                    // of the same item
                }else{
                    QMessageBox nostock;                                                         //warning if no stock
                    nostock.setText("No stock available for scanned item, please scan again");
                    nostock.setIcon(QMessageBox::Warning);
                    nostock.exec();

                }


            }

        }
}

void MainWindow::setCompleters(){

    //Create all completers and items on searchbars and dropboxes

    QStringList Ctypes , Qtypes, Ptypes , Pctgy , Custlist;
    Ctypes<< "Walk-in" <<" Regular";
    Qtypes<<"Per Kilo" <<"Per Piece" << "Per Bundle";
    Pctgy << "Furniture" << "Apparel" << "Glassware" << "Toys" << "Misc." << "Others";

    QSqlQuery prods , recpts , custs;
    QCompleter *prodcompleter , *custcompleter;

    if(prods.exec("SELECT Name from tblproducts;")){                    //filling up restock-combobox
        for(; prods.next() ;){
            Ptypes << prods.value(0).toString();
        }
        ui->prodstock->addItems(Ptypes);
        prodcompleter = new QCompleter(Ptypes, this);                       //initiating product completer
        prodcompleter->setCaseSensitivity(Qt::CaseInsensitive);

    }


    if(custs.exec("SELECT Name from tblcustomers;")){
        for(;custs.next();)
            Custlist << custs.value(0).toString();

        custcompleter = new QCompleter(Custlist, this);                 //initiating customer completer
        custcompleter->setCaseSensitivity(Qt::CaseInsensitive);

    }

    ui->Qtype->addItems(Qtypes);                    //filling up other combo boxes
    ui->Ctype->addItems(Ctypes);
    ui->Pctgy->addItems(Pctgy);
    ui->txtsearchcust->setCompleter(custcompleter);
    ui->txtsearchprod->setCompleter(prodcompleter);
    ui->txtinventsearch->setCompleter(prodcompleter);
    ui->prodlookup->setCompleter(prodcompleter);
    ui->prodlookup2->setCompleter(prodcompleter);

    if(recpts.exec("SELECT ID from tblreceipts; ")){            //get latest receipt number
        recpts.last();
        int recpnum = recpts.value(0).toInt();
        ui->latestrcpt->setValue(recpnum);                      //set current recept spinbox to latest receipt
        ui->latestrcpt2->setValue(recpnum);
    }
}

void MainWindow::setTables(){

    custtbl = new QSqlTableModel(this , db);
    prodtbl = new QSqlTableModel(this, db );
    inventbl = new QSqlRelationalTableModel(this , db);
    transtbl = new QSqlRelationalTableModel(this, db);

    transtbl->setTable("tbltransactions");
    transtbl->setRelation(3, QSqlRelation("tblproducts", "ID" , "Code"));
    transtbl->setEditStrategy(QSqlTableModel::OnFieldChange);


    inventbl->setTable("tblinventory");                     //setting relations in invetory and transaction tables
    inventbl->setRelation(0, QSqlRelation("tblproducts" ,"ID", "Name"));
    inventbl->setHeaderData(0, Qt::Horizontal, tr("Product"));
    inventbl->setHeaderData(2, Qt::Horizontal, tr("Storage ID"));
    inventbl->setEditStrategy(QSqlTableModel::OnFieldChange);



    prodtbl->setTable("tblproducts");
    prodtbl->setEditStrategy(QSqlTableModel::OnFieldChange);

    custtbl->setTable("tblcustomers");
    prodtbl->setEditStrategy(QSqlTableModel::OnFieldChange);

    inventbl->select();
    prodtbl->select();
    custtbl->select();                 //populate table
    transtbl->select();


    QStringList rcolnames,ccolnames , pcolnames;
    ccolnames << "ID" << "Name" << "Billing Add." << " Delivery Add." << "Contact No." << "Max Debt";
    pcolnames <<"ID"<<"Names"<<"Code" << "Category" << "Qty.Type" << "Pcs. per Bndle" << "Wgt. per Bndle/Pc." << "Wholesale" << "Retail";
    rcolnames <<"Transaction ID" << "Receipt ID" << "Date" << "Product" << "unit price" << "Quantity" << "User ID";

    for(int i = 0 ; i < ccolnames.count() ; i++)
        custtbl->setHeaderData(i , Qt::Horizontal , ccolnames[i]);              //setting column headers

    for(int i = 0 ; i < pcolnames.count() ; i++)
        prodtbl->setHeaderData(i , Qt::Horizontal, pcolnames[i]);

    for(int i= 0 ; i < rcolnames.count() ; i++)
        transtbl->setHeaderData(i,Qt::Horizontal,rcolnames[i]);



    ui->tblinvent->setModel(inventbl);
    ui->customertbl->setModel(custtbl);        //Display tables on table view
    ui->prodtableview->setModel(prodtbl);
    ui->receiptview->setModel(transtbl);
    ui->cashierview->setModel(transtbl);

    ui->receiptview->setAlternatingRowColors(true);
    ui->anlytblview->setAlternatingRowColors(true);
    ui->tblinvent->setAlternatingRowColors(true);
    ui->prodtableview->setAlternatingRowColors(true);   //turn on alternating colors
    ui->customertbl->setAlternatingRowColors(true);
    ui->cashierview->setAlternatingRowColors(true);

    ui->customertbl->show();
    ui->tblinvent->show();
    ui->prodtableview->show();                      //show tables
    ui->receiptview->show();

    ui->prodtableview->resizeColumnsToContents();           //resize column width
    ui->receiptview->setItemDelegate(new QSqlRelationalDelegate(ui->receiptview));      //setting foreign keysupport
    ui->cashierview->setItemDelegate(new QSqlRelationalDelegate(ui->cashierview));
    ui->tblinvent->setItemDelegate(new QSqlRelationalDelegate(ui->tblinvent));

    ui->receiptview->hideColumn(1);
    ui->receiptview->hideColumn(2);

    ui->cashierview->hideColumn(1);
    ui->cashierview->hideColumn(2);

    getsubtotal();
}

bool MainWindow::createconnection(){
    //open local database
    db = QSqlDatabase::addDatabase("QMYSQL");
    loadDBsettings();
    bool ok = db.open();
    return ok;
}

void MainWindow::loadDBsettings(){
    QString defhost = "127.0.0.1" , defdbname = "brodb" , defuser = "root", defpwd = "";     //default db values



    QSettings setting ("Ajsoftware" ,"Alki" );
    if(setting.value("hostname").toString() == 0 || setting.value("dbname").toString() == 0 ||setting.value("user").toString() == 0 ||
            setting.value("pwd").toString() == 0){
        openDBsettings();                                             //prompt for database connection info
        db.setHostName(setting.value("hostname").toString());      //getting stored settings,AFTER PROMPT
        db.setDatabaseName(setting.value("dbname").toString());
        db.setUserName(setting.value("user").toString());
        db.setPassword(setting.value("pwd").toString());

    }else{
        db.setHostName(setting.value("hostname").toString());      //getting stored settings, if null give default value
        db.setDatabaseName(setting.value("dbname").toString());
        db.setUserName(setting.value("user").toString());
        db.setPassword(setting.value("pwd").toString());
    }




}

void MainWindow::openDBsettings(){

    QString defhost = "127.0.0.1" , defdbname = "brodb" , defuser = "root", defpwd = "";     //default db values
   DatabaseDialog settingsDia;                                 //open settings dialogue box
   settingsDia.setModal(true);                                 //set modal mode

   QSettings setting ("Ajsoftware" ,"Alki" );                                       //open saving settings:

   settingsDia.sethost(setting.value("hostname",defhost).toString());               //getting stored settings, if null give default value
   settingsDia.setdb(setting.value("dbname",defdbname).toString());
   settingsDia.setuser(setting.value("user", defuser).toString());
   settingsDia.setpass(setting.value("pwd", defpwd).toString());


   if(settingsDia.exec()){
           setting.setValue("hostname" , settingsDia.gethost());                       //saving values
           setting.setValue("dbname",settingsDia.getdbname());
           setting.setValue("user",settingsDia.getuser());
           setting.setValue("pwd",settingsDia.getpass());

       qDebug()<<"stuff:"<<settingsDia.gethost()<<" "<<settingsDia.getdbname()<<" "<<settingsDia.getpass()<<""<<settingsDia.getuser()<<" ";

   }


}

void MainWindow::on_btnPrintAllBarcode_clicked()
{

     QFont sansFont("Helvetica [Cronyx]",10);    //working point size 6
     BarFont.setPointSize(50);       //working point size 16
                //ask to print out new receipt.

                //print out receipt

         QPrinter printer(QPrinter::HighResolution); //create your QPrinter (don't need to be high resolution, anyway)
         printer.setPageSize(QPrinter::A4);
         printer.setOrientation(QPrinter::Portrait);
         printer.setPageMargins (15,15,15,15,QPrinter::Millimeter);
         printer.setFullPage(false);
         printer.setOutputFormat(QPrinter::PdfFormat);
         printer.setOutputFileName("barcodesum.pdf");

         //loop through each row

         int rowCount = prodtbl->rowCount();
         QPainter painter;
         if (! painter.begin(&printer)) {                // failed to open file
                qWarning("failed to open file, is it writable?");
         }else{
                qDebug()<<"painter initialized";
                painter.setFont(BarFont);               //formerly used barcode front
                qDebug()<<QString::number(BarFont.pointSize());     //Debug

                QRect rect = QRect(0,0,4000,96*4);         //working was 2000 width and 96*2 height
                QRect rect2 = QRect(0,96*4,4000,96*4);
                QPen shape;
                shape.setWidth(20);
                QPen def;

                    for(int i = 0 ; i < rowCount ; i++){                      //looping through table to retrieve records
                        painter.setFont(BarFont);                                               //Painting onto summary output
                        QString barbar = "*"+prodtbl->record(i).value(2).toString()+"*";
                        painter.setPen(shape);
                        painter.drawRect(rect);
                        painter.setPen(def);
                        painter.drawRect(rect2);
                        painter.drawText(rect  ,Qt::AlignCenter,barbar);    //draw barcode in Code 39 Font
                        painter.setFont(sansFont);                                                          //reset font
                        painter.drawText(rect2,Qt::AlignHCenter,prodtbl->record(i).value(2).toString());    //draw Alpha-numeric equivalent
                        rect.adjust(0,96*8,0,96*8);
                        rect2.adjust(0,96*8,0,96*8);
                     }

         }
         painter.end();




    qDebug()<<"Bro";
}

void MainWindow::printreceipt(float cashamount){


    QFont sansFont("Helvetica [Cronyx]",7);                                 //print out receipt

         QPrinter printer(QPrinter::HighResolution);                       //create your QPrinter (don't need to be high resolution, anyway)
         printer.setPageSize(QPrinter::A4);
         printer.setOrientation(QPrinter::Portrait);
         printer.setOutputFormat(QPrinter::NativeFormat);
         printer.setFullPage(false);



         int rowCount = transtbl->rowCount();
         QPainter painter;
         if (! painter.begin(&printer)) {                                  // failed to open file
                qWarning("failed to open file, is it writable?");
         }else{
                qDebug()<<"painter initialized";

                QRect rect = QRect(0,0,350,50);                          //alignment container for text
                QPoint startline , endline;
                painter.setFont(sansFont);
                QPen line;                                                  //divider brush
                QPen def;                                                   //default brush
                float subtotal = 0;
                line.setWidth(10);


                    for(int i = 0 ; i < rowCount ; i++){                      //looping through table to retrieve records
                                                                              //Painting onto receipt output


                        if(i ==0 ){
                            painter.drawText(rect, Qt::AlignCenter, "StoreName");
                            rect.adjust(0,50,0,50);
                            painter.drawText(rect,Qt::AlignHCenter, "Street Name");
                            rect.adjust(0,50,0,50);
                            painter.drawText(rect,Qt::AlignHCenter,"Property and City");
                            rect.adjust(0,50,0,50);
                            painter.drawText(rect, Qt::AlignHCenter,"Contact num");
                            rect.adjust(0,50,0,50);

                                                                             //draw dividing line
                            startline.setX(rect.x());
                            startline.setY(rect.y());
                            endline = rect.topRight();
                            painter.setPen(line);
                            painter.drawLine(startline, endline);
                           }

                        painter.setPen(def);                                                             //reseting Pen

                        QString itmname = transtbl->record(i).value(3).toString();
                        QString qtyS = transtbl->record(i).value(5).toString();

                        painter.drawText(rect  ,Qt::AlignLeft | Qt::AlignVCenter, qtyS+" "+itmname);     //name of product

                        float price = transtbl->record(i).value(4).toFloat();                            //get qty and price
                        float qty = transtbl->record(i).value(5).toFloat();

                        QString total = QString::number(price*qty);                                       //convert to string
                        painter.drawText(rect,Qt::AlignRight | Qt::AlignVCenter,total);
                        rect.adjust(0,50,0,50);

                        subtotal += (price *qty);                                                       //calculating subtotal
                     }


                    startline.setX(rect.x());
                    startline.setY(rect.y());                               //closing line for item list and opening line for subtotal
                    endline = rect.topRight();
                    painter.setPen(line);
                    painter.drawLine(startline, endline);

                    painter.setPen(def);
                    painter.setFont(sansFont);
                    painter.drawText(rect, Qt::AlignLeft|Qt::AlignVCenter, "SUB TOTAL");
                    painter.drawText(rect, Qt::AlignRight|Qt::AlignVCenter, "P"+QString::number(subtotal));
                    rect.adjust(0,50,0,50);

                    painter.drawText(rect, Qt::AlignLeft|Qt::AlignVCenter, "VAT");
                    painter.drawText(rect, Qt::AlignRight|Qt::AlignVCenter, "P"+QString::number(subtotal*.20));      //spoof calculation
                    rect.adjust(0,50,0,50);

                    startline.setX(rect.x());
                    startline.setY(rect.y());                               //closing line subtotal
                    endline = rect.topRight();
                    painter.setPen(line);
                    painter.drawLine(startline, endline);

                    painter.drawText(rect, Qt::AlignLeft|Qt::AlignVCenter,"AMOUNT DUE:");                               //actual amout due
                    painter.drawText(rect, Qt::AlignRight|Qt::AlignVCenter, "P"+QString::number(subtotal+(subtotal*.20)));
                    rect.adjust(0,50,0,50);
                    painter.drawText(rect, Qt::AlignLeft|Qt::AlignVCenter,"CASH");          //must integrate payment prop
                    painter.drawText(rect, Qt::AlignRight|Qt::AlignVCenter, "P"+QString::number(cashamount));
                    rect.adjust(0,50,0,50);
                    painter.drawText(rect,Qt::AlignLeft|Qt::AlignVCenter,"Change");
                    painter.drawText(rect,Qt::AlignRight|Qt::AlignVCenter,"P500");
                    rect.adjust(0,50,0,50);

                    startline.setX(rect.x());
                    startline.setY(rect.y());                               //closing line payment
                    endline = rect.topRight();
                    painter.setPen(line);
                    painter.drawLine(startline, endline);

                    rect.adjust(0,50,0,50);
                    painter.drawText(rect,Qt::AlignCenter, "Thank you for shopping at SHOP NAME");              //end of receipt
                    rect.adjust(0,50,0,50);
                    painter.drawText(rect,Qt::AlignCenter, "this invoice is valid for (5) years");
                    rect.adjust(0,50,0,50);
                    painter.drawText(rect,Qt::AlignCenter, "Receipt number: #BLOOP");
                    rect.adjust(0,50,0,50);

         }

            painter.end();
            transtbl->select();

}

void MainWindow::on_pushButton_4_clicked()
{

    bool accpt;                                                                                             //flag for input dialogue

    double cashamount = QInputDialog::getDouble(this,tr("Payment"), tr("Cash Amount:"),0,0,500,1,&accpt);   //prompt payment w/ inputDialogue box

    if(accpt){

        int currrecpt = ui->latestrcpt->value();

        printreceipt(cashamount);                                         //print receipt and reset table view

        QSqlQuery newrecpt;
        newrecpt.prepare("INSERT INTO tblreceipts (ID , Customer_ID) VALUES(?,5) ");
        newrecpt.bindValue(0,currrecpt++);

        if(newrecpt.exec()){
            qDebug()<<"bruh";
            ui->latestrcpt->setValue( ui->latestrcpt->value()+1) ;
            ui->latestrcpt2->setValue(ui->latestrcpt->value()) ;
        }
    }
}

void MainWindow::deletetrans(){


    int latestrcpt = ui->latestrcpt->value();                   //get receipt number
    QSqlQuery DeleteLatest;                                     //declare query

    DeleteLatest.prepare("DELETE FROM tbltransactions WHERE RecieptID = ?;");
    DeleteLatest.bindValue(0,latestrcpt);

    if(DeleteLatest.exec())                                     //actual execution of query
        qDebug()<<"Trans Deleted";
    else
        qDebug()<<DeleteLatest.lastError();

    transtbl->select();                                         //refreshing table view


}

void MainWindow::on_btn_void_clicked()
{
    deletetrans();
}

void MainWindow::on_prodlookup_returnPressed()
{
    //search
}

void MainWindow::getsubtotal(){
    int rowcount = transtbl->rowCount();

    ui->subttl1->setValue(0);                                                       //reset subtotal spinbox values
    ui->subttl2->setValue(0);

    if(ui->subttl1->value() == ui->subttl2->value()){                                      //check if current subtotal values are same
        qDebug()<<"this happened";

        float subtotal = ui->subttl1->value();

        for(int i = 0 ; i < rowcount; i++ ){
            float price = transtbl->record(i).value(4).toFloat();                            //get qty and price
            float qty = transtbl->record(i).value(5).toFloat();
            subtotal+= qty*price;                                                           //calculate subtotal
        }
        qDebug()<<QString::number(subtotal);
        ui->subttl1->setValue(subtotal);
        ui->subttl2->setValue(subtotal);

        ui->discnt1->setValue(subtotal*.20);
     }
}

void MainWindow::on_pushButton_7_clicked()
{
    openDBsettings();
}
