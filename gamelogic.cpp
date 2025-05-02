#include "gamelogic.h"
#include <QRandomGenerator>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>

GameLogic::GameLogic(QObject *parent)
    : QObject(parent)
{
    gameTimer = new QTimer(this);
    countdownTimer = new QTimer(this);

    connect(gameTimer, &QTimer::timeout, this, &GameLogic::endGame);
    connect(countdownTimer, &QTimer::timeout, this, &GameLogic::updateTimer);
}

void GameLogic::startGame(Difficulty choosenDifficulty)
{
    difficulty = choosenDifficulty;
    score = 0;
    incorrects = 0;
    timeLeft = 30;

    emit timeUpdated(QString("Осталось: %1 с.").arg(timeLeft));

    // Настройка таймеров
    gameTimer->start(timeLeft * 1000);
    countdownTimer->start(1000);

    // Генерация первого примера
    generateProblem();
}


void GameLogic::generateProblem()
{
    int a, b;
    QString problem;
    int operation;

    switch(difficulty) {
    case Easy:
        a = QRandomGenerator::global()->bounded(2, 10);
        b = QRandomGenerator::global()->bounded(2, 10);
        if ((a % b == 0) && (a > b)) {
            operation = 3;
        } else {
            operation = QRandomGenerator::global()->bounded(3);
        }
        switch(operation) {
        case 0:
            problem = QString("%1 + %2 = ?").arg(a).arg(b);
            currentAnswer = a + b;
            break;
        case 1:
            problem = QString("%1 - %2 = ?").arg(a).arg(b);
            currentAnswer = a - b;
            break;
        case 2:
            problem = QString("%1 × %2 = ?").arg(a).arg(b);
            currentAnswer = a * b;
            break;
        case 3:
            problem = QString("%1 / %2 = ?").arg(a).arg(b);
            currentAnswer = a / b;
            break;
        }
        break;

    case Medium:
        a = QRandomGenerator::global()->bounded(2, 50);
        b = QRandomGenerator::global()->bounded(2, 50);
        if (a < 12 && b < 12) {
            operation = 2;
        } else if ((a % b == 0) && (a > b)) {
            operation = 3;
        } else {
            operation = QRandomGenerator::global()->bounded(2);
        }
        switch(operation) {
        case 0:
            problem = QString("%1 + %2 = ?").arg(a).arg(b);
            currentAnswer = a + b;
            break;
        case 1:
            problem = QString("%1 - %2 = ?").arg(a).arg(b);
            currentAnswer = a - b;
            break;
        case 2:
            problem = QString("%1 × %2 = ?").arg(a).arg(b);
            currentAnswer = a * b;
            break;
        case 3:
            problem = QString("%1 / %2 = ?").arg(a).arg(b);
            currentAnswer = a / b;
            break;
        }
        break;

    case Hard:
        a = QRandomGenerator::global()->bounded(2, 500);
        b = QRandomGenerator::global()->bounded(2, 500);
        if (a < 50 && b < 50) {
            operation = 2;
        } else if ((a % b == 0) && (a > b)) {
            operation = 3;
        } else {
            operation = QRandomGenerator::global()->bounded(2);
        }
        switch(operation) {
        case 0:
            problem = QString("%1 + %2 = ?").arg(a).arg(b);
            currentAnswer = a + b;
            break;
        case 1:
            problem = QString("%1 - %2 = ?").arg(a).arg(b);
            currentAnswer = a - b;
            break;
        case 2:
            problem = QString("%1 × %2 = ?").arg(a).arg(b);
            currentAnswer = a * b;
            break;
        case 3:
            problem = QString("%1 / %2 = ?").arg(a).arg(b);
            currentAnswer = a / b;
            break;
        }
        break;
    }

    emit newProblemGenerated(problem);
}


void GameLogic::checkAnswer(QLineEdit *answerInput)
{
    if(answerInput && answerInput->text().toInt() == currentAnswer) {
        score++;
    } else if (answerInput && answerInput->text().toInt() != currentAnswer) {
        incorrects++;
    }
    answerInput->clear();
    generateProblem();
}


void GameLogic::updateTimer()
{
    timeLeft--;

    QString color;
    if (timeLeft <= 15) color = "red";
    emit timeUpdated(QString("<font color='%1'>Осталось: %2 с.</font>").arg(color).arg(timeLeft));

    if(timeLeft <= 0) {
        endGame();
    }
}


void GameLogic::endGame()
{
    gameTimer->stop();
    countdownTimer->stop();
    emit gameFinished(score, incorrects, difficulty);
}


void GameLogic::endGameFromBack()
{
    gameTimer->stop();
    countdownTimer->stop();
}
