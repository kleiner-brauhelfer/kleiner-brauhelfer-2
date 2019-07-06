#ifndef IMPORTEXPORT_H
#define IMPORTEXPORT_H

#include <QString>

class ImportExport
{
public:
    static bool importMaischeMalzundMehr(const QString &fileName);
    static bool importBeerXml(const QString &fileName);
    static bool exportMaischeMalzundMehr(const QString &fileName, int sudRow);
    static bool exportBeerXml(const QString &fileName, int sudRow);
};

#endif // IMPORTEXPORT_H
