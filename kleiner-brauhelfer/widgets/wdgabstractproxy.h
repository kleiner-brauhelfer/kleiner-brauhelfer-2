#ifndef WDGABSTRACTPROXY_H
#define WDGABSTRACTPROXY_H

#include <QWidget>

class ProxyModel;

class WdgAbstractProxy : public QWidget
{
    Q_OBJECT

public:
    explicit WdgAbstractProxy(ProxyModel* model, int row, QLayout* parentLayout = nullptr, QWidget *parent = nullptr);

    int row() const;
    QVariant data(int col) const;
    bool setData(int col, const QVariant& value);

public slots:
    bool remove();
    bool moveUp();
    bool moveDown();

protected:
    ProxyModel* mModel;
    const int mRow;
    QLayout* mParentLayout;
};

#endif // WDGABSTRACTPROXY_H
