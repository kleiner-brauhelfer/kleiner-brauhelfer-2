#include "bewertungtreeproxymodel.h"
#include "modelbewertungen.h"

BewertungTreeProxyModel::BewertungTreeProxyModel(QObject *parent) :
    TreeProxyModel(parent)
{
}

QVariant BewertungTreeProxyModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole && isTopLevel(index))
    {
        if (index.column() == ModelBewertungen::ColSterne)
        {
            ModelBewertungen* model = static_cast<ModelBewertungen*>(sourceModel());
            QVariant sudName = TreeProxyModel::data(index.siblingAtColumn(ModelBewertungen::ColSudName));
            QVariant sudId = model->getValueFromSameRow(ModelBewertungen::ColSudName, sudName, ModelBewertungen::ColSudID);
            return model->mean(sudId);
        }
    }
    return TreeProxyModel::data(index, role);
}
