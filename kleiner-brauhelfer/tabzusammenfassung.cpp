#include "tabzusammenfassung.h"
#include "ui_tabzusammenfassung.h"
#include <QMessageBox>
#include <QFileInfo>
#include <QFileDialog>
#include <QDesktopServices>
#include "brauhelfer.h"
#include "settings.h"
#include "widgets/wdganhang.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

TabZusammenfassung::TabZusammenfassung(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabZusammenfassung),
    mTempCssFile(QDir::tempPath() + "/" + QCoreApplication::applicationName() + QLatin1String(".XXXXXX.css"))
{
    ui->setupUi(this);

    ui->tbTemplate->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    ui->tbTemplate->setTabStopDistance(2 * QFontMetrics(ui->tbTemplate->font()).width(' '));
    ui->btnSaveTemplate->setPalette(gSettings->paletteErrorButton);
    ui->treeViewTemplateTags->setColumnWidth(0, 150);
    ui->treeViewTemplateTags->setColumnWidth(1, 150);

    mHtmlHightLighter = new HtmlHighLighter(ui->tbTemplate->document());

    connect(bh, SIGNAL(modified()), this, SLOT(updateAll()));
    connect(bh, SIGNAL(discarded()), this, SLOT(updateAll()));
    connect(bh->sud(), SIGNAL(loadedChanged()), this, SLOT(updateAll()));

    on_cbEditMode_clicked(ui->cbEditMode->isChecked());
}

TabZusammenfassung::~TabZusammenfassung()
{
    delete ui;
}

void TabZusammenfassung::saveSettings()
{
}

void TabZusammenfassung::restoreView()
{
}

void TabZusammenfassung::updateAll()
{
    if (bh->sud()->getBierWurdeGebraut())
    {
        ui->cbTemplateAuswahl->setItemText(0, "zusammenfassung.html");
        ui->webview->setTemplateFile(gSettings->dataDir() + "zusammenfassung.html");
    }
    else
    {
        ui->cbTemplateAuswahl->setItemText(0, "spickzettel.html");
        ui->webview->setTemplateFile(gSettings->dataDir() + "spickzettel.html");
    }
    updateTemplateTags();
    updateWebView();
}

void TabZusammenfassung::on_btnToPdf_clicked()
{
    gSettings->beginGroup("General");

    QString path = gSettings->value("exportPath", QDir::homePath()).toString();

    QString fileName = bh->sud()->getSudname();
    QString filePath = QFileDialog::getSaveFileName(this, tr("PDF speichern unter"),
                                     path + "/" + fileName +  ".pdf", "PDF (*.pdf)");
    if (!filePath.isEmpty())
    {
        gSettings->setValue("exportPath", QFileInfo(filePath).absolutePath());
        ui->webview->printToPdf(filePath);
        QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
    }

    gSettings->endGroup();
}

bool TabZusammenfassung::checkSaveTemplate()
{
    if (ui->btnSaveTemplate->isVisible())
    {
        int ret = QMessageBox::question(this, tr("Änderungen speichern?"),
                                        tr("Sollen die Änderungen gespeichert werden?"),
                                        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if (ret == QMessageBox::Yes)
            on_btnSaveTemplate_clicked();
        else if (ret == QMessageBox::Cancel)
            return true;
    }
    return false;
}

void TabZusammenfassung::on_cbEditMode_clicked(bool checked)
{
    if (checkSaveTemplate())
    {
        ui->cbEditMode->setChecked(true);
        return;
    }

    ui->tbTemplate->setVisible(checked);
    ui->treeViewTemplateTags->setVisible(checked);
    ui->btnRestoreTemplate->setVisible(checked);
    ui->cbTemplateAuswahl->setVisible(checked);
    ui->btnSaveTemplate->setVisible(false);
    ui->splitterEdit->setHandleWidth(checked ? 5 : 0);

    if (checked)
    {
        QFile file(gSettings->dataDir() + ui->cbTemplateAuswahl->currentText());
        ui->btnSaveTemplate->setProperty("file", file.fileName());
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            ui->tbTemplate->setPlainText(file.readAll());
            file.close();
        }
    }

    updateWebView();
}

void TabZusammenfassung::on_cbTemplateAuswahl_currentIndexChanged(int)
{
    on_cbEditMode_clicked(ui->cbEditMode->isChecked());
}

