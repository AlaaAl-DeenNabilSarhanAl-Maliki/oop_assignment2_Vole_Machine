// MemoryPanel.cpp
#include "MemoryPanel.h"

#include "Memory.h"
#include "ThemeManager.h"

#include <QAction>
#include <QFontDatabase>
#include <QHeaderView>
#include <QMenu>

MemoryPanel::MemoryPanel(QWidget* parent)
    : QTableWidget(16, 16, parent)
{
    buildTable();
}

int MemoryPanel::addressOf(int row, int column) { return row * 16 + column; }
int MemoryPanel::rowOf(uint8_t address)         { return address >> 4; }
int MemoryPanel::columnOf(uint8_t address)      { return address & 0x0F; }

void MemoryPanel::buildTable()
{
    QStringList columns, rows;
    for (int i = 0; i < 16; ++i) {
        const QString digit = QString::number(i, 16).toUpper();
        columns << digit;
        rows << (digit + "_");
    }
    setHorizontalHeaderLabels(columns);
    setVerticalHeaderLabels(rows);

    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    horizontalHeader()->setMinimumSectionSize(24);

    setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    setSelectionMode(QAbstractItemView::SingleSelection);
    setContextMenuPolicy(Qt::CustomContextMenu);

    refreshing = true;
    for (int r = 0; r < 16; ++r) {
        for (int c = 0; c < 16; ++c) {
            const int address = addressOf(r, c);
            auto* item = new QTableWidgetItem(GuiUtil::formatByte(0, base));
            item->setTextAlignment(Qt::AlignCenter);
            if (address == Memory::screenAddress)
                item->setFlags(item->flags() & ~Qt::ItemIsEditable);   // FF is the screen
            item->setToolTip(tooltipFor(static_cast<uint8_t>(address), 0));
            setItem(r, c, item);
        }
    }
    refreshing = false;

    connect(this, &QTableWidget::itemChanged, this, &MemoryPanel::onItemChanged);
    connect(this, &QWidget::customContextMenuRequested,
            this, &MemoryPanel::onContextMenuRequested);
}

void MemoryPanel::showMemory(const Memory& memory,
                             uint8_t programCounter,
                             const MachineSnapshot& before)
{
    refreshing = true;
    const int pc = programCounter;

    for (int address = 0; address < 256; ++address) {
        const uint8_t a = static_cast<uint8_t>(address);
        const uint8_t value = memory.read(a);
        shownValues[address] = value;

        QTableWidgetItem* it = item(rowOf(a), columnOf(a));
        if (!it)
            continue;

        it->setText(GuiUtil::formatByte(value, base));
        it->setToolTip(tooltipFor(a, value));

        const bool isPc = (address == pc) || (address == pc + 1);
        const bool isChanged = before.memory[address] != value;
        styleCell(it, a, isPc, isChanged);
    }
    refreshing = false;
}

void MemoryPanel::setDisplayBase(DisplayBase newBase)
{
    base = newBase;

    horizontalHeader()->setSectionResizeMode(
        base == DisplayBase::Binary ? QHeaderView::ResizeToContents : QHeaderView::Stretch);

    // Re-render the texts from the last shown values; colors stay as they are.
    refreshing = true;
    for (int address = 0; address < 256; ++address) {
        const uint8_t a = static_cast<uint8_t>(address);
        if (QTableWidgetItem* it = item(rowOf(a), columnOf(a)))
            it->setText(GuiUtil::formatByte(shownValues[address], base));
    }
    refreshing = false;
}

DisplayBase MemoryPanel::displayBase() const
{
    return base;
}

void MemoryPanel::setEditingEnabled(bool enabled)
{
    editingEnabled = enabled;
    setEditTriggers(enabled
        ? (QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed
           | QAbstractItemView::AnyKeyPressed)
        : QAbstractItemView::NoEditTriggers);
}

void MemoryPanel::styleCell(QTableWidgetItem* item, uint8_t address,
                            bool isPc, bool isChanged) const
{
    const HighlightColors colors = ThemeManager::colors();

    QColor background = colors.background;
    if (isPc)
        background = colors.programCounter;
    else if (isChanged)
        background = colors.changed;
    else if (address == Memory::screenAddress)
        background = colors.screenCell;

    item->setBackground(QBrush(background));
    item->setForeground(QBrush(colors.text));

    QFont f = item->font();
    f.setBold(isPc);
    item->setFont(f);
}

QString MemoryPanel::tooltipFor(uint8_t address, uint8_t value) const
{
    QString tip = QString("Address %1\nHex %2   Dec %3   Bin %4\nASCII '%5'")
        .arg(GuiUtil::hexByte(address))
        .arg(GuiUtil::hexByte(value))
        .arg(value)
        .arg(GuiUtil::formatByte(value, DisplayBase::Binary))
        .arg(GuiUtil::asciiChar(value));
    if (address == Memory::screenAddress)
        tip += "\nSpecial cell: storing a value here prints it on the screen.";
    return tip;
}

void MemoryPanel::onItemChanged(QTableWidgetItem* changedItem)
{
    if (refreshing || !changedItem)
        return;

    const uint8_t address = static_cast<uint8_t>(addressOf(changedItem->row(), changedItem->column()));

    auto restore = [&]() {
        refreshing = true;
        changedItem->setText(GuiUtil::formatByte(shownValues[address], base));
        refreshing = false;
    };

    if (!editingEnabled) {
        restore();
        emit editRejected("Editing is disabled while the program is running.");
        return;
    }

    const std::optional<uint8_t> value = GuiUtil::parseByte(changedItem->text(), base);
    if (!value) {
        restore();
        emit editRejected(QString("Invalid value for cell %1. Expected %2.")
                              .arg(GuiUtil::hexByte(address))
                              .arg(GuiUtil::inputHint(base)));
        return;
    }

    emit memoryEdited(address, *value);
}

void MemoryPanel::onContextMenuRequested(const QPoint& position)
{
    QTableWidgetItem* it = itemAt(position);
    if (!it)
        return;

    const uint8_t address = static_cast<uint8_t>(addressOf(it->row(), it->column()));

    QMenu menu(this);
    QAction* setPc = menu.addAction(QString("Set PC here (%1)").arg(GuiUtil::hexByte(address)));
    QAction* chosen = menu.exec(viewport()->mapToGlobal(position));
    if (chosen == setPc)
        emit setProgramCounterRequested(address);
}
