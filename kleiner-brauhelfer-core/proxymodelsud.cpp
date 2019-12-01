#include "proxymodelsud.h"
#include "brauhelfer.h"
#include "modelsud.h"

ProxyModelSud::ProxyModelSud(QObject *parent) :
    ProxyModel(parent),
    mFilterMerkliste(false),
    mFilterStatus(Alle),
    mFilterText(QString())
{
    resetColumns();
}

void ProxyModelSud::resetColumns()
{
    setFilterDateColumn(ModelSud::ColBraudatum);
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
    QModelIndex idx;
    bool accept = ProxyModel::filterAcceptsRow(source_row, source_parent);
    if (accept && mFilterMerkliste)
    {
        idx = sourceModel()->index(source_row, ModelSud::ColMerklistenID, source_parent);
        if (idx.isValid())
            accept = sourceModel()->data(idx).toInt() > 0;
    }
    if (accept && mFilterStatus != Alle)
    {
        idx = sourceModel()->index(source_row, ModelSud::ColStatus, source_parent);
        if (idx.isValid())
        {
            switch (idx.data().toInt())
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
        idx = sourceModel()->index(source_row, ModelSud::ColSudname, source_parent);
        accept = sourceModel()->data(idx).toString().contains(rx);
        if (!accept)
        {
            idx = sourceModel()->index(source_row, ModelSud::ColKommentar, source_parent);
            accept = sourceModel()->data(idx).toString().contains(rx);
        }
        if (!accept)
        {
            ModelSud* modelSud = dynamic_cast<ModelSud*>(sourceModel());
            if (modelSud)
            {
                idx = sourceModel()->index(source_row, ModelSud::ColID, source_parent);
                QRegExp sudReg = QRegExp(QString("^%1$").arg(sourceModel()->data(idx).toInt()));

                ProxyModel modelMalzschuettung;
                modelMalzschuettung.setSourceModel(modelSud->bh->modelMalzschuettung());
                modelMalzschuettung.setFilterKeyColumn(ModelMalzschuettung::ColSudID);
                modelMalzschuettung.setFilterRegExp(sudReg);
                for (int i = 0; i < modelMalzschuettung.rowCount(); i++)
                {
                    QString name = modelMalzschuettung.index(i, ModelMalzschuettung::ColName).data().toString();
                    accept = name.contains(rx);
                    if (accept)
                        break;
                }
                if (!accept)
                {
                    ProxyModel modelHopfengaben;
                    modelHopfengaben.setSourceModel(modelSud->bh->modelHopfengaben());
                    modelHopfengaben.setFilterKeyColumn(ModelHopfengaben::ColSudID);
                    modelHopfengaben.setFilterRegExp(sudReg);
                    for (int i = 0; i < modelHopfengaben.rowCount(); i++)
                    {
                        QString name = modelHopfengaben.index(i, ModelHopfengaben::ColName).data().toString();
                        accept = name.contains(rx);
                        if (accept)
                            break;
                    }
                }
                if (!accept)
                {
                    ProxyModel modelHefegaben;
                    modelHefegaben.setSourceModel(modelSud->bh->modelHefegaben());
                    modelHefegaben.setFilterKeyColumn(ModelHefegaben::ColSudID);
                    modelHefegaben.setFilterRegExp(sudReg);
                    for (int i = 0; i < modelHefegaben.rowCount(); i++)
                    {
                        QString name = modelHefegaben.index(i, ModelHefegaben::ColName).data().toString();
                        accept = name.contains(rx);
                        if (accept)
                            break;
                    }
                }
                if (!accept)
                {
                    ProxyModel modelWeitereZutatenGaben;
                    modelWeitereZutatenGaben.setSourceModel(modelSud->bh->modelWeitereZutatenGaben());
                    modelWeitereZutatenGaben.setFilterKeyColumn(ModelWeitereZutatenGaben::ColSudID);
                    modelWeitereZutatenGaben.setFilterRegExp(sudReg);
                    for (int i = 0; i < modelWeitereZutatenGaben.rowCount(); i++)
                    {
                        QString name = modelWeitereZutatenGaben.index(i, ModelWeitereZutatenGaben::ColName).data().toString();
                        accept = name.contains(rx);
                        if (accept)
                            break;

                    }
                }
                if (!accept)
                {
                    ProxyModel modelTags;
                    modelTags.setSourceModel(modelSud->bh->modelTags());
                    modelTags.setFilterKeyColumn(ModelTags::ColSudID);
                    modelTags.setFilterRegExp(QString("^(%1|-.*)$").arg(sourceModel()->data(idx).toInt()));
                    for (int i = 0; i < modelTags.rowCount(); i++)
                    {
                        QString text = modelTags.index(i, ModelTags::ColKey).data().toString();
                        accept = text.contains(rx);
                        if (accept)
                            break;
                        text = modelTags.index(i, ModelTags::ColValue).data().toString();
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
