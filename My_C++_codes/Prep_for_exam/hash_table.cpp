#include <string>
#include <string>
#include <iostream>
#include <sstream>
#include <cassert>
 
struct TItem
{
    TItem(std::string key, std::string val, TItem *nextHash, TItem *nextOrd)
            : m_Key(key), m_Val(val), m_NextHash(nextHash), m_NextOrder(nextOrd){}
 
    std::string m_Key, m_Val;
    TItem *m_NextHash, *m_NextOrder;
};
 
class CHash
{
public:
    CHash(int m) : m_Table(NULL), m_Size(m), m_FirstOrder(NULL), m_LastOrder(NULL)
    {
        m_Table = new TItem *[m];
        for (int i = 0; i < m; i++)
            m_Table[i] = NULL;
    }
    ~CHash()
    {
        for (unsigned int i = 0; i < m_Size; i++) {
            while (m_Table[i] != nullptr) {
                TItem* current = m_Table[i];
                m_Table[i] = current->m_NextHash;
                delete current; 
            }
        }
        delete[] m_Table;
    }
    CHash(CHash &other) : m_FirstOrder(nullptr), m_LastOrder(nullptr) {
        m_Size = other.m_Size;        
        m_Table = new TItem *[m_Size];
        for (unsigned int i = 0; i < m_Size; i++)
            m_Table[i] = NULL;

        TItem* current = other.m_FirstOrder;
        while (current != nullptr) {
            Insert(current->m_Key, current->m_Val);
            current = current->m_NextOrder;
        }
    }

    CHash &operator=(CHash &other) {
        if (&other == this) return *this;

        for (unsigned int i = 0; i < m_Size; i++) {
            while (m_Table[i] != nullptr) {
                TItem* current = m_Table[i];
                m_Table[i] = current->m_NextHash;
                delete current; 
            }
        }
        delete m_Table;


        m_LastOrder = nullptr;
        m_FirstOrder = nullptr;
        m_Size = other.m_Size;
        m_Table = new TItem *[m_Size];
        for (unsigned int i = 0; i < other.m_Size; i++) {
            m_Table[i] = NULL;
        }

        TItem* current = other.m_FirstOrder;
        while (current != nullptr) {
            Insert(current->m_Key, current->m_Val);
            current = current->m_NextOrder;
        }
        return *this;
    }
 
    bool Insert(std::string key, std::string val)
    {
        if(IsSet(key)) return false;
        unsigned int hashed_key = hashFn(key,m_Size);

        TItem* value = new TItem(key, val, nullptr, nullptr);

        if (m_FirstOrder == nullptr) {
            m_FirstOrder = value;
            m_LastOrder = value;
        }
        else {
            m_LastOrder->m_NextOrder = value;
            m_LastOrder = value;
        }        

        value->m_NextHash = m_Table[hashed_key];
        m_Table[hashed_key] = value;
        std::cout << key << " = " << hashed_key << ", ";
        return true;
    }
    // cannot iterate through all items in table (would be too slow)
    bool IsSet(std::string key)
    {
        unsigned int hashed_key = hashFn(key, m_Size);
        if ( m_Table[hashed_key] == nullptr ) return false;
        TItem* current = m_Table[hashed_key];
        while (current != nullptr) {
            if (current->m_Key == key) return true;
            current = current->m_NextHash;
        }
        return false;
    }
    friend std::ostringstream & operator<<(std::ostringstream & os, const CHash & table) {
        TItem* current = table.m_FirstOrder;
        while (current != nullptr) {
            os << current->m_Key << " => " << current->m_Val;
            if (current->m_NextOrder != nullptr) {
                os << ", ";
            } 
            current = current->m_NextOrder;
        }
        
        return os;
    }
 
    TItem **m_Table;
    unsigned int m_Size;
    TItem *m_FirstOrder, *m_LastOrder;
private:
    unsigned int hashFn(std::string &str, size_t mod)
    {
        std::hash<std::string> hash_fn;
        return hash_fn(str) % mod;
    }
};
 
int main()
{
    std::ostringstream oss;
    CHash hashtable(4);
    assert(hashtable.Insert("h1", "car") == true);
    assert(hashtable.Insert("h1", "phone") == false);
    assert(hashtable.Insert("h2", "field") == true);
    assert(hashtable.Insert("h3", "house") == true);
    assert(hashtable.Insert("h4", "tree") == true);
    // testing the inside of table
    // order of next_hash is up to you, tests show older items as last
    assert(hashtable.m_FirstOrder->m_Key == "h1"
           && hashtable.m_Table[0]->m_Key == "h3" && hashtable.m_Table[0]->m_Val == "house"
           && hashtable.m_Table[1]->m_Key == "h1" && hashtable.m_Table[1]->m_Val == "car"
           && hashtable.m_Table[2] == nullptr
           && hashtable.m_Table[3]->m_Key == "h4" && hashtable.m_Table[3]->m_Val == "tree"
           && hashtable.m_Table[0]->m_NextHash->m_Key == "h2" && hashtable.m_Table[0]->m_NextHash->m_Val == "field"
           && hashtable.m_Table[0]->m_NextHash->m_NextHash == nullptr
           && hashtable.m_Table[1]->m_NextHash == nullptr
           && hashtable.m_Table[3]->m_NextHash == nullptr
    );
 
    oss << hashtable;
    assert(oss.str() == "h1 => car, h2 => field, h3 => house, h4 => tree");
    return 0;
}
