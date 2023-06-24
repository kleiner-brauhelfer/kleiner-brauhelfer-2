#ifndef IMPORTEXPORT_H
#define IMPORTEXPORT_H

#include "kleiner-brauhelfer-core_global.h"
#include <QByteArray>
#include <QStringList>

class Brauhelfer;

class LIB_EXPORT ImportExport
{
public:
    static int importKbh(Brauhelfer* bh, const QByteArray &content);
    static int importMaischeMalzundMehr(Brauhelfer* bh, const QByteArray &content);
    static int importBeerXml(Brauhelfer* bh, const QByteArray &content);
    static QByteArray exportKbh(Brauhelfer* bh, int sudRow, const QStringList &exclude = QStringList(), bool compact = false);
    static QByteArray exportBrautomat(Brauhelfer* bh, int sudRow, bool compact = false);
    static QByteArray exportMaischeMalzundMehr(Brauhelfer* bh, int sudRow, bool compact = false);
    static QByteArray exportBeerXml(Brauhelfer* bh, int sudRow, bool compact = false);
};

#endif // IMPORTEXPORT_H
