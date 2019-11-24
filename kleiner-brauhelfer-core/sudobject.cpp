#include "sudobject.h"
#include "database_defs.h"
#include "brauhelfer.h"
#include "modelsud.h"
#include <QDateTime>
#include <QtMath>

SudObject::SudObject(Brauhelfer *bh) :
    QObject(bh),
    bh(bh),
    mId(-1),
    mRowSud(-1),
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
    connect(bh, SIGNAL(saved()), this, SLOT(onSudLayoutChanged()));
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

void SudObject::init()
{
    qInfo() << "SudObject::init()";
    modelRasten()->setSourceModel(bh->modelRasten());
    modelRasten()->setFilterKeyColumn(ModelRasten::ColSudID);
    modelMalzschuettung()->setSourceModel(bh->modelMalzschuettung());
    modelMalzschuettung()->setFilterKeyColumn(ModelMalzschuettung::ColSudID);
    modelHopfengaben()->setSourceModel(bh->modelHopfengaben());
    modelHopfengaben()->setFilterKeyColumn(ModelHopfengaben::ColSudID);
    modelHopfengaben()->sort(ModelHopfengaben::ColZeit, Qt::DescendingOrder);
    modelHefegaben()->setSourceModel(bh->modelHefegaben());
    modelHefegaben()->setFilterKeyColumn(ModelHefegaben::ColSudID);
    modelHefegaben()->sort(ModelHefegaben::ColZugabeNach, Qt::AscendingOrder);
    modelWeitereZutatenGaben()->setSourceModel(bh->modelWeitereZutatenGaben());
    modelWeitereZutatenGaben()->setFilterKeyColumn(ModelWeitereZutatenGaben::ColSudID);
    modelSchnellgaerverlauf()->setSourceModel(bh->modelSchnellgaerverlauf());
    modelSchnellgaerverlauf()->setFilterKeyColumn(ModelSchnellgaerverlauf::ColSudID);
    modelSchnellgaerverlauf()->sort(ModelSchnellgaerverlauf::ColZeitstempel, Qt::AscendingOrder);
    modelHauptgaerverlauf()->setSourceModel(bh->modelHauptgaerverlauf());
    modelHauptgaerverlauf()->setFilterKeyColumn(ModelHauptgaerverlauf::ColSudID);
    modelHauptgaerverlauf()->sort(ModelHauptgaerverlauf::ColZeitstempel, Qt::AscendingOrder);
    modelNachgaerverlauf()->setSourceModel(bh->modelNachgaerverlauf());
    modelNachgaerverlauf()->setFilterKeyColumn(ModelNachgaerverlauf::ColSudID);
    modelNachgaerverlauf()->sort(ModelNachgaerverlauf::ColZeitstempel, Qt::AscendingOrder);
    modelBewertungen()->setSourceModel(bh->modelBewertungen());
    modelBewertungen()->setFilterKeyColumn(ModelBewertungen::ColSudID);
    modelAnhang()->setSourceModel(bh->modelAnhang());
    modelAnhang()->setFilterKeyColumn(ModelAnhang::ColSudID);
    modelFlaschenlabel()->setSourceModel(bh->modelFlaschenlabel());
    modelFlaschenlabel()->setFilterKeyColumn(ModelFlaschenlabel::ColSudID);
    modelFlaschenlabelTags()->setSourceModel(bh->modelFlaschenlabelTags());
    modelFlaschenlabelTags()->setFilterKeyColumn(ModelFlaschenlabelTags::ColSudID);

    QRegExp regExpId(QString("^%1$").arg(mId), Qt::CaseInsensitive, QRegExp::RegExp);
    modelRasten()->setFilterRegExp(regExpId);
    modelMalzschuettung()->setFilterRegExp(regExpId);
    modelHopfengaben()->setFilterRegExp(regExpId);
    modelHefegaben()->setFilterRegExp(regExpId);
    modelWeitereZutatenGaben()->setFilterRegExp(regExpId);
    modelSchnellgaerverlauf()->setFilterRegExp(regExpId);
    modelHauptgaerverlauf()->setFilterRegExp(regExpId);
    modelNachgaerverlauf()->setFilterRegExp(regExpId);
    modelBewertungen()->setFilterRegExp(regExpId);
    modelAnhang()->setFilterRegExp(regExpId);
    modelFlaschenlabel()->setFilterRegExp(regExpId);
    modelFlaschenlabelTags()->setFilterRegExp(QRegExp(QString("^(%1|-.*)$").arg(mId), Qt::CaseInsensitive, QRegExp::RegExp));
}

