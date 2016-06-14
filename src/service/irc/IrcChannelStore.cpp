#include "IrcChannelStore.hpp"

using namespace std;


IrcUserStore::IrcUserStore(std::string nick, std::string mode)
:
	nick{nick},
	mode{mode}
{
}

void IrcUserStore::setNick(std::string nick) {
	nick = nick;
}
void IrcUserStore::setMode(std::string mode) {
	mode = mode;
}

std::string IrcUserStore::getNick() const {
	return nick;
}

std::string IrcUserStore::getMode() const {
	return mode;
}

IrcChannelStore::IrcChannelStore(const std::string& channelPassword)
:
	channelPassword{channelPassword}
{
}

void IrcChannelStore::clear() {
	users.clear();
}

void IrcChannelStore::addUser(std::string name, std::string mode) {
	users.emplace(piecewise_construct,
		forward_as_tuple(name),
		forward_as_tuple(name, mode));
}

const std::map<std::string, IrcUserStore>& IrcChannelStore::getUsers() const {
	return users;
}

std::string IrcChannelStore::getChannelPassword() const {
	return channelPassword;
}

