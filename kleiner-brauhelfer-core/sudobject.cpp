#include "sudobject.h"
#include "database_defs.h"
#include "brauhelfer.h"
#include "modelsud.h"
#include <QDateTime>

SudObject::SudObject(Brauhelfer *bh) :
    QObject(bh),
    bh(bh),
    mId(-2),
    mRowSud(-1),
    proxyModelRasten(new ProxyModel(this)),
    proxyModelMalzschuettung(new ProxyModel(this)),
    proxyModelHopfengaben(new ProxyModel(this)),
    proxyModelWeitereZutatenGaben(new ProxyModel(this)),
    proxyModelSchnellgaerverlauf(new ProxyModel(this)),
    proxyModelHauptgaerverlauf(new ProxyModel(this)),
    proxyModelNachgaerverlauf(new ProxyModel(this)),
    proxyModelBewertungen(new ProxyModel(this)),
    proxyModelAnhang(new ProxyModel(this)),
    proxyModelFlaschenlabel(new ProxyModel(this)),
    proxyModelFlaschenlabelTags(new ProxyModel(this))
{
    connect(bh->modelSud(), SIGNAL(modified()), this, SIGNAL(modified()));
    connect(bh->modelSud(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)),
            this, SLOT(onSudDataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)));
}

SudObject::~SudObject()
{
    delete proxyModelRasten;
    delete proxyModelMalzschuettung;
    delete proxyModelHopfengaben;
    delete proxyModelWeitereZutatenGaben;
    delete proxyModelSchnellgaerverlauf;
    delete proxyModelHauptgaerverlauf;
    delete proxyModelNachgaerverlauf;
    delete proxyModelBewertungen;
    delete proxyModelAnhang;
    delete proxyModelFlaschenlabel;
    delete proxyModelFlaschenlabelTags;
}

void SudObject::load(int id)
{
    if (mId != id)
    {
        mId = id;
        QRegExp regExpId(QString("^%1$").arg(mId), Qt::CaseInsensitive, QRegExp::RegExp);
        mRowSud = static_cast<ModelSud*>(bh->modelSud())->getRowWithValue("ID", mId);
        modelRasten()->setFilterKeyColumn(bh->modelRasten()->fieldIndex("SudID"));
        modelRasten()->setFilterRegExp(regExpId);
        modelRasten()->setSourceModel(bh->modelRasten());
        modelRasten()->sort(bh->modelRasten()->fieldIndex("RastTemp"), Qt::AscendingOrder);
        modelMalzschuettung()->setFilterKeyColumn(bh->modelMalzschuettung()->fieldIndex("SudID"));
        modelMalzschuettung()->setFilterRegExp(regExpId);
        modelMalzschuettung()->setSourceModel(bh->modelMalzschuettung());
        modelHopfengaben()->setFilterKeyColumn(bh->modelHopfengaben()->fieldIndex("SudID"));
        modelHopfengaben()->setFilterRegExp(regExpId);
        modelHopfengaben()->setSourceModel(bh->modelHopfengaben());
        modelHopfengaben()->sort(bh->modelHopfengaben()->fieldIndex("Zeit"), Qt::DescendingOrder);
        modelWeitereZutatenGaben()->setFilterKeyColumn(bh->modelWeitereZutatenGaben()->fieldIndex("SudID"));
        modelWeitereZutatenGaben()->setFilterRegExp(regExpId);
        modelWeitereZutatenGaben()->setSourceModel(bh->modelWeitereZutatenGaben());
        modelSchnellgaerverlauf()->setFilterKeyColumn(bh->modelSchnellgaerverlauf()->fieldIndex("SudID"));
        modelSchnellgaerverlauf()->setFilterRegExp(regExpId);
        modelSchnellgaerverlauf()->setSourceModel(bh->modelSchnellgaerverlauf());
        modelSchnellgaerverlauf()->sort(bh->modelSchnellgaerverlauf()->fieldIndex("Zeitstempel"), Qt::AscendingOrder);
        modelHauptgaerverlauf()->setFilterKeyColumn(bh->modelHauptgaerverlauf()->fieldIndex("SudID"));
        modelHauptgaerverlauf()->setFilterRegExp(regExpId);
        modelHauptgaerverlauf()->setSourceModel(bh->modelHauptgaerverlauf());
        modelHauptgaerverlauf()->sort(bh->modelHauptgaerverlauf()->fieldIndex("Zeitstempel"), Qt::AscendingOrder);
        modelNachgaerverlauf()->setFilterKeyColumn(bh->modelNachgaerverlauf()->fieldIndex("SudID"));
        modelNachgaerverlauf()->setFilterRegExp(regExpId);
        modelNachgaerverlauf()->setSourceModel(bh->modelNachgaerverlauf());
        modelNachgaerverlauf()->sort(bh->modelNachgaerverlauf()->fieldIndex("Zeitstempel"), Qt::AscendingOrder);
        modelBewertungen()->setFilterKeyColumn(bh->modelBewertungen()->fieldIndex("SudID"));
        modelBewertungen()->setFilterRegExp(regExpId);
        modelBewertungen()->setSourceModel(bh->modelBewertungen());
        modelAnhang()->setFilterKeyColumn(bh->modelAnhang()->fieldIndex("SudID"));
        modelAnhang()->setFilterRegExp(regExpId);
        modelAnhang()->setSourceModel(bh->modelAnhang());
        modelFlaschenlabel()->setFilterKeyColumn(bh->modelFlaschenlabel()->fieldIndex("SudID"));
        modelFlaschenlabel()->setFilterRegExp(regExpId);
        modelFlaschenlabel()->setSourceModel(bh->modelFlaschenlabel());
        modelFlaschenlabelTags()->setFilterKeyColumn(bh->modelFlaschenlabelTags()->fieldIndex("SudID"));
        modelFlaschenlabelTags()->setFilterRegExp(QRegExp(QString("^(%1|-.*)$").arg(mId), Qt::CaseInsensitive, QRegExp::RegExp));
        modelFlaschenlabelTags()->setSourceModel(bh->modelFlaschenlabelTags());

        if (isLoaded())
        {
            QVariant qvId = getValue("ID");
            if (!qvId.isValid() || qvId.toInt() != mId)
            {
                unload();
                return;
            }
        }
        emit loadedChanged();
        emit modified();
    }
}

