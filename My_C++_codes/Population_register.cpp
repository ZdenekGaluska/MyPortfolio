#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>
#include <iostream>
#include <sstream>
#endif /* __PROGTEST__ */

class CRegister
{
      private:

    struct index_val {
        bool right;
        size_t id;
    };

    struct History {
        char date[12];
        char *street;
        char *city;        
    };

    struct Data {
        char id[12];
        char *name;
        char *surname;

        History* h;
        size_t count;
        size_t capacity;
    };

    struct RegisterData {
        Data* d;
        size_t count;
        size_t capacity;
        size_t refCount;
        public:
        RegisterData() : d(nullptr), count(0), capacity(0), refCount(1) {}

        ~RegisterData() {
            for (size_t i = 0; i < count; i++) {

                for (size_t j = 0; j < d[i].count;j++) {
                    delete[] d[i].h[j].city;
                    delete[] d[i].h[j].street;
                }
                delete[] d[i].name;
                delete[] d[i].surname;
                delete[] d[i].h;
            }
            delete[] d;
        }
    };

    RegisterData *pData;

    
    void mine_realloc() {
        if (pData->count >= pData->capacity) {
            if (pData->capacity != 0) {
                pData->capacity += 2; 
                pData->capacity *= 2;
                Data *newD = new Data[pData->capacity];
                for (size_t i = 0; i < pData->count; i++) {
                    newD[i] = pData->d[i];
                } 
                delete[] pData->d; 
                pData->d = newD;
            }
            else {
                pData->capacity = 8;
                pData->d = new Data[pData->capacity];
            }
        }
    }

    index_val lowiebondie(const char id[]) const {
        index_val returnie = {0, false};
        size_t low = 0, high = pData->count;
        while (low < high) {
            size_t mid = (low + high) / 2;
            int cmp = strcmp(pData->d[mid].id, id);
            if (cmp < 0)
                low = mid + 1;
            else
                high = mid;
        }
        if (low < pData->count && strcmp(pData->d[low].id, id) == 0)
            returnie.right = true;
        returnie.id = low;
        return returnie;
    }

    index_val lowiebondie(const char key[], const size_t index) const {
        index_val returnie = {0, false};
        size_t low = 0, high = pData->d[index].count;
        while (low < high) {
            size_t mid = (low + high) / 2;
            int cmp = strcmp(pData->d[index].h[mid].date, key);
            if (cmp < 0)
                low = mid + 1;
            else
                high = mid;
        }
        if (low < pData->d[index].count && strcmp(pData->d[index].h[low].date, key) == 0)
            returnie.right = true;
        returnie.id = low;
        return returnie;
    }

    void insertAt(const size_t id, const size_t index,const History & newData) {
        for (size_t i = pData->d[id].count; i > index; i--) {
            pData->d[id].h[i] = pData->d[id].h[i-1];
        }
    pData->d[id].h[index] = newData;
    pData->d[id].count++;
    }

    void insertAtProPlus(const int index,const Data & newData) {
        for (int i = pData->count; i > index; i--) {
            pData->d[i] = pData->d[i-1];
        }
    pData->d[index] = newData;
    pData->count++;
    }

    
    
  public:
        CRegister() :pData(new RegisterData) {}
        ~CRegister() {
            if (--pData->refCount == 0) {
                    delete pData;
            }
        }
    // copy constructor pro COW:
    CRegister(const CRegister &other) : pData(other.pData) {
        pData->refCount++;
    }

    //operator = pro shallow kopii:
    CRegister& operator=(const CRegister &other) {
        if (this != &other) {
            other.pData->refCount++;

            if (--pData->refCount == 0) delete pData;

            pData = other.pData;
        }
        return *this;
    }

    void ensureUnique() {
        if (pData->refCount > 1) {
            RegisterData* newData = new RegisterData;
            newData->capacity = pData->capacity;
            newData->count = pData->count;
            newData->d = new Data[newData->capacity];

            for (size_t i = 0; i < pData->count; i++) {
                strcpy(newData->d[i].id, pData->d[i].id);
                
                newData->d[i].name = new char[strlen(pData->d[i].name) + 1];
                strcpy(newData->d[i].name, pData->d[i].name);
                
                newData->d[i].surname = new char[strlen(pData->d[i].surname) + 1];
                strcpy(newData->d[i].surname, pData->d[i].surname);
                
                newData->d[i].capacity = pData->d[i].capacity;
                newData->d[i].count = pData->d[i].count;
                newData->d[i].h = new History[newData->d[i].capacity];
                for (size_t j = 0; j < pData->d[i].count; j++) {
                    strcpy(newData->d[i].h[j].date, pData->d[i].h[j].date);
                    newData->d[i].h[j].street = new char[strlen(pData->d[i].h[j].street) + 1];
                    strcpy(newData->d[i].h[j].street, pData->d[i].h[j].street);
                    newData->d[i].h[j].city = new char[strlen(pData->d[i].h[j].city) + 1];
                    strcpy(newData->d[i].h[j].city, pData->d[i].h[j].city);
                }
            }
            newData->refCount = 1;
            pData->refCount--;
            pData = newData;
        }
    }


