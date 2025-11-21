#pragma once
#include <QString>
#include <QSysInfo>

namespace Main
{
void
installTheme()
{
    QString osName = QSysInfo::prettyProductName().toLower();

    if(osName.contains("android"))
    {
        qputenv("QT_QUICK_CONTROLS_STYLE", QByteArray("Material"));
        qputenv("QT_QUICK_CONTROLS_MATERIAL_THEME", QByteArray("Dark"));
    }
    else if(osName.contains("ios"))
    {
        qputenv("QT_QUICK_CONTROLS_STYLE", QByteArray("iOS"));
    }
    else if(osName.contains("windows"))
    {
        qputenv("QT_QUICK_CONTROLS_STYLE", QByteArray("FluentWinUI3"));
        qputenv("QT_QUICK_CONTROLS_FLUENTWINUI_THEME", QByteArray("Dark"));
    }
    else if(osName.contains("macos"))
    {
        qputenv("QT_QUICK_CONTROLS_STYLE", QByteArray("MacOS"));
    }
    else if(osName.contains("linux"))
    {
        qputenv("QT_QUICK_CONTROLS_STYLE", QByteArray("Ubuntu"));
    }
    else
    {
        qputenv("QT_QUICK_CONTROLS_STYLE", QByteArray("Basic"));
    }
}
}    // namespace Main
