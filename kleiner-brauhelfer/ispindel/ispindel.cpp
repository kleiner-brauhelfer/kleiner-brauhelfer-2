#include "ispindel.h"

extern Settings* gSettings;

#define showDebug 1

Ispindel::Ispindel(QObject *parent) : QObject(parent),
    m_dbIspindelQuery(nullptr),
    mSqlModel(nullptr)
{
    // load Settings
    gSettings->beginGroup("iSpindel");
    mDbDriver = gSettings->value("Driver", QVariant("MySQL ODBC 8.0 Unicode Driver")).toString();
    mDbServer = gSettings->value("Server", QVariant("127.0.0.1")).toString();
    mDbDatabase = gSettings->value("Database", QVariant("iSpindle")).toString();
    mDbUsername = gSettings->value("Username", QVariant("iSpindle")).toString();
    mDbPwd = gSettings->value("Password", QVariant("passwort")).toString();

    mDbTableCalibration = gSettings->value("TableCalibration", QVariant()).toString();
    mDbTableData = gSettings->value("TableData", QVariant()).toString();

    gSettings->endGroup();

    //connectDatabaseIspindel();

}

Ispindel::~Ispindel()
{

}

void Ispindel::saveSettings()
{
    gSettings->beginGroup("iSpindel");
    gSettings->setValue("Driver", QVariant(mDbDriver));
    gSettings->setValue("Server", QVariant(mDbServer));
    gSettings->setValue("Database", QVariant(mDbDatabase));
    gSettings->setValue("Username", QVariant(mDbUsername));
    gSettings->setValue("Password", QVariant(mDbPwd));

    gSettings->setValue("TableCalibration", QVariant(mDbTableCalibration));
    gSettings->setValue("TableData", QVariant(mDbTableData));
    gSettings->endGroup();
}

bool Ispindel::connectDatabaseIspindel(bool showMessage)
{
    if(m_dbIspindel.isOpen())
    {
        m_dbIspindel.close();
        m_dbIspindel.removeDatabase(buildConnectionString());
    }

    m_dbIspindel = QSqlDatabase::addDatabase("QODBC");
    m_dbIspindel.setDatabaseName(buildConnectionString());
    if(!m_dbIspindel.open())
    {
        if(showMessage)
        {
            QMessageBox::warning(nullptr, tr("Verbindung zur Datenbank für die iSpindel konnte nicht hergestellt werden."),
                                  tr("Bitte öffne die Einstellung zur iSpindel und versuche ein Verbindung zur Datenbank herzustellen."),
                                  QMessageBox::Ok);
        }
        return false;
    }
    else {
        qDebug() << QString("%1 send at %2").arg("\"sig_DatabaseIspindelConnected\"").arg(QTime::currentTime().toString("hh:mm:ss:zzz"));
        m_dbIspindelQuery = new QSqlQuery(m_dbIspindel);
        emit sig_DatabaseIspindelConnected();
        return true;
    }
}

QString Ispindel::buildConnectionString() const
{
    QString tmpConnectionString = QString("Driver={%1};").arg(mDbDriver);
    tmpConnectionString.append(QString("server=%1;").arg(mDbServer));
    tmpConnectionString.append(QString("database=%1;").arg(mDbDatabase));
    tmpConnectionString.append(QString("uid=%1;").arg(mDbUsername));
    tmpConnectionString.append(QString("pwd=%1;").arg(mDbPwd));
    qDebug() << "Connection String: " << tmpConnectionString;
    return tmpConnectionString;
}

QString Ispindel::getDbTableData() const
{
    return mDbTableData;
}

void Ispindel::setDbTableData(const QString &dbTableData)
{
    mDbTableData = dbTableData;
}

QString Ispindel::getDbTableCalibration() const
{
    return mDbTableCalibration;
}

void Ispindel::setDbTableCalibration(const QString &dbTableCalibration)
{
    mDbTableCalibration = dbTableCalibration;
}

QSqlError Ispindel::getLastSqlError()
{
    return  m_dbIspindel.lastError();
}

QSqlQuery* Ispindel::getQuery()
{
    return m_dbIspindelQuery;
}

QSqlRecord Ispindel::getRecordSetQuery(QString query)
{
    qDebug() << QString("%1 - called").arg(Q_FUNC_INFO);
    execQueryAndCheckError(query);
    return m_dbIspindelQuery->record();
}

QStringList Ispindel::getStringListQuery(QString query)
{
    QStringList tmpList;

    execQueryAndCheckError(query);

    while(m_dbIspindelQuery->next())
    {
        tmpList.append(m_dbIspindelQuery->record().value(0).toString());
    }
    return tmpList;
}