void SudObject::unload()
{
    load(-1);
}

bool SudObject::isLoaded() const
{
    return mId != -1;
}

void SudObject::onSudDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    if (mRowSud >= topLeft.row() && mRowSud <= bottomRight.row())
        emit dataChanged(topLeft, bottomRight, roles);
}

ProxyModel* SudObject::modelRasten() const
{
    return proxyModelRasten;
}

ProxyModel* SudObject::modelMalzschuettung() const
{
    return proxyModelMalzschuettung;
}

ProxyModel* SudObject::modelHopfengaben() const
{
    return proxyModelHopfengaben;
}

ProxyModel* SudObject::modelWeitereZutatenGaben() const
{
    return proxyModelWeitereZutatenGaben;
}

ProxyModel* SudObject::modelSchnellgaerverlauf() const
{
    return proxyModelSchnellgaerverlauf;
}

ProxyModel* SudObject::modelHauptgaerverlauf() const
{
    return proxyModelHauptgaerverlauf;
}

ProxyModel* SudObject::modelNachgaerverlauf() const
{
    return proxyModelNachgaerverlauf;
}

ProxyModel* SudObject::modelBewertungen() const
{
    return proxyModelBewertungen;
}

ProxyModel* SudObject::modelAnhang() const
{
    return proxyModelAnhang;
}

ProxyModel* SudObject::modelFlaschenlabel() const
{
    return proxyModelFlaschenlabel;
}

ProxyModel* SudObject::modelFlaschenlabelTags() const
{
    return proxyModelFlaschenlabelTags;
}

int SudObject::id() const
{
    return mId;
}

int SudObject::row() const
{
    return mRowSud;
}

QVariant SudObject::getValue(const QString &fieldName) const
{
    return bh->modelSud()->data(mRowSud, fieldName);
}

bool SudObject::setValue(const QString &fieldName, const QVariant &value)
{
    return bh->modelSud()->setData(mRowSud, fieldName, value);
}

QVariant SudObject::getAnlageValue(const QString& fieldName) const
{
    return bh->modelSud()->dataAnlage(mRowSud, fieldName);
}

