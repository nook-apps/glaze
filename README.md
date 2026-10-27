# Glaze

Общий модуль приложений студии [Nook](https://github.com/nook-apps) на Qt 6 / QML: визуальный
язык Glaze (стекло, цвета из обоев) и всё, что на разных системах устроено по-разному. Первым его
получил [Jot](https://github.com/nook-apps/Jot); следом — Page и остальные приложения Plain.

## Что внутри

| | |
|---|---|
| `Glaze::setup` | до движка QML: фирменный цвет и шрифт значков приложения, вшитый JetBrains Mono, стиль Basic, сброс начертания от qt6ct, прозрачность окна на Windows |
| `Theme` | палитра и стекло — имена и логика как у Quickshell и Shelf; `Theme.brand` — фирменный цвет приложения |
| `ThemeSource` | откуда цвета: Quickshell, matugen (`~/.local/state/<приложение>/colors.json`, иначе шаблон Shelf), Material из шелла, светлая/тёмная из портала или Qt |
| `Backdrop` | размытие под окном: Hyprland сам, KDE / GNOME 51+ / niri / KWin X11 через KF6 WindowSystem, Windows 11 — Acrylic |
| `Instance` | одно окно на сеанс через локальный сокет (на Windows — именованный канал); имя — `desktopFileName` приложения |
| `FilePortal` | окна «Открыть» и «Сохранить» через xdg-desktop-portal; без портала — `failed`, и приложение показывает окно Qt |
| `Platform` | пути: `~` вместо домашней папки, `file://` → путь, обратные косые на Windows |
| `GlassButton`, `TextButton`, `GlassDialog`, `ConfirmDialog` | кнопки и диалоги в стекле |

## Подключение

Модуль подключается сабмодулем и собирается статически — вместе с программой, без отдельных `.so`
и `qmldir` при установке.

```sh
git submodule add https://github.com/nook-apps/glaze glaze
```

```cmake
qt_standard_project_setup(REQUIRES 6.7)
# Модули приложения и Glaze — в одной папке сборки, чтобы qmllint находил import Glaze
set(QT_QML_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/qml)
add_subdirectory(glaze)
# …
target_link_libraries(app PRIVATE glaze glazeplugin)
```

```cpp
#include "Glaze.h"
#include <QQmlExtensionPlugin>

Q_IMPORT_QML_PLUGIN(GlazePlugin)

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationName("jot");                     // папки настроек и colors.json
    app.setApplicationDisplayName("Jot");
    app.setDesktopFileName("io.github.nook_apps.Jot"); // app_id окна и имя сокета
    Glaze::setup({
        .brand = QColor(0xb4, 0x8c, 0xff),
        .brandDeep = QColor(0x8b, 0x5c, 0xf6),
        .iconFamily = "Jot Icons",
        .fonts = {":/qt/qml/Jot/fonts/JotIcons.ttf", ":/qt/qml/Jot/fonts/JotIconsPropo.ttf"},
    });
    // …
}
```

В QML — `import Glaze`.

### Значки

Значки — символы Nerd Font (`\u{f…}` в QML). Набор у каждого приложения свой: скрипт вырезает из
JetBrainsMono Nerd Font только символы из QML приложения и контролов Glaze.

```sh
pip install fonttools
python glaze/tools/subset_icons.py "Jot Icons" fonts \
    JetBrainsMonoNerdFont-Regular.ttf JetBrainsMonoNerdFontPropo-Regular.ttf qml glaze/qml
```

### Ключи сборки

| | |
|---|---|
| `GLAZE_WITH_DBUS` (ON) | портал выбора файлов и тема из портала — только Linux. OFF собирает Linux так, как его видит Windows |
| `GLAZE_WITH_KWINDOWSYSTEM` (ON) | размытие под окном через KF6 WindowSystem, если библиотека найдена |

Нужны Qt ≥ 6.7 (Base, Declarative), CMake ≥ 3.21 и C++20.

## Лицензия

GNU GPL v3 или более поздняя (GPL-3.0-or-later). JetBrains Mono 2.304 в `fonts/` — SIL Open Font
License 1.1 (`fonts/OFL.txt`).
