#ifndef PUSHBUTTON_H
#define PUSHBUTTON_H

#include <QPushButton>

class PushButton : public QPushButton
{
    Q_OBJECT

public:
    explicit PushButton(QWidget *parent = nullptr);
    void setAction(QAction *action);
    void setDefaultPalette(const QPalette &p);
    void setError(bool e);
private:
    bool event(QEvent *event) Q_DECL_OVERRIDE;
    void updatePalette();
private slots:
    void onActionChanged();
private:
    QPalette mDefaultPalette;
    QAction *mAction;
    bool mError;
};

#endif // PUSHBUTTON_H
