// RegisterPanel.cpp
#include "RegisterPanel.h"

#include "Cpu.h"
#include "InstructionDescriber.h"
#include "ThemeManager.h"

#include <QFontDatabase>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

RegisterPanel::RegisterPanel(QWidget* parent)
    : QWidget(parent)
{
    buildUi();
}

namespace {

int maxLengthFor(DisplayBase base)
{
    switch (base) {
    case DisplayBase::Hex:     return 2;
    case DisplayBase::Decimal: return 3;
    case DisplayBase::Binary:  return 8;
    }
    return 2;
}

} // namespace

void RegisterPanel::buildUi()
{
    const QFont mono = QFontDatabase::systemFont(QFontDatabase::FixedFont);

    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(4, 4, 4, 4);

    // ---- general purpose registers: two columns of eight ----
    auto* regBox = new QGroupBox("Registers", this);
    auto* grid = new QGridLayout(regBox);
    for (int i = 0; i < 16; ++i) {
        const int row = i % 8;
        const int col = (i / 8) * 2;

        auto* label = new QLabel(QString("R%1").arg(QString::number(i, 16).toUpper()), regBox);
        label->setFont(mono);

        auto* edit = new QLineEdit(regBox);
        edit->setFont(mono);
        edit->setAlignment(Qt::AlignCenter);
        edit->setMaxLength(maxLengthFor(base));
        edit->setPlaceholderText(GuiUtil::inputHint(base));
        edit->setText(GuiUtil::formatByte(0, base));
        connect(edit, &QLineEdit::editingFinished,
                this, &RegisterPanel::onRegisterEditingFinished);

        registerEdits[i] = edit;
        grid->addWidget(label, row, col);
        grid->addWidget(edit, row, col + 1);
    }
    root->addWidget(regBox);

    // ---- PC and IR ----
    auto* cpuBox = new QGroupBox("Control", this);
    auto* cpuGrid = new QGridLayout(cpuBox);

    auto* pcLabel = new QLabel("PC (hex)", cpuBox);
    pcEdit = new QLineEdit(cpuBox);
    pcEdit->setFont(mono);
    pcEdit->setAlignment(Qt::AlignCenter);
    pcEdit->setMaxLength(2);
    pcEdit->setText("00");
    pcEdit->setToolTip("Program counter: address of the next instruction (hex, even).");
    connect(pcEdit, &QLineEdit::editingFinished,
            this, &RegisterPanel::onProgramCounterEditingFinished);

    auto* irLabel = new QLabel("IR", cpuBox);
    irEdit = new QLineEdit(cpuBox);
    irEdit->setFont(mono);
    irEdit->setAlignment(Qt::AlignCenter);
    irEdit->setReadOnly(true);
    irEdit->setText("0000");

    irDescription = new QLabel("", cpuBox);
    irDescription->setFont(mono);
    irDescription->setWordWrap(true);

    cpuGrid->addWidget(pcLabel, 0, 0);
    cpuGrid->addWidget(pcEdit, 0, 1);
    cpuGrid->addWidget(irLabel, 1, 0);
    cpuGrid->addWidget(irEdit, 1, 1);
    cpuGrid->addWidget(irDescription, 2, 0, 1, 2);
    root->addWidget(cpuBox);

    root->addStretch(1);
}

void RegisterPanel::showRegisters(const Cpu& cpu, const MachineSnapshot& before)
{
    refreshing = true;

    for (int i = 0; i < 16; ++i) {
        const uint8_t value = cpu.readRegister(static_cast<uint8_t>(i));
        shownValues[i] = value;
        registerEdits[i]->setText(GuiUtil::formatByte(value, base));
        markEdit(registerEdits[i], before.registers[i] != value);
    }

    shownPc = cpu.getProgramCounter();
    pcEdit->setText(GuiUtil::hexByte(shownPc));
    markEdit(pcEdit, before.programCounter != shownPc);

    const uint16_t ir = cpu.getInstructionRegister();
    irEdit->setText(GuiUtil::formatWord(ir));
    irDescription->setText(QString::fromStdString(InstructionDescriber::describe(ir)));

    refreshing = false;
}

void RegisterPanel::setDisplayBase(DisplayBase newBase)
{
    base = newBase;

    refreshing = true;
    for (int i = 0; i < 16; ++i) {
        registerEdits[i]->setMaxLength(maxLengthFor(base));
        registerEdits[i]->setPlaceholderText(GuiUtil::inputHint(base));
        registerEdits[i]->setText(GuiUtil::formatByte(shownValues[i], base));
    }
    refreshing = false;
}

void RegisterPanel::setEditingEnabled(bool enabled)
{
    editingEnabled = enabled;
    for (QLineEdit* edit : registerEdits)
        edit->setReadOnly(!enabled);
    pcEdit->setReadOnly(!enabled);
}

void RegisterPanel::markEdit(QLineEdit* edit, bool changed) const
{
    if (!changed) {
        edit->setStyleSheet(QString());
        return;
    }
    const HighlightColors colors = ThemeManager::colors();
    edit->setStyleSheet(QString("QLineEdit { background-color: %1; color: %2; }")
                            .arg(colors.changed.name(), colors.text.name()));
}

int RegisterPanel::indexOfEditor(QObject* sender) const
{
    for (int i = 0; i < 16; ++i) {
        if (registerEdits[i] == sender)
            return i;
    }
    return -1;
}

void RegisterPanel::onRegisterEditingFinished()
{
    if (refreshing || !editingEnabled)
        return;

    const int index = indexOfEditor(sender());
    if (index < 0)
        return;

    QLineEdit* edit = registerEdits[index];
    const std::optional<uint8_t> value = GuiUtil::parseByte(edit->text(), base);

    if (!value) {
        refreshing = true;
        edit->setText(GuiUtil::formatByte(shownValues[index], base));
        refreshing = false;
        emit editRejected(QString("Invalid value for R%1. Expected %2.")
                              .arg(QString::number(index, 16).toUpper())
                              .arg(GuiUtil::inputHint(base)));
        return;
    }

    if (*value == shownValues[index]) {
        // Nothing changed (editingFinished also fires on focus loss): just normalise the text.
        refreshing = true;
        edit->setText(GuiUtil::formatByte(shownValues[index], base));
        refreshing = false;
        return;
    }

    emit registerEdited(static_cast<uint8_t>(index), *value);
}

void RegisterPanel::onProgramCounterEditingFinished()
{
    if (refreshing || !editingEnabled)
        return;

    const std::optional<uint8_t> value = GuiUtil::parseByte(pcEdit->text(), DisplayBase::Hex);

    if (!value) {
        refreshing = true;
        pcEdit->setText(GuiUtil::hexByte(shownPc));
        refreshing = false;
        emit editRejected("Invalid program counter. Expected a hex address 00-FF.");
        return;
    }

    if (*value == shownPc) {
        refreshing = true;
        pcEdit->setText(GuiUtil::hexByte(shownPc));
        refreshing = false;
        return;
    }

    emit programCounterEdited(*value);
}
