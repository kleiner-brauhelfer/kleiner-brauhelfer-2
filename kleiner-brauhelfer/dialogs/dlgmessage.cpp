#include "dlgmessage.h"
#include "ui_dlgmessage.h"
#include <QDebug>
#include "settings.h"

extern Settings* gSettings;

DlgMessage::DlgMessage(QWidget *parent, const QString &url) :
	QDialog(parent),
    ui(new Ui::DlgMessage),
    url(url)
{
    ui->setupUi(this);
}

DlgMessage::~DlgMessage()
{
	delete ui;
}

void DlgMessage::getMessage()
{
    Meldungen.clear();
    reply = qnam.get(QNetworkRequest(url));
    connect(reply, SIGNAL(finished()), this, SLOT(httpFinished()));
}

bool DlgMessage::hasMessage() const
{
    return Meldungen.count() > 0;
}

void DlgMessage::httpFinished()
{
    if (reply->error() == QNetworkReply::NoError)
    {
        QString rohMeldungen = reply->readAll();
        Meldungen = rohMeldungen.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);

        checkFilter();

        if (Meldungen.count() > 0)
        {
			aktMsg = 0;
			MeldungEintragen(aktMsg);
		}
        ui->button_next->setText(Meldungen.count() > 1 ? tr("Nächste Meldung") : tr("OK"));
	}
    emit finished();
}

void DlgMessage::checkFilter()
{
    int version = VER_MAJ * 1000000 + VER_MIN * 10000 + VER_PAT * 100;
    QStringList ml;
    gSettings->beginGroup("MessageIgnoreList");
    for (const QString &meldung : Meldungen)
    {
        QStringList slm = meldung.split("##");
        if (slm.count() >= 5)
        {
            bool ignore = gSettings->value(QString::number(slm[0].toInt()), false).toBool();
            if (!ignore)
            {
                if ((slm[2].toInt() <= version) && (slm[3].toInt() >= version))
                    ml.append(meldung);
                else if ((slm[2].toInt() <= version) && (slm[3].toInt() == 0))
                    ml.append(meldung);
			}
		}
    }
    gSettings->endGroup();
	Meldungen = ml;
}

void DlgMessage::MeldungEintragen(int nr)
{
    if (nr < Meldungen.count())
    {
		aktID = Meldungen[nr].split("##")[0].toInt();
		ui->textEdit->setText(Meldungen[nr].split("##")[1]);
	}
}

void DlgMessage::on_button_next_clicked()
{
    if (ui->checkBox_ignor->isChecked())
    {
        gSettings->beginGroup("MessageIgnoreList");
        gSettings->setValue(QString::number(aktID), true);
        gSettings->endGroup();
    }

	aktMsg++;
    if (aktMsg + 1 == Meldungen.count())
        ui->button_next->setText(tr("OK"));

	if (aktMsg < Meldungen.count())
		MeldungEintragen(aktMsg);
	else
		close();
}