    bool add       ( const char     id[],
                     const char     name[],
                     const char     surname[],
                     const char     date[],
                     const char     street[],
                     const char     city[] ) {
                        ensureUnique();
                        mine_realloc();
                        index_val index = lowiebondie(id);
                        if (index.right) return false;

                        Data newData;
                            strcpy(newData.id, id);
                            newData.name = new char[strlen(name)+1];
                            newData.surname = new char[strlen(surname)+1];
                            strcpy(newData.name, name);
                            strcpy(newData.surname, surname);
                            newData.h = new History [1];
                            newData.h[0].city = new char[strlen(city)+1];
                            newData.h[0].street = new char[strlen(street)+1];
                            strcpy(newData.h[0].city, city);
                            strcpy(newData.h[0].street, street);
                            strcpy(newData.h[0].date, date);
                            newData.count = 1;
                            newData.capacity = 1;

                            insertAtProPlus(index.id, newData);
                            return true;
                     }


    bool resettle  ( const char     id[],
                     const char     date[],
                     const char     street[],
                     const char     city[] ) {
                    ensureUnique();
                    index_val index = lowiebondie(id);
                    if (!index.right) return false;

                    if (pData->d[index.id].count >= pData->d[index.id].capacity) {
                        pData->d[index.id].capacity *=2;
                        pData->d[index.id].capacity +=2;

                        History *newBox = new History[pData->d[index.id].capacity];
                        for (size_t i = 0; i < pData->d[index.id].count; i++) {
                            newBox[i] = pData->d[index.id].h[i];
                        }
                        delete[] pData->d[index.id].h;
                        pData->d[index.id].h = newBox;
                    }

                    index_val index_home = lowiebondie(date, index.id);
                    if(index_home.right) {
                        return false;
                    }
                    else {
                        History BlackBox;
                            BlackBox.city = new char[strlen(city)+1];
                            BlackBox.street = new char[strlen(street)+1];
                            strcpy(BlackBox.city, city);
                            strcpy(BlackBox.street, street);
                            strcpy(BlackBox.date, date);                        
                        insertAt(index.id, index_home.id, BlackBox); 
                    }
                    return true;
                    }

    bool print     ( std::ostream & os,
                     const char     id[] ) const {
                        index_val index = lowiebondie(id);
                        if (!index.right) return false;
                        else {
                            os << pData->d[index.id].id << " " << pData->d[index.id].name << " " << pData->d[index.id].surname << "\n";
                        }
                        for(size_t i = 0; i < pData->d[index.id].count; i++) {
                            os << pData->d[index.id].h[i].date << " " << pData->d[index.id].h[i].street << " " << pData->d[index.id].h[i].city << "\n";
                        }
                        return true;
                     }
};

