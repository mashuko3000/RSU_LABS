#include <iostream>
#include <string>
#include <sstream>
#include "inc\set.hpp"

int main() {
    set_manager manager;

    std::cout << "Множества: new A, del A, add A x, rem A x, pow A, see (A), A+B, A&B, A-B, A<B, A=B" << std::endl;
    std::cout << "Для выхода введите 'exit'." << std::endl;

    std::string line;

    while (std::getline(std::cin, line) && line != "exit") {
        std::stringstream ss(line);
        std::string command;
        char arg1, arg2;

        if (!(ss >> command)) {
            continue;
        }

        if (command == "new" || command == "del" || command == "pow" || command == "see") {
            if (ss >> arg1 && ss.eof()) {
                if (command == "new") manager.new_set(arg1);
                else if (command == "del") manager.del_set(arg1);
                else if (command == "pow") manager.pow_set(arg1);
                else if (command == "see") manager.see(arg1);
            }
            else if (command == "see" && ss.eof()) {
                manager.see();
            }
            else if (!ss.fail()) {
                std::cout << "Неверный формат команды " << command << ". Слишком много аргументов." << std::endl;
            }
            else {
                std::cout << "Неверный формат команды " << command << ". Не хватает аргумента." << std::endl;
            }
        }
        else if (command == "add" || command == "rem") {
            if (ss >> arg1 >> arg2 && ss.eof()) {
                if (command == "add") manager.add_element(arg1, arg2);
                else if (command == "rem") manager.remove_element(arg1, arg2);
            } else {
                std::cout << "Неверный формат команды " << command << "." << std::endl;
            }
        }
        else if (command.length() == 1 && (command[0] == '+' || command[0] == '&' || command[0] == '-' || command[0] == '<' || command[0] == '=')) {
            char op = command[0];
            char set_a_name, set_b_name;
            if (ss >> set_a_name >> set_b_name && ss.eof()) {
                if (op == '+' || op == '&' || op == '-') manager.process_binary_op(set_a_name, op, set_b_name);
                else if (op == '<') manager.check_subset(set_a_name, set_b_name);
                else if (op == '=') manager.check_equality(set_a_name, set_b_name);
            } else {
                std::cout << "Неверный формат бинарной операции." << std::endl;
            }
        }
        else {
            std::cout << "Неизвестная команда." << std::endl;
        }
    }
    return 0;
}