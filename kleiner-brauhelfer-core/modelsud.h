#ifndef MODELSUD_H
#define MODELSUD_H

#include "sqltablemodel.h"

class Brauhelfer;

class ModelSud : public SqlTableModel
{
    Q_OBJECT

    friend class ProxyModelSud;

public:

    enum Column
    {
        ColID,
        ColSudname,
        ColSudnummer,
        ColAnlage,
        ColMenge,
        ColSW,
        ColhighGravityFaktor,
        ColFaktorHauptguss,
        ColWasserprofil,
        ColRestalkalitaetSoll,
        ColEinmaischenTemp,
        ColCO2,
        ColIBU,
        ColberechnungsArtHopfen,
        ColKochdauerNachBitterhopfung,
        ColNachisomerisierungszeit,
        ColReifezeit,
        ColKostenWasserStrom,
        ColKommentar,
        ColStatus,
        ColBraudatum,
        ColAbfuelldatum,
        ColErstellt,
        ColGespeichert,
        Colerg_S_Gesamt,
        Colerg_W_Gesamt,
        Colerg_WHauptguss,
        Colerg_WNachguss,
        Colerg_Farbe,
        ColSWKochende,
        ColSWAnstellen,
        ColSchnellgaerprobeAktiv,
        ColSWSchnellgaerprobe,
        ColSWJungbier,
        ColTemperaturJungbier,
        ColWuerzemengeVorHopfenseihen,
        ColWuerzemengeKochende,
        ColWuerzemengeAnstellen,
        ColSpunden,
        ColSpeisemenge,
        ColJungbiermengeAbfuellen,
        Colerg_AbgefuellteBiermenge,
        Colerg_Sudhausausbeute,
        Colerg_EffektiveAusbeute,
        Colerg_Preis,
        Colerg_Alkohol,
        ColAusbeuteIgnorieren,
        ColMerklistenID,
        // virtual
        ColDeleted,
        ColSWIst,
        ColSREIst,
        ColMengeIst,
        ColIbuIst,
        ColFarbeIst,
        ColCO2Ist,
        ColSpundungsdruck,
        ColGruenschlauchzeitpunkt,
        ColSpeiseNoetig,
        ColSpeiseAnteil,
        ColZuckerAnteil,
        ColWoche,
        ColReifezeitDelta,
        ColAbfuellenBereitZutaten,
        ColMengeSollKochbeginn,
        ColMengeSollKochende,
        ColWuerzemengeAnstellenTotal,
        ColSW_Malz,
        ColSW_WZ_Maischen,
        ColSW_WZ_Kochen,
        ColSW_WZ_Gaerung,
        ColSWSollKochbeginn,
        ColSWSollKochbeginnMitWz,
        ColSWSollKochende,
        ColSWSollAnstellen,
        ColVerdampfungsziffer,
        ColsEVG,
        ColtEVG,
        ColAnlageVerdampfungsziffer,
        ColAnlageSudhausausbeute,
        ColRestalkalitaetFaktor,
        ColFaktorHauptgussEmpfehlung,
        ColBewertungMittel
    };

public:

    ModelSud(Brauhelfer* bh, QSqlDatabase db = QSqlDatabase());
    void createConnections();
    QVariant dataExt(const QModelIndex &index) const Q_DECL_OVERRIDE;
    bool setDataExt(const QModelIndex &index, const QVariant &value) Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    bool removeRows(int row, int count = 1, const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE;
    void defaultValues(QVariantMap &values) const Q_DECL_OVERRIDE;
    QVariantMap copyValues(int row) const Q_DECL_OVERRIDE;
    QVariant dataAnlage(int row, const QString& fieldName) const;
    QVariant dataWasser(int row, const QString& fieldName) const;

private slots:

    void onModelReset();
    void onRowChanged(const QModelIndex &index);
    void onOtherModelRowChanged(const QModelIndex &index);
    void onAnlageDataChanged(const QModelIndex &index);

private:

    bool setDataExt_impl(const QModelIndex &index, const QVariant &value);
    void update(int row);
    void updateSwWeitereZutaten(int row);
    void updateFarbe(int row);
    void updatePreis(int row);
    QVariant SWIst(const QModelIndex &index) const;
    QVariant SREIst(const QModelIndex &index) const;
    QVariant CO2Ist(const QModelIndex &index) const;
    QVariant Spundungsdruck(const QModelIndex &index) const;
    QVariant Gruenschlauchzeitpunkt(const QModelIndex &index) const;
    QVariant SpeiseNoetig(const QModelIndex &index) const;
    QVariant SpeiseAnteil(const QModelIndex &index) const;
    QVariant ZuckerAnteil(const QModelIndex &index) const;
    QVariant ReifezeitDelta(const QModelIndex &index) const;
    QVariant AbfuellenBereitZutaten(const QModelIndex &index) const;
    QVariant MengeSollKochbeginn(const QModelIndex &index) const;
    QVariant MengeSollKochende(const QModelIndex &index) const;
    QVariant SWSollKochbeginn(const QModelIndex &index) const;
    QVariant SWSollKochbeginnMitWz(const QModelIndex &index) const;
    QVariant SWSollKochende(const QModelIndex &index) const;
    QVariant SWSollAnstellen(const QModelIndex &index) const;
    QVariant Verdampfungsziffer(const QModelIndex &index) const;
    QVariant RestalkalitaetFaktor(const QModelIndex &index) const;
    QVariant FaktorHauptgussEmpfehlung(const QModelIndex &index) const;
    void removeRowsFrom(SqlTableModel* model, int sudId);

private:

    Brauhelfer* bh;
    bool mUpdating;
    bool mSkipUpdateOnOtherModelChanged;
    QVector<double> swWzMaischenRecipe;
    QVector<double> swWzKochenRecipe;
    QVector<double> swWzGaerungRecipe;
    QVector<double> swWzGaerungCurrent;
};

#endif // MODELSUD_H
