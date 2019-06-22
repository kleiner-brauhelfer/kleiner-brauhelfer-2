#include "dlgrohstoffaustausch.h"
#include "ui_dlgrohstoffaustausch.h"
#include "proxymodel.h"

DlgRohstoffAustausch::DlgRohstoffAustausch(DlgType type, const QString &rohstoff, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgRohstoffAustausch),
    mType(type),
    mImportieren(false)
{
    ui->setupUi(this);
    switch (mType)
    {
    case NichtVorhanden:
        setWindowTitle(tr("Rohstoff nicht vorhanden"));
        ui->label->setText(tr("Dieser Rohstoff ist in der Rohstoffauswahl nicht vorhanden. Soll der Rohstoff im Sud ausgetauscht werden? Oder soll der Rohstoff in die Rohstoffauswahl importiert werden?"));
        break;
    case Loeschen:
        setWindowTitle(tr("Rohstoff wird verwendet"));
        ui->label->setText(tr("Dieser Rohstoff wird in einem noch nicht gebrauten Sud verwendet. Soll der Rohstoff im Sud ausgetauscht werden?"));
        ui->btnImportieren->setVisible(false);
        break;
    }
    ui->labelRohstoff->setText(rohstoff);
}

DlgRohstoffAustausch::~DlgRohstoffAustausch()
{
    delete ui;
}

void DlgRohstoffAustausch::setSud(const QString &sud)
{
    ui->labelSudLabel->setVisible(!sud.isEmpty());
    ui->labelSud->setVisible(!sud.isEmpty());
    ui->labelSud->setText(sud);
}

void DlgRohstoffAustausch::setNearestIndex()
{
    QString rohstoff = ui->labelRohstoff->text();
    if (ui->comboBoxAustausch->count() > 0)
    {
        unsigned int dist;
        unsigned int minDist = levenshtein_distance(rohstoff, ui->comboBoxAustausch->itemText(0));
        int index = 0;
        for (int i = 1; i < ui->comboBoxAustausch->count(); ++i) {
            dist = levenshtein_distance(rohstoff, ui->comboBoxAustausch->itemText(i));
            if (dist < minDist) {
                minDist = dist;
                index = i;
            }
        }
        ui->comboBoxAustausch->setCurrentIndex(index);
    }
}

void DlgRohstoffAustausch::setModel(QAbstractItemModel *model, int column)
{
    ProxyModel proxy;
    proxy.setSourceModel(model);
    QString rohstoff = ui->labelRohstoff->text();
    ui->comboBoxAustausch->clear();
    for (int i = 0; i < proxy.rowCount(); ++i)
    {
        QString text = proxy.index(i, column).data().toString();
        if (mType == Loeschen && text == rohstoff)
            continue;
        ui->comboBoxAustausch->addItem(text);
    }
    setNearestIndex();
}

void DlgRohstoffAustausch::setRohstoff(const QString &rohstoff)
{
    ui->labelRohstoff->setText(rohstoff);
    setNearestIndex();
}

QString DlgRohstoffAustausch::rohstoff() const
{
    return ui->comboBoxAustausch->currentText();
}

void DlgRohstoffAustausch::on_btnAustauschen_clicked()
{
    mImportieren = false;
    accept();
}

void DlgRohstoffAustausch::on_btnNichtAustauschen_clicked()
{
    mImportieren = false;
    reject();
}

void DlgRohstoffAustausch::on_btnImportieren_clicked()
{
    mImportieren = true;
    accept();
}

bool DlgRohstoffAustausch::importieren() const
{
    return mImportieren;
}

unsigned int DlgRohstoffAustausch::levenshtein_distance(const QString& s1, const QString& s2)
{
    unsigned int len1 = (unsigned int)s1.size(), len2 = (unsigned int)s2.size();
    std::vector<unsigned int> col(len2+1), prevCol(len2+1);
    for (unsigned int i = 0; i < prevCol.size(); i++)
        prevCol[i] = i;
    for (unsigned int i = 0; i < len1; i++) {
        col[0] = i+1;
        for (unsigned int j = 0; j < len2; j++)
            col[j+1] = std::min(prevCol[1 + j] + 1, std::min(col[j] + 1, prevCol[j] + (s1[i]==s2[j] ? 0 : 1)));
        col.swap(prevCol);
    }
    return prevCol[len2];
}
