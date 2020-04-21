#ifndef IMPORTEXPORT_H
#define IMPORTEXPORT_H

#include <QString>

class Brauhelfer;

class ImportExport
{
public:
    static int importKbh(Brauhelfer* bh, const QString &fileName);
    static int importMaischeMalzundMehr(Brauhelfer* bh, const QString &fileName);
    static int importBeerXml(Brauhelfer* bh, const QString &fileName);
    static bool exportKbh(Brauhelfer* bh, const QString &fileName, int sudRow);
    static bool exportMaischeMalzundMehr(Brauhelfer* bh, const QString &fileName, int sudRow);
    static bool exportBeerXml(Brauhelfer* bh, const QString &fileName, int sudRow);
};

#endif // IMPORTEXPORT_H
