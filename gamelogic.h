#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include <QObject>
#include <QTimer>
#include <QStackedWidget>
#include <QTime>
#include <QLineEdit>
#include <QLabel>

class GameLogic : public QObject
{
    Q_OBJECT
public:
    enum Difficulty { Easy, Medium, Hard };

    explicit GameLogic(QObject *parent = nullptr);

public slots:
    void startGame(Difficulty difficulty);
    void checkAnswer(QLineEdit *answerInput);
    void updateTimer();
    void endGameFromBack();

signals:
    void newProblemGenerated(const QString &problem);
    void timeUpdated(const QString &timeLeft);
    void gameFinished(int score, int incorrect, Difficulty diff);

private:
    void generateProblem();
    void endGame();

    QStackedWidget *stack;
    QTimer *gameTimer;
    QTimer *countdownTimer;
    Difficulty difficulty;
    int timeLeft;
    int score;
    int incorrects;
    int currentAnswer;
};

#endif // GAMELOGIC_H
