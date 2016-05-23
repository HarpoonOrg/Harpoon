#include "WebsocketServer.hpp"
#include "WebsocketServer_Impl.hpp"
#include "event/IUserEvent.hpp"
#include "event/EventQuit.hpp"
#include "event/EventLoginResult.hpp"
#include "event/EventLogout.hpp"
#include <iostream>
#include <sstream>
#include <json/json.h>

#ifdef USE_WEBSOCKET_SERVER_VERBOSE
#include <seasocks/PrintfLogger.h>
#define WEBSOCKET_LOGGER_TYPE seasocks::PrintfLogger
#else
#include <seasocks/IgnoringLogger.h>
#define WEBSOCKET_LOGGER_TYPE seasocks::IgnoringLogger
#endif

using namespace std;


WebsocketServer_Impl::WebsocketServer_Impl(EventQueue* queue, EventQueue* appQueue)
:
	queue{queue},
	appQueue{appQueue},
	server{make_shared<WEBSOCKET_LOGGER_TYPE>()}
{
	server.addWebSocketHandler("/ws", make_shared<WebsocketHandler>(appQueue, queue, clients));
	serverThread = thread([this]{
		server.serve("public", 8080);
	});
}

WebsocketServer_Impl::~WebsocketServer_Impl() {
	server.terminate();
	serverThread.join();
}

bool WebsocketServer_Impl::onEvent(std::shared_ptr<IEvent> event) {
	UUID eventType = event->getEventUuid();
	if (eventType == EventQuit::uuid) {
		return false;
	} else if (eventType == EventLoginResult::uuid) {
		auto* loginResult = event->as<EventLoginResult>();
		seasocks::WebSocket* socket = (seasocks::WebSocket*)loginResult->getData();
		if (loginResult->getSuccess()) {
			addClient(loginResult->getUserId(), socket);
#warning send current status to client (servers/channels/...)
		} else {
			socket->close();
		}
	} else if (eventType == EventLogout::uuid) {
#warning handle EventLogout
		auto logout = event->as<EventLogout>();
		removeClient((seasocks::WebSocket*)logout->getData());
	}
	return true;
}

void WebsocketServer_Impl::addClient(size_t userId, seasocks::WebSocket* socket) {
	auto it = userToClients.find(userId);
	bool found = it == userToClients.end();
	std::list<WebsocketClientData>* dataList;
	if (!found) {
		auto dataListIt = userToClients.emplace(
			piecewise_construct,
			forward_as_tuple(userId),
			forward_as_tuple());
		dataList = &dataListIt.first->second;
	} else {
		dataList = &it->second;
	}

	dataList->emplace_back(userId, socket);
	clients.emplace(socket, (++dataList->rbegin()).base()); // iterator to last element
}

void WebsocketServer_Impl::removeClient(seasocks::WebSocket* socket) {
	auto it = clients.find(socket);
	if (it != clients.end()) {
		auto data = it->second;
		size_t userId = data->userId;
		auto clientListIt = userToClients.find(userId);
		if (clientListIt != userToClients.end()) {
			clientListIt->second.erase(data);
			if (clientListIt->second.size() == 0)
				userToClients.erase(userId);
		}
		clients.erase(it);
	}
}

void WebsocketServer_Impl::sendEventToUser(std::shared_ptr<IEvent> event) {
	auto userEvent = event->as<IUserEvent>();
	if (userEvent != nullptr) {
		cout << "NonNULL event" << endl;
		auto it = userToClients.find(userEvent->getUserId());
		if (it != userToClients.end()) {
			list<WebsocketClientData>& clientDataList = it->second;
			for (auto& clientData : clientDataList) {
				clientData.socket;
#warning send event to user stub
			}
		}
	}
}
