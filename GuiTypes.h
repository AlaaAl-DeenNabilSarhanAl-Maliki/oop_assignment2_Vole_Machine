// GuiTypes.h
#ifndef VOLE_GUI_TYPES_H
#define VOLE_GUI_TYPES_H

#include <QString>
#include <cstdint>
#include <optional>

// How numeric values are shown and typed in the GUI.
enum class DisplayBase
{
    Hex,
    Decimal,
    Binary
};

// Overall state of the application, used to enable/disable controls.
enum class UiState
{
    NoProgram, // nothing loaded yet
    Ready,     // loaded, can step / run
    Running,   // animated run in progress (timer active)
    Halted,    // program finished (HALT or error)
};

namespace GuiUtil
{

    // "2A" / "42" / "00101010" depending on base. Hex is upper-case, zero padded.
    QString formatByte(uint8_t value, DisplayBase base);

    // Four hex digits, e.g. "2005".
    QString formatWord(uint16_t value);

    // Two hex digits, e.g. "0A".
    QString hexByte(uint8_t value);

    // Parses text typed by the user in the given base.
    // Returns std::nullopt when the text is invalid or outside 0..255.
    std::optional<uint8_t> parseByte(const QString &text, DisplayBase base);

    // Parses a hex address 00..FF (used for the start address field).
    std::optional<int> parseHexAddress(const QString &text);

    // Printable ASCII char for a byte, or '.' when not printable.
    QChar asciiChar(uint8_t value);

    // Short placeholder text for the input hint of a base ("00-FF", "0-255", "8 bits").
    QString inputHint(DisplayBase base);

} // namespace GuiUtil

#endif