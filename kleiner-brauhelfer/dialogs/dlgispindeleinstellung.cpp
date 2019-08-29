#include "dlgispindeleinstellung.h"
#include "ui_dlgispindeleinstellung.h"

extern Settings* gSettings;

DlgIspindeleinstellung::DlgIspindeleinstellung(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dlgispindeleinstellung),
    mIspindel(nullptr)
{
    ui->setupUi(this);

    ui->tab_2->setEnabled(false);

    gSettings->beginGroup("iSpindel");
    ui->lineEdit_OdbcTreiber->setText(gSettings->value("Driver", QVariant("MySQL ODBC 8.0 Unicode Driver")).toString());
    ui->lineEdit_IpDatabase->setText(gSettings->value("Server", QVariant("127.0.0.1")).toString());
    ui->lineEdit_NameDatabase->setText(gSettings->value("Database", QVariant("iSpindle")).toString());
    ui->lineEdit_DbUsername->setText(gSettings->value("Username", QVariant("iSpindle")).toString());
    ui->lineEdit_DbPassword->setText(gSettings->value("Password", QVariant("password")).toString());
    gSettings->endGroup();

    if(mIspindel == nullptr) {
        mIspindel = new Ispindel();
        setSpindelParameterFromUi();
    }

    ui->lineEditX_0->setValidator(new QDoubleValidator());
    ui->lineEditX_1->setValidator(new QDoubleValidator());
    ui->lineEditX_2->setValidator(new QDoubleValidator());
}

DlgIspindeleinstellung::~DlgIspindeleinstellung()
{
    delete mIspindel;
    delete ui;
}

void DlgIspindeleinstellung::databaseIsOpen()
{
    ui->comboBoxChooseSpindel->clear();
    QStringList tmpStrLst = mIspindel->getVerfuegbareSpindeln();
    for(QString tmp : tmpStrLst) {
        if(!tmp.isEmpty())
            ui->comboBoxChooseSpindel->addItem(tmp);
    }

    ui->tab_2->setEnabled(true);
}

void DlgIspindeleinstellung::on_btnSaveClose_clicked()
{
    qDebug() << QString("%1 called at %2").arg(Q_FUNC_INFO).arg(QTime::currentTime().toString("hh:mm:ss:zzz"));
    setSpindelParameterFromUi();
    mIspindel->setDbTableData(ui->comboBox_TabelleMessdaten->currentText());
    mIspindel->setDbTableCalibration(ui->comboBox_TabelleKalibrierung->currentText());
    mIspindel->saveSettings();
    this->close();
}

void DlgIspindeleinstellung::on_btnTestConnection_clicked()
{
    qDebug() << QString("%1 called at %2").arg(Q_FUNC_INFO).arg(QTime::currentTime().toString("hh:mm:ss:zzz"));
    setSpindelParameterFromUi();

    QString msg;

    if(mIspindel->connectDatabaseIspindel()) {
        // true
        msg = "<font color=\"green\">Die Verbindung mit der Datenbank war erfolgreich.</font><br>";
        msg.append("Es wurden folgende Tabellen in der Datenbank gefunden: <br>");
        setUiDatabaseElementsEnable(false);

        QStringList RecordSet = mIspindel->getTablenames();

        foreach(QString tmpMsg, RecordSet)
        {
            qDebug() << tmpMsg;
            msg.append(QString("- %1<br>").arg(tmpMsg));
        }

        // set UI
        ui->comboBox_TabelleMessdaten->addItems(RecordSet);
        ui->comboBox_TabelleMessdaten->setCurrentIndex(
                    ui->comboBox_TabelleMessdaten->findText(mIspindel->getDbTableData()));

        ui->comboBox_TabelleKalibrierung->addItems(RecordSet);
        ui->comboBox_TabelleKalibrierung->setCurrentIndex(
                    ui->comboBox_TabelleKalibrierung->findText(mIspindel->getDbTableCalibration()));
        this->databaseIsOpen();

    }
    else {
        setUiDatabaseElementsEnable(true);
        QSqlError error = mIspindel->getLastSqlError();
        msg = "<font color=\"red\">Die Verbindung mit der Datenbank war nicht erfolgreich. <br>";
        msg.append(QString("Folgender Fehler ist aufgetreten: </font><br> \"%1\"").arg(error.text()));

    }

    ui->textBrowser->setVisible(true);
    ui->textBrowser->setHtml(msg);
}

// Kalibrierung
void DlgIspindeleinstellung::on_comboBoxChooseSpindel_currentIndexChanged(const QString &text)
{
    QStringList listCalibrationData = mIspindel->getCalibrationData(text);

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
    mIspindel->setCalibrationData(ui->comboBoxChooseSpindel->currentText(),
                                  param);
}

void DlgIspindeleinstellung::on_butSetResetflag_clicked()
{
    mIspindel->setResetFlag(ui->comboBoxChooseSpindel->currentText(),
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
void DlgIspindeleinstellung::setSpindelParameterFromUi() const
{
    mIspindel->setDbDriver(ui->lineEdit_OdbcTreiber->text());
    mIspindel->setDbServer(ui->lineEdit_IpDatabase->text());
    mIspindel->setDbDatabase(ui->lineEdit_NameDatabase->text());
    mIspindel->setDbUsername(ui->lineEdit_DbUsername->text());
    mIspindel->setDbPwd(ui->lineEdit_DbPassword->text());
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
