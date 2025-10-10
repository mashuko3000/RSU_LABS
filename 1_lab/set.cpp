#include"../inc/set.hpp"
#include <iostream>
#include <string>
#include <cstdlib>
#include <utility>

Set::Set() : name('\0'), head(nullptr){}
Set::~Set(){
    Node* current = head;
    while(current)
    {
        Node* next = current->next;
        delete current;
        current = next;
    }
}

Set::Set(const Set& other) : name(other.name), head(nullptr)
{
    if (other.head)
    {
        head = new Node{other.head->data, nullptr};
        Node* current_old = head;
        Node* current_new = other.head->next;
        while(current_new)
        {
            current_old->next = new Node{current_new->data, nullptr};
            current_old = current_old->next;
            current_new = current_new->next;
        }
    }
}
Set::Set(Set&& other) noexcept : name(other.name), head(other.head)
{
    other.name = '\0';
    other.head = nullptr;
}

Set& Set::operator=(const Set& other)
{
    if(this == &other) return *this;
    this->~Set();
    name = other.name;
    head = nullptr;
    if (other.head)
    {
        head = new Node{other.head->data, nullptr};
        Node* current_old = head;
        Node* current_new = other.head->next;
        while(current_new)
        {
            current_old->next = new Node{current_new->data, nullptr};
            current_old = current_old->next;
            current_new = current_new->next;
        }
    }
    return *this;
}
Set& Set::operator=(Set&& other) noexcept
{
    if(this == &other) return *this;
    this->~Set();
    name = other.name;
    head = other.head;
    other.name = '\0';
    other.head = nullptr;
    return *this;
}

Set* set_manager::find_set(char name)
{
    if ((name >= 'A' || name <= 'Z') && set_count > 0)
    {
        for(int i = 0; i < set_count; ++i)
        {
            if (sets[i].name == name)
            {
                return &sets[i];
            }
        }
        return nullptr;
    }
    return nullptr;
}
Node* set_manager::create_node(char data)
{
    return new Node{data, nullptr};
}
void set_manager::clear_list(Node* head)
{
    Node* current = head;
    while (current) {
        Node* next = current->next;
        delete current;
        current = next;
    }
}


set_manager::set_manager() : set_count(0)
{

}
set_manager::~set_manager() noexcept {}

set_manager::set_manager(const set_manager& other) : set_count(other.set_count)
{
   for(int i = 0; i < set_count; ++i)
   {
       sets[i] = other.sets[i];
   }
}
set_manager::set_manager(set_manager&& other) noexcept
{
    for (int i = 0; i < set_count; ++i) {
        sets[i] = std::move(other.sets[i]);
    }
    other.set_count = 0;
}

set_manager& set_manager::operator=(const set_manager& other)
{
    if (this != &other) {
        set_count = other.set_count;
        for (int i = 0; i < set_count; ++i) {
            sets[i] = other.sets[i];
        }
    }
    return *this;
}
set_manager set_manager::operator=(set_manager&& other) noexcept
{
    if (this != &other) {
        for (int i = 0; i < set_count; ++i) {
            sets[i].~Set();
        }
        set_count = other.set_count;
        for (int i = 0; i < set_count; ++i) {
            sets[i] = std::move(other.sets[i]);
        }
        other.set_count = 0;
    }
    return *this;
}