// Sql-Funktionen
// gibt die verfügbaren Tabellennamen in der DB zurück
QStringList Ispindel::getTablenames()
{
    qDebug() << QString("%1 - called").arg(Q_FUNC_INFO);
    QStringList tmpList;
    QString query = "SELECT TABLE_NAME FROM information_schema.tables WHERE table_schema ='iSpindle';";

    execQueryAndCheckError(query);

    while(m_dbIspindelQuery->next())
        tmpList.append(m_dbIspindelQuery->record().value(0).toString());

    return tmpList;
}

QStringList Ispindel::getVerfuegbareSpindeln()
{
    qDebug() << QString("%1 - called").arg(Q_FUNC_INFO);
    QStringList tmpStringList = getStringListQuery(
                QString("SELECT Name FROM %1.%2 group by Name;").arg(mDbDatabase).arg(mDbTableData));

    for (int i = 0; i < tmpStringList.size(); i++) {
        if(tmpStringList.at(i).isEmpty())
            continue;

        //get ID from Table
        QString query = QString(
                    "SELECT ID FROM %1.%2 WHERE NAME LIKE '%3' LIMIT 1")
                .arg(mDbDatabase)
                .arg(mDbTableData)
                .arg(tmpStringList.at(i));

        int ID = 0;

        execQueryAndCheckError(query);
        while(m_dbIspindelQuery->next())
            ID = m_dbIspindelQuery->record().value("ID").toInt();

        //get CalibtraionData from table with id
        query = QString(
                    "SELECT * FROM %1.%2 WHERE ID = %3")
                .arg(mDbDatabase)
                .arg(mDbTableCalibration)
                .arg(ID);

        execQueryAndCheckError(query);
        while(m_dbIspindelQuery->next())
        {
            QVariantMap calibData({{ "Name", QString("%1").arg(tmpStringList.at(i))},
                                   { "ID", ID},
                                   { "x_2", m_dbIspindelQuery->record().value("const1")},
                                   { "x_1", m_dbIspindelQuery->record().value("const2")},
                                   { "x_0", m_dbIspindelQuery->record().value("const3")}});
            mIdCalibrationDataDevice.append(calibData);
        }
    }
    return tmpStringList;
}

int Ispindel::getAnzahlDatensaetze(QString NameSpindel)
{
    qDebug() << QString("%1 - called").arg(Q_FUNC_INFO);
    QList<QVariant> tmpLst;
    QString query = QString("SELECT count(Name) FROM %1.%2 "
                            "where Name like '%3';")
                          .arg(mDbDatabase)
                          .arg(mDbTableData)
                          .arg(NameSpindel);

    execQueryAndCheckError(query);
    while(m_dbIspindelQuery->next())
        tmpLst.append(m_dbIspindelQuery->record().value(0));

    return static_cast<int>(tmpLst.first().toLongLong());
}

// gibt Zeitpunkt, Name und Rezept der letzten ResetFlags == 1 zurück
QList<QPair<QDateTime, QStringList> > Ispindel::getResetFlags(QString NameSpindel)
{
    qDebug() << QString("%1 - called").arg(Q_FUNC_INFO);

    QList<QPair<QDateTime, QStringList> > tmpReturn;
    QString query =QString("SELECT * FROM %1.%2 "
                           "where ResetFlag = 1 AND (Name like \"%3\" or Name IS NULL or Name =\"\") "
                           "order by TimeStamp desc;")
                   .arg(mDbDatabase)
                   .arg(mDbTableData)
                   .arg(NameSpindel);

    execQueryAndCheckError(query);
    while(m_dbIspindelQuery->next())
    {
        QStringList lst;
        lst.append(m_dbIspindelQuery->record().value("Name").toString());
        lst.append(m_dbIspindelQuery->record().value("Recipe").toString());
        tmpReturn << qMakePair(m_dbIspindelQuery->record().value("TimeStamp").toDateTime(),
                               lst);
    }

    return tmpReturn;
}

void Ispindel::setResetFlag(const QString NameSpindel, const int Recipe)
{
    QString query = QString("INSERT INTO %1.%2 (`Timestamp`, `Name`, `ResetFlag`, `Recipe`) "
                           "VALUES(NOW(), '%3', 1, %4);")
            .arg(mDbDatabase)
            .arg(mDbTableData)
            .arg(NameSpindel)
            .arg(Recipe);

    execQueryAndCheckError(query);
}

