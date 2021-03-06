#ifndef LOGINDATABASE_DUMMY_H
#define LOGINDATABASE_DUMMY_H

#include "queue/EventQueue.hpp"
#include "queue/EventLoop.hpp"


/// Simple database dummy for testing
/// Default credentials are "user" "password"
class LoginDatabase_Dummy : public EventLoop {
    EventQueue* appQueue;
public:
    explicit LoginDatabase_Dummy(EventQueue* appQueue);
    virtual ~LoginDatabase_Dummy();
    virtual bool onEvent(std::shared_ptr<IEvent> event) override;
};

#endif
