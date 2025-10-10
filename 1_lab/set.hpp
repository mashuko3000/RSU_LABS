
#ifndef INC_1_LAB_SET_HPP
#define INC_1_LAB_SET_HPP

struct Node
{
    char data;
    Node* next;
};

struct Set
{
    char name;
    Node* head;

    Set();
    ~Set() noexcept;

    Set(const Set& other);
    Set(Set&& other) noexcept;

    Set& operator=(const Set& other);
    Set& operator=(Set&& other) noexcept;
};

class set_manager final
{
private:
    Set sets[26];
    int set_count;

    Set* find_set(char name);
    Node* create_node(char data);
    void clear_list(Node* head);

public:
    set_manager();
    ~set_manager() noexcept;

    set_manager(const set_manager& other);
    set_manager(set_manager&& other) noexcept;

    set_manager& operator=(const set_manager& other);
    set_manager operator=(set_manager&& other) noexcept;

public:
    void new_set(char name);
    void del_set(char name);
    void add_element(char set_name, char element);
    void remove_element(char set_name, char element);
    void pow_set(char name);
    void see(char name = '\0');
    void process_binary_op(char set_a_name, char op, char set_b_name);
    void check_subset(char set_a_name, char set_b_name);
    void check_equality(char set_a_name, char set_b_name);
};

#endif //INC_1_LAB_SET_HPP
