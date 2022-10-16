#include <exception>
#include <filesystem>
#include <fmt/args.h>
#include <fmt/core.h>
#include <fstream>
#include <iostream>
#include <list>
#include <nlohmann/json.hpp>
#include <path_helper.h>
#include <string>
#include <unistd.h>

template<class T>
void ask_and_get_answer(const std::string &question, T &user_input) {
  printf("%s", question.c_str());
  std::cin >> user_input;
  // 清理剩余的所有字符
  for (int ch; (ch = std::getchar()) != '\n';) {}
}

void option_clean(const std::string &public_dir) {
  auto dir = std::filesystem::current_path();
  dir += public_dir;
  char delete_option = 0;
  printf("delete %s? [y/N]", dir.c_str());
  std::cin >> delete_option;
  if (delete_option == 'y') {
    std::filesystem::remove_all(dir);
  }
}

auto SplitString(const std::string &string) -> std::vector<std::string> {
  std::vector<std::string> SubStrings;
  std::string word;
  bool not_inside_quote = true;
  for (auto &&CurrentChar : string) {
    if (CurrentChar == '\'')
      not_inside_quote = !not_inside_quote;
    if (CurrentChar == ' ' && not_inside_quote) {
      SubStrings.push_back(word);
      word = "";
    } else {
      word += CurrentChar;
    }
  }
  if (!word.empty())
    SubStrings.push_back(word);
  return SubStrings;
}

void ExecuteCommand(const std::string &ExecutablePath, const std::string &ProgramName, const std::vector<std::string> &Arguments) {
  auto argvLength = Arguments.size() + 2;
  const char **argv = new const char *[argvLength];
  argv[0] = ProgramName.c_str();
  for (int j = 0, size = argvLength - 1; j < size; ++j)// copy args
    argv[j + 1] = Arguments[j].c_str();
  argv[argvLength - 1] = NULL;
  execve(ExecutablePath.c_str(), (char **) argv, NULL);
  assert(0 && "Unreachable line, this is after execv()");
  delete[] argv;
}

int main(int argc, char **argv) {

  // 读取配置文件，与可执行文件同目录的config.json
  using json = nlohmann::json;
  std::ifstream ConfigFileHandler(exec_dir(argv[0]) + "/config.json");
  json config = json::parse(ConfigFileHandler);

  auto HugoCommand = config["hugo_exe"].get<std::string>();
  // clean作为特殊的option，锁定在第一位，使用专门的function来执行这个功能
  std::string OptionListString;
  OptionListString += "1. clean\n";

  // 这里i从2开始，所以options列表要-2
  int i = 2;
  for (json::iterator it = config["options"].begin(), end = config["options"].end(); it != end; ++it) {
    OptionListString += fmt::format("{}. {}\n", i, config["options"][i - 2]["name"]);
    ++i;
  }

  int option = 0;
  ask_and_get_answer(OptionListString, option);
  if (option < 1) {
    throw std::invalid_argument(fmt::format("Input number should be between 1 and {}, but you hit {}.", i - 1, option).c_str());
  }
  if (option == 1) {
    option_clean(config["clean"]);
    return 0;
  }

  int params = (config["options"][option - 2]["param"]).get<int>();
  // 如果是没有自定义参数的选项，就直接执行hugo + command字段
  if (params == 0) {
    auto ArgsFromConfig = (config["options"][option - 2]["command"]).get<std::string>();
    if (ArgsFromConfig.empty())
      ExecuteCommand(HugoCommand, "hugo", {});
    else
      ExecuteCommand(HugoCommand, "hugo", SplitString(ArgsFromConfig));
  }
  // param不能小于0
  else if (params < 0) {
    throw std::logic_error("field 'params' should be a positive integer, check your config.json.");
  }
  // 如果有自定义参数，就要按顺序format了
  else {
    std::string command = (config["options"][option - 2]["command"]).get<std::string>();
    //用dynamic arg list（fmt 7.0开始支持）来实现按顺序format
    fmt::dynamic_format_arg_store<fmt::format_context> store;
    for (int position = 0; position < params; position++) {
      std::string input;
      fmt::print("command: {}\ntaking input for argument {}:", command, position + 1);
      std::getline(std::cin, input);
      store.push_back(input);
    }
    command = fmt::vformat(command, store);
    ExecuteCommand(HugoCommand, "hugo", SplitString(command));
  }

  return 0;
}
