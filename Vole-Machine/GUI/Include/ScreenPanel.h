// ScreenPanel.h
#ifndef VOLE_GUI_SCREENPANEL_H
#define VOLE_GUI_SCREENPANEL_H

#include <QWidget>
#include <cstdint>
#include <vector>

class QPlainTextEdit;
class QComboBox;
class QPushButton;
class QLabel;

// Output written by "STORE R -> FF". The machine stores raw bytes;
// this panel decides how to present them.
class ScreenPanel : public QWidget {
    Q_OBJECT

public:
    enum class Format { Ascii, Hex, Decimal, All };   // All = "41  65  'A'" per line

    explicit ScreenPanel(QWidget* parent = nullptr);

    void showOutput(const std::vector<uint8_t>& output);
    void setFormat(Format format);
    Format format() const;

private slots:
    void onFormatChanged(int comboIndex);
    void onCopyClicked();

private:
    void buildUi();
    void render();
    QString formatOutput() const;

    Format currentFormat = Format::All;
    std::vector<uint8_t> lastOutput;

    QPlainTextEdit* textView = nullptr;    // read only, monospace
    QComboBox*      formatCombo = nullptr;
    QPushButton*    copyButton = nullptr;
    QLabel*         countLabel = nullptr;  // "3 bytes"
};

#endif