#include <iostream>
#include <string>
#include <format>
#include <fmt/core.h>
#define COMMAND_NOT_FOUND (-1)

int main() {
    using std::string;
    const std::string kCommand = "hugo";
    auto check_command = fmt::format("which {} > /dev/null 2>&1", kCommand.c_str());
    if (system(check_command.c_str())) {
        fmt::print("failed, no command");
        exit(COMMAND_NOT_FOUND);
    }
//        fmt::print("good");

    using std::cout;
    int option;
    cout<<"option:\n1: new post\n2. generate\n3. server\n";
    std::cin>>option;
    switch (option) {
        case 1: {
            cout << "title: ";
            string post_title;
            std::cin >> post_title;
            auto command = fmt::format("{} new post/{}", kCommand, post_title);
            system(command.c_str());
            break;
        }
        case 2: {
            system(kCommand.c_str());
            break;
        }
        case 3:{
            system((kCommand+" server").c_str());
            break;
        }
        default:
            break;
    }
    return 0;

}
