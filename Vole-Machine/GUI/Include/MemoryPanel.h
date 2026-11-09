// MemoryPanel.h
#ifndef VOLE_GUI_MEMORYPANEL_H
#define VOLE_GUI_MEMORYPANEL_H

#include <QTableWidget>
#include <array>
#include <cstdint>
#include "GuiTypes.h"
#include "MachineSnapshot.h"

class Memory;

// 16 x 16 table: row = high nibble, column = low nibble of the address.
// Address of cell (row, col) = row * 16 + col.
class MemoryPanel : public QTableWidget {
    Q_OBJECT

public:
    explicit MemoryPanel(QWidget* parent = nullptr);

    // Refills the table. Highlights: PC instruction (PC and PC+1),
    // cells changed since 'before', and the special cell FF.
    void showMemory(const Memory& memory,
                    uint8_t programCounter,
                    const MachineSnapshot& before);

    void setDisplayBase(DisplayBase base);
    DisplayBase displayBase() const;

    // Disabled while an animated run is in progress.
    void setEditingEnabled(bool enabled);

signals:
    // User typed a valid value in a cell.
    void memoryEdited(uint8_t address, uint8_t value);
    // User typed something invalid; the old value is restored.
    void editRejected(const QString& reason);
    // Context menu: "Set PC here".
    void setProgramCounterRequested(uint8_t address);

private slots:
    void onItemChanged(QTableWidgetItem* item);
    void onContextMenuRequested(const QPoint& position);

private:
    static int  addressOf(int row, int column);
    static int  rowOf(uint8_t address);
    static int  columnOf(uint8_t address);

    void buildTable();
    void styleCell(QTableWidgetItem* item, uint8_t address,
                   bool isPc, bool isChanged) const;
    QString tooltipFor(uint8_t address, uint8_t value) const;

    DisplayBase base = DisplayBase::Hex;
    bool editingEnabled = true;
    bool refreshing = false;                 // blocks itemChanged while filling
    std::array<uint8_t, 256> shownValues{};  // used to restore on rejected edits
};

#endif