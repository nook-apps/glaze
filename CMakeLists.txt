# Glaze — общий модуль приложений Nook: платформенный слой (одно окно на сеанс,
# окна выбора файлов, источник темы, размытие под окном, пути) и QML-модуль Glaze
# с темой, кнопками и диалогами. Подключается из приложения:
#
#   add_subdirectory(glaze)
#   target_link_libraries(app PRIVATE glaze glazeplugin)   # + Q_IMPORT_QML_PLUGIN(GlazePlugin) в main
#
# Ниже 6.7 не опускаемся: ThemeSource читает ~/.local/state через GenericStateLocation
cmake_minimum_required(VERSION 3.21)

find_package(Qt6 6.7 REQUIRED COMPONENTS Core Gui Quick QuickControls2 Network)

# D-Bus — только на Linux: портал выбора файлов и светлая/тёмная тема из портала.
# На Windows и macOS модуль собирается без него и берёт окна выбора и тему у Qt.
# -DGLAZE_WITH_DBUS=OFF собирает Linux-версию так, как её видит Windows: проверка
# запасных путей без второй машины
option(GLAZE_WITH_DBUS "Портал и тема через D-Bus (только Linux)" ON)
if(UNIX AND NOT APPLE AND GLAZE_WITH_DBUS)
    find_package(Qt6 REQUIRED COMPONENTS DBus)
    set(GLAZE_DBUS ON)
endif()
# Размытие под окном в KDE, GNOME 51+, niri (ext-background-effect-v1) и KWin на X11.
# Нет библиотеки — всё соберётся и так, просто там окно будет непрозрачным
option(GLAZE_WITH_KWINDOWSYSTEM "Размытие под окном через KF6 WindowSystem (Linux)" ON)
if(UNIX AND NOT APPLE AND GLAZE_WITH_KWINDOWSYSTEM)
    find_package(KF6WindowSystem QUIET)
    if(KF6WindowSystem_FOUND)
        set(GLAZE_KWINDOWSYSTEM ON)
    else()
        message(STATUS "KF6WindowSystem не найден: размытие только в Hyprland")
    endif()
endif()

set(GLAZE_QML_FILES
    qml/Theme.qml
    qml/GlassButton.qml
    qml/GlassDialog.qml
    qml/TextButton.qml
    qml/ConfirmDialog.qml
)
set_source_files_properties(qml/Theme.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)
# Файлы лежат в qml/, но в модуле — в корне: иначе синглтон Theme
# не виден из соседних файлов (неявный импорт идёт по каталогу файла)
foreach(f ${GLAZE_QML_FILES})
    get_filename_component(name ${f} NAME)
    set_source_files_properties(${f} PROPERTIES QT_RESOURCE_ALIAS ${name})
endforeach()

# Статически: модуль вшивается в программу, рядом не нужно ставить ни .so, ни qmldir
qt_add_qml_module(glaze
    URI Glaze
    VERSION 1.0
    STATIC
    QML_FILES ${GLAZE_QML_FILES}
    RESOURCES
        fonts/JetBrainsMono-Regular.otf fonts/JetBrainsMono-Italic.otf fonts/JetBrainsMono-SemiBold.otf
        fonts/JetBrainsMono-Bold.otf fonts/JetBrainsMono-BoldItalic.otf
    SOURCES
        src/Glaze.h src/Glaze.cpp
        src/Platform.h src/Platform.cpp
        src/Instance.h src/Instance.cpp
        src/FilePortal.h src/FilePortal.cpp
        src/ThemeSource.h src/ThemeSource.cpp
        src/Backdrop.h src/Backdrop.cpp
)

# Только байт-код, без C++ для привязок (AOT): сгенерированный C++ пользуется
# закрытым API Qt и привязал бы сборку к точной версии Qt (см. Shelf)
set_target_properties(glaze PROPERTIES QT_QMLCACHEGEN_ARGUMENTS "--only-bytecode")
target_include_directories(glaze PUBLIC src)
target_link_libraries(glaze PUBLIC Qt6::Quick Qt6::QuickControls2 Qt6::Network)
# PUBLIC: от них зависит устройство классов в заголовках
if(GLAZE_DBUS)
    target_link_libraries(glaze PUBLIC Qt6::DBus)
    target_compile_definitions(glaze PUBLIC GLAZE_HAVE_DBUS)
endif()
if(GLAZE_KWINDOWSYSTEM)
    target_link_libraries(glaze PRIVATE KF6::WindowSystem)
    target_compile_definitions(glaze PUBLIC GLAZE_HAVE_KWINDOWSYSTEM)
endif()
if(WIN32)
    target_link_libraries(glaze PRIVATE dwmapi)
endif()
if(MSVC)
    target_compile_options(glaze PRIVATE /W4 /utf-8)
else()
    target_compile_options(glaze PRIVATE -Wall -Wextra)
endif()
