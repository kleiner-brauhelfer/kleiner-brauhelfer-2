#include "wdgsudstatus.h"
#include <QPainter>
#include <QStaticText>
#include "settings.h"
#include "brauhelfer.h"

extern Settings* gSettings;

WdgSudStatus::WdgSudStatus(QWidget *parent) :
    QWidget(parent),
    mSud(nullptr)
{
}

QSize WdgSudStatus::minimumSizeHint() const
{
    return QSize(18, 18);
}

QSize WdgSudStatus::sizeHint() const
{
    return QSize(400, 18);
}

void WdgSudStatus::setSud(SudObject *sud)
{
    mSud = sud;
    connect(mSud, &SudObject::loadedChanged, this, [this](){update();});
    connect(mSud, &SudObject::dataChanged, this, [this](){update();});
}

SudObject* WdgSudStatus::getSud() const
{
    return mSud;
}


void WdgSudStatus::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    if (!mSud)
        return;

    const bool enabled = mSud->isLoaded();
    const Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(mSud->getStatus());

    const QColor colorBase = palette().color(QPalette::ColorGroup::Disabled, QPalette::Text);
    const QColor colorText = palette().color(enabled ? QPalette::ColorGroup::Active : QPalette::ColorGroup::Disabled, QPalette::Text);

    const bool isHorizontal = width() > height();

    QString str;
    QPainter painter(this);
    QFont font = painter.font();
    QFontMetrics fm(font);
    QLocale locale = QLocale();
    QRect bbox;

    const int d = 14;
    const int dh = d/2;

    if (isHorizontal)
    {
        int y = height()/2;

        int x0 = 10;
        if (enabled)
        {
            str = mSud->getSudname();
            font.setBold(true);
            bbox = QFontMetrics(font).boundingRect(str);
            painter.setPen(QPen(enabled ? gSettings->colorRezept : colorBase, 2, enabled ? Qt::SolidLine : Qt::DashLine));
            painter.drawLine(10, y, 20, y);
            painter.setPen(colorText);
            painter.setFont(font);
            x0 += bbox.width() + 30;
            painter.drawStaticText(20 + 10, y - bbox.height()/2, QStaticText(str));
            font.setBold(false);
            painter.setFont(font);
        }

        int w = width() - x0;
        int x1 = 1*w/5 + x0;
        int x2 = 2*w/5 + x0;
        int x3 = 3*w/5 + x0;
        int x4 = 4*w/5 + x0;

        painter.setPen(QPen(enabled ? gSettings->colorRezept : colorBase, 2, enabled ? Qt::SolidLine : Qt::DashLine));
        painter.drawLine(x0, y, x1 - dh - 10, y);

        str = tr("Rezept");
        bbox = fm.boundingRect(str);
        painter.setPen(QPen(enabled ? gSettings->colorGebraut : colorBase, 2, (enabled && status >= Brauhelfer::SudStatus::Gebraut) ? Qt::SolidLine : Qt::DashLine));
        painter.drawLine(x1 + bbox.width() + d + 10, y, x2 - dh - 10, y);
        painter.setPen(colorBase);
        painter.setPen(colorText);
        painter.drawStaticText(x1 + d, y - bbox.height()/2, QStaticText(str));

        if (status >= Brauhelfer::SudStatus::Gebraut && w > 800)
            str = tr("Gebraut") + " " + locale.toString(mSud->getBraudatum().date(), QLocale::ShortFormat);
        else
            str = tr("Gebraut");
        bbox = fm.boundingRect(str);
        painter.setPen(QPen(enabled ? gSettings->colorAbgefuellt : colorBase, 2, (enabled && status >= Brauhelfer::SudStatus::Abgefuellt) ? Qt::SolidLine : Qt::DashLine));
        painter.drawLine(x2 + bbox.width() + d + 10, y, x3 - dh - 10, y);
        painter.setPen(colorText);
        painter.drawStaticText(x2 + d, y - bbox.height()/2, QStaticText(str));

        if (status >= Brauhelfer::SudStatus::Abgefuellt && w > 800)
            str = tr("Abgefüllt") + " " + locale.toString(mSud->getAbfuelldatum().date(), QLocale::ShortFormat);
        else
            str = tr("Abgefüllt");
        bbox = fm.boundingRect(str);
        painter.setPen(QPen(enabled ? gSettings->colorAusgetrunken : colorBase, 2, (enabled && status >= Brauhelfer::SudStatus::Verbraucht) ? Qt::SolidLine : Qt::DashLine));
        painter.drawLine(x3 + bbox.width() + d + 10, y, x4 - dh - 10, y);
        painter.setPen(colorText);
        painter.drawStaticText(x3 + d, y - bbox.height()/2, QStaticText(str));

        str = tr("Ausgetrunken");
        bbox = fm.boundingRect(str);
        painter.setPen(QPen(colorBase, 2, (enabled && status >= Brauhelfer::SudStatus::Verbraucht) ? Qt::SolidLine : Qt::DashLine));
        painter.drawLine(x4 + bbox.width() + d + 10, y, width()-20, y);
        painter.setPen(colorText);
        painter.drawStaticText(x4 + d, y - bbox.height()/2, QStaticText(str));

        painter.setPen(QPen(colorBase, 2));
        painter.setBrush(palette().base().color());
        painter.drawEllipse(x1-dh, y-dh, d, d);
        painter.drawEllipse(x2-dh, y-dh, d, d);
        painter.drawEllipse(x3-dh, y-dh, d, d);
        painter.drawEllipse(x4-dh, y-dh, d, d);

        if (enabled)
        {
            int d2 = d - 4;
            painter.setPen(Qt::NoPen);
            painter.setBrush(gSettings->colorRezept);
            painter.drawEllipse(x1-d2/2, y-d2/2, d2, d2);
            if (status >= Brauhelfer::SudStatus::Gebraut)
            {
                painter.setBrush(gSettings->colorGebraut);
                painter.drawEllipse(x2-d2/2, y-d2/2, d2, d2);
            }
            if (status >= Brauhelfer::SudStatus::Abgefuellt)
            {
                painter.setBrush(gSettings->colorAbgefuellt);
                painter.drawEllipse(x3-d2/2, y-d2/2, d2, d2);
            }
            if (status >= Brauhelfer::SudStatus::Verbraucht)
            {
                painter.setBrush(gSettings->colorAusgetrunken);
                painter.drawEllipse(x4-d2/2, y-d2/2, d2, d2);
            }
        }
    }
    else
    {
        int x = width()/2;

        int y0 = 10;
        if (enabled)
        {
            str = mSud->getSudname();
            font.setBold(true);
            bbox = QFontMetrics(font).boundingRect(str);
            painter.setPen(QPen(enabled ? gSettings->colorRezept : colorBase, 2, enabled ? Qt::SolidLine : Qt::DashLine));
            painter.drawLine(x, 10, x, 20);
            painter.setPen(colorText);
            painter.setFont(font);
            y0 += bbox.width() + 30;
            painter.save();
            painter.translate(x, 30 + bbox.width() /2);
            painter.rotate(-90);
            painter.drawStaticText(-bbox.width() / 2, -bbox.height() / 2, QStaticText(str));
            painter.restore();
            font.setBold(false);
            painter.setFont(font);
        }

        int h = height() - y0;
        int y1 = 1*h/5 + y0;
        int y2 = 2*h/5 + y0;
        int y3 = 3*h/5 + y0;
        int y4 = 4*h/5 + y0;

        painter.setPen(QPen(enabled ? gSettings->colorRezept : colorBase, 2, enabled ? Qt::SolidLine : Qt::DashLine));
        painter.drawLine(x, y0, x, y1 - dh - 10);

        str = tr("Rezept");
        bbox = fm.boundingRect(str);
        painter.setPen(QPen(enabled ? gSettings->colorGebraut : colorBase, 2, (enabled && status >= Brauhelfer::SudStatus::Gebraut) ? Qt::SolidLine : Qt::DashLine));
        painter.drawLine(x, y1 + bbox.width() + d + 10, x, y2 - dh - 10);
        painter.setPen(colorBase);
        painter.setPen(colorText);
        painter.save();
        painter.translate(x, y1 + d);
        painter.rotate(-90);
        painter.drawStaticText(-bbox.width(), -bbox.height() / 2, QStaticText(str));
        painter.restore();

        if (status >= Brauhelfer::SudStatus::Gebraut && h > 800)
            str = tr("Gebraut") + " " + locale.toString(mSud->getBraudatum().date(), QLocale::ShortFormat);
        else
            str = tr("Gebraut");
        bbox = fm.boundingRect(str);
        painter.setPen(QPen(enabled ? gSettings->colorAbgefuellt : colorBase, 2, (enabled && status >= Brauhelfer::SudStatus::Abgefuellt) ? Qt::SolidLine : Qt::DashLine));
        painter.drawLine(x, y2 + bbox.width() + d + 10, x, y3 - dh - 10);
        painter.setPen(colorText);
        painter.save();
        painter.translate(x, y2 + d);
        painter.rotate(-90);
        painter.drawStaticText(-bbox.width(), -bbox.height() / 2, QStaticText(str));
        painter.restore();

        if (status >= Brauhelfer::SudStatus::Abgefuellt && h > 800)
            str = tr("Abgefüllt") + " " + locale.toString(mSud->getAbfuelldatum().date(), QLocale::ShortFormat);
        else
            str = tr("Abgefüllt");
        bbox = fm.boundingRect(str);
        painter.setPen(QPen(enabled ? gSettings->colorAusgetrunken : colorBase, 2, (enabled && status >= Brauhelfer::SudStatus::Verbraucht) ? Qt::SolidLine : Qt::DashLine));
        painter.drawLine(x, y3 + bbox.width() + d + 10, x, y4 - dh - 10);
        painter.setPen(colorText);
        painter.save();
        painter.translate(x, y3 + d);
        painter.rotate(-90);
        painter.drawStaticText(-bbox.width(), -bbox.height() / 2, QStaticText(str));
        painter.restore();

        str = tr("Ausgetrunken");
        bbox = fm.boundingRect(str);
        painter.setPen(QPen(colorBase, 2, (enabled && status >= Brauhelfer::SudStatus::Verbraucht) ? Qt::SolidLine : Qt::DashLine));
        painter.drawLine(x, y4 + bbox.width() + d + 10, x, height()-20);
        painter.setPen(colorText);
        painter.save();
        painter.translate(x, y4 + d);
        painter.rotate(-90);
        painter.drawStaticText(-bbox.width(), -bbox.height() / 2, QStaticText(str));
        painter.restore();

        painter.setPen(QPen(colorBase, 2));
        painter.setBrush(palette().base().color());
        painter.drawEllipse(x-dh, y1-dh, d, d);
        painter.drawEllipse(x-dh, y2-dh, d, d);
        painter.drawEllipse(x-dh, y3-dh, d, d);
        painter.drawEllipse(x-dh, y4-dh, d, d);

        if (enabled)
        {
            int d2 = d - 4;
            painter.setPen(Qt::NoPen);
            painter.setBrush(gSettings->colorRezept);
            painter.drawEllipse(x-d2/2, y1-d2/2, d2, d2);
            if (status >= Brauhelfer::SudStatus::Gebraut)
            {
                painter.setBrush(gSettings->colorGebraut);
                painter.drawEllipse(x-d2/2, y2-d2/2, d2, d2);
            }
            if (status >= Brauhelfer::SudStatus::Abgefuellt)
            {
                painter.setBrush(gSettings->colorAbgefuellt);
                painter.drawEllipse(x-d2/2, y3-d2/2, d2, d2);
            }
            if (status >= Brauhelfer::SudStatus::Verbraucht)
            {
                painter.setBrush(gSettings->colorAusgetrunken);
                painter.drawEllipse(x-d2/2, y4-d2/2, d2, d2);
            }
        }
    }
}
