#include "musicplayer.h"
#include <QDebug>


MusicPlayer::MusicPlayer(QObject *parent) : QObject(parent)
{
    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);

    player->setAudioOutput(audioOutput);
    player->setLoops(QMediaPlayer::Infinite); // Бесконечное повторение

    audioOutput->setVolume(savedVolume / 100.0f);
}


MusicPlayer::~MusicPlayer()
{
    stopMusic();
    delete player;
    delete audioOutput;
}


void MusicPlayer::playBackgroundMusic()
{
    player->setSource(QUrl("qrc:/music/back_music.wav"));
    player->play();
}


void MusicPlayer::playBackgroundGameMusic()
{

    player->setSource(QUrl("qrc:/music/game_music.wav"));
    player->play();
}


void MusicPlayer::stopMusic()
{
    player->stop();
}

void MusicPlayer::setVolume(int volume)
{
    volume = qBound(0, volume, 100);

    audioOutput->setVolume(volume / 100.0f);

    if (volume > 0 && this->isMuted()) {
        currentVolume = volume;
        audioOutput->setVolume(volume / 100.0f);
    }
}

int MusicPlayer::volume() const
{
    return qRound(audioOutput->volume() * 100);
}

bool MusicPlayer::isPlaying() const
{
    return player->playbackState() == QMediaPlayer::PlayingState;
}

void MusicPlayer::toggleMute() {
    if (isMuted()) {
        int newVolume = currentVolume > 0 ? currentVolume : 50;
        setVolume(newVolume);
    } else {
        currentVolume = this->volume();
        setVolume(0);
    }
}

bool MusicPlayer::isMuted() const
{
    return qFuzzyIsNull(audioOutput->volume());
}
