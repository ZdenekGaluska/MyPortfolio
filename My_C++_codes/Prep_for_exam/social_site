#include <cassert>
#include <vector>
#include <memory>
#include <string>
#include <iostream>
 

 
class CSocialNetwork {
public:
    CSocialNetwork(size_t size) : m_Size(size), m_Table(new CPerson *[m_Size]) {
        for (size_t i = 0; i < m_Size; ++i) {
            m_Table[i] = nullptr;
        }
    }
 
    CSocialNetwork(const CSocialNetwork &src) : m_Size(src.m_Size), m_Table(new CPerson *[m_Size]) {
        for (size_t i = 0; i < src.m_Size; i++) m_Table[i] = nullptr;
        for (size_t i = 0; i < src.m_Size; i++) {
            CPerson* current = src.m_Table[i];
            while (current != nullptr) {
                CPerson* new_person = new CPerson;
                new_person->m_Name = current->m_Name;
                new_person->m_Next = nullptr;
                if (m_Table[i] == nullptr) {
                    m_Table[i] = new_person;

                }
                else {
                    CPerson* find_last = m_Table[i];
                    while (find_last->m_Next != nullptr) {
                        find_last = find_last->m_Next;
                    }
                    find_last->m_Next = new_person;                     
                }
                current = current->m_Next;
            }
        }
        for(size_t i = 0; i < m_Size; i++) {
            CPerson* current = m_Table[i];
            CPerson* src_current = src.m_Table[i];
            while (current != nullptr) {
                // std::cout << src_current->m_Friends.size() << " ";
                for (size_t j = 0; j < src_current->m_Friends.size(); j++) {
                    std::string current_name = src_current->m_Friends[j]->m_Name;
                    CPerson* find_person = findPerson(current_name);
                    current->m_Friends.push_back(find_person);
                }
                current = current->m_Next;
                src_current = src_current->m_Next;
            }
        }
    }
 
    CSocialNetwork &operator=(const CSocialNetwork &other) = delete;
 
    ~CSocialNetwork() {
        for (size_t i = 0; i < m_Size; i++) {
            CPerson* current = m_Table[i];
            while(current != nullptr) {
                CPerson* next_current = current->m_Next;
                delete current;
                current = next_current;
            }
        }
        delete []m_Table;
    }
 
    void addPerson(const std::string &name) {
        size_t index = hashFunction(name);
        auto *newPerson = new CPerson(name, m_Table[index]);
        m_Table[index] = newPerson;
    }
 
    void addFriend(const std::string &name, const std::string &friendName) {
        CPerson *person = findPerson(name);
        CPerson *friendOfPerson = findPerson(friendName);
        person->m_Friends.push_back(friendOfPerson);
        friendOfPerson->m_Friends.push_back(person);
    }
 
    bool areFriends(const std::string &name1, const std::string &name2) {
 
 
        CPerson *person1 = findPerson(name1);
        CPerson *person2 = findPerson(name2);
 
        for (CPerson *personFriend: person1->m_Friends) {
            if (personFriend == person2) return true;
        }
 
        return false;
    }
    void print_all() {
                std::cout << "-----------------------------\n";
        for (size_t i = 0; i < m_Size; i++) {
            CPerson* current = m_Table[i];
            while (current != nullptr) {
                std::cout << hashFunction(current->m_Name) << "|";
                std::cout << current->m_Name << " -> ";
                for (size_t j = 0; j < current->m_Friends.size(); j++) {
                    std::cout << current->m_Friends[j]->m_Name<< " ";
                }
                current = current->m_Next;
                std::cout << "\n";
            }
        }
        std::cout << "-----------------------------\n";
    }
 
private:
    struct CPerson {
        CPerson() = default;
 
        CPerson(const std::string &name, CPerson *next) : m_Name(name), m_Next(next) {}
 
        std::string m_Name;
        CPerson *m_Next = nullptr;
        std::vector<CPerson *> m_Friends;
    };
 
    size_t hashFunction(const std::string &name) const {
        size_t hash = 3001;
        for (char c: name) {
            hash = hash * 31 + c;
        }
        return hash % m_Size;
    }
 
    CPerson *findPerson(const std::string &name) const {
        size_t index = hashFunction(name);
        for (CPerson *tmp = m_Table[index]; tmp; tmp = tmp->m_Next)
            if (tmp->m_Name == name) return tmp;
        return nullptr;
    }
 
