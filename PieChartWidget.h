#ifndef PIECHARTWIDGET_H
#define PIECHARTWIDGET_H

#include <QWidget>
#include <QPair>
#include <QList>
#include <QPainter>
#include <QColor>
#include <QRandomGenerator>
#include <QPen>

class PieChartWidget : public QWidget
{
    Q_OBJECT

public:
    // Static function to draw a pie chart on the given widget
    static void drawPieChart(const QList<QPair<QString, qreal>>& data, QWidget* widget);

private:
    // Helper function to draw the pie chart on the widget
    static void paintPieChart(const QList<QPair<QString, qreal>>& data, QPainter &painter, const QRect& chartArea);
};

#endif // PIECHARTWIDGET_H
