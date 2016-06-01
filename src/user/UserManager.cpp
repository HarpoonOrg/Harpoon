#include <iostream>
#include "UserManager.hpp"
#include "event/IUserEvent.hpp"
#include "event/EventQuit.hpp"
#include "event/EventLoginResult.hpp"
#include "event/IActivateServiceEvent.hpp"

using namespace std;


UserManager::UserManager(EventQueue* appQueue) :
	ManagingEventLoop({
		EventQuit::uuid,
		EventLoginResult::uuid,
	}, {
		&EventGuard<IActivateServiceEvent>
	}),
	appQueue{appQueue}
{
}

void UserManager::sendEventToUser(std::shared_ptr<IEvent> event) {
	auto userEvent = event->as<IUserEvent>();
	if (userEvent != nullptr) {
		cout << "NonNULL event" << endl;
		auto it = users.find(userEvent->getUserId());
		if (it != users.end()) {
			for (auto loop : it->second) {
				EventQueue* queue = loop.second->getEventQueue();
				if (queue->canProcessEvent(event.get()))
					queue->sendEvent(event);
			}
		}
	}
}

bool UserManager::onEvent(std::shared_ptr<IEvent> event) {
	UUID eventType = event->getEventUuid();

	auto userEvent = event->as<IUserEvent>();
	if (userEvent) {
		std::cout << "UM received User event" << std::endl;
		size_t userId = userEvent->getUserId();

		auto activateEvent = event->as<IActivateServiceEvent>();
		if (activateEvent) {
			std::cout << "UM received Activate event" << std::endl;
			cout << "[UM] Activate user: " << activateEvent->getUserId() << endl;
			std::map<size_t, std::shared_ptr<EventLoop>>* serviceMap;
			auto serviceMapIt = users.find(userId);
			if (serviceMapIt == users.end()) {
				auto it = users.emplace(piecewise_construct,
					forward_as_tuple(userId),
					forward_as_tuple());
				serviceMap = &it.first->second;
			} else {
				serviceMap = &serviceMapIt->second;
			}
			auto serviceIt = serviceMap->find(eventType);
			if (serviceIt == serviceMap->end()) {
				auto serviceInstanceIt = serviceMap->emplace(eventType, activateEvent->instantiateService(userId, appQueue));
				serviceInstanceIt.first->second->getEventQueue()->sendEvent(event);
			}
		}
		if (eventType == EventLoginResult::uuid) {
			auto loginResult = event->as<EventLoginResult>();
			if (loginResult->getSuccess()) {
				auto it = users.find(userId);
				if (it != users.end()) {
					for (auto p : it->second) {
						p.second->getEventQueue()->sendEvent(event);
					}
				}
			}
		}
	}

	if (eventType == EventQuit::uuid) {
		cout << "[UM] received QUIT event" << endl;
		for (auto& user : users) {
			for (auto p : user.second)
				p.second->getEventQueue()->sendEvent(event);
		}
		for (auto& user : users) {
			for (auto p : user.second)
				p.second->join();
		}
		return false;
	}

	return true;
}

