#include <iostream>
#include "LoginDatabase_Ini.hpp"
#include "event/EventInit.hpp"
#include "event/EventQuit.hpp"
#include "event/EventLogin.hpp"
#include "event/EventLoginResult.hpp"
#include "event/irc/EventIrcJoinChannel.hpp"
#include "utils/Password.hpp"

using namespace std;


LoginDatabase_Ini::LoginDatabase_Ini(EventQueue* appQueue)
    : EventLoop({
          EventInit::uuid,
          EventQuit::uuid,
          EventLogin::uuid
      })
    , appQueue{appQueue}
    , usersIni{"config/users.ini"}
{
}

bool LoginDatabase_Ini::onEvent(std::shared_ptr<IEvent> event) {
    UUID eventType = event->getEventUuid();
    if (eventType == EventQuit::uuid) {
        std::cout << "DB received QUIT event" << std::endl;
        return false;
    } else if (eventType == EventLogin::uuid) {
        auto login = event->as<EventLogin>();

        bool success;
        Ini::Entries* user = usersIni.getEntry(login->getUsername());
        if (user) {
            string salt, password;
            success = (
                       usersIni.getEntry(*user, "salt", salt)
                       && usersIni.getEntry(*user, "password", password)
                       && Password(salt, login->getPassword()).equals(password)
                       );
        } else {
            success = false;
        }

        shared_ptr<EventLoginResult> loginResult{make_shared<EventLoginResult>(login->getTarget(), success, 1, login->getData())};
        appQueue->sendEvent(loginResult);
    }
    return true;
}