void SudObject::load(int id)
{
    if (mId != id)
    {
        mId = id;
        mRowSud = bh->modelSud()->getRowWithValue(ModelSud::ColID, mId);

        if (mId != -1)
            qInfo() << "SudObject::load():" << getSudname() << "(" << mId << ")";
        else
            qInfo() << "SudObject::unload()";

        QRegExp regExpId(QString("^%1$").arg(mId), Qt::CaseInsensitive, QRegExp::RegExp);
        modelRasten()->setFilterRegExp(regExpId);
        modelMalzschuettung()->setFilterRegExp(regExpId);
        modelHopfengaben()->setFilterRegExp(regExpId);
        modelHefegaben()->setFilterRegExp(regExpId);
        modelWeitereZutatenGaben()->setFilterRegExp(regExpId);
        modelSchnellgaerverlauf()->setFilterRegExp(regExpId);
        modelHauptgaerverlauf()->setFilterRegExp(regExpId);
        modelNachgaerverlauf()->setFilterRegExp(regExpId);
        modelBewertungen()->setFilterRegExp(regExpId);
        modelAnhang()->setFilterRegExp(regExpId);
        modelFlaschenlabel()->setFilterRegExp(regExpId);
        modelFlaschenlabelTags()->setFilterRegExp(QRegExp(QString("^(%1|-.*)$").arg(mId), Qt::CaseInsensitive, QRegExp::RegExp));

        if (isLoaded())
        {
            QVariant qvId = getValue(ModelSud::ColID);
            if (!qvId.isValid() || qvId.toInt() != mId)
            {
                qCritical("SudObject::load() ID mismatch");
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

void SudObject::onSudLayoutChanged()
{
    if (getValue(ModelSud::ColDeleted).toBool())
    {
        unload();
    }
    else if (mId != getValue(ModelSud::ColID).toInt())
    {
        mRowSud = bh->modelSud()->getRowWithValue(ModelSud::ColID, mId);
        if (mRowSud < 0)
           unload();
    }
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

QVariant SudObject::getValue(int col) const
{
    return bh->modelSud()->data(mRowSud, col);
}

bool SudObject::setValue(int col, const QVariant &value)
{
    qInfo() << "SudObject::setValue():" << bh->modelSud()->fieldName(col) << "=" << value.toString();
    return bh->modelSud()->setData(mRowSud, col, value);
}

QVariant SudObject::getAnlageData(int col) const
{
    return bh->modelSud()->dataAnlage(mRowSud, col);
}

QVariant SudObject::getWasserData(int col) const
{
    return bh->modelSud()->dataWasser(mRowSud, col);
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
    for (int r = 0; r < mList->rowCount(); ++r)
    {
        row = mSubstract->getRowWithValue(ModelMalz::ColBeschreibung, mList->data(r, ModelMalzschuettung::ColName));
        if (row >= 0)
        {
            mengeTotal = mSubstract->data(row, ModelMalz::ColMenge).toDouble() - mList->data(r, ModelMalzschuettung::Colerg_Menge).toDouble();
            if (mengeTotal < 0.0)
                mengeTotal = 0.0;
            mSubstract->setData(row, ModelMalz::ColMenge, mengeTotal);
        }
    }

    // Hopfen
    mList = modelHopfengaben();
    mSubstract = bh->modelHopfen();
    for (int r = 0; r < mList->rowCount(); ++r)
    {
        row = mSubstract->getRowWithValue(ModelHopfen::ColBeschreibung, mList->data(r, ModelHopfengaben::ColName));
        if (row >= 0)
        {
            mengeTotal = mSubstract->data(row, ModelHopfen::ColMenge).toDouble() - mList->data(r, ModelHopfengaben::Colerg_Menge).toDouble();
            if (mengeTotal < 0.0)
                mengeTotal = 0.0;
            mSubstract->setData(row, ModelHopfen::ColMenge, mengeTotal);
        }
    }

    // Hefe
    mList = modelHefegaben();
    mSubstract = bh->modelHefe();
    for (int r = 0; r < mList->rowCount(); ++r)
    {
        if (mList->data(r, ModelHefegaben::ColZugabeNach).toInt() == 0)
        {
            row = mSubstract->getRowWithValue(ModelHefe::ColBeschreibung, mList->data(r, ModelHefegaben::ColName));
            if (row >= 0)
            {
                mengeTotal = mSubstract->data(row, ModelHefe::ColMenge).toInt() - mList->data(r, ModelHefegaben::ColMenge).toInt();
                if (mengeTotal < 0.0)
                    mengeTotal = 0.0;
                mSubstract->setData(row, ModelHefe::ColMenge, mengeTotal);
            }
        }
    }

    // Weitere Zutaten
    mList = modelWeitereZutatenGaben();
    for (int r = 0; r < mList->rowCount(); ++r)
    {
        if (mList->data(r, ModelWeitereZutatenGaben::ColZeitpunkt).toInt() != EWZ_Zeitpunkt_Gaerung ||
            mList->data(r, ModelWeitereZutatenGaben::ColZugabeNach).toInt() == 0)
        {
            if (mList->data(r, ModelWeitereZutatenGaben::ColTyp).toInt() != EWZ_Typ_Hopfen)
            {
                mSubstract = bh->modelWeitereZutaten();
                row = mSubstract->getRowWithValue(ModelWeitereZutaten::ColBeschreibung, mList->data(r, ModelWeitereZutatenGaben::ColName).toString());
                if (row != -1)
                {
                    mengeTotal = mSubstract->data(row, ModelWeitereZutaten::ColMenge).toDouble();
                    switch (mList->data(r, ModelWeitereZutatenGaben::ColEinheit).toInt())
                    {
                    case EWZ_Einheit_Kg:
                        mengeTotal -= mList->data(r, ModelWeitereZutatenGaben::Colerg_Menge).toDouble() / 1000;
                        break;
                    case EWZ_Einheit_g:
                        mengeTotal -= mList->data(r, ModelWeitereZutatenGaben::Colerg_Menge).toDouble();
                        break;
                    case EWZ_Einheit_mg:
                        mengeTotal -= mList->data(r, ModelWeitereZutatenGaben::Colerg_Menge).toDouble() * 1000;
                        break;
                    case EWZ_Einheit_Stk:
                        mengeTotal -= qCeil(mList->data(r, ModelWeitereZutatenGaben::Colerg_Menge).toDouble());
                        break;
                    }
                    if (mengeTotal < 0.0)
                        mengeTotal = 0.0;
                    mSubstract->setData(row, ModelWeitereZutaten::ColMenge, mengeTotal);
                }
            }
            else
            {
                mSubstract = bh->modelHopfen();
                row = mSubstract->getRowWithValue(ModelHopfen::ColBeschreibung, mList->data(r, ModelWeitereZutatenGaben::ColName).toString());
                if (row >= 0)
                {
                    mengeTotal = mSubstract->data(row, ModelHopfen::ColMenge).toDouble();
                    switch (mList->data(r, ModelWeitereZutatenGaben::ColEinheit).toInt())
                    {
                    case EWZ_Einheit_Kg:
                        mengeTotal -= mList->data(r, ModelWeitereZutatenGaben::Colerg_Menge).toDouble() / 1000;
                        break;
                    case EWZ_Einheit_g:
                        mengeTotal -= mList->data(r, ModelWeitereZutatenGaben::Colerg_Menge).toDouble();
                        break;
                    case EWZ_Einheit_mg:
                        mengeTotal -= mList->data(r, ModelWeitereZutatenGaben::Colerg_Menge).toDouble() * 1000;
                        break;
                    case EWZ_Einheit_Stk:
                        mengeTotal -= qCeil(mList->data(r, ModelWeitereZutatenGaben::Colerg_Menge).toDouble());
                        break;
                    }
                    if (mengeTotal < 0.0)
                        mengeTotal = 0.0;
                    mSubstract->setData(row, ModelHopfen::ColMenge, mengeTotal);
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
        row = mSubstract->getRowWithValue(ModelHopfen::ColBeschreibung, zutat);
        if (row != -1)
        {
            mengeTotal = mSubstract->data(row, ModelHopfen::ColMenge).toDouble() - menge;
            if (mengeTotal < 0.0)
                mengeTotal = 0.0;
            mSubstract->setData(row, ModelHopfen::ColMenge, mengeTotal);
        }
        break;
    case 1:
        mSubstract = bh->modelHefe();
        row = mSubstract->getRowWithValue(ModelHefe::ColBeschreibung, zutat);
        if (row != -1)
        {
            mengeTotal = mSubstract->data(row, ModelHefe::ColMenge).toInt() - menge;
            if (mengeTotal < 0.0)
                mengeTotal = 0.0;
            mSubstract->setData(row, ModelHefe::ColMenge, mengeTotal);
        }
        break;
    case 2:
        mSubstract = bh->modelWeitereZutaten();
        row = mSubstract->getRowWithValue(ModelWeitereZutaten::ColBeschreibung, zutat);
        if (row != -1)
        {
            mengeTotal = mSubstract->data(row, ModelWeitereZutaten::ColMenge).toDouble();
            switch (mSubstract->data(row, ModelWeitereZutaten::ColEinheiten).toInt())
            {
            case EWZ_Einheit_Kg:
                mengeTotal -= menge / 1000;
                break;
            case EWZ_Einheit_g:
                mengeTotal -= menge;
                break;
            case EWZ_Einheit_mg:
                mengeTotal -= menge * 1000;
                break;
            case EWZ_Einheit_Stk:
                mengeTotal -= qCeil(menge);
                break;
            }
            if (mengeTotal < 0.0)
                mengeTotal = 0.0;
            mSubstract->setData(row, ModelWeitereZutaten::ColMenge, mengeTotal);
        }
        break;
    }
}
