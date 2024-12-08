#ifndef SUDWOCHE_H
#define SUDWOCHE_H

#include "textdelegate.h"

class SudWocheDelegate : public TextDelegate
{
    Q_OBJECT

public:
    explicit SudWocheDelegate(QObject *parent = nullptr);
protected:
    void initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const Q_DECL_OVERRIDE;
};

#endif // SUDWOCHE_H
