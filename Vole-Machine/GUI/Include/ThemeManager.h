// ThemeManager.h
#ifndef VOLE_GUI_THEMEMANAGER_H
#define VOLE_GUI_THEMEMANAGER_H

#include <QColor>

enum class Theme { Light, Dark };

// Colors used by the panels for highlighting.
struct HighlightColors {
    QColor programCounter;   // cells/rows at the current PC
    QColor changed;          // value changed by the last step
    QColor screenCell;       // memory cell FF (maps to the screen)
    QColor breakpoint;       // breakpoint marker
    QColor invalid;          // rejected input
    QColor text;
    QColor background;
};

class ThemeManager {
public:
    // Applies palette + stylesheet to the whole application.
    static void apply(Theme theme);

    static Theme current();
    static HighlightColors colors();           // for the current theme
    static HighlightColors colors(Theme theme);

private:
    ThemeManager() = delete;
    static Theme currentTheme;
};

#endif