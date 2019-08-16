#include "dlgispindeleinstellung.h"
#include "ui_dlgispindeleinstellung.h"

extern Settings* gSettings;
extern Ispindel* gIspindel;

DlgIspindeleinstellung::DlgIspindeleinstellung(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dlgispindeleinstellung)
{
    ui->setupUi(this);

    ui->tab_2->setEnabled(false);

    gSettings->beginGroup("iSpindel");
    ui->lineEdit_OdbcTreiber->setText(gSettings->value("Driver", QVariant("MySQL ODBC 8.0 Unicode Driver")).toString());
    ui->lineEdit_IpDatabase->setText(gSettings->value("Server", QVariant("192.168.178.45")).toString());
    ui->lineEdit_NameDatabase->setText(gSettings->value("Database", QVariant("iSpindle")).toString());
    ui->lineEdit_DbUsername->setText(gSettings->value("Username", QVariant("iSpindle")).toString());
    ui->lineEdit_DbPassword->setText(gSettings->value("Password", QVariant("reno17")).toString());
    gSettings->endGroup();

    if(gIspindel == nullptr) {
        gIspindel = new Ispindel();
        setDbParameter();
    }

    if(gIspindel->isDatabaseOpen()) {
        on_btnTestConnection_clicked();
    }
    else {
        ui->textBrowser->setVisible(false);
    }

    ui->lineEditX_0->setValidator(new QDoubleValidator());
    ui->lineEditX_1->setValidator(new QDoubleValidator());
    ui->lineEditX_2->setValidator(new QDoubleValidator());
}

DlgIspindeleinstellung::~DlgIspindeleinstellung()
{
    delete ui;
}

void DlgIspindeleinstellung::databaseIsOpen()
{
    ui->comboBoxChooseSpindel->clear();
    QStringList tmpStrLst = gIspindel->getVerfuegbareSpindeln();
    for(QString tmp : tmpStrLst) {
        if(!tmp.isEmpty())
            ui->comboBoxChooseSpindel->addItem(tmp);
    }

    ui->tab_2->setEnabled(true);
}

void DlgIspindeleinstellung::on_btnSaveClose_clicked()
{
    qDebug() << QString("%1 called at %2").arg(Q_FUNC_INFO).arg(QTime::currentTime().toString("hh:mm:ss:zzz"));
    gIspindel->setDbTableData(ui->comboBox_TabelleMessdaten->currentText());
    gIspindel->setDbTableCalibration(ui->comboBox_TabelleKalibrierung->currentText());
    gIspindel->connectDatabaseIspindel();
    gIspindel->saveSettings();
    this->close();
}

void DlgIspindeleinstellung::on_btnTestConnection_clicked()
{
    qDebug() << QString("%1 called at %2").arg(Q_FUNC_INFO).arg(QTime::currentTime().toString("hh:mm:ss:zzz"));
    setDbParameter();

    QString msg;

    if(gIspindel->connectDatabaseIspindel()) {
        // true
        msg = "<font color=\"green\">Die Verbindung mit der Datenbank war erfolgreich.</font><br>";
        msg.append("Es wurden folgende Tabellen in der Datenbank gefunden: <br>");
        setUiDatabaseElementsEnable(false);

        QStringList RecordSet = gIspindel->getTablenames();

        foreach(QString tmpMsg, RecordSet)
        {
            qDebug() << tmpMsg;
            msg.append(QString("- %1<br>").arg(tmpMsg));
        }

        // set UI
        ui->comboBox_TabelleMessdaten->addItems(RecordSet);
        ui->comboBox_TabelleMessdaten->setCurrentIndex(
                    ui->comboBox_TabelleMessdaten->findText(gIspindel->getDbTableData()));

        ui->comboBox_TabelleKalibrierung->addItems(RecordSet);
        ui->comboBox_TabelleKalibrierung->setCurrentIndex(
                    ui->comboBox_TabelleKalibrierung->findText(gIspindel->getDbTableCalibration()));
        this->databaseIsOpen();
    }
    else {
        setUiDatabaseElementsEnable(true);
        QSqlError error = gIspindel->getLastSqlError();
        msg = "<font color=\"red\">Die Verbindung mit der Datenbank war nicht erfolgreich. <br>";
        msg.append(QString("Folgender Fehler ist aufgetreten: </font><br> \"%1\"").arg(error.text()));

    }

    ui->textBrowser->setVisible(true);
    ui->textBrowser->setHtml(msg);
}

// Kalibrierung
void DlgIspindeleinstellung::on_comboBoxChooseSpindel_currentIndexChanged(const QString &text)
{
    QStringList listCalibrationData = gIspindel->getCalibrationData(text);

    ui->lineEditX_0->setText(listCalibrationData.at(0));
    ui->lineEditX_1->setText(listCalibrationData.at(1));
    ui->lineEditX_2->setText(listCalibrationData.at(2));
}

void DlgIspindeleinstellung::on_butSaveParameter_clicked()
{
    QStringList param;
    param.append(ui->lineEditX_0->text());
    param.append(ui->lineEditX_1->text());
    param.append(ui->lineEditX_2->text());
    gIspindel->setCalibrationData(ui->comboBoxChooseSpindel->currentText(),
                                  param);
}

void DlgIspindeleinstellung::on_butSetResetflag_clicked()
{
    gIspindel->setResetFlag(ui->comboBoxChooseSpindel->currentText(),
                            ui->spinBoxReceipeNr->value());
}

void DlgIspindeleinstellung::on_lineEditX_0_editingFinished()
{
    QString text = ui->lineEditX_0->text();
    text.replace(",", ".");
    ui->lineEditX_0->setText(text);
}

void DlgIspindeleinstellung::on_lineEditX_1_editingFinished()
{
    QString text = ui->lineEditX_1->text();
    text.replace(",", ".");
    ui->lineEditX_1->setText(text);
}

void DlgIspindeleinstellung::on_lineEditX_2_editingFinished()
{
    QString text = ui->lineEditX_2->text();
    text.replace(",", ".");
    ui->lineEditX_2->setText(text);
}

// private functions
void DlgIspindeleinstellung::setDbParameter() const
{
    gIspindel->setDbDriver(ui->lineEdit_OdbcTreiber->text());
    gIspindel->setDbServer(ui->lineEdit_IpDatabase->text());
    gIspindel->setDbDatabase(ui->lineEdit_NameDatabase->text());
    gIspindel->setDbUsername(ui->lineEdit_DbUsername->text());
    gIspindel->setDbPwd(ui->lineEdit_DbPassword->text());
}

void DlgIspindeleinstellung::setUiDatabaseElementsEnable(bool setEnable) const
{
    ui->btnTestConnection->setEnabled(setEnable);

    ui->lineEdit_OdbcTreiber->setEnabled(setEnable);
    ui->lineEdit_IpDatabase->setEnabled(setEnable);
    ui->lineEdit_NameDatabase->setEnabled(setEnable);
    ui->lineEdit_DbUsername->setEnabled(setEnable);
    ui->lineEdit_DbPassword->setEnabled(setEnable);
}
