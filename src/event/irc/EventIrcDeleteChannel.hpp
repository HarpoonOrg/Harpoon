#ifndef EVENTIRCDELETECHANNEL_H
#define EVENTIRCDELETECHANNEL_H

#include "IIrcCommand.hpp"
#include <string>


class EventIrcDeleteChannel : public IIrcCommand {
    size_t userId;
    size_t serverId;
    std::string channelName;
public:
    static constexpr UUID uuid = 18;
    virtual UUID getEventUuid() const override;

    EventIrcDeleteChannel(size_t userId,
                          size_t serverId,
                          const std::string& channelName);
    virtual size_t getUserId() const override;
    virtual size_t getServerId() const override;
    std::string getChannelName() const;
};

#endif
