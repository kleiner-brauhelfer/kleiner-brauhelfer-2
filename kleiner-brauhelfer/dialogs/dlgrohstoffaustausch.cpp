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

static double jaroDistance(const QString& a, const QString& b)
{
    if (a.isEmpty() || b.isEmpty())
        return 0.0;

    int s1 = a.size();
    int s2 = b.size();
    int maxRange = qMax(0, qMax(s1, s2) / 2 - 1);

    std::vector<bool> aMatch(s1, false);
    std::vector<bool> bMatch(s2, false);

    int m = 0;
    for (int aIndex = 0; aIndex < s1; ++aIndex)
    {
        int minIndex = qMax(aIndex - maxRange, 0);
        int maxIndex = qMin(aIndex + maxRange + 1, s2);

        if (minIndex >= maxIndex)
            break;

        for (int bIndex = minIndex; bIndex < maxIndex; ++bIndex)
        {
            if (!bMatch.at(bIndex) && a.at(aIndex) == b.at(bIndex))
            {
                aMatch[aIndex] = true;
                bMatch[bIndex] = true;
                ++m;
                break;
            }
        }
    }

    if (m == 0)
        return 0.0;

    std::vector<int> aPosition(m, 0);
    std::vector<int> bPosition(m, 0);

    for (int aIndex = 0, positionIndex = 0; aIndex < s1; ++aIndex)
    {
        if (aMatch.at(aIndex))
        {
            aPosition[positionIndex] = aIndex;
            ++positionIndex;
        }
    }

    for (int bIndex = 0, positionIndex = 0; bIndex < s2; ++bIndex)
    {
        if (bMatch.at(bIndex))
        {
            bPosition[positionIndex] = bIndex;
            ++positionIndex;
        }
    }

    int transpositions = 0;
    for (int index = 0; index < m; ++index)
    {
        if (a.at(aPosition.at(index)) != b.at(bPosition.at(index)))
        {
            ++transpositions;
        }
    }

    return ((double)m / s1 + (double)m / s2 + (double)(m - transpositions / 2) / m) / 3.0;
}

void DlgRohstoffAustausch::setNearestIndex()
{
    QString rohstoff = ui->labelRohstoff->text();
    if (ui->comboBoxAustausch->count() > 0)
    {
        double dist;
        double maxDist = jaroDistance(rohstoff, ui->comboBoxAustausch->itemText(0));
        int index = 0;
        for (int i = 1; i < ui->comboBoxAustausch->count(); ++i)
        {
            dist = jaroDistance(rohstoff, ui->comboBoxAustausch->itemText(i));
            if (dist > maxDist)
            {
                maxDist = dist;
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
