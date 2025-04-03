#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cassert>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <memory>
#include <compare>
#include <functional>
#include <stdexcept>
#include <algorithm>
#endif /* __PROGTEST__ */

struct Person {
std::string nam;
std::string adr;
std::string acc;
int sumIncome;
int sumExpense;
};

class CIterator
{
  size_t index;
  const std::vector<Person>* data;
  public:
    CIterator() : index(0), data(nullptr) {}

    CIterator(const std::vector<Person>* d, size_t idx) : index(idx), data(d) {}
    bool                atEnd   () const {
      return (data == nullptr || index == data->size());
    }    
    void                next    () {
      if (!atEnd())
      ++index;
    }
    const std::string & name    () const {
      return (*data)[index].nam;
    }
    const std::string & addr    () const {
      return (*data)[index].adr;
    }
    const std::string & account () const {
      return (*data)[index].acc;
    }
  private:
    // todo
};

class CTaxRegister
{
    std::vector<Person> p;
    std::vector<Person> a;
  public:

    CTaxRegister() {}
    bool      birth      ( const std::string    & name,
                           const std::string    & addr,
                           const std::string    & account ) {
                            auto it_account = std::lower_bound(a.begin(), a.end(), account,
                                [](const Person &person, const std::string &acc) {
                                    return person.acc < acc;
                                });
                            if (it_account != a.end() && it_account->acc == account) {
                                return false;
                            }


                            auto it = std::lower_bound(p.begin(), p.end(), Person {name, addr, account, 0, 0},
                            [](const Person &a, const Person &b) {
                                if (a.nam != b.nam) return a.nam < b.nam;
                                return a.adr < b.adr;
                            });

                            if (it != p.end() && it->nam == name && it->adr == addr) return false;
                            Person current {name, addr, account, 0, 0};
                            p.insert(it, current);
                            a.insert(it_account, current);
                            return true;
                           }
    bool      death      ( const std::string    & name,
                           const std::string    & addr ) {

                           auto it = std::lower_bound(p.begin(), p.end(), Person {name, addr, "", 0, 0}, []
                            (const Person &a, const Person &b) {
                              if (a.nam != b.nam) return a.nam < b.nam;
                              return a.adr < b.adr;
                            });
                            std::string account;
                            if (it != p.end() && it->nam == name && it->adr == addr) {
                              account = it->acc;
                              p.erase(it);
                            }
                            else return false;

                            auto it_account = std::lower_bound(a.begin(), a.end(), account,
                                [](const Person &person, const std::string &acc) {
                                    return person.acc < acc;
                                });
                               if (it_account != a.end() && it_account->acc == account) {
                                    a.erase(it_account);
                                    return true;
                            }
                              return false;
                           }