void TabZusammenfassung::on_tbTemplate_textChanged()
{
    if (ui->tbTemplate->hasFocus())
    {
        updateWebView();
        ui->btnSaveTemplate->setVisible(true);
    }
}

void TabZusammenfassung::on_btnSaveTemplate_clicked()
{
    QFile file(ui->btnSaveTemplate->property("file").toString());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    file.write(ui->tbTemplate->toPlainText().toUtf8());
    file.close();
    ui->btnSaveTemplate->setVisible(false);
}

void TabZusammenfassung::on_btnRestoreTemplate_clicked()
{
    int ret = QMessageBox::question(this, tr("Template wiederherstellen?"),
                                    tr("Soll das Standardtemplate wiederhergestellt werden?"));
    if (ret == QMessageBox::Yes)
    {
        QFile file(gSettings->dataDir() + ui->cbTemplateAuswahl->currentText());
        QFile file2(":/data/" + ui->cbTemplateAuswahl->currentText());
        file.remove();
        if (file2.copy(file.fileName()))
            file.setPermissions(QFile::ReadOwner | QFile::WriteOwner);
        on_cbEditMode_clicked(ui->cbEditMode->isChecked());
    }
}

void TabZusammenfassung::updateTemplateTags()
{
    double f;
    QString s;
    QLocale locale;

    mTemplateTags.clear();
    WebView::erstelleTagListe(mTemplateTags, bh->sud()->row());

    if (bh->sud()->getBierWurdeGebraut())
    {
        int bewertung = bh->sud()->getBewertungMax();
        if (bewertung > 0)
        {
            if (bewertung > Bewertung_MaxSterne)
                bewertung = Bewertung_MaxSterne;
            s = "";
            for (int i = 0; i < bewertung; i++)
                s += "<img class='star' style='padding:0px;margin:0px;' width='24' border=0>";
            for (int i = bewertung; i < Bewertung_MaxSterne; i++)
                s += "<img class='star_grey' style='padding:0px;margin:0px;' width='24' border=0>";
            mTemplateTags["Sterne"] = s;
        }

    }
    else
    {
    }

    QVariantMap ctxZutaten;

    // Malz
    s = "<table><tbody>";
    for (int i=0; i < bh->sud()->modelMalzschuettung()->rowCount(); i++)
    {
        s += "<tr>";
        s += "<td>" + bh->sud()->modelMalzschuettung()->data(i, "Name").toString() + "</td>";
        s += "<td class='value' align='right'>" + locale.toString(bh->sud()->modelMalzschuettung()->data(i, "erg_Menge").toDouble(), 'f', 2) + "</td>";
        s += "<td>" + tr("kg") + "</td>";
        s += "<td class='value' align='right'>" + locale.toString(bh->sud()->modelMalzschuettung()->data(i, "Prozent").toDouble(), 'f', 1) + "</td>";
        s += "<td>" + tr("%") + "</td>";
        s += "</tr>";
    }
    s += "<tr>";
    s += "<td style='font-weight:bold;'>" + tr("Gesamt") + "</td>";
    s += "<td class='value' style='font-weight:bold;' align='right'>" + locale.toString(bh->sud()->geterg_S_Gesammt(), 'f', 2) + "</td>";
    s += "<td>" + tr("kg") + "</td>";
    s += "</tr>";
    s += "</tbody></table>";
    ctxZutaten["Malz"] = s;

    // Hopfen
    s = "<table><tbody>";
    for (int i=0; i < bh->sud()->modelHopfengaben()->rowCount(); i++)
    {
        s += "<tr>";
        QString s2 = QString("%1 (%2%)")
              .arg(bh->sud()->modelHopfengaben()->data(i, "Name").toString())
              .arg(locale.toString(bh->sud()->modelHopfengaben()->data(i, "Alpha").toDouble(), 'f', 1));
        if (bh->sud()->modelHopfengaben()->data(i, "Vorderwuerze").toBool())
            s += "<td>" + tr("VWH ") + s2 + "</td>";
        else
            s += "<td>" + s2 + "</td>";
        s += "<td class='value' align='right'>" + QString::number(bh->sud()->modelHopfengaben()->data(i, "erg_Menge").toInt()) + "</td>";
        s += "<td>" + tr("g") + "</td>";
        s += "<td class='value' align='right'>" + QString::number(bh->sud()->modelHopfengaben()->data(i, "Zeit").toInt()) + "</td>";
        s += "<td>" + tr("min") + "</td>";
        s += "</tr>";
    }
    for (int i=0; i < bh->sud()->modelWeitereZutatenGaben()->rowCount(); i++)
    {
      if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Typ").toInt() == EWZ_Typ_Hopfen)
      {
        s += "<tr>";
        s += "<td>" + bh->sud()->modelWeitereZutatenGaben()->data(i, "Name").toString() + "</td>";
        s += "<td class='value' align='right'>" + QString::number(bh->sud()->modelWeitereZutatenGaben()->data(i, "erg_Menge").toInt()) + "</td>";
        s += "<td>" + tr("g") + "</td>";
        if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Zeitpunkt").toInt() == EWZ_Zeitpunkt_Gaerung)
          s += "<td class='value' align='right' colspan='2'>" + tr("Gärung") + "</td>";
        else
          s += "<td class='value' align='right' colspan='2'>" + tr("Anstellen") + "</td>";
        s += "</tr>";
        if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Zugabestatus").toInt() > EWZ_Zugabestatus_nichtZugegeben) {
          s += "<tr>";
          s += "<td colspan='5' class='kommentar'>";
          s += "<p>Zugegeben am "+ bh->sud()->modelWeitereZutatenGaben()->data(i, "Zeitpunkt_von").toDate().toString("dd.MM.yyyy")+"</p>";
          if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Entnahmeindex").toInt() == EWZ_Entnahmeindex_MitEntnahme) {
            if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Zugabestatus").toInt() == EWZ_Zugabestatus_Entnommen) {
              s += "<p>"+ tr("Entnommen am ")+ bh->sud()->modelWeitereZutatenGaben()->data(i, "Zeitpunkt_bis").toDate().toString("dd.MM.yyyy")
                  + " (" + tr("Tage: ") +
                  QString::number(bh->sud()->modelWeitereZutatenGaben()->data(i, "Zugabedauer").toInt()/1440)+")</p>";
            }
          }
          s += "</td>";
          s += "</tr>";
        }
        if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Bemerkung").toString() != "")
        {
          s += "<tr>";
          s += "<td colspan='5' class='kommentar'>" + bh->sud()->modelWeitereZutatenGaben()->data(i, "Bemerkung").toString().replace("\n","<br>") + "</td>";
          s += "</tr>";
        }
      }
    }
    s += "</tbody></table>";
    ctxZutaten["Hopfen"] = s;

    // Wasser
    f = bh->sud()->getRestalkalitaetFaktor();
    s = "<table><tbody>";
    s += "<tr>";
    s += "<td>" + tr("Hauptguss") + "</td>";
    s += "<td class='value' align='right'>" + locale.toString(bh->sud()->geterg_WHauptguss(), 'f', 1) + "</td>";
    s += "<td>" + tr("Liter") + "</td>";
    s += "</tr>";
    s += "<tr>";
    s += "<td>" + tr("Milchsäure (80%)") + "</td>";
    s += "<td class='value' align='right'>" + locale.toString(bh->sud()->geterg_WHauptguss() * f, 'f', 1) + "</td>";
    s += "<td>" + tr("ml") + "</td>";
    s += "</tr>";
    s += "<tr>";
    s += "<td>" + tr("Nachguss") + "</td>";
    s += "<td class='value' align='right'>" + locale.toString(bh->sud()->geterg_WNachguss(), 'f', 1) + "</td>";
    s += "<td>" + tr("Liter") + "</td>";
    s += "</tr>";
    s += "<tr>";
    s += "<td>" + tr("Milchsäure (80%)") + "</td>";
    s += "<td class='value' align='right'>" + locale.toString(bh->sud()->geterg_WNachguss() * f, 'f', 1) + "</td>";
    s += "<td>" + tr("ml") + "</td>";
    s += "</tr>";
    s += "<tr>";
    s += "<td>" + tr("Gesamt") + "</td>";
    s += "<td class='value' style='font-weight:bold;' align='right'>" + locale.toString(bh->sud()->geterg_S_Gesammt(), 'f', 1) + "</td>";
    s += "<td>" + tr("Liter") + "</td>";
    s += "</tr>";
    s += "</tbody></table>";
    ctxZutaten["Wasser"] = s;

    // Hefe
    QString HefeName = bh->sud()->getAuswahlHefe();
    s = "<p>" + HefeName + "</p>";
    QString sEinheiten;
    sEinheiten = tr("Anzahl Einheiten:") + " <span class='value'>" + QString::number(bh->sud()->getHefeAnzahlEinheiten()) +"</span>";
    //Verpackungsgrösse aus den Rohstoffdaten auslesen
    if (HefeName != "")
    {
      for (int i=0; i < bh->modelHefe()->rowCount(); i++)
      {
        if (bh->modelHefe()->data(i, "Beschreibung").toString() == HefeName){
          sEinheiten += tr(" zu ") + bh->modelHefe()->data(i, "Verpackungsmenge").toString();
          break;
        }
      }
    }
    s += "<p>" + sEinheiten +"</p>";
    ctxZutaten["Hefe"] = s;

    //Honig
    bool HonigVorhanden = false;
    for (int i=0; i < bh->sud()->modelWeitereZutatenGaben()->rowCount(); i++){
      if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Typ").toInt() == EWZ_Typ_Honig){
        HonigVorhanden = true;
      }
    }
    if (HonigVorhanden){
      s = "<table><tbody>";
      for (int i=0; i < bh->sud()->modelWeitereZutatenGaben()->rowCount(); i++){
        if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Typ").toInt() == EWZ_Typ_Honig){
          s += "<tr>";
          s += "<td>";
          s += "<p>" + bh->sud()->modelWeitereZutatenGaben()->data(i, "Name").toString() + "</p>";
          s += "</td>";
          s += "<td align='right'>";
          if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Einheit").toInt() == EWZ_Einheit_Kg)
            s += "<p class='value'>" + locale.toString(bh->sud()->modelWeitereZutatenGaben()->data(i, "erg_Menge").toDouble() / 1000, 'f', 2) + "</p>";
          else
            s += "<p class='value'>" + QString::number(bh->sud()->modelWeitereZutatenGaben()->data(i, "erg_Menge").toInt()) + "</p>";
          s += "</td>";
          s += "<td>";
          if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Einheit").toInt() == EWZ_Einheit_Kg)
            s += "<p>" + tr("kg") + "</p>";
          else
            s += "<p>" + tr("g") + "</p>";
          s += "</td>";
          s += "<td align='right' colspan='2'>";
          if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Zeitpunkt").toInt() == EWZ_Zeitpunkt_Gaerung)
            s += "<p class='value'>" + tr("Gärung") + "</p>";
          else if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Zeitpunkt").toInt() == EWZ_Zeitpunkt_Kochen)
            s += "<p class='value'>" + tr("Kochbeginn") + "</p>";
          else if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Zeitpunkt").toInt() == EWZ_Zeitpunkt_Maischen)
            s += "<p class='value'>" + tr("Maischen") + "</p>";
          s += "</td>";
          s += "</tr>";
          if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Bemerkung").toString() != ""){
            s += "<tr>";
            s += "<td colspan='4' class='kommentar'>" + bh->sud()->modelWeitereZutatenGaben()->data(i, "Bemerkung").toString().replace("\n","<br>") + "</td>";
            s += "</tr>";
          }
        }
      }
      s += "</table>";
    }
    else {
      s = "";
    }
    ctxZutaten["Honig"] = s;

    //Zucker
    bool ZuckerVorhanden = false;
    for (int i=0; i < bh->sud()->modelWeitereZutatenGaben()->rowCount(); i++){
      if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Typ").toInt() == EWZ_Typ_Zucker){
        ZuckerVorhanden = true;
      }
    }
    if (ZuckerVorhanden){
      s = "<table><tbody>";
      for (int i=0; i < bh->sud()->modelWeitereZutatenGaben()->rowCount(); i++){
        if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Typ").toInt() == EWZ_Typ_Zucker){
          s += "<tr>";
          s += "<td>";
          s += "<p>" + bh->sud()->modelWeitereZutatenGaben()->data(i, "Name").toString() + "</p>";
          s += "</td>";
          s += "<td align='right'>";
          if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Einheit").toInt() == EWZ_Einheit_Kg)
            s += "<p class='value'>" + locale.toString(bh->sud()->modelWeitereZutatenGaben()->data(i, "erg_Menge").toDouble() / 1000, 'f', 2) + "</p>";
          else
            s += "<p class='value'>" + QString::number(bh->sud()->modelWeitereZutatenGaben()->data(i, "erg_Menge").toInt()) + "</p>";
          s += "</td>";
          s += "<td>";
          if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Einheit").toInt() == EWZ_Einheit_Kg)
            s += "<p>" + tr("kg") + "</p>";
          else
            s += "<p>" + tr("g") + "</p>";
          s += "</td>";
          s += "<td align='right' colspan='2'>";
          if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Zeitpunkt").toInt() == EWZ_Zeitpunkt_Gaerung)
            s += "<p class='value'>" + tr("Gärung") + "</p>";
          else if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Zeitpunkt").toInt() == EWZ_Zeitpunkt_Kochen)
            s += "<p class='value'>" + tr("Kochbeginn") + "</p>";
          else if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Zeitpunkt").toInt() == EWZ_Zeitpunkt_Maischen)
            s += "<p class='value'>" + tr("Maischen") + "</p>";
          s += "</td>";
          s += "</tr>";
          if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Bemerkung").toString() != ""){
            s += "<tr>";
            s += "<td colspan='4' class='kommentar'>" + bh->sud()->modelWeitereZutatenGaben()->data(i, "Bemerkung").toString().replace("\n","<br>") + "</td>";
            s += "</tr>";
          }
        }
      }
      s += "</table>";
    }
    else {
      s = "";
    }
    ctxZutaten["Zucker"] = s;

    //Gewürz
    bool GewuerzVorhanden = false;
    for (int i=0; i < bh->sud()->modelWeitereZutatenGaben()->rowCount(); i++){
      if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Typ").toInt() == EWZ_Typ_Gewuerz){
        GewuerzVorhanden = true;
      }
    }
    if (GewuerzVorhanden){
      s = "<table><tbody>";
      for (int i=0; i < bh->sud()->modelWeitereZutatenGaben()->rowCount(); i++){
        if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Typ").toInt() == EWZ_Typ_Gewuerz){
          s += "<tr>";
          s += "<td>";
          s += "<p>" + bh->sud()->modelWeitereZutatenGaben()->data(i, "Name").toString() + "</p>";
          s += "</td>";
          s += "<td align='right'>";
          if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Einheit").toInt() == EWZ_Einheit_Kg)
            s += "<p class='value'>" + locale.toString(bh->sud()->modelWeitereZutatenGaben()->data(i, "erg_Menge").toDouble() / 1000, 'f', 2) + "</p>";
          else
            s += "<p class='value'>" + QString::number(bh->sud()->modelWeitereZutatenGaben()->data(i, "erg_Menge").toInt()) + "</p>";
          s += "</td>";
          s += "<td>";
          if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Einheit").toInt() == EWZ_Einheit_Kg)
            s += "<p>" + tr("kg") + "</p>";
          else
            s += "<p>" + tr("g") + "</p>";
          s += "</td>";
          s += "<td align='right' colspan='2'>";
          if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Zeitpunkt").toInt() == EWZ_Zeitpunkt_Gaerung)
            s += "<p class='value'>" + tr("Gärung") + "</p>";
          else if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Zeitpunkt").toInt() == EWZ_Zeitpunkt_Kochen)
            s += "<p class='value'>" + tr("Kochen") + " (" + QString::number(bh->sud()->modelWeitereZutatenGaben()->data(i, "Zugabedauer").toInt()) + "min) </p>";
          else if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Zeitpunkt").toInt() == EWZ_Zeitpunkt_Maischen)
            s += "<p class='value'>" + tr("Maischen") + "</p>";
          s += "</td>";
          s += "</tr>";
          if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Bemerkung").toString() != ""){
            s += "<tr>";
            s += "<td colspan='4' class='kommentar'>" + bh->sud()->modelWeitereZutatenGaben()->data(i, "Bemerkung").toString().replace("\n","<br>") + "</td>";
            s += "</tr>";
          }
        }
      }
      s += "</table>";
    }
    else {
      s = "";
    }
    ctxZutaten["Gewuerz"] = s;

    //Frucht
    bool FruchtVorhanden = false;
    for (int i=0; i < bh->sud()->modelWeitereZutatenGaben()->rowCount(); i++){
      if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Typ").toInt() == EWZ_Typ_Frucht){
        FruchtVorhanden = true;
      }
    }
    if (FruchtVorhanden){
      s = "<table><tbody>";
      for (int i=0; i < bh->sud()->modelWeitereZutatenGaben()->rowCount(); i++){
        if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Typ").toInt() == EWZ_Typ_Frucht){
          s += "<tr>";
          s += "<td>";
          s += "<p>" + bh->sud()->modelWeitereZutatenGaben()->data(i, "Name").toString() + "</p>";
          s += "</td>";
          s += "<td align='right'>";
          if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Einheit").toInt() == EWZ_Einheit_Kg)
            s += "<p class='value'>" + locale.toString(bh->sud()->modelWeitereZutatenGaben()->data(i, "erg_Menge").toDouble() / 1000, 'f', 2) + "</p>";
          else
            s += "<p class='value'>" + QString::number(bh->sud()->modelWeitereZutatenGaben()->data(i, "erg_Menge").toInt()) + "</p>";
          s += "</td>";
          s += "<td>";
          if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Einheit").toInt() == EWZ_Einheit_Kg)
            s += "<p>" + tr("kg") + "</p>";
          else
            s += "<p>" + tr("g") + "</p>";
          s += "</td>";
          s += "<td align='right' colspan='2'>";
          if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Zeitpunkt").toInt() == EWZ_Zeitpunkt_Gaerung)
            s += "<p class='value'>" + tr("Gärung") + "</p>";
          else if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Zeitpunkt").toInt() == EWZ_Zeitpunkt_Kochen)
            s += "<p class='value'>" + tr("Kochen") + " (" + QString::number(bh->sud()->modelWeitereZutatenGaben()->data(i, "Zugabedauer").toInt()) + "min) </p>";
          else if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Zeitpunkt").toInt() == EWZ_Zeitpunkt_Maischen)
            s += "<p class='value'>" + tr("Maischen") + "</p>";
          s += "</td>";
          s += "</tr>";
          if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Bemerkung").toString() != ""){
            s += "<tr>";
            s += "<td colspan='4' class='kommentar'>" + bh->sud()->modelWeitereZutatenGaben()->data(i, "Bemerkung").toString().replace("\n","<br>") + "</td>";
            s += "</tr>";
          }
        }
      }
      s += "</table>";
    }
    else {
      s = "";
    }
    ctxZutaten["Frucht"] = s;

    //Sonstiges
    bool SonstigesVorhanden = false;
    for (int i=0; i < bh->sud()->modelWeitereZutatenGaben()->rowCount(); i++){
      if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Typ").toInt() == EWZ_Typ_Sonstiges){
        SonstigesVorhanden = true;
      }
    }
    if (SonstigesVorhanden){
      s = "<table><tbody>";
      for (int i=0; i < bh->sud()->modelWeitereZutatenGaben()->rowCount(); i++){
        if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Typ").toInt() == EWZ_Typ_Sonstiges){
          s += "<tr>";
          s += "<td>";
          s += "<p>" + bh->sud()->modelWeitereZutatenGaben()->data(i, "Name").toString() + "</p>";
          s += "</td>";
          s += "<td align='right'>";
          if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Einheit").toInt() == EWZ_Einheit_Kg)
            s += "<p class='value'>" + locale.toString(bh->sud()->modelWeitereZutatenGaben()->data(i, "erg_Menge").toDouble() / 1000, 'f', 2) + "</p>";
          else
            s += "<p class='value'>" + QString::number(bh->sud()->modelWeitereZutatenGaben()->data(i, "erg_Menge").toInt()) + "</p>";
          s += "</td>";
          s += "<td>";
          if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Einheit").toInt() == EWZ_Einheit_Kg)
            s += "<p>" + tr("kg") + "</p>";
          else
            s += "<p>" + tr("g") + "</p>";
          s += "</td>";
          s += "<td align='right' colspan='2'>";
          if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Zeitpunkt").toInt() == EWZ_Zeitpunkt_Gaerung)
            s += "<p class='value'>" + tr("Gärung") + "</p>";
          else if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Zeitpunkt").toInt() == EWZ_Zeitpunkt_Kochen)
            s += "<p class='value'>" + tr("Kochen") + " (" + QString::number(bh->sud()->modelWeitereZutatenGaben()->data(i, "Zugabedauer").toInt()) + "min) </p>";
          else if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Zeitpunkt").toInt() == EWZ_Zeitpunkt_Maischen)
            s += "<p class='value'>" + tr("Maischen") + "</p>";
          s += "</td>";
          s += "</tr>";
          if (bh->sud()->modelWeitereZutatenGaben()->data(i, "Bemerkung").toString() != ""){
            s += "<tr>";
            s += "<td colspan='4' class='kommentar'>" + bh->sud()->modelWeitereZutatenGaben()->data(i, "Bemerkung").toString().replace("\n","<br>") + "</td>";
            s += "</tr>";
          }
        }
      }
      s += "</table>";
    }
    else {
      s = "";
    }
    ctxZutaten["Sonstiges"] = s;

    mTemplateTags["Zutaten"] = ctxZutaten;

    // Rasten
    if (bh->sud()->modelRasten()->rowCount() > 0)
    {
        s = "<table><tbody>";
        s += "<tr>";
        s += "<td>";
        s += "<p>" + tr("Einmaischen bei") + "</p>";
        s += "</td>";
        s += "<td>";
        s += "<p class='value'>" + QString::number(bh->sud()->getEinmaischenTemp()) + "</p>";
        s += "</td>";
        s += "<td>";
        s += "<p>" + tr("°C") + "</p>";
        s += "</td>";
        s += "</tr>";
        for (int i = 0; i < bh->sud()->modelRasten()->rowCount(); i++)
        {
            s += "<tr>";
            s += "<td>";
            s += "<p>" + bh->sud()->modelRasten()->data(i, "RastName").toString() + "</p>";
            s += "</td>";
            s += "<td>";
            s += "<p class='value'>" + QString::number(bh->sud()->modelRasten()->data(i, "RastTemp").toInt()) + "</p>";
            s += "</td>";
            s += "<td>";
            s += "<p>" + tr("°C") + "</p>";
            s += "</td>";
            s += "<td>";
            s += "<p class='value'>" + QString::number(bh->sud()->modelRasten()->data(i, "RastDauer").toInt()) + "</p>";
            s += "</td>";
            s += "<td>";
            s += "<p>" + tr("min") + "</p>";
            s += "</td>";
            s += "</tr>";
        }
        s += "</tbody></table>";
        mTemplateTags["Rasten"] = s;
    }

    // Anhang
    QDir databasePath = QDir(gSettings->databaseDir());
    s = "";
    for (int i = 0; i < bh->sud()->modelAnhang()->rowCount(); ++i)
    {
        QString pfad = bh->modelAnhang()->data(i, "Pfad").toString();
        if (QDir::isRelativePath(pfad))
            pfad = QDir::cleanPath(databasePath.filePath(pfad));
        if (WdgAnhang::isImage(pfad))
            s += "<img style=\"max-width:80%;\" src=\"" + pfad + "\" alt=\"" + pfad + "\"></br></br>";
        else
            s += "<a href=\"" + pfad + "\">" + pfad + "</a></br></br>";
    }
    mTemplateTags["Anhang"] = s;


    ui->treeViewTemplateTags->clear();
    for (QVariantMap::const_iterator it = mTemplateTags.begin(); it != mTemplateTags.end(); ++it)
    {
        if (it.value().canConvert<QVariantMap>())
        {
            QVariantMap hash = it.value().toMap();
            QTreeWidgetItem *t = new QTreeWidgetItem(ui->treeViewTemplateTags, {it.key()});
            for (QVariantMap::const_iterator it2 = hash.begin(); it2 != hash.end(); ++it2)
                t->addChild(new QTreeWidgetItem(t, {it2.key(), it2.value().toString()}));
            ui->treeViewTemplateTags->addTopLevelItem(t);
        }
        else
        {
            ui->treeViewTemplateTags->addTopLevelItem(new QTreeWidgetItem(ui->treeViewTemplateTags, {it.key(), it.value().toString()}));
        }
    }
}

void TabZusammenfassung::updateWebView()
{
    if (ui->cbEditMode->isChecked())
    {
        if (ui->cbTemplateAuswahl->currentIndex() == 0)
        {
            ui->webview->renderText(ui->tbTemplate->toPlainText(), mTemplateTags);
        }
        else
        {
            mTempCssFile.open();
            mTempCssFile.write(ui->tbTemplate->toPlainText().toUtf8());
            mTempCssFile.flush();
            mTemplateTags["Style"] = mTempCssFile.fileName();
            ui->webview->renderTemplate(mTemplateTags);
        }
    }
    else
    {
        ui->webview->renderTemplate(mTemplateTags);
    }
}
