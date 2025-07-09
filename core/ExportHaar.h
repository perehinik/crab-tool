#ifndef EXPORTHAAR_H
#define EXPORTHAAR_H

#include "ExportBase.h"

class ExportHaar : public ExportBase {
    Q_OBJECT

public:
    ExportHaar(ProjectData *data, QWidget *parent = nullptr);

private:
    void exportAndSave() override;
};

#endif // EXPORTHAAR_H