    bool      income     ( const std::string    & account,
                           int                    amount ) {
                            auto it = std::lower_bound(a.begin(), a.end(), account,
                                [](const Person &person, const std::string &acc) {
                                    return person.acc < acc;
                                });
                            if ((it != a.end() && it->acc != account) || it == a.end()) {
                                return false;
                            }
                            it->sumIncome += amount;
                            auto itos = std::lower_bound(p.begin(), p.end(), Person{it->nam, it->adr, "", 0 ,0}, [] 
                            (const Person &a, const Person &b) {
                              if (a.nam != b.nam) return a.nam < b.nam;
                              return a.adr < b.adr;
                            });
                            if (itos != p.end() && itos->nam == it->nam && itos->adr == it->adr) {
                          itos->sumIncome += amount;
                          return true;
                            }
                            return false;
                           }
    bool      income     ( const std::string    & name,
                           const std::string    & addr,
                           int                    amount ) {
                            auto it = std::lower_bound(p.begin(), p.end(), Person{name, addr, "", 0 ,0}, [] 
                            (const Person &a, const Person &b) {
                              if (a.nam != b.nam) return a.nam < b.nam;
                              return a.adr < b.adr;
                            });
                            if ((it != p.end() && (it->nam != name || it->adr != addr)) || it == p.end()) {;
                              return false;
                            }                      
                            it->sumIncome += amount;
                            auto itos = std::lower_bound(a.begin(), a.end(), it->acc,
                                [](const Person &person, const std::string &acc) {
                                    return person.acc < acc;
                                });            
                                if (itos != a.end() && itos->acc == it->acc) {                             
                                
                            itos->sumIncome +=amount;           
                            return true;
                                }
                                return false;
                           }
    bool      expense    ( const std::string    & account,
                           int                    amount ) {
                            auto it = std::lower_bound(a.begin(), a.end(), account,
                                [](const Person &person, const std::string &acc) {
                                    return person.acc < acc;
                                });
                            if ((it != a.end() && it->acc != account) || it == a.end()) {
                                return false;
                            }
                            it->sumExpense += amount;
                            auto itos = std::lower_bound(p.begin(), p.end(), Person{it->nam, it->adr, "", 0 ,0}, [] 
                            (const Person &a, const Person &b) {
                              if (a.nam != b.nam) return a.nam < b.nam;
                              return a.adr < b.adr;
                            });
                            if (itos != p.end() && itos->nam == it->nam && itos->adr == it->adr) {
                          itos->sumExpense += amount;
                          return true;
                            }
                            return false;
                           }
    bool      expense    ( const std::string    & name,
                           const std::string    & addr,
                           int                    amount ) {
                            auto it = std::lower_bound(p.begin(), p.end(), Person{name, addr, "", 0 ,0}, [] 
                            (const Person &a, const Person &b) {
                              if (a.nam != b.nam) return a.nam < b.nam;
                              return a.adr < b.adr;
                            });
                            if ((it != p.end() && (it->nam != name || it->adr != addr)) || it == p.end()) {;
                              return false;
                            }                      
                            it->sumExpense += amount;
                            auto itos = std::lower_bound(a.begin(), a.end(), it->acc,
                                [](const Person &person, const std::string &acc) {
                                    return person.acc < acc;
                                });            
                                if (itos != a.end() && itos->acc == it->acc) {                             
                                
                            itos->sumExpense +=amount;           
                            return true;
                                }
                                return false;
                           }
                
    bool      audit      ( const std::string    & name,
                           const std::string    & addr,
                           std::string          & account,
                           int                  & sumIncome,
                           int                  & sumExpense ) const {
                              auto it = std::lower_bound(p.begin(), p.end(), Person{name, addr, "", 0, 0 }, []
                              (const Person &a, const Person &b){
                                if (a.nam != b.nam) return a.nam < b.nam;
                                return a.adr < b.adr;
                              });
                              if (it != p.end() && it->nam == name && it->adr == addr) {
                                account = it->acc;
                                sumIncome = it->sumIncome;
                                sumExpense = it->sumExpense;
                                return true;
                              }
                              return false;
                           }

    void print_list() const{
        for (const Person &pepeno : p) {
            std::cout << pepeno.nam << " " << pepeno.adr << " " << pepeno.acc << " " << pepeno.sumIncome << "/" << pepeno.sumExpense << std::endl;
        }
        std::cout << "---------------------" << std::endl;
        return;
    }
    CIterator listByName () const {
      return CIterator(&p, 0);
    }
  private:
};

