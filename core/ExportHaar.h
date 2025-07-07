#ifndef EXPORTHAAR_H
#define EXPORTHAAR_H

#include "ExportBase.h"

class ExportHaar : public ExportBase {
    Q_OBJECT

public:
    ExportHaar(ProjectData *data, QWidget *parent = nullptr);

private:
    virtual void acceptExportHandler();
    virtual void rejectExportHandler();
};

#endif // EXPORTHAAR_H
