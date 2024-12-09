#include <QStyledItemDelegate>
#include <QSpinBox>

class IntegerDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit IntegerDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

    QString displayText(const QVariant &value, const QLocale &locale) const override {
        if (value.type() == QVariant::Double || value.type() == QVariant::LongLong || value.type() == QVariant::ULongLong) {
            return locale.toString(value.toLongLong());  // Format large integers without scientific notation
        }
        return QStyledItemDelegate::displayText(value, locale);
    }
};
