#include "proxymodelsud.h"
#include "brauhelfer.h"
#include "modelsud.h"

ProxyModelSud::ProxyModelSud(QObject *parent) :
    ProxyModel(parent),
    mColumnId(-1),
    mColumnStatus(-1),
    mColumnMerklistenID(-1),
    mFilterMerkliste(false),
    mFilterStatus(Alle),
    mFilterText(QString())
{
    connect(this, SIGNAL(sourceModelChanged()), this, SLOT(onSourceModelChanged()));
}

void ProxyModelSud::onSourceModelChanged()
{
    if(SqlTableModel* model = dynamic_cast<SqlTableModel*>(sourceModel()))
    {
        mColumnId = model->fieldIndex("ID");
        mColumnSudname = model->fieldIndex("Sudname");
        mColumnKommentar = model->fieldIndex("Kommentar");
        mColumnStatus = model->fieldIndex("Status");
        mColumnMerklistenID = model->fieldIndex("MerklistenID");
        setFilterDateColumn(model->fieldIndex("Braudatum"));
    }
    else if(ProxyModel* model = dynamic_cast<ProxyModel*>(sourceModel()))
    {
        mColumnId = model->fieldIndex("ID");
        mColumnSudname = model->fieldIndex("Sudname");
        mColumnKommentar = model->fieldIndex("Kommentar");
        mColumnStatus = model->fieldIndex("Status");
        mColumnMerklistenID = model->fieldIndex("MerklistenID");
        setFilterDateColumn(model->fieldIndex("Braudatum"));
    }
}

bool ProxyModelSud::filterMerkliste() const
{
    return mFilterMerkliste;
}

void ProxyModelSud::setFilterMerkliste(bool value)
{
    if (mFilterMerkliste != value)
    {
        mFilterMerkliste = value;
        invalidate();
    }
}

ProxyModelSud::FilterStatus ProxyModelSud::filterStatus() const
{
    return mFilterStatus;
}

void ProxyModelSud::setFilterStatus(FilterStatus status)
{
    if (mFilterStatus != status)
    {
        mFilterStatus = status;
        invalidate();
    }
}

QString ProxyModelSud::filterText() const
{
    return mFilterText;
}

void ProxyModelSud::setFilterText(const QString& text)
{
    if (mFilterText != text)
    {
        mFilterText = text;
        invalidate();
    }
}

