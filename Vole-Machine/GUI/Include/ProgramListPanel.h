// ProgramListPanel.h
#ifndef VOLE_GUI_PROGRAMLISTPANEL_H
#define VOLE_GUI_PROGRAMLISTPANEL_H

#include <QTableWidget>
#include <cstdint>
#include <set>

class Machine;

// Lists the loaded instructions: breakpoint marker | address | word | description.
// The row at the current PC is highlighted; clicking the first column toggles a
// breakpoint; double-clicking a row asks to move the PC there.
class ProgramListPanel : public QTableWidget {
    Q_OBJECT

public:
    enum Column { ColBreakpoint = 0, ColAddress, ColWord, ColDescription, ColumnCount };

    explicit ProgramListPanel(QWidget* parent = nullptr);

    // Rebuilds the list from memory: 'wordCount' instructions from 'startAddress'.
    // Words are read from the machine (so manual memory edits show up too).
    void showProgram(const Machine& machine,
                     int startAddress,
                     int wordCount,
                     uint8_t programCounter);

    void setBreakpoints(const std::set<uint8_t>& breakpoints);
    void clearProgram();

signals:
    void breakpointToggled(uint8_t address, bool enabled);
    void setProgramCounterRequested(uint8_t address);

private slots:
    void onCellClicked(int row, int column);
    void onCellDoubleClicked(int row, int column);

private:
    void buildTable();
    void styleRow(int row, bool isPc) const;
    uint8_t addressOfRow(int row) const;
    int rowOfAddress(uint8_t address) const;     // -1 when not in the list

    std::set<uint8_t> breakpoints;
    int firstAddress = 0;
    int rowCountShown = 0;
};

#endif