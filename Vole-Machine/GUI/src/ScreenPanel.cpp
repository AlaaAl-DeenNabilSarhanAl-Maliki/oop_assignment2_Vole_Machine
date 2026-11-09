// ScreenPanel.cpp
#include "ScreenPanel.h"
#include "GuiTypes.h"

#include <QApplication>
#include <QClipboard>
#include <QComboBox>
#include <QFontDatabase>
#include <QHBoxLayout>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QScrollBar>
#include <QSignalBlocker>
#include <QStringList>
#include <QVBoxLayout>

ScreenPanel::ScreenPanel(QWidget* parent)
    : QWidget(parent)
{
    buildUi();
    render();
}

void ScreenPanel::buildUi()
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(4, 4, 4, 4);

    auto* top = new QHBoxLayout;
    top->addWidget(new QLabel("Screen (output of STORE -> FF)", this));
    top->addStretch(1);

    formatCombo = new QComboBox(this);
    // Order must match the Format enum: Ascii, Hex, Decimal, All.
    formatCombo->addItem("ASCII");
    formatCombo->addItem("Hex");
    formatCombo->addItem("Decimal");
    formatCombo->addItem("All");
    {
        QSignalBlocker blocker(formatCombo);
        formatCombo->setCurrentIndex(static_cast<int>(currentFormat));
    }
    top->addWidget(formatCombo);

    copyButton = new QPushButton("Copy", this);
    copyButton->setToolTip("Copy the screen text to the clipboard");
    top->addWidget(copyButton);
    layout->addLayout(top);

    textView = new QPlainTextEdit(this);
    textView->setReadOnly(true);
    textView->setLineWrapMode(QPlainTextEdit::NoWrap);
    textView->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    layout->addWidget(textView, 1);

    countLabel = new QLabel("0 bytes", this);
    layout->addWidget(countLabel);

    connect(formatCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ScreenPanel::onFormatChanged);
    connect(copyButton, &QPushButton::clicked, this, &ScreenPanel::onCopyClicked);
}

void ScreenPanel::showOutput(const std::vector<uint8_t>& output)
{
    if (output == lastOutput)
        return;                      // nothing new; avoids resetting the scroll position
    lastOutput = output;
    render();
}

void ScreenPanel::setFormat(Format format)
{
    currentFormat = format;
    {
        QSignalBlocker blocker(formatCombo);
        formatCombo->setCurrentIndex(static_cast<int>(format));
    }
    render();
}

ScreenPanel::Format ScreenPanel::format() const
{
    return currentFormat;
}

void ScreenPanel::onFormatChanged(int comboIndex)
{
    if (comboIndex < 0 || comboIndex > static_cast<int>(Format::All))
        return;
    currentFormat = static_cast<Format>(comboIndex);
    render();
}

void ScreenPanel::onCopyClicked()
{
    QApplication::clipboard()->setText(textView->toPlainText());
}

void ScreenPanel::render()
{
    textView->setPlainText(formatOutput());
    QScrollBar* bar = textView->verticalScrollBar();
    bar->setValue(bar->maximum());

    const std::size_t n = lastOutput.size();
    countLabel->setText(QString("%1 byte%2").arg(n).arg(n == 1 ? "" : "s"));
}

QString ScreenPanel::formatOutput() const
{
    QString out;

    switch (currentFormat) {
    case Format::Ascii:
        for (uint8_t b : lastOutput) {
            if (b == 0x0A)      out += QChar('\n');
            else if (b == 0x0D) continue;
            else if (b == 0x09) out += QChar('\t');
            else                out += GuiUtil::asciiChar(b);
        }
        break;

    case Format::Hex: {
        QStringList parts;
        for (uint8_t b : lastOutput)
            parts << GuiUtil::hexByte(b);
        out = parts.join(' ');
        break;
    }

    case Format::Decimal: {
        QStringList parts;
        for (uint8_t b : lastOutput)
            parts << QString::number(b);
        out = parts.join(' ');
        break;
    }

    case Format::All: {
        QStringList lines;
        for (uint8_t b : lastOutput) {
            lines << QString("%1  %2  '%3'")
                         .arg(GuiUtil::hexByte(b))
                         .arg(QString::number(b).rightJustified(3))
                         .arg(GuiUtil::asciiChar(b));
        }
        out = lines.join('\n');
        break;
    }
    }
    return out;
}
