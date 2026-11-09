// ThemeManager.cpp
#include "ThemeManager.h"

#include <QApplication>
#include <QPalette>
#include <QStyle>

Theme ThemeManager::currentTheme = Theme::Light;

HighlightColors ThemeManager::colors(Theme theme)
{
    HighlightColors c;
    if (theme == Theme::Dark) {
        c.programCounter = QColor("#6B5E00");
        c.changed        = QColor("#1E5E2A");
        c.screenCell     = QColor("#1F4468");
        c.breakpoint     = QColor("#FF5252");
        c.invalid        = QColor("#7A2323");
        c.text           = QColor("#E6E6E6");
        c.background     = QColor("#1E1E1E");
    } else {
        c.programCounter = QColor("#FFF3A0");
        c.changed        = QColor("#B7F0B1");
        c.screenCell     = QColor("#CDE6FF");
        c.breakpoint     = QColor("#E53935");
        c.invalid        = QColor("#FFB3B3");
        c.text           = QColor("#1E1E1E");
        c.background     = QColor("#FFFFFF");
    }
    return c;
}

HighlightColors ThemeManager::colors()
{
    return colors(currentTheme);
}

Theme ThemeManager::current()
{
    return currentTheme;
}

void ThemeManager::apply(Theme theme)
{
    currentTheme = theme;

    QApplication::setStyle("Fusion");
    const HighlightColors c = colors(theme);

    QPalette palette;
    if (theme == Theme::Dark) {
        const QColor window("#2B2B2B");
        const QColor button("#3A3A3A");
        const QColor disabled("#7F7F7F");

        palette.setColor(QPalette::Window, window);
        palette.setColor(QPalette::WindowText, c.text);
        palette.setColor(QPalette::Base, c.background);
        palette.setColor(QPalette::AlternateBase, QColor("#262626"));
        palette.setColor(QPalette::ToolTipBase, button);
        palette.setColor(QPalette::ToolTipText, c.text);
        palette.setColor(QPalette::Text, c.text);
        palette.setColor(QPalette::Button, button);
        palette.setColor(QPalette::ButtonText, c.text);
        palette.setColor(QPalette::BrightText, Qt::red);
        palette.setColor(QPalette::Link, QColor("#4DA3FF"));
        palette.setColor(QPalette::Highlight, QColor("#2F65CA"));
        palette.setColor(QPalette::HighlightedText, Qt::white);
        palette.setColor(QPalette::PlaceholderText, disabled);
        palette.setColor(QPalette::Disabled, QPalette::Text, disabled);
        palette.setColor(QPalette::Disabled, QPalette::ButtonText, disabled);
        palette.setColor(QPalette::Disabled, QPalette::WindowText, disabled);
    } else {
        palette = qApp->style()->standardPalette();
    }
    qApp->setPalette(palette);

    qApp->setStyleSheet(QString(
        "QToolTip { color: %1; background-color: %2; border: 1px solid #888888; }")
        .arg(c.text.name(), theme == Theme::Dark ? "#3A3A3A" : "#FFFFE1"));
}
