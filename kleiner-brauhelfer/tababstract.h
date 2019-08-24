#ifndef TABABSTRACT_H
#define TABABSTRACT_H

#include <QWidget>

class TabAbstract : public QWidget
{
    Q_OBJECT

public:
    explicit TabAbstract(QWidget *parent = nullptr);

    virtual void saveSettings();
    virtual void restoreView();

    void setTabActive(bool active);
    bool isTabActive() const;

protected:
    virtual void onTabActivated();
    virtual void onTabDeactivated();

private:
    bool mIsTabActive;
};

#endif // TABABSTRACT_H
