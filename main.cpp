#include <iostream>
#include <string>
#include <format>
#include <fmt/core.h>
#include <filesystem>
#include <list>
#define COMMAND_NOT_FOUND (-1)

bool command_exist(const std::string& command){
    return !(system(fmt::format("which {} > /dev/null 2>&1", command.c_str()).c_str()));
}

std::string init_option_str(){
    const std::list<std::string> options = {"new post",
                                       "generate",
                                       "server",
                                       "clean"};
    std::string option_str;
    int i = 1;
    for(auto&& a_op : options){
        option_str+=fmt::format("{}. {}\n",i,a_op);
        ++i;
    }
    return option_str;
}

template<class T>
void ask_and_get_answer(const std::string& print_str, T& user_input){
    std::cout<<print_str;
    std::cin>>user_input;
}

int main() {
    using std::string;
    using std::cout;
    const static std::string kCommand = "hugo";
//    auto command_exist = fmt::format("which {} > /dev/null 2>&1", kCommand.c_str());
    if (!command_exist(kCommand)) {
        fmt::print("failed, no command");
        exit(COMMAND_NOT_FOUND);
    }
//        fmt::print("good");


    int option;
//    cout<<init_option_str();
//
////    cout<<"option:\n1: new post\n2. generate\n3. server\n4. clean";
//    std::cin>>option;

    ask_and_get_answer(init_option_str(),option);
    switch (option) {
        case 1: {
            string post_title;
//            cout << "title: ";
//            std::cin >> post_title;
            ask_and_get_answer("title: ",post_title);
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
        case 4:{
            auto dir = std::filesystem::current_path();
            dir+="/public";
            char delete_option;
            fmt::print(fmt::format("delete {}? [y/N]",dir.c_str()));
            std::cin >> delete_option;
            switch (delete_option) {
                case 'y':
                {
                    std::filesystem::remove_all(dir);
                    break;
                }
                case 'N':
                default:
                    break;
            }
            break;
        }
        default:
            break;
    }
    return 0;

}
