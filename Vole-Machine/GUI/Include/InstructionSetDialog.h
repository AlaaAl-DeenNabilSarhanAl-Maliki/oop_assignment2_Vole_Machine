// InstructionSetDialog.h
#ifndef VOLE_GUI_INSTRUCTIONSETDIALOG_H
#define VOLE_GUI_INSTRUCTIONSETDIALOG_H

#include <QDialog>

// Read-only reference table of the supported Vole instructions:
// op-code | operand format | description | example.
// Makes the app easy to understand without the textbook.
class InstructionSetDialog : public QDialog {
    Q_OBJECT

public:
    explicit InstructionSetDialog(QWidget* parent = nullptr);

private:
    void buildUi();
    void fillTable();
};

#endif