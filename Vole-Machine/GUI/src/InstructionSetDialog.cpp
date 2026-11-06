#include "InstructionSetDialog.h"

#include <QFontDatabase>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

InstructionSetDialog::InstructionSetDialog(QWidget* parent)
    : QDialog(parent)
{
    buildUi();
    fillTable();
}

void InstructionSetDialog::buildUi()
{
    setWindowTitle("Vole instruction set");
    resize(860, 460);

    auto* layout = new QVBoxLayout(this);

    auto* intro = new QLabel(
        "Every instruction is 16 bits = four hex digits. R, S, T are register numbers (0-F); "
        "XY is an 8-bit value or memory address. Registers and memory cells are 8 bits.\n"
        "Address FF is the screen: storing to FF prints on the screen.", this);
    intro->setWordWrap(true);
    layout->addWidget(intro);

    auto* table = new QTableWidget(0, 4, this);
    table->setObjectName("instructionTable");
    table->setHorizontalHeaderLabels({"Op-code", "Operand", "Description", "Example"});
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setWordWrap(true);
    table->verticalHeader()->setVisible(false);
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    layout->addWidget(table, 1);

    auto* closeButton = new QPushButton("Close", this);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
    layout->addWidget(closeButton, 0, Qt::AlignRight);
}

void InstructionSetDialog::fillTable()
{
    auto* table = findChild<QTableWidget*>("instructionTable");
    if (table == nullptr) return;

    struct Row { const char* opcode; const char* operand; const char* description; const char* example; };
    static const Row rows[] = {
        {"1", "RXY", "LOAD register R with the contents of memory cell XY",
                     "14A3: register 4 <- memory[A3]"},
        {"2", "RXY", "LOAD register R with the value XY",
                     "20A3: register 0 <- A3"},
        {"3", "RXY", "STORE the contents of register R into memory cell XY",
                     "35B1: memory[B1] <- register 5"},
        {"3", "RFF", "STORE register R to the screen (address FF is the screen)",
                     "32FF: print register 2 on the screen"},
        {"4", "0RS", "MOVE (copy) register R to register S",
                     "40A4: register 4 <- register A"},
        {"5", "RST", "ADD registers S and T as two's complement; result in R",
                     "5726: register 7 <- register 2 + register 6"},
        {"6", "RST", "ADD registers S and T as floating point; result in R",
                     "634E: register 3 <- register 4 + register E (float)"},
        {"7", "RST", "OR registers S and T; result in R",
                     "7CB4: register C <- register B OR register 4"},
        {"8", "RST", "AND registers S and T; result in R",
                     "8045: register 0 <- register 4 AND register 5"},
        {"9", "RST", "XOR registers S and T; result in R",
                     "95F3: register 5 <- register F XOR register 3"},
        {"A", "R0X", "ROTATE register R right X times (circular)",
                     "A403: rotate register 4 right 3 bits"},
        {"B", "RXY", "JUMP to address XY if register R equals register 0",
                     "B43C: if R4 == R0 then PC <- 3C"},
        {"C", "000", "HALT execution",
                     "C000: stop"},
    };

    for (const Row& row : rows) {
        const int index = table->rowCount();
        table->insertRow(index);
        table->setItem(index, 0, new QTableWidgetItem(row.opcode));
        table->setItem(index, 1, new QTableWidgetItem(row.operand));
        table->setItem(index, 2, new QTableWidgetItem(row.description));
        table->setItem(index, 3, new QTableWidgetItem(row.example));
        table->item(index, 0)->setTextAlignment(Qt::AlignCenter);
        table->item(index, 1)->setTextAlignment(Qt::AlignCenter);
    }
    table->resizeRowsToContents();
}
