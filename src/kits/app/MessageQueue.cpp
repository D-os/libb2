/*
 * Copyright 2001-2014 Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Unknown? Eric?
 *		Axel Dörfler, axeld@pinc-software.de
 */


// Queue for holding BMessages


#include <MessageQueue.h>
#include <Autolock.h>
#include <Message.h>
#include <MessagePrivate.h>


BMessageQueue::BMessageQueue()
    :
    fHead(NULL),
    fTail(NULL),
    fMessageCount(0),
    fLock("BMessageQueue Lock")
{
}


BMessageQueue::~BMessageQueue()
{
    if (!Lock())
        return;

    BMessage* message = fHead;
    while (message != NULL) {
        BMessage* next = message->d_func()->fQueueLink;

        delete message;
        message = next;
    }
}


void
BMessageQueue::AddMessage(BMessage* message)
{
    if (message == NULL)
        return;

    BAutolock _(fLock);
    if (!IsLocked())
        return;

    // The message passed in will be the last message on the queue so its
    // link member should be set to null.
    message->d_func()->fQueueLink = NULL;

    fMessageCount++;

    if (fTail == NULL) {
        // there are no messages in the queue yet
        fHead = fTail = message;
    } else {
        // just add it after the tail
        fTail->d_func()->fQueueLink = message;
        fTail = message;
    }
}


void
BMessageQueue::RemoveMessage(BMessage* message)
{
    if (message == NULL)
        return;

    BAutolock _(fLock);
    if (!IsLocked())
        return;

    BMessage* last = NULL;
    for (BMessage* entry = fHead; entry != NULL; entry = entry->d_func()->fQueueLink) {
        if (entry == message) {
            // remove this one
            if (entry == fHead)
                fHead = entry->d_func()->fQueueLink;
            else
                last->d_func()->fQueueLink = entry->d_func()->fQueueLink;

            if (entry == fTail)
                fTail = last;

            fMessageCount--;
            return;
        }
        last = entry;
    }
}


int32
BMessageQueue::CountMessages() const
{
    return fMessageCount;
}


bool
BMessageQueue::IsEmpty() const
{
    return fMessageCount == 0;
}


BMessage*
BMessageQueue::FindMessage(int32 index) const
{
    BAutolock _(fLock);
    if (!IsLocked())
        return NULL;

    if (index < 0 || index >= fMessageCount)
        return NULL;

    for (BMessage* message = fHead; message != NULL; message = message->d_func()->fQueueLink) {
        // If the index reaches zero, then we have found a match.
        if (index == 0)
            return message;

        index--;
    }

    return NULL;
}


BMessage*
BMessageQueue::FindMessage(uint32 what, int32 index) const
{
    BAutolock _(fLock);
    if (!IsLocked())
        return NULL;

    if (index < 0 || index >= fMessageCount)
        return NULL;

    for (BMessage* message = fHead; message != NULL; message = message->d_func()->fQueueLink) {
        if (message->what == what) {
            // If the index reaches zero, then we have found a match.
            if (index == 0)
                return message;

            index--;
        }
    }

    return NULL;
}


bool
BMessageQueue::Lock()
{
    return fLock.Lock();
}


void
BMessageQueue::Unlock()
{
    fLock.Unlock();
}


bool
BMessageQueue::IsLocked() const
{
    return fLock.IsLocked();
}


BMessage*
BMessageQueue::NextMessage()
{
    BAutolock _(fLock);
    if (!IsLocked())
        return NULL;

    // remove the head of the queue, if any, and return it

    BMessage* head = fHead;
    if (head == NULL)
        return NULL;

    fMessageCount--;
    fHead = head->d_func()->fQueueLink;

    if (fHead == NULL) {
        // If the queue is empty after removing the front element,
        // we need to set the tail of the queue to NULL since the queue
        // is now empty.
        fTail = NULL;
    }

    return head;
}


bool
BMessageQueue::IsNextMessage(const BMessage* message) const
{
    BAutolock _(fLock);
    return fHead == message;
}


// This method is only here for R5 binary compatibility!
// It should be dropped as soon as possible (it misses the const qualifier).
bool
BMessageQueue::IsLocked()
{
    return fLock.IsLocked();
}


void BMessageQueue::_ReservedMessageQueue1() {}
void BMessageQueue::_ReservedMessageQueue2() {}
void BMessageQueue::_ReservedMessageQueue3() {}

