#include "wdgbewertung.h"
#include "ui_wdgbewertung.h"
#include <QMouseEvent>
#include "brauhelfer.h"
#include "commands/undostack.h"

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
    connect(bh, &Brauhelfer::modified, this, &WdgBewertung::updateValues);
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

QVariant WdgBewertung::data(int col) const
{
    return bh->sud()->modelBewertungen()->data(mIndex, col);
}

bool WdgBewertung::setData(int col, const QVariant &value)
{
    gUndoStack->push(new SetModelDataCommand(bh->sud()->modelBewertungen(), mIndex, col, value));
    return true;
}

void WdgBewertung::mousePressEvent(QMouseEvent *e)
{
    QWidget::mousePressEvent(e);
    emit clicked(mIndex);
}

void WdgBewertung::updateValues()
{
    ui->lblInfo->setText(tr("Woche") + " " + data(ModelBewertungen::ColWoche).toString() +
                         " (" + QLocale().toString(data(ModelBewertungen::ColDatum).toDate(), QLocale::ShortFormat) + ")");
    ui->wdgRating->setSterne(data(ModelBewertungen::ColSterne).toInt());
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
