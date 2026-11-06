// StatusText.h
#ifndef VOLE_GUI_STATUSTEXT_H
#define VOLE_GUI_STATUSTEXT_H

#include <QString>
#include <cstdint>
#include "Common.h"      // StepStatus, LoadResult

enum class MessageSeverity { Info, Warning, Error };

struct StatusMessage {
    QString text;
    MessageSeverity severity = MessageSeverity::Info;
    bool showDialog = false;     // true for errors the user must notice
};

namespace StatusText {

// Covers all five StepStatus values.
StatusMessage fromStep(StepStatus status, uint8_t programCounter);

// Wraps a failed/successful LoadResult (line number + message).
StatusMessage fromLoad(const LoadResult& result);

// Messages for rejected edits (bad hex, out of range, FF cell, ...).
StatusMessage invalidInput(const QString& what);

} // namespace StatusText

#endif