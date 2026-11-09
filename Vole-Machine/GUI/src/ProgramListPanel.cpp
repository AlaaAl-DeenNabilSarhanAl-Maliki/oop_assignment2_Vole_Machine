// ProgramListPanel.cpp
#include "ProgramListPanel.h"

#include "GuiTypes.h"
#include "InstructionDescriber.h"
#include "Machine.h"
#include "ThemeManager.h"

#include <QFontDatabase>
#include <QHeaderView>

namespace {
const QString kBreakpointGlyph = QStringLiteral("\u25CF");   // filled circle
}

ProgramListPanel::ProgramListPanel(QWidget* parent)
    : QTableWidget(0, ColumnCount, parent)
{
    buildTable();
}

void ProgramListPanel::buildTable()
{
    setHorizontalHeaderLabels({"", "Addr", "Word", "Instruction"});
    verticalHeader()->hide();
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setShowGrid(false);
    setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));

    QHeaderView* header = horizontalHeader();
    header->setSectionResizeMode(ColBreakpoint, QHeaderView::Fixed);
    header->setSectionResizeMode(ColAddress, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(ColWord, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(ColDescription, QHeaderView::Stretch);
    setColumnWidth(ColBreakpoint, 28);

    setToolTip("Click the first column to toggle a breakpoint.\n"
               "Double-click a row to move the PC there.");

    connect(this, &QTableWidget::cellClicked, this, &ProgramListPanel::onCellClicked);
    connect(this, &QTableWidget::cellDoubleClicked, this, &ProgramListPanel::onCellDoubleClicked);
}

uint8_t ProgramListPanel::addressOfRow(int row) const
{
    return static_cast<uint8_t>(firstAddress + 2 * row);
}

int ProgramListPanel::rowOfAddress(uint8_t address) const
{
    const int diff = static_cast<int>(address) - firstAddress;
    if (diff < 0 || (diff % 2) != 0)
        return -1;
    const int row = diff / 2;
    return row < rowCountShown ? row : -1;
}

void ProgramListPanel::showProgram(const Machine& machine,
                                   int startAddress,
                                   int wordCount,
                                   uint8_t programCounter)
{
    const Memory& memory = machine.getMemory();

    // Count how many whole words fit inside memory.
    int count = 0;
    for (int i = 0; i < wordCount; ++i) {
        const int address = startAddress + 2 * i;
        if (address + 1 >= Memory::Size)
            break;
        ++count;
    }

    // Rebuild the rows only when the shape of the list changed. During an animated
    // run only the PC row moves, so we just update the cells that differ.
    const bool structureChanged = (count != rowCountShown) || (startAddress != firstAddress);
    firstAddress = startAddress;
    rowCountShown = count;
    if (structureChanged) {
        setRowCount(0);
        setRowCount(count);
    }

    auto put = [this](int row, int col, const QString& text, bool centered) {
        QTableWidgetItem* it = item(row, col);
        if (!it) {
            it = new QTableWidgetItem(text);
            it->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            if (centered)
                it->setTextAlignment(Qt::AlignCenter);
            setItem(row, col, it);
        } else if (it->text() != text) {
            it->setText(text);
        }
    };

    const HighlightColors colors = ThemeManager::colors();

    for (int row = 0; row < count; ++row) {
        const uint8_t address = addressOfRow(row);
        const uint16_t word = static_cast<uint16_t>(
            (memory.read(address) << 8) | memory.read(static_cast<uint8_t>(address + 1)));

        put(row, ColBreakpoint, breakpoints.count(address) ? kBreakpointGlyph : QString(), true);
        put(row, ColAddress, GuiUtil::hexByte(address), false);
        put(row, ColWord, GuiUtil::formatWord(word), false);
        put(row, ColDescription,
            QString::fromStdString(InstructionDescriber::describe(word)), false);

        const bool isPc = (address == programCounter);
        const QTableWidgetItem* marker = item(row, ColAddress);
        const QColor wanted = isPc ? colors.programCounter : colors.background;
        if (marker->background().color() != wanted || marker->font().bold() != isPc)
            styleRow(row, isPc);
    }

    const int pcRow = rowOfAddress(programCounter);
    if (pcRow >= 0)
        scrollToItem(item(pcRow, ColAddress), QAbstractItemView::EnsureVisible);
}

void ProgramListPanel::setBreakpoints(const std::set<uint8_t>& newBreakpoints)
{
    breakpoints = newBreakpoints;
    for (int row = 0; row < rowCountShown; ++row) {
        if (QTableWidgetItem* bp = item(row, ColBreakpoint)) {
            const QString text = breakpoints.count(addressOfRow(row)) ? kBreakpointGlyph : QString();
            if (bp->text() != text)
                bp->setText(text);
        }
    }
}

void ProgramListPanel::clearProgram()
{
    setRowCount(0);
    rowCountShown = 0;
}

void ProgramListPanel::styleRow(int row, bool isPc) const
{
    const HighlightColors colors = ThemeManager::colors();
    const QBrush background(isPc ? colors.programCounter : colors.background);

    for (int col = 0; col < ColumnCount; ++col) {
        QTableWidgetItem* it = item(row, col);
        if (!it)
            continue;
        it->setBackground(background);
        it->setForeground(QBrush(col == ColBreakpoint ? colors.breakpoint : colors.text));

        QFont f = it->font();
        f.setBold(isPc);
        it->setFont(f);
    }
}

void ProgramListPanel::onCellClicked(int row, int column)
{
    if (column != ColBreakpoint || row < 0 || row >= rowCountShown)
        return;

    const uint8_t address = addressOfRow(row);
    const bool enable = breakpoints.count(address) == 0;
    if (enable)
        breakpoints.insert(address);
    else
        breakpoints.erase(address);

    if (QTableWidgetItem* bp = item(row, ColBreakpoint))
        bp->setText(enable ? kBreakpointGlyph : QString());

    emit breakpointToggled(address, enable);
}

void ProgramListPanel::onCellDoubleClicked(int row, int column)
{
    if (column == ColBreakpoint || row < 0 || row >= rowCountShown)
        return;
    emit setProgramCounterRequested(addressOfRow(row));
}
