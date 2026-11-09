// RegisterPanel.h
#ifndef VOLE_GUI_REGISTERPANEL_H
#define VOLE_GUI_REGISTERPANEL_H

#include <QWidget>
#include <array>
#include <cstdint>
#include "GuiTypes.h"
#include "MachineSnapshot.h"

class Cpu;
class QLineEdit;
class QLabel;

// Shows R0..RF, the program counter (editable) and the instruction
// register (read only, with a text description of the current instruction).
class RegisterPanel : public QWidget {
    Q_OBJECT

public:
    explicit RegisterPanel(QWidget* parent = nullptr);

    void showRegisters(const Cpu& cpu, const MachineSnapshot& before);

    void setDisplayBase(DisplayBase base);
    void setEditingEnabled(bool enabled);

signals:
    void registerEdited(uint8_t index, uint8_t value);
    void programCounterEdited(uint8_t address);
    void editRejected(const QString& reason);

private slots:
    void onRegisterEditingFinished();
    void onProgramCounterEditingFinished();

private:
    void buildUi();
    void markEdit(QLineEdit* edit, bool changed) const;
    int  indexOfEditor(QObject* sender) const;     // -1 if not a register editor

    DisplayBase base = DisplayBase::Hex;
    bool refreshing = false;
    bool editingEnabled = true;

    std::array<QLineEdit*, 16> registerEdits{};
    std::array<uint8_t, 16>    shownValues{};
    uint8_t shownPc = 0;

    QLineEdit* pcEdit = nullptr;
    QLineEdit* irEdit = nullptr;            // read only, always 4 hex digits
    QLabel*    irDescription = nullptr;     // e.g. "LOAD R0 <- 05"
};

#endif