void set_manager::new_set(char name)
{
    if (find_set(name)) {
        std::cout << "Множество с именем " << name << " уже существует." << std::endl;
        return;
    }
    if (set_count >= 26) {
        std::cout << "Достигнуто максимальное количество множеств." << std::endl;
        return;
    }

    sets[set_count] = Set();
    sets[set_count].name = name;
    set_count++;
    std::cout << "Множество " << name << " создано." << std::endl;
}
void set_manager::del_set(char name)
{
    Set* s = find_set(name);
    if (!s) {
        std::cout << "Множество " << name << " не найдено." << std::endl;
        return;
    }

    s->~Set();

    for (int i = 0; i < set_count; ++i) {
        if (&sets[i] == s) {
            for (int j = i; j < set_count - 1; ++j) {
                sets[j] = std::move(sets[j + 1]);
            }
            sets[set_count - 1].~Set();
            set_count--;
            std::cout << "Множество " << name << " удалено." << std::endl;
            return;
        }
    }
}
void set_manager::add_element(char set_name, char element)
{
    Set* s = find_set(set_name);
    if (!s) {
        std::cout << "Множество " << set_name << " не найдено." << std::endl;
        return;
    }

    if (!s->head || element < s->head->data) {
        if (s->head && element == s->head->data) return;
        Node* new_node = create_node(element);
        new_node->next = s->head;
        s->head = new_node;
    } else {
        Node* current = s->head;
        while (current->next && current->next->data < element) {
            current = current->next;
        }
        if (current->data == element || (current->next && current->next->data == element)) {
            return;
        }
        Node* new_node = create_node(element);
        new_node->next = current->next;
        current->next = new_node;
    }
    std::cout << "Элемент " << element << " добавлен в множество " << set_name << "." << std::endl;
}
void set_manager::remove_element(char set_name, char element)
{
    Set* s = find_set(set_name);
    if (!s) {
        std::cout << "Множество " << set_name << " не найдено." << std::endl;
        return;
    }

    if (!s->head) return;

    if (s->head->data == element) {
        Node* to_delete = s->head;
        s->head = s->head->next;
        delete to_delete;
        std::cout << "Элемент " << element << " удален из множества " << set_name << "." << std::endl;
        return;
    }

    Node* current = s->head;
    while (current->next && current->next->data != element) {
        current = current->next;
    }

    if (current->next) {
        Node* to_delete = current->next;
        current->next = current->next->next;
        delete to_delete;
        std::cout << "Элемент " << element << " удален из множества " << set_name << "." << std::endl;
    } else {
        std::cout << "Элемент " << element << " не найден в множестве " << set_name << "." << std::endl;
    }
}
void set_manager::pow_set(char name)
{
    Set* s = find_set(name);
    if (!s) {
        std::cout << "Множество " << name << " не найдено." << std::endl;
        return;
    }

    int count = 0;
    Node* current = s->head;
    while (current) {
        count++;
        current = current->next;
    }

    long long power_of_2 = 1;
    for (int i = 0; i < count; ++i) {
        power_of_2 *= 2;
    }
    std::cout << "Мощность множества " << name << " равна " << count << ", булеан = " << power_of_2 << std::endl;
}
void set_manager::see(char name)
{
    if (name == '\0') {
        if (set_count == 0) {
            std::cout << "Множеств не найдено." << std::endl;
        } else {
            for (int i = 0; i < set_count; ++i) {
                std::cout << "Множество " << sets[i].name << ": { ";
                Node* current = sets[i].head;
                while (current) {
                    std::cout << current->data << " ";
                    current = current->next;
                }
                std::cout << "}" << std::endl;
            }
        }
    } else {
        Set* s = find_set(name);
        if (!s) {
            std::cout << "Множество " << name << " не найдено." << std::endl;
            return;
        }
        std::cout << "Множество " << s->name << ": { ";
        Node* current = s->head;
        while (current) {
            std::cout << current->data << " ";
            current = current->next;
        }
        std::cout << "}" << std::endl;
    }
}
void set_manager::process_binary_op(char set_a_name, char op, char set_b_name)
{
    Set* s_a = find_set(set_a_name);
    Set* s_b = find_set(set_b_name);

    if (!s_a || !s_b) {
        std::cout << "Одно или оба множества не найдены." << std::endl;
        return;
    }

    Node* current_a = s_a->head;
    Node* current_b = s_b->head;

    if (op == '+') {
        std::cout << "Объединение " << s_a->name << " + " << s_b->name << ": { ";
        while (current_a || current_b) {
            if (!current_a) {
                std::cout << current_b->data << " ";
                current_b = current_b->next;
            } else if (!current_b) {
                std::cout << current_a->data << " ";
                current_a = current_a->next;
            } else if (current_a->data < current_b->data) {
                std::cout << current_a->data << " ";
                current_a = current_a->next;
            } else if (current_b->data < current_a->data) {
                std::cout << current_b->data << " ";
                current_b = current_b->next;
            } else {
                std::cout << current_a->data << " ";
                current_a = current_a->next;
                current_b = current_b->next;
            }
        }
        std::cout << "}" << std::endl;
    } else if (op == '&') {
        std::cout << "Пересечение " << s_a->name << " & " << s_b->name << ": { ";
        while (current_a && current_b) {
            if (current_a->data < current_b->data) {
                current_a = current_a->next;
            } else if (current_b->data < current_a->data) {
                current_b = current_b->next;
            } else {
                std::cout << current_a->data << " ";
                current_a = current_a->next;
                current_b = current_b->next;
            }
        }
        std::cout << "}" << std::endl;
    } else if (op == '-') {
        std::cout << "Разность " << s_a->name << " - " << s_b->name << ": { ";
        while (current_a) {
            if (!current_b || current_a->data < current_b->data) {
                std::cout << current_a->data << " ";
                current_a = current_a->next;
            } else if (current_b->data < current_a->data) {
                current_b = current_b->next;
            } else {
                current_a = current_a->next;
                current_b = current_b->next;
            }
        }
        std::cout << "}" << std::endl;
    }
}
void set_manager::check_subset(char set_a_name, char set_b_name)
{
    Set* s_a = find_set(set_a_name);
    Set* s_b = find_set(set_b_name);

    if (!s_a || !s_b) {
        std::cout << "false" << std::endl;
        return;
    }

    Node* current_a = s_a->head;
    Node* current_b = s_b->head;

    while (current_a) {
        bool found = false;
        while (current_b && current_b->data <= current_a->data) {
            if (current_a->data == current_b->data) {
                found = true;
                break;
            }
            current_b = current_b->next;
        }
        if (!found) {
            std::cout << "false" << std::endl;
            return;
        }
        current_a = current_a->next;
    }
    std::cout << "true" << std::endl;
}
void set_manager::check_equality(char set_a_name, char set_b_name)
{
    Set* s_a = find_set(set_a_name);
    Set* s_b = find_set(set_b_name);

    if (!s_a || !s_b) {
        std::cout << "false" << std::endl;
        return;
    }

    Node* current_a = s_a->head;
    Node* current_b = s_b->head;

    while (current_a && current_b) {
        if (current_a->data != current_b->data) {
            std::cout << "false" << std::endl;
            return;
        }
        current_a = current_a->next;
        current_b = current_b->next;
    }

    if (current_a || current_b) {
        std::cout << "false" << std::endl;
    } else {
        std::cout << "true" << std::endl;
    }
}