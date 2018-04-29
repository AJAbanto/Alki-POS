#ifndef MULTIEDITDIALOGUE_H
#define MULTIEDITDIALOGUE_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>

namespace Ui {
class MultiEditDialogue;
}
class MultiEditDialogue : public QDialog
{
    Q_OBJECT

public:
    MultiEditDialogue();
};

#endif // MULTIEDITDIALOGUE_H
