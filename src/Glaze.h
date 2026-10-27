#pragma once

#include <QColor>
#include <QObject>
#include <QStringList>
#include <QtQml/qqmlregistration.h>

namespace Glaze {

// Что у приложений Nook своё, а не общее
struct Style
{
    // Фирменный цвет — на тёмном стекле и на светлом (там светлый теряется).
    // Только штрихами: курсор, маркеры, точка несохранённого; остальное — из обоев
    QColor brand;
    QColor brandDeep;
    // Семейство шрифта значков; «<имя> Propo» — его вариант для Material.
    // Набор значков у каждого приложения свой (tools/subset_icons.py)
    QString iconFamily;
    // Свои шрифты из ресурсов (значки и прочее); шрифты Glaze подключаются сами
    QStringList fonts;
};

// В main после QGuiApplication и до движка QML: шрифты, стиль контролов,
// прозрачность окна на Windows
void setup(const Style &style);

}

// Style для QML: Theme берёт отсюда brand и iconFont
class Brand : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(QColor color READ color CONSTANT)
    Q_PROPERTY(QColor deep READ deep CONSTANT)
    Q_PROPERTY(QString iconFamily READ iconFamily CONSTANT)

public:
    explicit Brand(QObject *parent = nullptr);

    QColor color() const;
    QColor deep() const;
    QString iconFamily() const;
};
