#include "multieditdialogue.h"
#include "ui_multieditdialogue.h"


MultiEditDialogue::MultiEditDialogue(QDialog *parent)
{
    QDialog(parent);
    ui(new Ui::Dialog)
    {
        ui->setup()
    }

}
