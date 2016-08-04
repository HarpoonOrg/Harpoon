#ifndef EVENTDATABASERESULT_H
#define EVENTDATABASERESULT_H

#include "IDatabaseEvent.hpp"
#include "db/DatabaseQuery.hpp"
#include <memory>
#include <list>


class EventQueue;
class EventDatabaseResult : public IDatabaseEvent {
    EventQueue* target;
    std::list<std::string> columns;
    std::list<std::string> results;
    std::shared_ptr<IEvent> eventOrigin;
public:
    static UUID uuid;
    virtual UUID getEventUuid() const override;

    EventDatabaseResult(EventQueue* target, std::shared_ptr<IEvent> eventOrigin);
    void addResult(const std::string& result);
    const std::list<std::string>& getResults() const;
};

#endif