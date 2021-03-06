#include "EventIrcHostAdded.hpp"


UUID EventIrcHostAdded::getEventUuid() const {
    return this->uuid;
}

EventIrcHostAdded::EventIrcHostAdded(size_t userId,
                                         size_t serverId,
                                         const std::string& host,
                                         int port,
                                         const std::string& password,
                                         bool ipV6,
                                         bool ssl)
    : userId{userId}
    , serverId{serverId}
    , host{host}
    , port{port}
    , password{password}
    , ipV6{ipV6}
    , ssl{ssl}
{
}

size_t EventIrcHostAdded::getUserId() const {
    return userId;
}

size_t EventIrcHostAdded::getServerId() const {
    return serverId;
}

std::string EventIrcHostAdded::getHost() const {
    return host;
}

std::string EventIrcHostAdded::getPassword() const {
    return password;
}

int EventIrcHostAdded::getPort() const {
    return port;
}

bool EventIrcHostAdded::getIpV6() const {
    return ipV6;
}

bool EventIrcHostAdded::getSsl() const {
    return ssl;
}
