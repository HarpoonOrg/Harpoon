#ifndef HACKCONNECTION_H
#define HACKCONNECTION_H

#include "queue/EventLoop.hpp"
#include <memory>
#include <mutex>
#include <map>
#include <string>


class HackServerConfiguration;
class HackChannelStore;
class EventQueue;
class HackConnection_Impl;
class HackConnection : public EventLoop {
    std::shared_ptr<HackConnection_Impl> impl;
public:
    HackConnection(EventQueue* appQueue, size_t userId, const HackServerConfiguration& configuration);
    virtual ~HackConnection();
    virtual bool onEvent(std::shared_ptr<IEvent> event) override;

    std::string getActiveNick() const;
    size_t getServerId() const;
    std::string getServerName() const;
    std::mutex& getChannelLoginDataMutex() const;
    void addHost(const std::string& hostName,
                 const std::string& websocketUri,
                 int port,
                 bool ipV6,
                 bool ssl);
    void removeHost(const std::string& host, int port);
    const HackServerConfiguration& getServerConfiguration() const;
    const std::map<std::string, HackChannelStore>& getChannelStore() const;
    const HackChannelStore* getChannelStore(const std::string& channelName) const;
};

#endif