    size_t m_Size;
    CPerson* *m_Table;
 
    friend class Tester;
};
 
class Tester {
public:
    static void testCopyConstructor() {
        CSocialNetwork network1(10);
        network1.addPerson("Alice");
        network1.addPerson("Bob");
        network1.addPerson("Charlie");
        network1.addFriend("Alice", "Bob");
        network1.addFriend("Bob", "Charlie");
 
        CSocialNetwork network2(network1);
        // network1.print_all();
        // network2.print_all();
 
        // Basic friendship check
        assert(network2.areFriends("Alice", "Bob") == true);
        assert(network2.areFriends("Bob", "Alice") == true); // Bilateral check
        assert(network2.areFriends("Bob", "Charlie") == true);
        assert(network2.areFriends("Charlie", "Bob") == true); // Bilateral check
        assert(network2.areFriends("Alice", "Charlie") == false);
        assert(network2.areFriends("Charlie", "Alice") == false); // Bilateral check

        // Ensure changes in original network do not affect the copy
        network1.addFriend("Alice", "Charlie");
        assert(network2.areFriends("Alice", "Charlie") == false);
        assert(network2.areFriends("Charlie", "Alice") == false); // Bilateral check
 
        // Ensure changes in the copy do not affect the original
        network2.addFriend("Bob", "Alice");
        assert(network1.areFriends("Bob", "Alice") == true); // Should be true already
        assert(network1.areFriends("Alice", "Bob") == true); // Bilateral check
 
        // Check if all persons are copied correctly
        assert(network2.areFriends("Alice", "Bob") == true);
        assert(network2.areFriends("Bob", "Alice") == true); // Bilateral check
        assert(network2.areFriends("Bob", "Charlie") == true);
        assert(network2.areFriends("Charlie", "Bob") == true); // Bilateral check
        assert(network2.areFriends("Alice", "Charlie") == false);
        assert(network2.areFriends("Charlie", "Alice") == false); // Bilateral check
 
        // Add more tests to ensure deep copy
        network1.addPerson("David");
        network1.addFriend("Charlie", "David");
        CSocialNetwork network3(network1);
        assert(network3.areFriends("Charlie", "David") == true);
        assert(network3.areFriends("David", "Charlie") == true); // Bilateral check
        assert(network3.areFriends("Alice", "Charlie") == true);
        assert(network3.areFriends("Charlie", "Alice") == true); // Bilateral check
        assert(network3.areFriends("Bob", "Alice") == true);
        assert(network3.areFriends("Alice", "Bob") == true); // Bilateral check
        network3.print_all();
 
        // Verify that new person in original doesn't exist in copy
        network1.addPerson("Eve");
        assert(network3.findPerson("Eve") == nullptr);
 
        // Verify that friendship in original doesn't affect the copy
        network1.addFriend("Alice", "David");
        assert(network3.areFriends("Alice", "David") == false);
        assert(network3.areFriends("David", "Alice") == false); // Bilateral check
 
        // Verify new friendship in copy does not affect the original
        network3.addFriend("Charlie", "Alice");
        assert(network3.areFriends("Charlie", "Alice") == true);
        assert(network3.areFriends("Alice", "Charlie") == true); // Bilateral check
 
        // Ensure original network doesn't have new people added in copy
        network3.addPerson("Frank");
        assert(network1.findPerson("Frank") == nullptr);
 
        // Verify that the copied network has correct persons
        assert(network2.findPerson("Alice") != nullptr);
        assert(network2.findPerson("Bob") != nullptr);
        assert(network2.findPerson("Charlie") != nullptr);
 
    //     // Test with multiple persons having the same name
        CSocialNetwork n1(10);
        n1.addPerson("Alice");
        n1.addPerson("Peter");
        n1.addFriend("Alice", "Peter");
        n1.addPerson("Alice");
        n1.print_all();

        CSocialNetwork n2(n1);

        assert(n2.findPerson("Alice")->m_Friends.empty());
        assert(n2.findPerson("Alice")->m_Next->m_Friends[0] == n2.findPerson("Peter"));
    }
};
 
int main() {
    Tester::testCopyConstructor();
    return 0;
}

