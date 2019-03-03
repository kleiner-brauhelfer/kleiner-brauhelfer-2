#ifndef DLGMESSAGE_H
#define DLGMESSAGE_H

#include <QDialog>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>

namespace Ui {
class DlgMessage;
}

class DlgMessage : public QDialog
{
    Q_OBJECT

public:
    explicit DlgMessage(QWidget *parent = nullptr, const QString &url = QString());
	~DlgMessage();
    void getMessage();
    bool hasMessage() const;

signals:
    void finished();

private slots:
    void httpFinished();
	void on_button_next_clicked();
	
private:
	void checkFilter();
	void MeldungEintragen(int nr);

private:
	Ui::DlgMessage *ui;
	QUrl url;
    QNetworkAccessManager qnam;
	QNetworkReply *reply;
    QStringList Meldungen;
    int aktMsg;
    int aktID;
};

#endif // DLGMESSAGE_H
