#ifndef DOCKTITLEBAR_H
#define DOCKTITLEBAR_H

#include <QWidget>

class QDockWidget;
class QLabel;

class DockTitleBar : public QWidget
{
    Q_OBJECT

public:
    explicit DockTitleBar(QWidget *parent = nullptr);
    explicit DockTitleBar(bool minimalSize, QWidget *parent = nullptr);

private slots:
    void updateLayout();

private:
    QDockWidget *mDockWidget;
    QLabel *mIconLabel;
    QLabel *mTitleLabel;
    bool mMinimalSize;
};

#endif // DOCKTITLEBAR_H
