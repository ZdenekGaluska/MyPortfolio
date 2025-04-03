#ifndef __PROGTEST__
#include <cassert>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <algorithm>
#include <memory>
#include <compare>
#include <functional>
#include <optional>

class CTimeStamp
{
  public:
      CTimeStamp    ( int               year,
        int               month,
        int               day,
        int               hour,
        int               minute,
        double            sec ) : year(year), month(month), day(day), hour(hour), minute(minute), sec(sec) {}
    
    int  compare       ( const CTimeStamp & x ) const {
      if (this->year != x.year) {
        return this->year - x.year;
      } 
      else if (this->month != x.month) {
        return this->month - x.month;
      }
      else if (this->day != x.day) {
        return this->day - x.day;
      }
      else if (this->hour != x.hour) {
        return this->hour - x.hour;
      }
      else if (this->minute != x.minute) {
        return this->minute - x.minute;
      }
      else if (this->sec != x.sec){
        if (this->sec < x.sec) return -1;
        else return 1;
      }
      return 0;
    } 

  friend std::ostream & operator << ( std::ostream & os, const CTimeStamp & x ) {
    os << std::setfill('0')
      << std::setw(4) << x.year << "-"
      << std::setw(2) << x.month << "-"
      << std::setw(2) << x.day << " "
      << std::setw(2) << x.hour << ":"
      << std::setw(2) << x.minute << ":"
      << std::fixed << std::setprecision(3) << std::setw(6) << x.sec;
    return os;
  }
  private:

  int year, month, day, hour, minute;
  double sec;
};
class CMail
{
  public:
                          CMail         ( const CTimeStamp & timeStamp,
                                          const std::string & from,
                                          const std::string & to,
                                          const std::optional<std::string> & subject ) :
                        tS(timeStamp), f(from), t(to), s(subject) {}
    int                   compareByTime ( const CTimeStamp & x ) const {
      return tS.compare(x);
    }
    int                   compareByTime ( const CMail      & x ) const {
      return tS.compare(x.tS);
    }
    const std::string   & from          () const {
      return f;
    }
    const std::string   & to            () const {
      return t;
    }
    const std::optional<std::string>  & subject       () const {
      return s;
    }
    const CTimeStamp    & timeStamp     () const {
      return tS;
    }
    friend std::ostream & operator <<   ( std::ostream     & os,
                                          const CMail      & x ) {
      os << x.tS << " " << x.f << " " << "-> "  << x.t;
      if (x.s.has_value()) {
        os << ", subject: " << *x.s;
      }
      return os;
    }
  private:
  CTimeStamp tS;
  std::string f;
  std::string t;
  std::optional<std::string> s;
};
// your code will be compiled in a separate namespace
namespace MysteriousNamespace {
#endif /* __PROGTEST__ */
//----------------------------------------------------------------------------------------
int month_to_number(const std::string &month) {
  if (month == "Jan") return 1;
  if (month == "Feb") return 2;
  if (month == "Mar") return 3;
  if (month == "Apr") return 4;
  if (month == "May") return 5;
  if (month == "Jun") return 6;
  if (month == "Jul") return 7;
  if (month == "Aug") return 8;
  if (month == "Sep") return 9;
  if (month == "Oct") return 10;
  if (month == "Nov") return 11;
  if (month == "Dec") return 12;
  return 0;
}
class CMailLog
{
    struct Info{
      std::string fromwho;
      std::optional<std::string> sub;
    };
  public:
    int                   parseLog      ( std::istream     & in ) {
      int count_of_mails = 0; 
      std::string input;

      while(std::getline(in, input)) {
        std::string MONTHSTR, OCLOCK, RELAY, MAILID, MESSAGES;
        int DAY, YEAR;
        std::istringstream iss(input);
        if (!(iss >> MONTHSTR >> DAY >> YEAR >> OCLOCK >> RELAY >> MAILID)) continue;
        getline(iss >> std::ws, MESSAGES);
      
        
        MAILID.pop_back();
        if (MESSAGES.find("to=") == 0) {
          std::string HOURSTR, MINUTESTR, SECONDSTR;
          std::istringstream issoclock(OCLOCK);
          std::getline(issoclock, HOURSTR, ':');
          std::getline(issoclock, MINUTESTR, ':');
          std::getline(issoclock, SECONDSTR);

          double SECOND = std::stod(SECONDSTR);       
          CTimeStamp TIME (YEAR, month_to_number(MONTHSTR), DAY, std::stoi(HOURSTR), std::stoi(MINUTESTR), SECOND);
          
          if (preMail.find(MAILID) == preMail.end()) continue;
          CMail newMail(TIME, preMail[MAILID].fromwho, MESSAGES.substr(3), preMail[MAILID].sub);
          auto it = std::upper_bound(Mails.begin(), Mails.end(), newMail,  
          [](const CMail &a, const CMail &b) {
            return a.timeStamp().compare(b.timeStamp()) < 0;
          });
          Mails.insert(it, newMail);
          count_of_mails++;
        }
        if (MESSAGES.find("from=") == 0) {
          preMail[MAILID].fromwho = MESSAGES.substr(5);
        }
        if (MESSAGES.find("subject=") == 0) {
          auto it = preMail.find(MAILID);
          if (it != preMail.end()) {
            preMail[MAILID].sub = MESSAGES.substr(8);            
          }
        }
        continue;
      }
    return count_of_mails;
    }