// löscht Datensätze zwischen zwei Zeitstempeln
void Ispindel::deleteDatabaseBetweenTimestamps(const QString NameSpindel,
                                               const QDateTime FirstDate,
                                               const QDateTime LastDate)
{
    QString query = QString("DELETE FROM %1.%2 "
                           "WHERE `Timestamp` "
                            "BETWEEN "
                            "'%3' "
                            "AND "
                            "'%4' "
                            "AND "
                            "NAME LIKE '%5';")
            .arg(mDbDatabase)
            .arg(mDbTableData)
            .arg(LastDate.toString("yyyy-MM-dd hh:mm:ss"))
            .arg(FirstDate.toString("yyyy-MM-dd hh:mm:ss"))
            .arg(NameSpindel);

    execQueryAndCheckError(query);
}

// Gibt die Daten zwischen zwei Zeitpunkten zurück
QList<QVariantMap> Ispindel::getPlatoBetweenTimestamps(QString NameSpindel, QDateTime DateNew, QDateTime DateOld)
{
    qDebug() << QString("%1 - called").arg(Q_FUNC_INFO);

    execQueryAndCheckError(getQueryGetPlatoFromDb(NameSpindel, DateNew, DateOld));

    // Get Datasets from Table
    QList<QVariantMap> returnFilteredDataset;
    while(m_dbIspindelQuery->next())
    {
        returnFilteredDataset.append(QVariantMap({{"Datum", m_dbIspindelQuery->record().value("Timestamp")},
                                                {"Temp", m_dbIspindelQuery->record().value("Temperature")},
                                                {"Plato", m_dbIspindelQuery->record().value("Plato")}}));
    }

    return returnFilteredDataset;
}

QSqlQueryModel* Ispindel::getPlatoBetweenTimestampsAsModel(QString NameSpindel, QDateTime DateNew, QDateTime DateOld)
{
    qDebug() << QString("%1 - called").arg(Q_FUNC_INFO);

    if(mSqlModel == nullptr)
        mSqlModel = new QSqlQueryModel;
    //return QueryModel
    mSqlModel->setQuery(getQueryGetPlatoFromDb(NameSpindel, DateNew, DateOld));
    mSqlModel->setHeaderData(0, Qt::Horizontal, tr("Zeitstempel"));
    mSqlModel->setHeaderData(1, Qt::Horizontal, tr("Spindelname"));
    mSqlModel->setHeaderData(3, Qt::Horizontal, tr("Temperatur"));
    mSqlModel->setHeaderData(4, Qt::Horizontal, tr("Winkel"));
    mSqlModel->setHeaderData(6, Qt::Horizontal, tr("Plato"));
    mSqlModel->setHeaderData(5, Qt::Horizontal, tr("Rezept"));

    return mSqlModel;
}

QStringList Ispindel::getCalibrationData(const QString NameSpindel)
{
    // getID of Device
    int ID;
    QString query = QString(
                "SELECT NAME, ID FROM %1.%2 WHERE `Name` LIKE '%3' LIMIT 1")
            .arg(mDbDatabase)
            .arg(mDbTableData)
            .arg(NameSpindel);

    execQuery(query);
    while(m_dbIspindelQuery->next())
        ID = m_dbIspindelQuery->record().value("ID").toInt();

    // Extrahiere den Satz mit Kalibrationsdaten für die entsprechende Spindel
    QVariantMap tmpCalibrationSet;
    for (int i = 0; i < mIdCalibrationDataDevice.size(); i++) {
        if(mIdCalibrationDataDevice.at(i).value("Name").toString().contains(NameSpindel))
            tmpCalibrationSet = mIdCalibrationDataDevice.at(i);
    };

    QStringList returnList;
    returnList.append(tmpCalibrationSet.value("x_0").toString());
    returnList.append(tmpCalibrationSet.value("x_1").toString());
    returnList.append(tmpCalibrationSet.value("x_2").toString());

    return returnList;
}

void Ispindel::setCalibrationData(const QString NameSpindel, const QStringList &Parameter0_2)
{
    // getID of Device
    int ID = 0;
    QString query = QString(
                "SELECT NAME, ID FROM %1.%2 WHERE `Name` LIKE '%3' LIMIT 1")
            .arg(mDbDatabase)
            .arg(mDbTableData)
            .arg(NameSpindel);

    execQuery(query);
    while(m_dbIspindelQuery->next())
        ID = m_dbIspindelQuery->record().value("ID").toInt();

    query = QString("UPDATE %1.%2 ").arg(mDbDatabase).arg(mDbTableCalibration);
    query += QString("SET const1 = %1, const2 = %2, const3 = %3 ")
            .arg(Parameter0_2.at(2))
            .arg(Parameter0_2.at(1))
            .arg(Parameter0_2.at(0));
    query += QString("WHERE ID = %1").arg(ID);

    execQueryAndCheckError(query);
}

