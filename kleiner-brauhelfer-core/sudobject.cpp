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
    mLoading(false),
    proxyModelRasten(new ProxyModel(this)),
    proxyModelMalzschuettung(new ProxyModel(this)),
    proxyModelHopfengaben(new ProxyModel(this)),
    proxyModelHefegaben(new ProxyModel(this)),
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
    connect(bh->modelSud(), SIGNAL(layoutChanged()), this, SLOT(onSudLayoutChanged()));
    connect(bh->modelSud(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)),
            this, SLOT(onSudDataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)));
}

SudObject::~SudObject()
{
    delete proxyModelRasten;
    delete proxyModelMalzschuettung;
    delete proxyModelHopfengaben;
    delete proxyModelHefegaben;
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
        mLoading = true;
        modelRasten()->setSourceModel(bh->modelRasten());
        modelRasten()->setFilterKeyColumn(bh->modelRasten()->fieldIndex("SudID"));
        modelRasten()->setFilterRegExp(regExpId);
        modelMalzschuettung()->setSourceModel(bh->modelMalzschuettung());
        modelMalzschuettung()->setFilterKeyColumn(bh->modelMalzschuettung()->fieldIndex("SudID"));
        modelMalzschuettung()->setFilterRegExp(regExpId);
        modelHopfengaben()->setSourceModel(bh->modelHopfengaben());
        modelHopfengaben()->setFilterKeyColumn(bh->modelHopfengaben()->fieldIndex("SudID"));
        modelHopfengaben()->setFilterRegExp(regExpId);
        modelHopfengaben()->sort(bh->modelHopfengaben()->fieldIndex("Zeit"), Qt::DescendingOrder);
        modelHefegaben()->setSourceModel(bh->modelHefegaben());
        modelHefegaben()->setFilterKeyColumn(bh->modelHefegaben()->fieldIndex("SudID"));
        modelHefegaben()->setFilterRegExp(regExpId);
        modelHefegaben()->sort(bh->modelHefegaben()->fieldIndex("ZugabeNach"), Qt::AscendingOrder);
        modelWeitereZutatenGaben()->setSourceModel(bh->modelWeitereZutatenGaben());
        modelWeitereZutatenGaben()->setFilterKeyColumn(bh->modelWeitereZutatenGaben()->fieldIndex("SudID"));
        modelWeitereZutatenGaben()->setFilterRegExp(regExpId);
        modelSchnellgaerverlauf()->setSourceModel(bh->modelSchnellgaerverlauf());
        modelSchnellgaerverlauf()->setFilterKeyColumn(bh->modelSchnellgaerverlauf()->fieldIndex("SudID"));
        modelSchnellgaerverlauf()->setFilterRegExp(regExpId);
        modelSchnellgaerverlauf()->sort(bh->modelSchnellgaerverlauf()->fieldIndex("Zeitstempel"), Qt::AscendingOrder);
        modelHauptgaerverlauf()->setSourceModel(bh->modelHauptgaerverlauf());
        modelHauptgaerverlauf()->setFilterKeyColumn(bh->modelHauptgaerverlauf()->fieldIndex("SudID"));
        modelHauptgaerverlauf()->setFilterRegExp(regExpId);
        modelHauptgaerverlauf()->sort(bh->modelHauptgaerverlauf()->fieldIndex("Zeitstempel"), Qt::AscendingOrder);
        modelNachgaerverlauf()->setSourceModel(bh->modelNachgaerverlauf());
        modelNachgaerverlauf()->setFilterKeyColumn(bh->modelNachgaerverlauf()->fieldIndex("SudID"));
        modelNachgaerverlauf()->setFilterRegExp(regExpId);
        modelNachgaerverlauf()->sort(bh->modelNachgaerverlauf()->fieldIndex("Zeitstempel"), Qt::AscendingOrder);
        modelBewertungen()->setSourceModel(bh->modelBewertungen());
        modelBewertungen()->setFilterKeyColumn(bh->modelBewertungen()->fieldIndex("SudID"));
        modelBewertungen()->setFilterRegExp(regExpId);
        modelAnhang()->setSourceModel(bh->modelAnhang());
        modelAnhang()->setFilterKeyColumn(bh->modelAnhang()->fieldIndex("SudID"));
        modelAnhang()->setFilterRegExp(regExpId);
        modelFlaschenlabel()->setSourceModel(bh->modelFlaschenlabel());
        modelFlaschenlabel()->setFilterKeyColumn(bh->modelFlaschenlabel()->fieldIndex("SudID"));
        modelFlaschenlabel()->setFilterRegExp(regExpId);
        modelFlaschenlabelTags()->setSourceModel(bh->modelFlaschenlabelTags());
        modelFlaschenlabelTags()->setFilterKeyColumn(bh->modelFlaschenlabelTags()->fieldIndex("SudID"));
        modelFlaschenlabelTags()->setFilterRegExp(QRegExp(QString("^(%1|-.*)$").arg(mId), Qt::CaseInsensitive, QRegExp::RegExp));
        mLoading = false;

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

bool SudObject::isLoading() const
{
    return mLoading;
}

bool SudObject::isLoaded() const
{
    return mId != -1;
}

void SudObject::onSudLayoutChanged()
{
    if (mId != getValue("ID").toInt() || getValue("deleted").toBool())
        unload();
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

ProxyModel* SudObject::modelHefegaben() const
{
    return proxyModelHefegaben;
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

QVariant SudObject::getAnlageData(const QString& fieldName) const
{
    return bh->modelSud()->dataAnlage(mRowSud, fieldName);
}

QVariant SudObject::getWasserData(const QString& fieldName) const
{
    return bh->modelSud()->dataWasser(mRowSud, fieldName);
}

void SudObject::brauzutatenAbziehen()
{
    int row;
    double mengeTotal;
    ProxyModel *mList;
    SqlTableModel *mSubstract;

    // Malz
    mList = modelMalzschuettung();
    mSubstract = bh->modelMalz();
    for (int i = 0; i < mList->rowCount(); ++i)
    {
        row = mSubstract->getRowWithValue("Beschreibung", mList->data(i, "Name"));
        if (row >= 0)
        {
            mengeTotal = mSubstract->data(row, "Menge").toDouble() - mList->data(i, "erg_Menge").toDouble();
            if (mengeTotal < 0.0)
                mengeTotal = 0.0;
            mSubstract->setData(row, "Menge", mengeTotal);
        }
    }

    // Hopfen
    mList = modelHopfengaben();
    mSubstract = bh->modelHopfen();
    for (int i = 0; i < mList->rowCount(); ++i)
    {
        row = mSubstract->getRowWithValue("Beschreibung", mList->data(i, "Name"));
        if (row >= 0)
        {
            mengeTotal = mSubstract->data(row, "Menge").toDouble() - mList->data(i, "erg_Menge").toDouble();
            if (mengeTotal < 0.0)
                mengeTotal = 0.0;
            mSubstract->setData(row, "Menge", mengeTotal);
        }
    }

    // Hefe
    mList = modelHefegaben();
    mSubstract = bh->modelHefe();
    for (int i = 0; i < mList->rowCount(); ++i)
    {
        if (mList->data(i, "ZugabeNach").toInt() == 0)
        {
            row = mSubstract->getRowWithValue("Beschreibung", mList->data(i, "Name"));
            if (row >= 0)
            {
                mengeTotal = mSubstract->data(row, "Menge").toInt() - mList->data(i, "Menge").toInt();
                if (mengeTotal < 0.0)
                    mengeTotal = 0.0;
                mSubstract->setData(row, "Menge", mengeTotal);
            }
        }
    }

    // Weitere Zutaten
    mList = modelWeitereZutatenGaben();
    for (int i = 0; i < mList->rowCount(); ++i)
    {
        if (mList->data(i, "Zeitpunkt").toInt() != EWZ_Zeitpunkt_Gaerung ||
            mList->data(i, "ZugabeNach").toInt() == 0)
        {
            if (mList->data(i, "Typ").toInt() != EWZ_Typ_Hopfen)
            {
                mSubstract = bh->modelWeitereZutaten();
                row = mSubstract->getRowWithValue("Beschreibung", mList->data(i, "Name").toString());
                if (row != -1)
                {
                    mengeTotal = mSubstract->data(row, "Menge").toDouble();
                    if (mList->data(i, "Einheit").toInt() == EWZ_Einheit_Kg)
                        mengeTotal -= mList->data(i, "erg_Menge").toDouble() / 1000;
                    else
                        mengeTotal -= mList->data(i, "erg_Menge").toDouble();
                    if (mengeTotal < 0.0)
                        mengeTotal = 0.0;
                    mSubstract->setData(row, "Menge", mengeTotal);
                }
            }
            else
            {
                mSubstract = bh->modelHopfen();
                row = mSubstract->getRowWithValue("Beschreibung", mList->data(i, "Name").toString());
                if (row >= 0)
                {
                    mengeTotal = mSubstract->data(row, "Menge").toDouble();
                    if (mList->data(i, "Einheit").toInt() == EWZ_Einheit_Kg)
                        mengeTotal -= mList->data(i, "erg_Menge").toDouble() / 1000;
                    else
                        mengeTotal -= mList->data(i, "erg_Menge").toDouble();
                    if (mengeTotal < 0.0)
                        mengeTotal = 0.0;
                    mSubstract->setData(row, "Menge", mengeTotal);
                }
            }
        }
    }
}

void SudObject::zutatAbziehen(const QString& zutat, int typ, double menge)
{
    int row;
    double mengeTotal;
    SqlTableModel *mSubstract;
    switch (typ)
    {
    case 0:
        mSubstract = bh->modelHopfen();
        row = mSubstract->getRowWithValue("Beschreibung", zutat);
        if (row != -1)
        {
            mengeTotal = mSubstract->data(row, "Menge").toDouble() - menge;
            if (mengeTotal < 0.0)
                mengeTotal = 0.0;
            mSubstract->setData(row, "Menge", mengeTotal);
        }
        break;
    case 1:
        mSubstract = bh->modelHefe();
        row = mSubstract->getRowWithValue("Beschreibung", zutat);
        if (row != -1)
        {
            mengeTotal = mSubstract->data(row, "Menge").toInt() - menge;
            if (mengeTotal < 0.0)
                mengeTotal = 0.0;
            mSubstract->setData(row, "Menge", mengeTotal);
        }
        break;
    case 2:
        mSubstract = bh->modelWeitereZutaten();
        row = mSubstract->getRowWithValue("Beschreibung", zutat);
        if (row != -1)
        {
            mengeTotal = mSubstract->data(row, "Menge").toDouble();
            if (mSubstract->data(row, "Einheiten").toInt() == EWZ_Einheit_Kg)
                mengeTotal -= menge / 1000;
            else
                mengeTotal -= menge;
            if (mengeTotal < 0.0)
                mengeTotal = 0.0;
            mSubstract->setData(row, "Menge", mengeTotal);
        }
        break;
    }
}