    std::list<CMail>      listMail      ( const CTimeStamp & from,
                                          const CTimeStamp & to ) const {
      std::list<CMail> FinalList;

      CMail dummy(from, "", "", std::nullopt);
      auto it = std::lower_bound(Mails.begin(), Mails.end(), dummy, 
      [](const CMail &a, const CMail &b) {
        return a.timeStamp().compare(b.timeStamp()) < 0;
      }); 

      while (it != Mails.end() && it->timeStamp().compare(to) <= 0) {
        FinalList.push_back(*it);
        ++it;
      }
      return FinalList;
    }


    std::set<std::string> activeUsers   ( const CTimeStamp & from,
                                          const CTimeStamp & to ) const {
      std::set<std::string> FinalList;
      CMail dummy(from, "", "", std::nullopt);
      auto it = std::lower_bound(Mails.begin(), Mails.end(), dummy, 
      [](const CMail &a, const CMail &b) {
        return a.timeStamp().compare(b.timeStamp()) < 0;
      });       
      while (it != Mails.end() && it->timeStamp().compare(to) <= 0) {
        FinalList.insert((*it).from());
        FinalList.insert((*it).to());        
        ++it;
      }      
      return FinalList;
    }
  private:
    std::map<std::string,Info> preMail;
    std::vector<CMail> Mails;
};
//----------------------------------------------------------------------------------------
#ifndef __PROGTEST__
} // namespace
std::string             printMail       ( const std::list<CMail> & all )
{
  std::ostringstream oss;
  for ( const auto & mail : all )
    oss << mail << "\n";
  return oss . str ();
}
int                     main ()
{
  MysteriousNamespace::CMailLog m;
  std::list<CMail> mailList;
  std::set<std::string> users;
  std::istringstream iss;

  iss . clear ();
  iss . str (
    "Mar 29 2025 12:35:32.233 relay.fit.cvut.cz ADFger72343D: from=user1@fit.cvut.cz\n"
    "Mar 29 2025 12:37:16.234 relay.fit.cvut.cz JlMSRW4232Df: from=person3@fit.cvut.cz\n"
    "Mar 29 2025 12:55:13.023 relay.fit.cvut.cz JlMSRW4232Df: subject=New progtest homework!\n"
    "Mar 29 2025 13:38:45.043 relay.fit.cvut.cz Kbced342sdgA: from=office13@fit.cvut.cz\n"
    "Mar 29 2025 13:36:13.023 relay.fit.cvut.cz JlMSRW4232Df: to=user76@fit.cvut.cz\n"
    "Mar 29 2025 13:55:31.456 relay.fit.cvut.cz KhdfEjkl247D: from=PR-department@fit.cvut.cz\n"
    "Mar 29 2025 14:18:12.654 relay.fit.cvut.cz Kbced342sdgA: to=boss13@fit.cvut.cz\n"
    "Mar 29 2025 14:48:32.563 relay.fit.cvut.cz KhdfEjkl247D: subject=Business partner\n"
    "Mar 29 2025 14:58:32.000 relay.fit.cvut.cz KhdfEjkl247D: to=HR-department@fit.cvut.cz\n"
    "Mar 29 2025 14:25:23.233 relay.fit.cvut.cz ADFger72343D: mail undeliverable\n"
    "Mar 29 2025 15:02:34.231 relay.fit.cvut.cz KhdfEjkl247D: to=CIO@fit.cvut.cz\n"
    "Mar 29 2025 15:02:34.230 relay.fit.cvut.cz KhdfEjkl247D: to=CEO@fit.cvut.cz\n"
    "Mar 29 2025 15:02:34.230 relay.fit.cvut.cz KhdfEjkl247D: to=dean@fit.cvut.cz\n"
    "Mar 29 2025 15:02:34.230 relay.fit.cvut.cz KhdfEjkl247D: to=vice-dean@fit.cvut.cz\n"
    "Mar 29 2025 15:02:34.230 relay.fit.cvut.cz KhdfEjkl247D: to=archive@fit.cvut.cz\n" );
  assert ( m . parseLog ( iss ) == 8 );
  mailList = m . listMail ( CTimeStamp ( 2025, 3, 28, 0, 0, 0 ),
                            CTimeStamp ( 2025, 3, 29, 23, 59, 59 ) );
                          //  std::cout << printMail(mailList);
assert ( printMail ( mailList ) == R"###(2025-03-29 13:36:13.023 person3@fit.cvut.cz -> user76@fit.cvut.cz, subject: New progtest homework!
2025-03-29 14:18:12.654 office13@fit.cvut.cz -> boss13@fit.cvut.cz
2025-03-29 14:58:32.000 PR-department@fit.cvut.cz -> HR-department@fit.cvut.cz, subject: Business partner
2025-03-29 15:02:34.230 PR-department@fit.cvut.cz -> CEO@fit.cvut.cz, subject: Business partner
2025-03-29 15:02:34.230 PR-department@fit.cvut.cz -> dean@fit.cvut.cz, subject: Business partner
2025-03-29 15:02:34.230 PR-department@fit.cvut.cz -> vice-dean@fit.cvut.cz, subject: Business partner
2025-03-29 15:02:34.230 PR-department@fit.cvut.cz -> archive@fit.cvut.cz, subject: Business partner
2025-03-29 15:02:34.231 PR-department@fit.cvut.cz -> CIO@fit.cvut.cz, subject: Business partner
)###");
  mailList = m . listMail ( CTimeStamp ( 2025, 3, 28, 0, 0, 0 ),
                            CTimeStamp ( 2025, 3, 29, 14, 58, 32 ) );
  assert ( printMail ( mailList ) == R"###(2025-03-29 13:36:13.023 person3@fit.cvut.cz -> user76@fit.cvut.cz, subject: New progtest homework!
2025-03-29 14:18:12.654 office13@fit.cvut.cz -> boss13@fit.cvut.cz
2025-03-29 14:58:32.000 PR-department@fit.cvut.cz -> HR-department@fit.cvut.cz, subject: Business partner
)###" );
  mailList = m . listMail ( CTimeStamp ( 2025, 3, 30, 0, 0, 0 ),
                            CTimeStamp ( 2025, 3, 30, 23, 59, 59 ) );
  assert ( printMail ( mailList ) == "" );
  users = m . activeUsers ( CTimeStamp ( 2025, 3, 28, 0, 0, 0 ),
                            CTimeStamp ( 2025, 3, 29, 23, 59, 59 ) );
  assert ( users == std::set<std::string>( { "CEO@fit.cvut.cz", "CIO@fit.cvut.cz", "HR-department@fit.cvut.cz", "PR-department@fit.cvut.cz", "archive@fit.cvut.cz", "boss13@fit.cvut.cz", "dean@fit.cvut.cz", "office13@fit.cvut.cz", "person3@fit.cvut.cz", "user76@fit.cvut.cz", "vice-dean@fit.cvut.cz" } ) );
  users = m . activeUsers ( CTimeStamp ( 2025, 3, 28, 0, 0, 0 ),
                            CTimeStamp ( 2025, 3, 29, 13, 59, 59 ) );
  assert ( users == std::set<std::string>( { "person3@fit.cvut.cz", "user76@fit.cvut.cz" } ) );
  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
