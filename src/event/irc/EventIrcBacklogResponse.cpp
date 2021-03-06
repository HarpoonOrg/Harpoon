#include "EventIrcBacklogResponse.hpp"
#include "service/irc/IrcDatabaseMessageType.hpp"


UUID EventIrcBacklogResponse::getEventUuid() const {
    return this->uuid;
}

EventIrcBacklogResponse::EventIrcBacklogResponse(size_t userId,
                                                 size_t serverId,
                                                 const std::string& channel,
                                                 std::list<IrcMessageData>&& events)
    : userId{userId}
    , serverId{serverId}
    , channel{channel}
    , events{std::move(events)}
{
}

size_t EventIrcBacklogResponse::getUserId() const {
    return userId;
}

size_t EventIrcBacklogResponse::getServerId() const {
    return serverId;
}

std::string EventIrcBacklogResponse::getChannel() const {
    return channel;
}

const std::list<IrcMessageData>& EventIrcBacklogResponse::getData() const {
    return events;
}