void SudObject::substractBrewIngredients()
{
    int row;
    double quantity;
    ProxyModel *mList;
    SqlTableModel *mSubstract;

    // Malz
    mList = modelMalzschuettung();
    mSubstract = bh->modelMalz();
    for (int i = 0; i < mList->rowCount(); ++i)
    {
        row = mSubstract->getRowWithValue("Beschreibung", mList->data(i, "Name").toString());
        if (row != -1)
        {
            quantity = mSubstract->data(row, "Menge").toDouble() - mList->data(i, "erg_Menge").toDouble();
            if (quantity < 0.0)
                quantity = 0.0;
            mSubstract->setData(row, "Menge", quantity);
        }
    }

    // Hopfen
    mList = modelHopfengaben();
    mSubstract = bh->modelHopfen();
    for (int i = 0; i < mList->rowCount(); ++i)
    {
        row = mSubstract->getRowWithValue("Beschreibung", mList->data(i, "Name").toString());
        if (row != -1)
        {
            quantity = mSubstract->data(row, "Menge").toDouble() - mList->data(i, "erg_Menge").toDouble();
            if (quantity < 0.0)
                quantity = 0.0;
            mSubstract->setData(row, "Menge", quantity);
        }
    }

    // Hefe
    mSubstract = bh->modelHefe();
    row = mSubstract->getRowWithValue("Beschreibung", getAuswahlHefe());
    if (row != -1)
    {
        quantity = mSubstract->data(row, "Menge").toDouble() - getHefeAnzahlEinheiten();
        if (quantity < 0.0)
            quantity = 0.0;
        mSubstract->setData(row, "Menge", quantity);
    }

    // Weitere Zutaten
    mList = modelWeitereZutatenGaben();
    for (int i = 0; i < mList->rowCount(); ++i)
    {
        if (mList->data(i, "Zeitpunkt").toInt() != EWZ_Zeitpunkt_Gaerung)
        {
            if (mList->data(i, "Typ").toInt() != EWZ_Typ_Hopfen)
            {
                mSubstract = bh->modelWeitereZutaten();
                row = mSubstract->getRowWithValue("Beschreibung", mList->data(i, "Name").toString());
                if (row != -1)
                {
                    quantity = mSubstract->data(row, "Menge").toDouble();
                    if (mList->data(i, "Einheit").toInt() == EWZ_Einheit_Kg)
                        quantity -= mList->data(i, "erg_Menge").toDouble() / 1000;
                    else
                        quantity -= mList->data(i, "erg_Menge").toDouble();
                    if (quantity < 0.0)
                        quantity = 0.0;
                    mSubstract->setData(row, "Menge", quantity);
                }
            }
            else
            {
                mSubstract = bh->modelHopfen();
                row = mSubstract->getRowWithValue("Beschreibung", mList->data(i, "Name").toString());
                if (row != -1)
                {
                    quantity = mSubstract->data(row, "Menge").toDouble();
                    if (mList->data(i, "Einheit").toInt() == EWZ_Einheit_Kg)
                        quantity -= mList->data(i, "erg_Menge").toDouble() / 1000;
                    else
                        quantity -= mList->data(i, "erg_Menge").toDouble();
                    if (quantity < 0.0)
                        quantity = 0.0;
                    mSubstract->setData(row, "Menge", quantity);
                }
            }
        }
    }
}

void SudObject::substractIngredient(const QString& ingredient, bool hopfen, double quantity)
{
    int row;
    double totalQuantity;
    SqlTableModel *mSubstract;
    if (hopfen)
    {
        mSubstract = bh->modelHopfen();
        row = mSubstract->getRowWithValue("Beschreibung", ingredient);
        if (row != -1)
        {
            totalQuantity = mSubstract->data(row, "Menge").toDouble() - quantity;
            if (totalQuantity < 0.0)
                totalQuantity = 0.0;
            mSubstract->setData(row, "Menge", totalQuantity);
        }
    }
    else
    {
        mSubstract = bh->modelWeitereZutaten();
        row = mSubstract->getRowWithValue("Beschreibung", ingredient);
        if (row != -1)
        {
            totalQuantity = mSubstract->data(row, "Menge").toDouble();
            if (mSubstract->data(row, "Einheiten").toInt() == EWZ_Einheit_Kg)
                totalQuantity -= quantity / 1000;
            else
                totalQuantity -= quantity;
            if (totalQuantity < 0.0)
                totalQuantity = 0.0;
            mSubstract->setData(row, "Menge", totalQuantity);
        }
    }
}
