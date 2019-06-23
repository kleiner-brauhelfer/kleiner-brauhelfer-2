#include "wdgbewertung.h"
#include "ui_wdgbewertung.h"
#include <QMouseEvent>
#include "brauhelfer.h"

extern Brauhelfer* bh;

WdgBewertung::WdgBewertung(int index, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WdgBewertung),
    mIndex(index),
    mActive(false)
{
	ui->setupUi(this);
    foreach (QWidget *wdg, findChildren<QWidget*>())
    {
        wdg->setFocusPolicy(Qt::StrongFocus);
        wdg->installEventFilter(this);
    }
    ui->wdgRating->setReadOnly(true);
    ui->wdgRating->installEventFilter(this);
    updateValues();
    connect(bh, SIGNAL(discarded()), this, SLOT(updateValues()));
    connect(bh->sud()->modelBewertungen(), SIGNAL(modified()), this, SLOT(updateValues()));
    connect(bh->sud(), SIGNAL(modified()), this, SLOT(updateValues()));
}

WdgBewertung::~WdgBewertung()
{
    delete ui;
}

bool WdgBewertung::eventFilter(QObject *obj, QEvent *e)
{
    if(e->type() == QEvent::MouseButtonPress ||
       e->type() == QEvent::MouseButtonDblClick)
    {
        emit clicked(mIndex);
    }
    return QWidget::eventFilter(obj, e);
}

void WdgBewertung::setActive(bool active)
{
    mActive = active;
}

bool WdgBewertung::isActive() const
{
    return mActive;
}

QVariant WdgBewertung::data(const QString &fieldName) const
{
    return bh->sud()->modelBewertungen()->data(mIndex, fieldName);
}

bool WdgBewertung::setData(const QString &fieldName, const QVariant &value)
{
    return bh->sud()->modelBewertungen()->setData(mIndex, fieldName, value);
}

void WdgBewertung::mousePressEvent(QMouseEvent *e)
{
    QWidget::mousePressEvent(e);
    emit clicked(mIndex);
}

void WdgBewertung::updateValues()
{
    ui->lblInfo->setText(tr("Woche") + " " + data("Woche").toString() + " (" + QLocale().toString(data("Datum").toDate(), QLocale::ShortFormat) + ")");
    ui->wdgRating->setSterne(data("Sterne").toInt());
    if (isActive())
    {
        QFont f = font();
        f.setBold(true);
        ui->lblInfo->setFont(f);
    }
    else
    {
        ui->lblInfo->setFont(font());
    }
}

void WdgBewertung::remove()
{
    bh->sud()->modelBewertungen()->removeRow(mIndex);
}

void WdgBewertung::on_btnLoeschen_clicked()
{
    remove();
}
