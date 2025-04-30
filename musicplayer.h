#ifndef MUSICPLAYER_H
#define MUSICPLAYER_H

#include <QObject>
#include <QMediaPlayer>
#include <QAudioOutput>

class MusicPlayer : public QObject
{
    Q_OBJECT
public:
    explicit MusicPlayer(QObject *parent = nullptr);
    ~MusicPlayer();

    void playBackgroundMusic();
    void stopMusic();
    void setVolume(int volume);
    int volume() const;
    bool isPlaying() const;
    void toggleMute();
    bool isMuted() const;

private:
    QMediaPlayer *player;
    QAudioOutput *audioOutput;
    int savedVolume = 50;
    int currentVolume = 50;
};

#endif // MUSICPLAYER_H
