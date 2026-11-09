// StatusText.cpp
#include "StatusText.h"
#include "GuiTypes.h"

namespace StatusText {

StatusMessage fromStep(StepStatus status, uint8_t programCounter)
{
    StatusMessage m;
    const QString pc = GuiUtil::hexByte(programCounter);

    switch (status) {
    case StepStatus::OK:
        m.text = QString("Step executed (PC was %1).").arg(pc);
        m.severity = MessageSeverity::Info;
        m.showDialog = false;
        break;
    case StepStatus::HALTED:
        m.text = "Program halted.";
        m.severity = MessageSeverity::Info;
        m.showDialog = false;
        break;
    case StepStatus::INVALID_INSTRUCTION:
        m.text = QString("Invalid instruction at address %1. Execution stopped.").arg(pc);
        m.severity = MessageSeverity::Error;
        m.showDialog = true;
        break;
    case StepStatus::INVALID_PROGRAM_COUNTER:
        m.text = QString("Invalid program counter (%1): it must be an even address "
                         "below FF so a full instruction can be fetched.").arg(pc);
        m.severity = MessageSeverity::Error;
        m.showDialog = true;
        break;
    case StepStatus::STEP_LIMIT_REACHED:
        m.text = "Step limit reached. The program may contain an infinite loop.";
        m.severity = MessageSeverity::Warning;
        m.showDialog = true;
        break;
    }
    return m;
}

StatusMessage fromLoad(const LoadResult& result)
{
    StatusMessage m;
    if (result.success) {
        m.text = QString("Loaded %1 instruction word(s).").arg(result.wordsLoaded);
        m.severity = MessageSeverity::Info;
        m.showDialog = false;
        return m;
    }

    const QString reason = QString::fromStdString(result.message);
    if (result.lineNumber > 0)
        m.text = QString("Could not load the program (line %1): %2")
                     .arg(result.lineNumber).arg(reason);
    else
        m.text = QString("Could not load the program: %1").arg(reason);
    m.severity = MessageSeverity::Error;
    m.showDialog = true;
    return m;
}

StatusMessage invalidInput(const QString& what)
{
    StatusMessage m;
    m.text = what;
    m.severity = MessageSeverity::Warning;
    m.showDialog = false;
    return m;
}

} // namespace StatusText
