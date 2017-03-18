#include <iostream>
#include <algorithm>
#include <array>
#include <sstream>
#include "app/Application.hpp"
#include "utils/Password.hpp"
#include "utils/Ini.hpp"
#include "utils/IdProvider.hpp"
#include "utils/Filesystem.hpp"
#include "version.hpp"

using namespace std;


void doGenUser(bool save) {
    string username, password;

    cout << "Username: ";
    cin >> username;

    cout << "Password: ";
    cin >> password;

    Password p(password);
    if (save) {
        Ini users("config/users.ini");
        auto& user = users.expectCategory(username);

        string userIdString;
        users.getEntry(user, "id", userIdString);
        if (userIdString.size() == 0) {
            userIdString = to_string(IdProvider::getInstance().generateNewId("user"));
            users.setEntry(user, "id", userIdString);
        }

        stringstream userDirectory;
        userDirectory << "config/user" << userIdString;
        Filesystem::getInstance().createPathRecursive(userDirectory.str());

        users.setEntry(user, "salt", p.getSaltBase64());
        users.setEntry(user, "password", p.getHashBase64());
        cout << "users.ini updated." << endl;
    } else {
        cout << "[" << username << "]" << endl
             << "salt=" << p.getSaltBase64() << endl
             << "password=" << p.getHashBase64() << endl;
    }
}

template<class T, class R>
inline static bool isValidChoice(const T& validChoices, const R& choice) {
    auto end = validChoices.cend();
    return std::find(validChoices.cbegin(), end, choice) == end;
}

template<class T>
inline static void getChoice(const std::string& message, const T& validChoices, std::string& choice, const std::string& defaultValue) {
    do {
        cout << message;
        getline(cin, choice);
        if (choice.size() == 0) // auto value
            choice = defaultValue;
    } while(!isValidChoice(validChoices, choice));
}

void doSetup(bool save) {
    Filesystem::getInstance().createPathRecursive("config");

    bool usersIniExists;
    {
        Ini users("config/users.ini");
        usersIniExists = !users.isNew();
    }

    string loginDatabaseType,
        backlogDatabaseType,
        ircSettingsDatabaseType,
        enableWebChat,
        enableIrcService,
        enableIrcBacklog;
    static const array<string, 2> validBacklogDatabaseTypes{"none", "postgres"};
    static const array<string, 2> validLoginDatabaseTypes{"dummy", "ini"};
    static const array<string, 2> validIrcDatabaseTypes{"dummy", "ini"};
    static const array<string, 2> validYesNoAnswers{"y", "n"};

    getChoice("Login database type (dummy/ini) [ini]: ", validLoginDatabaseTypes, loginDatabaseType, "ini");
    getChoice("Backlog database type (none/postgres) [postgres]: ", validBacklogDatabaseTypes, backlogDatabaseType, "postgres");

    getChoice("Enable IRC service (y/n) [y]: ", validYesNoAnswers, enableIrcService, "y");
    if (enableIrcService == "y") {
        getChoice("IRC settings database type (dummy/ini) [ini]: ", validIrcDatabaseTypes, ircSettingsDatabaseType, "ini");
        getChoice("Collect IRC backlog (y/n) [y]: ", validYesNoAnswers, enableIrcBacklog, "y");
    }

    getChoice("Enable WebChat (y/n) [y]: ", validYesNoAnswers, enableWebChat, "y");

    // write core configuration
    {
        Ini core("config/core.ini");

        auto& modules = core.expectCategory("modules");
        core.setEntry(modules, "login", loginDatabaseType);
        core.setEntry(modules, "webchat", enableWebChat);
        core.setEntry(modules, "database", backlogDatabaseType);

        auto& services = core.expectCategory("services");
        core.setEntry(services, "irc", enableIrcService);
    }

    // write irc configuration
    {
        Ini irc("config/irc.ini");

        auto& modules = irc.expectCategory("modules");
        irc.setEntry(modules, "settings_database", ircSettingsDatabaseType);
        irc.setEntry(modules, "backlog", enableIrcBacklog);
    }

    // create first user if no users.ini exists
    if (!usersIniExists) {
        cout << endl << "The first can now be created" << endl;
        doGenUser(save);
    }
}

bool checkArgs(int argc, char** argv) {
    bool help = false,
        setup = false,
        genUser = false,
        save = false,
        version = false;

    // check parameters
    for (int currentArgI = 1; currentArgI < argc; ++currentArgI) {
        string arg(argv[currentArgI]);
        if (arg == "-v" || arg == "--version") version = true;
        if (arg == "--setup") setup = true;
        if (arg == "--genuser") genUser = true;
        if (arg == "-h" || arg == "--help") help = true;
        if (arg == "--save") save = true;
    }

    // actions
    if (help) {
        cout << "Valid commands:" << endl
             << "  --setup     Generate initial configuration. (Useful with --save)" << endl
             << "  --genuser   Generate some user account. (Useful with --save)" << endl
             << "  --save      Save the configured data into the corresponding ini file. (No process must be running)" << endl;
        return false;
    }
    if (version) {
        cout << "Harpoon v" << VERSION_MAJOR << "." << VERSION_MINOR << "." << VERSION_PATCH << endl;
        return false;
    }
    if (setup) {
        doSetup(save);
        return false;
    }
    if (genUser) {
        doGenUser(save);
        return false;
    }
    return true;
}

int main(int argc, char** argv) {
    if (!checkArgs(argc, argv))
        return 0;

    try {
        Application app;
    } catch(runtime_error& run) {
        cout << run.what() << endl;
    }

    std::cout << "Application stopped" << std::endl;
    return 0;
}

