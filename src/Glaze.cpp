#include "Glaze.h"

#include <QFont>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QQuickStyle>
#include <QQuickWindow>

namespace {
Glaze::Style s_style;
}

void Glaze::setup(const Style &style)
{
    s_style = style;

    // JetBrains Mono вшит: на Windows и в чистом дистрибутиве его нет, а вид должен быть один
    for (const char *file : {"JetBrainsMono-Regular.otf", "JetBrainsMono-Italic.otf", "JetBrainsMono-SemiBold.otf",
                             "JetBrainsMono-Bold.otf", "JetBrainsMono-BoldItalic.otf"})
        QFontDatabase::addApplicationFont(QStringLiteral(":/qt/qml/Glaze/fonts/") + QLatin1String(file));
    for (const QString &file : style.fonts)
        QFontDatabase::addApplicationFont(file);

    // qt6ct кладёт в шрифт приложения имя начертания («Regular»), его наследуют
    // все шрифты, и оно перебивает жирность: font.bold и жирное в тексте
    // рисовались бы обычным. Начертание выбираем по весу сами
    QFont font = QGuiApplication::font();
    if (!font.styleName().isEmpty()) {
        font.setStyleName(QString());
        QGuiApplication::setFont(font);
    }

    // Свои контролы рисуем сами; Basic не тянет вид qt6ct/kvantum
    QQuickStyle::setStyle(QStringLiteral("Basic"));
#ifdef Q_OS_WIN
    // Без альфа-канала окно на Windows непрозрачное, и фон Acrylic под ним не виден
    QQuickWindow::setDefaultAlphaBuffer(true);
#endif
}

Brand::Brand(QObject *parent)
    : QObject(parent)
{
}

QColor Brand::color() const
{
    return s_style.brand;
}

QColor Brand::deep() const
{
    return s_style.brandDeep.isValid() ? s_style.brandDeep : s_style.brand;
}

QString Brand::iconFamily() const
{
    return s_style.iconFamily;
}