QString Ispindel::getQueryGetPlatoFromDb(QString &NameSpindel, QDateTime &DateNew, QDateTime &DateOld)
{
    qDebug() << QString("%1 - called").arg(Q_FUNC_INFO);
    // getID of Device
    int ID;
    QString query = QString(
                "SELECT NAME, ID FROM %1.%2 WHERE `Name` LIKE '%3' LIMIT 1")
            .arg(mDbDatabase)
            .arg(mDbTableData)
            .arg(NameSpindel);

    execQuery(query);
    while(m_dbIspindelQuery->next())
        ID = m_dbIspindelQuery->record().value("ID").toInt();

    // Extrahiere den Satz mit Kalibrationsdaten für die entsprechende Spindel
    QVariantMap tmpCalibrationSet;
    for (int i = 0; i < mIdCalibrationDataDevice.size(); i++) {
        if(mIdCalibrationDataDevice.at(i).value("Name").toString().contains(NameSpindel))
            tmpCalibrationSet = mIdCalibrationDataDevice.at(i);
    };

    double constX_2 = tmpCalibrationSet.value("x_2").toDouble();
    double constX_1 = tmpCalibrationSet.value("x_1").toDouble();
    double constX_0 = tmpCalibrationSet.value("x_0").toDouble();

    /*
     * Hole die Daten zwischen zwei Zeiten aus der Datenbank.
     * Da uns lediglich die Plato interessieren, baue die Berechnung
     * direkt in die Query mit ein!
     * const1 * angle² + const2 * angle + const3
     * Ich bin mir nicht sicher ob dieses Vorgehen Geschwindigkeitsnachteile
     * mit sich bringt. Ausprobieren!
     */
    query = QString("SELECT `Timestamp`, `Name`, `ID`, `Temperature`, `Angle`, `Recipe`, "
                            "(%1 * POW(`Angle`, 2) + %2 *`Angle` + %3) AS 'Plato' ")
            .arg(constX_2)
            .arg(constX_1)
            .arg(constX_0)
            +
            QString(
                "FROM %1.%2 "
                "WHERE `Timestamp` "
                "BETWEEN "
                "'%3' "
                "AND "
                "'%4' "
                "AND "
                "NAME LIKE '%5' "
                "AND ID != 0 "
                "ORDER BY `Timestamp` asc")
            .arg(mDbDatabase)
            .arg(mDbTableData)
            .arg(DateOld.toString("yyyy-MM-dd hh:mm:ss"))
            .arg(DateNew.toString("yyyy-MM-dd hh:mm:ss"))
            .arg(NameSpindel);

    return query;
}

bool Ispindel::isDatabaseOpen()
{
    return m_dbIspindel.isOpen();
}

void Ispindel::setDbPwd(const QString &dbPwd)
{
    mDbPwd = dbPwd;
}

void Ispindel::setDbUsername(const QString &dbUsername)
{
    mDbUsername = dbUsername;
}

void Ispindel::setDbDatabase(const QString &dbDatabase)
{
    mDbDatabase = dbDatabase;
}

void Ispindel::setDbServer(const QString &dbServer)
{
    mDbServer = dbServer;
}

void Ispindel::setDbDriver(const QString &dbDriver)
{
    mDbDriver = dbDriver;
}

void Ispindel::execQueryAndCheckError(QString query)
{
#ifdef showDebug
        qDebug() << query;
#endif
        m_dbIspindelQuery->exec(query);
        checkErrorDatabase();
}

void Ispindel::execQuery(QString &query)
{
    m_dbIspindelQuery->exec(query);
}

void Ispindel::checkErrorDatabase()
{
    // check for error
    if(!m_dbIspindelQuery->lastError().text().isEmpty())
        error(m_dbIspindelQuery->lastError());
}

template<class T>
void Ispindel::error(T error)
{
//    QMessageBox::warning(nullptr, tr("Verbindung zur Datenbank für die iSpindel konnte nicht hergestellt werden."),
//                          tr("Bitte öffne die Einstellung zur iSpindel und versuche ein Verbindung zur Datenbank herzustellen."),
//                          QMessageBox::Ok);
    qDebug() << error;
}
