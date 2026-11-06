// ControlBar.h
#ifndef VOLE_GUI_CONTROLBAR_H
#define VOLE_GUI_CONTROLBAR_H

#include <QWidget>
#include <optional>
#include "GuiTypes.h"

class QPushButton;
class QLineEdit;
class QSlider;
class QLabel;

// Top bar: Load | Start address | Step | Run All | Run/Pause | Speed | Restart | Reset
class ControlBar : public QWidget
{
    Q_OBJECT

public:
    explicit ControlBar(QWidget *parent = nullptr);

    // Parses the start address field (hex). nullopt if empty/invalid.
    std::optional<int> startAddress() const;
    void setStartAddress(int address);

    // Delay between steps in the animated run, in milliseconds.
    int stepDelayMs() const;

    // Enables/disables buttons according to the application state.
    void setUiState(UiState state);

signals:
    void loadClicked();
    void stepClicked();
    void runAllClicked();
    void runToggled(bool running); // Run/Pause button
    void restartClicked();         // reload the same file from the beginning
    void resetClicked();           // clear the whole machine
    void speedChanged(int delayMs);

private slots:
    void onRunButtonToggled(bool checked);
    void onSliderChanged(int value);

private:
    void buildUi();
    static int delayForSlider(int sliderValue);

    QPushButton *loadButton = nullptr;
    QLineEdit *startEdit = nullptr; // hex, validator: even addresses < FF
    QPushButton *stepButton = nullptr;
    QPushButton *runAllButton = nullptr;
    QPushButton *runButton = nullptr; // checkable: Run / Pause
    QSlider *speedSlider = nullptr;
    QLabel *speedLabel = nullptr; // "250 ms/step"
    QPushButton *restartButton = nullptr;
    QPushButton *resetButton = nullptr;
};

#endif