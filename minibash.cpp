#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>
#include <sstream>
#include <cstring>

std::vector<char*> parse_input(const std::string& input) {
    std::stringstream ss(input);
    std::string token;
    std::vector<char*> args;

    while (ss >> token) {
        char* arg = new char[token.size() + 1];
        std::strcpy(arg, token.c_str());
        args.push_back(arg);
    }

    args.push_back(nullptr);
    return args;
}

int main() {
    std::string input;

    while (true) {
        std::cout << "$ ";
        std::getline(std::cin, input);

        if (input.empty())
            continue;

        if (input == "exit")
            break;

        if (input.substr(0, 2) == "cd") {
            std::string dir = input.substr(3);
            if (chdir(dir.c_str()) != 0)
                perror("cd");
            continue;
        }

        auto args = parse_input(input);

        pid_t pid = fork();

        if (pid == 0) {
            execvp(args[0], args.data());
            perror("exec");
            exit(1);
        } else if (pid > 0) {
            wait(nullptr);
        } else {
            perror("fork");
        }

        for (char* arg : args)
            delete[] arg;
    }

    return 0;
}
