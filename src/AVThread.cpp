/******************************************************************************
    QtAV:  Media play library based on Qt and FFmpeg
    Copyright (C) 2012-2013 Wang Bin <wbsecg1@gmail.com>

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

#include <QtAV/AVThread.h>
#include <private/AVThread_p.h>
#include <QtAV/AVOutput.h>
#include <QtAV/Filter.h>
#include <QtAV/OutputSet.h>

namespace QtAV {

AVThreadPrivate::~AVThreadPrivate() {
    demux_end = true;
    stop = true;
    if (!paused) {
        qDebug("~AVThreadPrivate wake up paused thread");
        paused = false;
        next_pause = false;
        cond.wakeAll();
    }
    packets.setBlocking(true); //???
    packets.clear();
    //not neccesary context is managed by filters.
    filter_context = 0;
    qDeleteAll(filters); //TODO: is it safe?
    filters.clear();
    update_outputs.clear();
}

AVThread::AVThread(QObject *parent) :
    QThread(parent)
{
}

AVThread::AVThread(AVThreadPrivate &d, QObject *parent)
    :QThread(parent),DPTR_INIT(&d)
{
}

AVThread::~AVThread()
{
    //d_ptr destroyed automatically
}

bool AVThread::isPaused() const
{
    return d_func().paused;
}

void AVThread::stop()
{
    pause(false);
    DPTR_D(AVThread);
    if (d.writer)
        d.writer->pause(false); //stop waiting
    d.stop = true;
    //terminate();
    d.packets.setBlocking(false); //stop blocking take()
    d.packets.clear();
}

//TODO: output set
void AVThread::pause(bool p)
{
    DPTR_D(AVThread);
    if (d.paused == p)
        return;
    d.paused = p;
    if (!d.paused) {
        qDebug("wake up paused thread");
        d.next_pause = false;
        d.cond.wakeAll();
    }
}

void AVThread::nextAndPause()
{
    DPTR_D(AVThread);
    d.next_pause = true;
    d.paused = true;
    d.cond.wakeAll();
}

void AVThread::lock()
{
    d_func().mutex.lock();
}

void AVThread::unlock()
{
    d_func().mutex.unlock();
}

void AVThread::setClock(AVClock *clock)
{
    d_func().clock = clock;
}

AVClock* AVThread::clock() const
{
    return d_func().clock;
}

PacketQueue* AVThread::packetQueue() const
{
    return const_cast<PacketQueue*>(&d_func().packets);
}

void AVThread::setDecoder(AVDecoder *decoder)
{
    d_func().dec = decoder;
}

AVDecoder* AVThread::decoder() const
{
    return d_func().dec;
}

void AVThread::setOutput(AVOutput *out)
{
    d_func().writer = out;
}

AVOutput* AVThread::output() const
{
    return d_func().writer;
}

void AVThread::setOutputSet(OutputSet *set)
{
    d_func().outputSet = set;
    connect(set, SIGNAL(updateParametersRequired(AVOutput*)), SLOT(addOutputToBeUpdated(AVOutput*)));
}

OutputSet* AVThread::outputSet() const
{
    return d_func().outputSet;
}

void AVThread::setDemuxEnded(bool ended)
{
    d_func().demux_end = ended;
}

void AVThread::addOutputToBeUpdated(AVOutput *output)
{
    DPTR_D(AVThread);
    if (d.update_outputs.contains(output))
        return;
    d.update_outputs.append(output);
}

void AVThread::resetState()
{
    DPTR_D(AVThread);
    pause(false);
    if (d.writer)
        d.writer->pause(false); //stop waiting. Important when replay
    d.stop = false;
    d.demux_end = false;
    d.packets.setBlocking(true);
    d.packets.clear();
    //not neccesary context is managed by filters.
    d.filter_context = 0;
}

bool AVThread::tryPause()
{
    DPTR_D(AVThread);
    if (!d.paused && !d.next_pause)
        return false;
    QMutexLocker lock(&d.mutex);
    Q_UNUSED(lock);
    d.cond.wait(&d.mutex);
    qDebug("paused thread waked up!!!");
    return true;
}

void AVThread::setStatistics(Statistics *statistics)
{
    DPTR_D(AVThread);
    d.statistics = statistics;
}

} //namespace QtAV