#ifndef __PROGTEST__
int main ()
{
  char   lID[12], lDate[12], lName[50], lSurname[50], lStreet[50], lCity[50];
  std::ostringstream oss;
  CRegister  a;
  
  assert ( a . add ( "123456/7890", "John", "Smith", "2000-01-01", "Main street", "Seattle" ) == true );
  assert ( a . add ( "987654/3210", "Freddy", "Kruger", "2001-02-03", "Elm street", "Sacramento" ) == true );
  assert ( a . resettle ( "123456/7890", "2003-05-12", "Elm street", "Atlanta" ) == true );
  assert ( a . resettle ( "123456/7890", "2002-12-05", "Sunset boulevard", "Los Angeles" ) == true );
  oss . str ( "" );
  assert ( a . print ( oss, "123456/7890" ) == true );

//   a.print(std::cout, "123456/7890");
  assert ( ! strcmp ( oss . str () . c_str (), R"###(123456/7890 John Smith
2000-01-01 Main street Seattle
2002-12-05 Sunset boulevard Los Angeles
2003-05-12 Elm street Atlanta
)###" ) );
  oss . str ( "" );
  assert ( a . print ( oss, "987654/3210" ) == true );
  assert ( ! strcmp ( oss . str () . c_str (), R"###(987654/3210 Freddy Kruger
2001-02-03 Elm street Sacramento
)###" ) );
  CRegister b ( a );
  assert ( b . resettle ( "987654/3210", "2008-04-12", "Elm street", "Cinccinati" ) == true );
  assert ( a . resettle ( "987654/3210", "2007-02-11", "Elm street", "Indianapolis" ) == true );
  oss . str ( "" );
  assert ( a . print ( oss, "987654/3210" ) == true );
  assert ( ! strcmp ( oss . str () . c_str (), R"###(987654/3210 Freddy Kruger
2001-02-03 Elm street Sacramento
2007-02-11 Elm street Indianapolis
)###" ) );
  oss . str ( "" );
  assert ( b . print ( oss, "987654/3210" ) == true );
  assert ( ! strcmp ( oss . str () . c_str (), R"###(987654/3210 Freddy Kruger
2001-02-03 Elm street Sacramento
2008-04-12 Elm street Cinccinati
)###" ) );
  a = b;
  assert ( a . resettle ( "987654/3210", "2011-05-05", "Elm street", "Salt Lake City" ) == true );
  oss . str ( "" );
  assert ( a . print ( oss, "987654/3210" ) == true );
  assert ( ! strcmp ( oss . str () . c_str (), R"###(987654/3210 Freddy Kruger
2001-02-03 Elm street Sacramento
2008-04-12 Elm street Cinccinati
2011-05-05 Elm street Salt Lake City
)###" ) );
  oss . str ( "" );
  assert ( b . print ( oss, "987654/3210" ) == true );
  assert ( ! strcmp ( oss . str () . c_str (), R"###(987654/3210 Freddy Kruger
2001-02-03 Elm street Sacramento
2008-04-12 Elm street Cinccinati
)###" ) );
  assert ( b . add ( "987654/3210", "Joe", "Lee", "2010-03-17", "Abbey road", "London" ) == false );
  assert ( a . resettle ( "987654/3210", "2001-02-03", "Second street", "Milwaukee" ) == false );
  oss . str ( "" );
  assert ( a . print ( oss, "666666/6666" ) == false );

  CRegister  c;
  strncpy ( lID, "123456/7890", sizeof ( lID ) );
  strncpy ( lName, "John", sizeof ( lName ) );
  strncpy ( lSurname, "Smith", sizeof ( lSurname ) );
  strncpy ( lDate, "2000-01-01", sizeof ( lDate) );
  strncpy ( lStreet, "Main street", sizeof ( lStreet ) );
  strncpy ( lCity, "Seattle", sizeof ( lCity ) );
  assert (  c . add ( lID, lName, lSurname, lDate, lStreet, lCity ) == true );
  strncpy ( lID, "987654/3210", sizeof ( lID ) );
  strncpy ( lName, "Freddy", sizeof ( lName ) );
  strncpy ( lSurname, "Kruger", sizeof ( lSurname ) );
  strncpy ( lDate, "2001-02-03", sizeof ( lDate) );
  strncpy ( lStreet, "Elm street", sizeof ( lStreet ) );
  strncpy ( lCity, "Sacramento", sizeof ( lCity ) );
  assert (  c . add ( lID, lName, lSurname, lDate, lStreet, lCity ) == true );
  strncpy ( lID, "123456/7890", sizeof ( lID ) );
  strncpy ( lDate, "2003-05-12", sizeof ( lDate) );
  strncpy ( lStreet, "Elm street", sizeof ( lStreet ) );
  strncpy ( lCity, "Atlanta", sizeof ( lCity ) );
  assert ( c . resettle ( lID, lDate, lStreet, lCity ) == true );
  strncpy ( lID, "123456/7890", sizeof ( lID ) );
  strncpy ( lDate, "2002-12-05", sizeof ( lDate) );
  strncpy ( lStreet, "Sunset boulevard", sizeof ( lStreet ) );
  strncpy ( lCity, "Los Angeles", sizeof ( lCity ) );
  assert ( c . resettle ( lID, lDate, lStreet, lCity ) == true );
  oss . str ( "" );
  assert ( c . print ( oss, "123456/7890" ) == true );
  assert ( ! strcmp ( oss . str () . c_str (), R"###(123456/7890 John Smith
2000-01-01 Main street Seattle
2002-12-05 Sunset boulevard Los Angeles
2003-05-12 Elm street Atlanta
)###" ) );


  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
