// ControlBar.cpp
#include "ControlBar.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QSignalBlocker>
#include <QSlider>

namespace {

// Slider position -> delay between steps. Left = slow, right = fast.
// The fastest value is ~one screen frame (60 Hz): anything quicker executes several
// steps between two repaints, so the highlight appears to jump.
constexpr int kDelays[] = {1000, 500, 250, 100, 50, 30, 16};
constexpr int kDelayCount = static_cast<int>(sizeof(kDelays) / sizeof(kDelays[0]));
constexpr int kDefaultSlider = 2;   // 250 ms/step

} // namespace

ControlBar::ControlBar(QWidget* parent)
    : QWidget(parent)
{
    buildUi();
    setUiState(UiState::NoProgram);
}

int ControlBar::delayForSlider(int sliderValue)
{
    if (sliderValue < 0) sliderValue = 0;
    if (sliderValue >= kDelayCount) sliderValue = kDelayCount - 1;
    return kDelays[sliderValue];
}

void ControlBar::buildUi()
{
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(6, 4, 6, 4);
    layout->setSpacing(6);

    loadButton = new QPushButton("Load...", this);
    loadButton->setToolTip("Open a program file (Ctrl+O)");
    layout->addWidget(loadButton);

    layout->addWidget(new QLabel("Start (hex):", this));
    startEdit = new QLineEdit("00", this);
    startEdit->setMaxLength(2);
    startEdit->setFixedWidth(44);
    startEdit->setAlignment(Qt::AlignCenter);
    startEdit->setPlaceholderText("00");
    startEdit->setToolTip("Address where the program is loaded (hex, even, 00-FE)");
    startEdit->setValidator(new QRegularExpressionValidator(
        QRegularExpression("[0-9A-Fa-f]{0,2}"), startEdit));
    layout->addWidget(startEdit);

    layout->addSpacing(8);

    stepButton = new QPushButton("Step", this);
    stepButton->setToolTip("Execute one instruction (F10)");
    layout->addWidget(stepButton);

    runAllButton = new QPushButton("Run All", this);
    runAllButton->setToolTip("Run until HALT without animation (F5)");
    layout->addWidget(runAllButton);

    runButton = new QPushButton("Run", this);
    runButton->setCheckable(true);
    runButton->setToolTip("Animated run; stops at breakpoints (F6)");
    layout->addWidget(runButton);

    layout->addSpacing(8);
    layout->addWidget(new QLabel("Speed:", this));

    speedSlider = new QSlider(Qt::Horizontal, this);
    speedSlider->setRange(0, kDelayCount - 1);
    speedSlider->setValue(kDefaultSlider);
    speedSlider->setFixedWidth(130);
    speedSlider->setToolTip("Delay between steps in the animated run");
    layout->addWidget(speedSlider);

    speedLabel = new QLabel(this);
    speedLabel->setMinimumWidth(80);
    speedLabel->setText(QString("%1 ms/step").arg(delayForSlider(kDefaultSlider)));
    layout->addWidget(speedLabel);

    layout->addStretch(1);

    restartButton = new QPushButton("Restart", this);
    restartButton->setToolTip("Reload the same file from the beginning");
    layout->addWidget(restartButton);

    resetButton = new QPushButton("Reset", this);
    resetButton->setToolTip("Clear memory, registers and screen (Ctrl+R)");
    layout->addWidget(resetButton);

    connect(loadButton,    &QPushButton::clicked, this, &ControlBar::loadClicked);
    connect(stepButton,    &QPushButton::clicked, this, &ControlBar::stepClicked);
    connect(runAllButton,  &QPushButton::clicked, this, &ControlBar::runAllClicked);
    connect(restartButton, &QPushButton::clicked, this, &ControlBar::restartClicked);
    connect(resetButton,   &QPushButton::clicked, this, &ControlBar::resetClicked);
    connect(runButton,     &QPushButton::toggled, this, &ControlBar::onRunButtonToggled);
    connect(speedSlider,   &QSlider::valueChanged, this, &ControlBar::onSliderChanged);
}

std::optional<int> ControlBar::startAddress() const
{
    const QString text = startEdit->text().trimmed();
    if (text.isEmpty())
        return std::nullopt;
    return GuiUtil::parseHexAddress(text);
}

void ControlBar::setStartAddress(int address)
{
    if (address < 0 || address > 0xFF)
        return;
    startEdit->setText(GuiUtil::hexByte(static_cast<uint8_t>(address)));
}

int ControlBar::stepDelayMs() const
{
    return delayForSlider(speedSlider->value());
}

void ControlBar::setUiState(UiState state)
{
    bool load = false, start = false, step = false, runAll = false;
    bool run = false, restart = false, reset = true;
    bool running = false;

    switch (state) {
    case UiState::NoProgram:
        load = true; start = true;
        break;
    case UiState::Ready:
        load = true; start = true; step = true; runAll = true; run = true; restart = true;
        break;
    case UiState::Running:
        run = true; running = true;
        break;
    case UiState::Halted:
        load = true; start = true; restart = true;
        break;
    }

    loadButton->setEnabled(load);
    startEdit->setEnabled(start);
    stepButton->setEnabled(step);
    runAllButton->setEnabled(runAll);
    runButton->setEnabled(run);
    restartButton->setEnabled(restart);
    resetButton->setEnabled(reset);

    {
        QSignalBlocker blocker(runButton);
        runButton->setChecked(running);
        runButton->setText(running ? "Pause" : "Run");
    }
}

void ControlBar::onRunButtonToggled(bool checked)
{
    runButton->setText(checked ? "Pause" : "Run");
    emit runToggled(checked);
}

void ControlBar::onSliderChanged(int value)
{
    const int delay = delayForSlider(value);
    speedLabel->setText(QString("%1 ms/step").arg(delay));
    emit speedChanged(delay);
}
