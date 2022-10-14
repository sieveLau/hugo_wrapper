#include <iostream>
#include <string>
#include <format>
#include <fmt/core.h>
#include <filesystem>
#include <list>
#include <fstream>
#include <nlohmann/json.hpp>
#include <path_helper.h>
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


int main(int argc, char** argv) {
    using std::string;
    using std::cout;
    const static std::string kCommand = "hugo";
    if (!command_exist(kCommand)) {
        std::cerr<<"failed, no command";
        exit(COMMAND_NOT_FOUND);
    }

    // 读取配置文件，与可执行文件同目录的config.json
    using json = nlohmann::json;
    std::ifstream f(exec_dir(argv[0])+"/config.json");
    json config = json::parse(f);

    int option;
    ask_and_get_answer(init_option_str(),option);
    switch (option) {
        case 1: {
            string post_title;
            ask_and_get_answer("title: ",post_title);
            auto command = fmt::format("{} new {}/'{}'.md", kCommand, config["post_dir"],post_title);
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
        }
        default:
            break;
    }
    return 0;

}
