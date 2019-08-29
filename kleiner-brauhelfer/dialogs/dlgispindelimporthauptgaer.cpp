#include "dlgispindelimporthauptgaer.h"
#include "ui_dlgispindelimporthauptgaer.h"

DlgIspindelImportHauptgaer::DlgIspindelImportHauptgaer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgIspindelImportHauptgaer),
    mSqlModelView(nullptr)
{
    ui->setupUi(this);

    mIspindel = new Ispindel();
    if(!mIspindel->connectDatabaseIspindel(true))
        QMessageBox::warning(this,
                             "Datenbank konfigurieren",
                             "Hast du die Datenbank in den Einstellungen konfiguriert?\n"
                             "Wenn nicht, dann tue dies bitte jetzt!");


    QStringList tmpStrLst = mIspindel->getVerfuegbareSpindeln();
    qDebug() << tmpStrLst;
    for(QString tmp : tmpStrLst)
    {
        if(!tmp.isEmpty())
            ui->comboBox_chooseIspindel->addItem(tmp);
    }

    ui->tabViewShowData->setVisible(false);
}

DlgIspindelImportHauptgaer::~DlgIspindelImportHauptgaer()
{
    delete mIspindel;
    delete ui;
}

int DlgIspindelImportHauptgaer::getMultiplikatorForDataImport()
{
    return ui->spinBox->value();
}

//Private Slots UI
void DlgIspindelImportHauptgaer::on_comboBox_chooseIspindel_currentIndexChanged(const QString &text)
{
    QString tmp;
    int CountDataset = mIspindel->getAnzahlDatensaetze(text);
    if(CountDataset == 1)
        tmp = QString("Es wurde %1 Datensatz für %2 gefunden.").arg(CountDataset).arg(text);
    else {
        tmp = QString("Es wurden %1 Datensätze für %2 gefunden.").arg(CountDataset).arg(text);
    }
    ui->label_CountDataset->setText(tmp);

    ui->listWidgetTimestampReset->clear();
    ResetFlags.clear();

    ResetFlags = mIspindel->getResetFlags(text);

    QListWidgetItem *widgetItemFirst = new QListWidgetItem("aktuell", ui->listWidgetTimestampReset);
    widgetItemFirst->setData(Qt::UserRole, -1);
    ui->listWidgetTimestampReset->addItem(widgetItemFirst);

    for(int i = 0; i < ResetFlags.size(); i++)
    {
        QDateTime tmpDateTimeFirst = ResetFlags.at(i).first;

        // darstellung
        QString itemWidget = QString("[%1 %2]")
                .arg(tmpDateTimeFirst.toString("dd.MM.yyyy"))
                .arg(tmpDateTimeFirst.toString("hh:mm"));

        if(!ResetFlags.at(i).second.at(0).isEmpty()) {
            itemWidget.append(QString(" - %1").arg(ResetFlags.at(i).second.at(0)));
        }
        if(!ResetFlags.at(i).second.at(1).isEmpty())
            itemWidget.append(QString(" Rezept: %1").arg(ResetFlags.at(i).second.at(1)));

        QListWidgetItem *widgetItem = new QListWidgetItem(itemWidget, ui->listWidgetTimestampReset);
        widgetItem->setData(Qt::UserRole, i);
        ui->listWidgetTimestampReset->addItem(widgetItem);
    }
}

// Es müssen mindestens zwei Datensätze gewählt werden!
void DlgIspindelImportHauptgaer::on_listWidgetTimestampReset_itemSelectionChanged()
{
    qDebug() << "List item Changed!";
    bool statusButton = false;
    if(ui->listWidgetTimestampReset->selectedItems().count() >= 2)
        statusButton = true;

    ui->butImport->setEnabled(statusButton);
    ui->butShowData->setEnabled(statusButton);
}


// Import der Daten
void DlgIspindelImportHauptgaer::on_butImport_clicked()
{
    QDateTime timestampFirst, timestampLast;
    getChooseDateTime(&timestampFirst, &timestampLast);
    qDebug() << QString("Zeitstempel first: ") << timestampFirst << "Last: " << timestampLast;

    // Hole die Daten aus der Datenbank
    mValuePlatoDatabase = mIspindel->getPlatoBetweenTimestamps(ui->comboBox_chooseIspindel->currentText(),
                                         timestampFirst,
                                         timestampLast);

    //prüfe die CheckBoxen
    if(ui->checkBoxDeleteDB->isChecked())
    {
        // lösche die Datenbank im Anschluss
        mIspindel->deleteDatabaseBetweenTimestamps(ui->comboBox_chooseIspindel->currentText(),
                                                   timestampFirst,
                                                   timestampLast);
    }

    if(ui->checkBoxSetResetflag->isChecked())
    {
        //setze ResetFlag
        mIspindel->setResetFlag(ui->comboBox_chooseIspindel->currentText(),
                                0);
    }
    this->accept();
}

void DlgIspindelImportHauptgaer::on_butShowData_clicked()
{
    ui->tabViewShowData->setVisible(true);

    QDateTime timestampFirst, timestampLast;
    getChooseDateTime(&timestampFirst, &timestampLast);
    qDebug() << QString("Zeitstempel first: ") << timestampFirst << "Last: " << timestampLast;

    mSqlModelView = mIspindel->getPlatoBetweenTimestampsAsModel(ui->comboBox_chooseIspindel->currentText(),
                                                                        timestampFirst,
                                                                        timestampLast);

    ui->tabViewShowData->setModel(mSqlModelView);
    ui->tabViewShowData->show();
}

void DlgIspindelImportHauptgaer::getChooseDateTime(QDateTime *First, QDateTime *Last)
{
    QList<QListWidgetItem*> selectedItems = ui->listWidgetTimestampReset->selectedItems();

    QList<int> selectedIDs;
    for (int i = 0; i < selectedItems.count(); i++)
    {
        selectedIDs << selectedItems.at(i)->data(Qt::UserRole).toInt();
    }
    qSort(selectedIDs);

    // hole den markierten Zwischenraum
    if(selectedIDs.first() == -1)
        *First = QDateTime::currentDateTime();
    else
        *Last = ResetFlags.at(selectedIDs.first()).first;

    *Last = ResetFlags.at(selectedIDs.last()).first;
}

QList<QVariantMap> DlgIspindelImportHauptgaer::getValuePlatoDatabase() const
{
    return mValuePlatoDatabase;
}
