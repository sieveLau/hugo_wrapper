#include <iostream>
#include <string>
#include <format>
#include <fmt/core.h>
#include <fmt/args.h>
#include <filesystem>
#include <list>
#include <fstream>
#include <nlohmann/json.hpp>
#include <path_helper.h>
#include <exception>
#define COMMAND_NOT_FOUND (-1)
#define BAD_CONFIG_FILE_OPTION (-2)
#define WRONG_OPTION (-3)

bool command_exist(const std::string& command){
    return !(system(fmt::format("which {} > /dev/null 2>&1", command.c_str()).c_str()));
}

template<class T>
void ask_and_get_answer(const std::string& print_str, T& user_input){
    std::cout<<print_str;
    std::cin>>user_input;
}

void option_clean(const std::string& public_dir){
    auto dir = std::filesystem::current_path();
    dir+=public_dir;
    char delete_option;
    printf("delete %s? [y/N]", dir.c_str());
    std::cin >> delete_option;
    if(delete_option == 'y'){
        std::filesystem::remove_all(dir);
    }
}


int main(int argc, char** argv) {
    using std::string;
    using std::cout;
    const static std::string kCommand = "hugo";

    if (!command_exist(kCommand)) {
        throw std::runtime_error(fmt::format("command {} not found, have you installed hugo?",kCommand));
    }

    // 读取配置文件，与可执行文件同目录的config.json
    using json = nlohmann::json;
    std::ifstream f(exec_dir(argv[0])+"/config.json");
    json config = json::parse(f);

// clean作为特殊的option，锁定在第一位，使用专门的function来执行这个功能
    std::string option_list_str;
    option_list_str+="1. clean\n";

    // 这里i从2开始，所以options列表要-2
    int i = 2;
    for (json::iterator it = config["options"].begin(); it != config["options"].end(); ++it) {
        option_list_str+=fmt::format("{}. {}\n",i,config["options"][i-2]["name"]);
        ++i;
    }

    int option;
    ask_and_get_answer(option_list_str,option);
    if(option<1) {
        throw std::invalid_argument(fmt::format("Input number should be between 1 and {}, but you hit {}.",i-1, option).c_str());
    }
    if(option==1) {
        option_clean(config["clean"]);
        return 0;
    }

    int params = (config["options"][option-2]["param"]).get<int>();
    // 如果是没有自定义参数的选项，就直接执行hugo + command字段
    if(params==0){
        std::clog<<"real option: "<< config["options"][option-2]["name"]<<"\n";
        system((
                kCommand+" "+
                (config["options"][option-2]["command"]).get<std::string>()
                ).c_str()
                );
    }
    // param不能小于0
    else if (params<0) {
        throw std::logic_error("field 'params' should be a positive integer, check your config.json.");
    }
    // 如果有自定义参数，就要按顺序format了
    else {
        std::string command = (config["options"][option - 2]["command"]).get<std::string>();
        //用dynamic arg list（fmt 7.0开始支持）来实现按顺序format
        fmt::dynamic_format_arg_store<fmt::format_context> store;
        for (int position = 0; position < params;position++) {
            std::string input;
            cout<<fmt::format("command: {}\ntaking input for argument {}:", command, position+1);\
            std::cin>>input;
            store.push_back(input);
        }
        command = fmt::vformat(command, store);
        std::clog<<command;
        system((kCommand + " " + command).c_str());
    }

    return 0;
}
