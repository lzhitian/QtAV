/******************************************************************************
    QtAV:  Media play library based on Qt and FFmpeg
    Copyright (C) 2013 Wang Bin <wbsecg1@gmail.com>

*   This file is part of QtAV

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
******************************************************************************/

#ifndef QTAV_QML_AVPLAYER_H
#define QTAV_QML_AVPLAYER_H

#include <QmlAV/Export.h>
#include <QtCore/QObject>
#include <QmlAV/QQuickItemRenderer.h>

namespace QtAV {
class AVPlayer;
}
using namespace QtAV;
class QMLAV_EXPORT QmlAVPlayer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(int duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(int position READ position NOTIFY positionChanged)
    Q_PROPERTY(bool muted READ isMuted WRITE setMuted NOTIFY mutedChanged)
    Q_PROPERTY(PlaybackState playbackState READ playbackState NOTIFY playbackStateChanged)
    Q_PROPERTY(qreal speed READ speed WRITE setSpeed NOTIFY speedChanged)
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_ENUMS(PlaybackState)
public:
    enum PlaybackState {
        PlayingState,
        PausedState,
        StoppedState
    };

    explicit QmlAVPlayer(QObject *parent = 0);
    QUrl source() const;
    void setSource(const QUrl& url);
    QObject* videoOut();
    void setVideoOut(QObject* out);
    qreal volume() const;
    void setVolume(qreal volume);
    bool isMuted() const;
    void setMuted(bool m);
    int duration() const;
    int position() const;
    PlaybackState playbackState() const;
    void setPlaybackState(PlaybackState playbackState);
    qreal speed() const;
    void setSpeed(qreal s);
    Q_INVOKABLE void play(const QUrl& url);
    AVPlayer *player();

public Q_SLOTS:
    void play();
    void pause();
    void resume();
    void togglePause();
    void stop();
    void nextFrame();
    void seek(qreal position);
    void seekForward();
    void seekBackward();

Q_SIGNALS:
    void volumeChanged();
    void mutedChanged();
    void durationChanged();
    void positionChanged();
    void sourceChanged();
    void videoOutChanged();
    void playbackStateChanged();
    void speedChanged();
    void paused();
    void stopped();
    void playing();
private Q_SLOTS:
    void _q_started();
    void _q_stopped();
    void _q_paused(bool);

private:
    Q_DISABLE_COPY(QmlAVPlayer)
    PlaybackState mPlaybackState;
    QtAV::AVPlayer *mpPlayer;
    QUrl mSource;
};

#endif // QTAV_QML_AVPLAYER_H
