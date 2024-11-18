#include "PieChartWidget.h"
#include <QPainter>
#include <QColor>
#include <QRandomGenerator>
#include <QPen>
#include <cmath>

void PieChartWidget::drawPieChart(const QList<QPair<QString, qreal>>& data, QWidget* widget)
{
    // Ensure widget is valid and has a proper paint event
    if (widget == nullptr) {
        return;
    }

    // Create a painter for the widget
    QPainter painter(widget);

    // Ensure the painter is active
    if (!painter.isActive()) {
        return;
    }

    painter.setRenderHint(QPainter::Antialiasing);  // Enable antialiasing for smoother drawing

    // Define the chart area inside the widget with some padding
    QRect chartArea(10, 10, widget->width() - 20, widget->height() - 20);

    // Call the helper function to draw the pie chart
    paintPieChart(data, painter, chartArea);
}

void PieChartWidget::paintPieChart(const QList<QPair<QString, qreal>>& data, QPainter &painter, const QRect& chartArea)
{
    qreal total = 0;
    for (const auto& pair : data)
        total += pair.second;

    if (total <= 0) {
        return;
    }

    // Set the brush and pen for the pie chart
    painter.setPen(QPen(Qt::black));  // Set black outline for the segments

    // Random number generator for colors
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);

    qreal startAngle = 0.0;
    for (const auto& pair : data) {
        qreal angleSpan = 360.0 * (pair.second / total);

        QColor color = QColor::fromHsv(dis(gen), 255, 200);  // Random color generation
        painter.setBrush(color);

        // Draw the pie slice
        painter.drawPie(chartArea, int(startAngle * 16), int(angleSpan * 16));

        // Calculate the label's position
        qreal midAngle = startAngle + angleSpan / 2;
        qreal radius = chartArea.width() / 2 + 10;
        QPointF labelPos(
            chartArea.center().x() + radius * qCos(qDegreesToRadians(midAngle)),
            chartArea.center().y() - radius * qSin(qDegreesToRadians(midAngle))
        );

        // Draw the label text
        painter.setPen(Qt::black);
        painter.drawText(labelPos, pair.first);

        startAngle += angleSpan;
    }
}
