#include "wdgrating.h"
#include "ui_wdgrating.h"
#include "settings.h"

extern Settings* gSettings;

WdgRating::WdgRating(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WdgRating),
    mReadOnly(false)
{    
    ui->setupUi(this);
    mPixmapStar = QIcon(":/images/star.svg").pixmap(ui->btnStar1->size());
    if (gSettings->theme() == Qt::ColorScheme::Dark)
        mPixmapNoStar = QIcon(":/images/dark/star_gr.svg").pixmap(ui->btnStar1->size());
    else
        mPixmapNoStar = QIcon(":/images/light/star_gr.svg").pixmap(ui->btnStar1->size());
    setSterne(0);
}

WdgRating::~WdgRating()
{
    delete ui;
}

void WdgRating::setReadOnly(bool readonly)
{
    mReadOnly = readonly;
    if (readonly)
    {
        ui->btnStar1->installEventFilter(this);
        ui->btnStar2->installEventFilter(this);
        ui->btnStar3->installEventFilter(this);
        ui->btnStar4->installEventFilter(this);
        ui->btnStar5->installEventFilter(this);
    }
    else
    {
        ui->btnStar1->removeEventFilter(this);
        ui->btnStar2->removeEventFilter(this);
        ui->btnStar3->removeEventFilter(this);
        ui->btnStar4->removeEventFilter(this);
        ui->btnStar5->removeEventFilter(this);
    }
}

bool WdgRating::eventFilter(QObject *obj, QEvent *e)
{
    if (mReadOnly)
    {
        if(e->type() == QEvent::MouseButtonPress ||
           e->type() == QEvent::MouseButtonDblClick)
        {
            e->ignore();
            return true;
        }
    }
    return QWidget::eventFilter(obj, e);
}

bool WdgRating::isReadOnly() const
{
    return mReadOnly;
}

void WdgRating::setSterne(int sterne)
{
    mSterne = sterne;
    ui->btnStar1->setIcon(mSterne > 0 ? mPixmapStar : mPixmapNoStar);
    ui->btnStar2->setIcon(mSterne > 1 ? mPixmapStar : mPixmapNoStar);
    ui->btnStar3->setIcon(mSterne > 2 ? mPixmapStar : mPixmapNoStar);
    ui->btnStar4->setIcon(mSterne > 3 ? mPixmapStar : mPixmapNoStar);
    ui->btnStar5->setIcon(mSterne > 4 ? mPixmapStar : mPixmapNoStar);
}

int WdgRating::sterne() const
{
    return mSterne;
}

void WdgRating::on_btnStar1_clicked()
{
    if (!mReadOnly)
    {
        setSterne(mSterne == 1 ? 0 : 1);
        emit clicked(mSterne);
    }
}

void WdgRating::on_btnStar2_clicked()
{
    if (!mReadOnly)
    {
        setSterne(mSterne == 2 ? 0 : 2);
        emit clicked(mSterne);
    }
}

void WdgRating::on_btnStar3_clicked()
{
    if (!mReadOnly)
    {
        setSterne(mSterne == 3 ? 0 : 3);
        emit clicked(mSterne);
    }
}

void WdgRating::on_btnStar4_clicked()
{
    if (!mReadOnly)
    {
        setSterne(mSterne == 4 ? 0 : 4);
        emit clicked(mSterne);
    }
}

void WdgRating::on_btnStar5_clicked()
{
    if (!mReadOnly)
    {
        setSterne(mSterne == 5 ? 0 : 5);
        emit clicked(mSterne);
    }
}
