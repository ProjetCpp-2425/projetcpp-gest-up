#ifndef INTEGERDELEGATE_H
#define INTEGERDELEGATE_H

#include <QStyledItemDelegate>

class IntegerDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    // Constructor
    explicit IntegerDelegate(QObject *parent = nullptr);

    // Override displayText to format large integers without scientific notation
    QString displayText(const QVariant &value, const QLocale &locale) const override;
};

#endif // INTEGERDELEGATE_H
