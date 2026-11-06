// GuiTypes.cpp
#include "GuiTypes.h"

#include <QChar>

namespace GuiUtil
{

    QString hexByte(uint8_t value)
    {
        return QString("%1").arg(static_cast<uint>(value), 2, 16, QChar('0')).toUpper();
    }

    QString formatWord(uint16_t value)
    {
        return QString("%1").arg(static_cast<uint>(value), 4, 16, QChar('0')).toUpper();
    }

    QString formatByte(uint8_t value, DisplayBase base)
    {
        switch (base)
        {
        case DisplayBase::Hex:
            return hexByte(value);
        case DisplayBase::Decimal:
            return QString::number(static_cast<uint>(value));
        case DisplayBase::Binary:
            return QString("%1").arg(static_cast<uint>(value), 8, 2, QChar('0'));
        }
        return hexByte(value);
    }

    namespace
    {

        int radixOf(DisplayBase base)
        {
            switch (base)
            {
            case DisplayBase::Hex:
                return 16;
            case DisplayBase::Decimal:
                return 10;
            case DisplayBase::Binary:
                return 2;
            }
            return 16;
        }

        // Value of one ASCII digit, or -1 when it is not a digit at all.
        int digitValue(QChar ch)
        {
            const ushort u = ch.unicode();
            if (u >= '0' && u <= '9')
                return u - '0';
            if (u >= 'A' && u <= 'F')
                return 10 + (u - 'A');
            if (u >= 'a' && u <= 'f')
                return 10 + (u - 'a');
            return -1;
        }

    } // namespace

    std::optional<uint8_t> parseByte(const QString &text, DisplayBase base)
    {
        QString t = text.trimmed();
        if (base == DisplayBase::Hex && t.startsWith("0x", Qt::CaseInsensitive))
            t = t.mid(2);
        if (t.isEmpty())
            return std::nullopt;

        const int radix = radixOf(base);
        unsigned value = 0;
        for (QChar ch : t)
        {
            const int d = digitValue(ch);
            if (d < 0 || d >= radix)
                return std::nullopt;
            value = value * radix + static_cast<unsigned>(d);
            if (value > 255)
                return std::nullopt;
        }
        return static_cast<uint8_t>(value);
    }

    std::optional<int> parseHexAddress(const QString &text)
    {
        const std::optional<uint8_t> value = parseByte(text, DisplayBase::Hex);
        if (!value)
            return std::nullopt;
        return static_cast<int>(*value);
    }

    QChar asciiChar(uint8_t value)
    {
        if (value >= 32 && value <= 126)
            return QChar(static_cast<ushort>(value));
        return QChar('.');
    }

    QString inputHint(DisplayBase base)
    {
        switch (base)
        {
        case DisplayBase::Hex:
            return "00-FF";
        case DisplayBase::Decimal:
            return "0-255";
        case DisplayBase::Binary:
            return "8 bits";
        }
        return "00-FF";
    }

} // namespace GuiUtil