bool ProxyModelSud::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    QModelIndex index2;
    bool accept = ProxyModel::filterAcceptsRow(source_row, source_parent);
    if (accept && mFilterMerkliste)
    {
        index2 = sourceModel()->index(source_row, mColumnMerklistenID, source_parent);
        if (index2.isValid())
            accept = sourceModel()->data(index2).toInt() > 0;
    }
    if (accept && mFilterStatus != Alle)
    {
        index2 = sourceModel()->index(source_row, mColumnStatus, source_parent);
        if (index2.isValid())
        {
            switch (index2.data().toInt())
            {
            case Sud_Status_Rezept:
                accept = mFilterStatus & Rezept;
                break;
            case Sud_Status_Gebraut:
                accept = mFilterStatus & Gebraut;
                break;
            case Sud_Status_Abgefuellt:
                accept = mFilterStatus & Abgefuellt;
                break;
            case Sud_Status_Verbraucht:
                accept = mFilterStatus & Verbraucht;
                break;
            }
        }
    }
    if (accept && !mFilterText.isEmpty())
    {
        QRegExp rx(mFilterText, Qt::CaseInsensitive, QRegExp::FixedString);
        index2 = sourceModel()->index(source_row, mColumnSudname, source_parent);
        accept = sourceModel()->data(index2).toString().contains(rx);
        if (!accept)
        {
            index2 = sourceModel()->index(source_row, mColumnKommentar, source_parent);
            accept = sourceModel()->data(index2).toString().contains(rx);
        }
        if (!accept)
        {
            ModelSud* modelSud = dynamic_cast<ModelSud*>(sourceModel());
            if (modelSud)
            {
                index2 = sourceModel()->index(source_row, mColumnId, source_parent);
                QRegExp sudReg = QRegExp(QString("^%1$").arg(sourceModel()->data(index2).toInt()));
                int colName = -1;

                ProxyModel modelMalzschuettung;
                modelMalzschuettung.setSourceModel(modelSud->bh->modelMalzschuettung());
                modelMalzschuettung.setFilterKeyColumn(modelSud->bh->modelMalzschuettung()->fieldIndex("SudID"));
                modelMalzschuettung.setFilterRegExp(sudReg);
                colName = modelSud->bh->modelMalzschuettung()->fieldIndex("Name");
                for (int i = 0; i < modelMalzschuettung.rowCount(); i++)
                {
                    QString name = modelMalzschuettung.index(i, colName).data().toString();
                    accept = name.contains(rx);
                    if (accept)
                        break;
                }
                if (!accept)
                {
                    ProxyModel modelHopfengaben;
                    modelHopfengaben.setSourceModel(modelSud->bh->modelHopfengaben());
                    modelHopfengaben.setFilterKeyColumn(modelSud->bh->modelHopfengaben()->fieldIndex("SudID"));
                    modelHopfengaben.setFilterRegExp(sudReg);
                    colName = modelSud->bh->modelHopfengaben()->fieldIndex("Name");
                    for (int i = 0; i < modelHopfengaben.rowCount(); i++)
                    {
                        QString name = modelHopfengaben.index(i, colName).data().toString();
                        accept = name.contains(rx);
                        if (accept)
                            break;
                    }
                }
                if (!accept)
                {
                    ProxyModel modelHefegaben;
                    modelHefegaben.setSourceModel(modelSud->bh->modelHefegaben());
                    modelHefegaben.setFilterKeyColumn(modelSud->bh->modelHefegaben()->fieldIndex("SudID"));
                    modelHefegaben.setFilterRegExp(sudReg);
                    colName = modelSud->bh->modelHefegaben()->fieldIndex("Name");
                    for (int i = 0; i < modelHefegaben.rowCount(); i++)
                    {
                        QString name = modelHefegaben.index(i, colName).data().toString();
                        accept = name.contains(rx);
                        if (accept)
                            break;
                    }
                }
                if (!accept)
                {
                    ProxyModel modelWeitereZutatenGaben;
                    modelWeitereZutatenGaben.setSourceModel(modelSud->bh->modelWeitereZutatenGaben());
                    modelWeitereZutatenGaben.setFilterKeyColumn(modelSud->bh->modelWeitereZutatenGaben()->fieldIndex("SudID"));
                    modelWeitereZutatenGaben.setFilterRegExp(sudReg);
                    colName = modelSud->bh->modelWeitereZutatenGaben()->fieldIndex("Name");
                    for (int i = 0; i < modelWeitereZutatenGaben.rowCount(); i++)
                    {
                        QString name = modelWeitereZutatenGaben.index(i, colName).data().toString();
                        accept = name.contains(rx);
                        if (accept)
                            break;

                    }
                }
                if (!accept)
                {
                    ProxyModel modelFlaschenlabelTags;
                    modelFlaschenlabelTags.setSourceModel(modelSud->bh->modelFlaschenlabelTags());
                    modelFlaschenlabelTags.setFilterKeyColumn(modelSud->bh->modelFlaschenlabelTags()->fieldIndex("SudID"));
                    modelFlaschenlabelTags.setFilterRegExp(QString("^(%1|-.*)$").arg(sourceModel()->data(index2).toInt()));
                    colName = modelSud->bh->modelFlaschenlabelTags()->fieldIndex("TagName");
                    int colValue = modelSud->bh->modelFlaschenlabelTags()->fieldIndex("Value");
                    for (int i = 0; i < modelFlaschenlabelTags.rowCount(); i++)
                    {
                        QString text = modelFlaschenlabelTags.index(i, colName).data().toString();
                        accept = text.contains(rx);
                        if (accept)
                            break;
                        text = modelFlaschenlabelTags.index(i, colValue).data().toString();
                        accept = text.contains(rx);
                        if (accept)
                            break;
                    }
                }
            }
        }
    }
    return accept;
}