#ifndef __PROGTEST__
int main ()
{
  //   CTaxRegister hovno;
  // assert ( hovno . birth ( "John Smith", "Oak Road 23", "123/456/789" ) );
  // assert ( hovno . birth ( "Jane Hacker", "Main Street 17", "Xuj5#94" ) );
  // assert ( hovno . birth ( "Peter Hacker", "Main Street 17", "634oddT" ) );
  // assert ( hovno . birth ( "John Smith", "Main Street 17", "Z343rwZ" ) );
  //   hovno.print_list();
  // hovno.death("John Smith", "Oak Road 23");
  // hovno.death("John Smith", "Oak Road 23");
  // hovno.death("John Smith", "Oak Ro54");
  // hovno.income("Jane Hacker", "Main Street 17", 1000);
  // hovno.income("Z343rwZ", 500);
  // hovno.income("Xuj5#94", 69);
  // hovno.expense("Jane Hacker", "Main Street 17", 100);
  // hovno.expense("Z343rwZ", 39);
  // hovno.expense("Xuj5#94", 420);
  // hovno.print_list();


  std::string acct;
  int    sumIncome, sumExpense;
  CTaxRegister b0;
  assert ( b0 . birth ( "John Smith", "Oak Road 23", "123/456/789" ) );
  assert ( b0 . birth ( "Jane Hacker", "Main Street 17", "Xuj5#94" ) );
  assert ( b0 . birth ( "Peter Hacker", "Main Street 17", "634oddT" ) );
  assert ( b0 . birth ( "John Smith", "Main Street 17", "Z343rwZ" ) );
  assert ( b0 . income ( "Xuj5#94", 1000 ) );
  assert ( b0 . income ( "634oddT", 2000 ) );
  assert ( b0 . income ( "123/456/789", 3000 ) );
  assert ( b0 . income ( "634oddT", 4000 ) );
  assert ( b0 . income ( "Peter Hacker", "Main Street 17", 2000 ) );
  assert ( b0 . expense ( "Jane Hacker", "Main Street 17", 2000 ) );
  assert ( b0 . expense ( "John Smith", "Main Street 17", 500 ) );
  assert ( b0 . expense ( "Jane Hacker", "Main Street 17", 1000 ) );
  assert ( b0 . expense ( "Xuj5#94", 1300 ) );
  assert ( b0 . audit ( "John Smith", "Oak Road 23", acct, sumIncome, sumExpense ) );
  assert ( acct == "123/456/789" );
  assert ( sumIncome == 3000 );
  assert ( sumExpense == 0 );
  assert ( b0 . audit ( "Jane Hacker", "Main Street 17", acct, sumIncome, sumExpense ) );
  assert ( acct == "Xuj5#94" );
  assert ( sumIncome == 1000 );
  assert ( sumExpense == 4300 );
  assert ( b0 . audit ( "Peter Hacker", "Main Street 17", acct, sumIncome, sumExpense ) );
  assert ( acct == "634oddT" );
  assert ( sumIncome == 8000 );
  assert ( sumExpense == 0 );
  assert ( b0 . audit ( "John Smith", "Main Street 17", acct, sumIncome, sumExpense ) );
  assert ( acct == "Z343rwZ" );
  assert ( sumIncome == 0 );
  assert ( sumExpense == 500 );


  CIterator it = b0 . listByName ();
  assert ( ! it . atEnd ()
           && it . name () == "Jane Hacker"
           && it . addr () == "Main Street 17"
           && it . account () == "Xuj5#94" );
  it . next ();
  assert ( ! it . atEnd ()
           && it . name () == "John Smith"
           && it . addr () == "Main Street 17"
           && it . account () == "Z343rwZ" );
  it . next ();
  assert ( ! it . atEnd ()
           && it . name () == "John Smith"
           && it . addr () == "Oak Road 23"
           && it . account () == "123/456/789" );
  it . next ();
  assert ( ! it . atEnd ()
           && it . name () == "Peter Hacker"
           && it . addr () == "Main Street 17"
           && it . account () == "634oddT" );
  it . next ();
  assert ( it . atEnd () );

  assert ( b0 . death ( "John Smith", "Main Street 17" ) );

  CTaxRegister b1;
  assert ( b1 . birth ( "John Smith", "Oak Road 23", "123/456/789" ) );
  assert ( b1 . birth ( "Jane Hacker", "Main Street 17", "Xuj5#94" ) );
  assert ( !b1 . income ( "634oddT", 4000 ) );
  assert ( !b1 . expense ( "John Smith", "Main Street 18", 500 ) );
  assert ( !b1 . audit ( "John Nowak", "Second Street 23", acct, sumIncome, sumExpense ) );
  assert ( !b1 . death ( "Peter Nowak", "5-th Avenue" ) );
  assert ( !b1 . birth ( "Jane Hacker", "Main Street 17", "4et689A" ) );
  assert ( !b1 . birth ( "Joe Hacker", "Elm Street 23", "Xuj5#94" ) );
  assert ( b1 . death ( "Jane Hacker", "Main Street 17" ) );
  assert ( b1 . birth ( "Joe Hacker", "Elm Street 23", "Xuj5#94" ) );
  assert ( b1 . audit ( "Joe Hacker", "Elm Street 23", acct, sumIncome, sumExpense ) );
  assert ( acct == "Xuj5#94" );
  assert ( sumIncome == 0 );
  assert ( sumExpense == 0 );
  assert ( !b1 . birth ( "Joe Hacker", "Elm Street 23", "AAj5#94" ) );

  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
