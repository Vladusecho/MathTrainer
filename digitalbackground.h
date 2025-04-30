#ifndef DIGITALBACKGROUND_H
#define DIGITALBACKGROUND_H

#include <QWidget>
#include <QTimer>
#include <QVector>
#include <QFont>

class DigitalBackground : public QWidget {
    Q_OBJECT
public:
    explicit DigitalBackground(QWidget *parent = nullptr);
    void setDigitColors(const QVector<QColor>& colors);
    void setDigitSpeed(int speed);
    void setFontSizeRange(int min, int max);

protected:
    void paintEvent(QPaintEvent *) override;
    void resizeEvent(QResizeEvent *) override;

private slots:
    void updateDigits();

private:
    struct Digit {
        QPointF position;
        qreal speed;
        QString value;
        QColor color;
        int fontSize;
        qreal opacity;
    };

    QTimer *m_timer;
    QVector<Digit> m_digits;
    QVector<QColor> m_colors;
    QFont m_font;
    int m_minFontSize = 12;
    int m_maxFontSize = 36;
    int m_speed = 2;

    void initDigits();
    QString randomDigit() const;
};

#endif // DIGITALBACKGROUND_H
