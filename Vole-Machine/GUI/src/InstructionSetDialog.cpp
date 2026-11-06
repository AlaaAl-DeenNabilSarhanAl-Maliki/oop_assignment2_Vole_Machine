// InstructionSetDialog.cpp
#include "InstructionSetDialog.h"

#include <QDialogButtonBox>
#include <QFontDatabase>
#include <QHeaderView>
#include <QLabel>
#include <QTableWidget>
#include <QVBoxLayout>

InstructionSetDialog::InstructionSetDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Vole instruction set");
    resize(860, 440);
    buildUi();
    fillTable();
}

void InstructionSetDialog::buildUi()
{
    auto* layout = new QVBoxLayout(this);

    auto* intro = new QLabel(
        "Each instruction is 4 hex digits. R, S, T are register digits (0-F); "
        "XY is a two-digit hex value or address.", this);
    intro->setWordWrap(true);
    layout->addWidget(intro);

    auto* table = new QTableWidget(this);
    table->setObjectName("instructionTable");
    table->setColumnCount(4);
    table->setHorizontalHeaderLabels({"Op-code", "Operand", "Description", "Example"});
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setAlternatingRowColors(true);
    table->verticalHeader()->hide();
    table->horizontalHeader()->setStretchLastSection(true);
    table->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    layout->addWidget(table, 1);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Close, this);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    layout->addWidget(buttons);
}

void InstructionSetDialog::fillTable()
{
    auto* table = findChild<QTableWidget*>("instructionTable");
    if (!table)
        return;

    struct Row { const char* op; const char* operand; const char* description; const char* example; };
    static const Row rows[] = {
        {"1", "RXY", "LOAD register R with the contents of memory cell XY",           "14A3  ->  LOAD R4 <- [A3]"},
        {"2", "RXY", "LOAD register R with the value XY itself (immediate)",           "2005  ->  LOAD R0 <- 05"},
        {"3", "RXY", "STORE the contents of R in memory cell XY (XY = FF prints to the screen)", "35B1  ->  STORE R5 -> [B1]"},
        {"3", "RFF", "STORE R to cell FF: the byte is written to the screen",          "3AFF  ->  STORE RA -> SCREEN"},
        {"4", "0RS", "MOVE the contents of register R into register S",                "40A4  ->  MOVE R4 <- RA"},
        {"5", "RST", "ADD S and T as two's complement integers, result in R",          "5726  ->  ADD R7 <- R2 + R6"},
        {"6", "RST", "ADD S and T as floating point numbers, result in R",             "634E  ->  FADD R3 <- R4 + RE"},
        {"7", "RST", "OR the bit patterns of S and T, result in R",                    "7CB4  ->  OR RC <- RB | R4"},
        {"8", "RST", "AND the bit patterns of S and T, result in R",                   "8045  ->  AND R0 <- R4 & R5"},
        {"9", "RST", "XOR the bit patterns of S and T, result in R",                   "95F3  ->  XOR R5 <- RF ^ R3"},
        {"A", "R0X", "ROTATE the bit pattern in R to the right X times",               "A403  ->  ROTATE R4 right by 3"},
        {"B", "RXY", "JUMP to the instruction at XY if R equals R0 (otherwise continue)", "B43C  ->  JUMP to 3C if R4 == R0"},
        {"C", "000", "HALT execution",                                                  "C000  ->  HALT"},
    };

    const int count = static_cast<int>(sizeof(rows) / sizeof(rows[0]));
    table->setRowCount(count);
    for (int i = 0; i < count; ++i) {
        table->setItem(i, 0, new QTableWidgetItem(rows[i].op));
        table->setItem(i, 1, new QTableWidgetItem(rows[i].operand));
        table->setItem(i, 2, new QTableWidgetItem(rows[i].description));
        table->setItem(i, 3, new QTableWidgetItem(rows[i].example));
    }
    table->resizeColumnsToContents();
    table->horizontalHeader()->setStretchLastSection(true);
}
