#include "digitalbackground.h"
#include <QRandomGenerator>
#include <QDateTime>
#include <QPainter>


DigitalBackground::DigitalBackground(QWidget *parent)
    : QWidget(parent), m_colors({Qt::green, Qt::cyan, Qt::white}) {
    setAttribute(Qt::WA_TranslucentBackground);
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &DigitalBackground::updateDigits);
    m_timer->start(40); // Частота обновления

    initDigits();
}


void DigitalBackground::initDigits() {
    m_digits.clear();
    auto *rng = QRandomGenerator::global();

    for (int i = 0; i < 50; ++i) {
        Digit d;
        d.position = QPointF(rng->bounded(width()), rng->bounded(height()));
        d.speed = 1 + rng->bounded(m_speed);
        d.value = randomDigit();
        d.color = m_colors[rng->bounded(m_colors.size())];
        d.fontSize = m_minFontSize + rng->bounded(m_maxFontSize - m_minFontSize);
        d.opacity = 0.1 + rng->bounded(90) / 100.0;
        m_digits.append(d);
    }
}


QString DigitalBackground::randomDigit() const {
    static const QString digits = "0123456789+=-";
    auto *rng = QRandomGenerator::global();
    return QString(digits[rng->bounded(digits.length())]);
}


void DigitalBackground::updateDigits() {
    auto *rng = QRandomGenerator::global();

    for (Digit &d : m_digits) {
        // Движение вниз с разной скоростью
        d.position.ry() += d.speed;

        // Если цифра ушла за границу - создаём новую вверху
        if (d.position.y() > height()) {
            d.position = QPointF(rng->bounded(width()), -20);
            d.value = randomDigit();
        }

        // Случайное изменение цифры (с вероятностью 10%)
        if (rng->bounded(100) < 10) {
            d.value = randomDigit();
        }
    }
    update();
}


void DigitalBackground::paintEvent(QPaintEvent *) {
    QPainter painter(this);

    QLinearGradient gradient(0, 0, 0, height());
    gradient.setColorAt(0, QColor(170, 255, 255));
    gradient.setColorAt(1, QColor(170, 255, 255));
    painter.fillRect(rect(), gradient);

    auto *rng = QRandomGenerator::global();

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);

    for (const Digit &d : m_digits) {
        QFont font("Verdana", d.fontSize, QFont::Bold);
        font.setStyleStrategy(QFont::PreferAntialias);
        painter.setFont(font);

        // 3.2. Настройка цвета с прозрачностью
        QColor color = d.color;
        color.setAlphaF(d.opacity);

        // 3.4. Рисуем саму цифру
        painter.save();
        painter.translate(d.position);

        // // Случайный наклон (-15..+15 градусов)
        // painter.rotate(rng->bounded(30) - 15);

        // Случайное масштабирование (0.8..1.2)
        // qreal scale = 0.8 + rng->bounded(40) / 100.0;
        // painter.scale(scale, scale);

        painter.setPen(color);
        painter.drawText(0, 0, d.value);
        painter.restore();
    }
}

void DigitalBackground::resizeEvent(QResizeEvent *) {
    initDigits();
}

// Дополнительные настройки
void DigitalBackground::setDigitColors(const QVector<QColor>& colors) {
    m_colors = colors;
    initDigits();
}

void DigitalBackground::setDigitSpeed(int speed) {
    m_speed = speed;
}

void DigitalBackground::setFontSizeRange(int min, int max) {
    m_minFontSize = min;
    m_maxFontSize = max;
    initDigits();
}
