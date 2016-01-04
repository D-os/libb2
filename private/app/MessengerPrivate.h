/*
 * Copyright 2003-2005, Haiku.
 * Distributed under the terms of the MIT License.
 */
#ifndef MESSENGER_PRIVATE_H
#define MESSENGER_PRIVATE_H


#include <Messenger.h>
#include <TokenSpace.h>


class BMessenger::Private {
public:
    port_id					fPort;
    int32					fHandlerToken;
    team_id					fTeam;

    Private() : fPort(-1), fHandlerToken(B_NULL_TOKEN), fTeam(-1) {}

    Private(BMessenger* messenger) : q_ptr(messenger) {}
    Private(BMessenger& messenger) : q_ptr(&messenger) {}

    Private& operator=(const Private& other)
    {
        if (this != &other) {
            fPort = other.fPort;
            fHandlerToken = other.fHandlerToken;
            fTeam = other.fTeam;
        }

        return *this;
    }

    bool operator==(const Private& other) const
    {
        // Note: The fTeam fields are not compared.
        return fPort == other.fPort && fHandlerToken == other.fHandlerToken;
    }

    void					SetTo(team_id team, port_id port,
                                  int32 token);
    void					InitData(const char* signature,
                                     team_id team, status_t* result);
    void					InitData(const BHandler* handler,
                                     const BLooper *looper,
                                     status_t* result);

    port_id	Port() const            { return fPort; }
    int32 Token() const             { return fHandlerToken; }
    team_id	Team() const            { return fTeam; }
    bool IsPreferredTarget() const  { return fHandlerToken == B_PREFERRED_TOKEN; }

private:
    B_DECLARE_PUBLIC(BMessenger)
    BMessenger* q_ptr;
};

#endif	// MESSENGER_PRIVATE_H
