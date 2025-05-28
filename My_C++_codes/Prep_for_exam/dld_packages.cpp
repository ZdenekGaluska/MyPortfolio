 #include<iostream>
#include<cassert>
#include<vector>
#include<set>
#include<map>
#include<string>
#include<unordered_map>
#include<memory>
#include<unordered_set>
#include<queue>
#include<list>
#include<sstream>
#include<cstring>
 
using namespace std;
 
class CPkg{
public:
    CPkg(std::string str) : pkgName(str){}
    ~CPkg() = default;

    auto operator<(const CPkg &other) const {
        return pkgName < other.pkgName;
    }

    CPkg& addDepend(std::string str) {
        dep.insert(str);
        return *this;
    }
    CPkg& addRecommend(std::string str) {
        rec.insert(str);
        return *this;
    }
 
    std::string pkgName;
    std::set<std::string> dep;
    std::set<std::string> rec;
};
class CPkgSys{
public:
    CPkgSys() = default;
    ~CPkgSys() = default;

    CPkgSys& addPkg(CPkg f) {
        files.insert(f);
        return *this;
    }
    std::set<std::string> listAffected(std::list<std::string> pkgList, bool reco) {
        std::queue<std::string> togo;
        std::set<std::string> visited;
        for (auto i : pkgList) {

            togo.push(i);
            visited.insert(i);
        }

        while(!togo.empty()) {
            for (auto pkg : files) {
                if(pkg.dep.find(togo.front()) != pkg.dep.end() &&
                   visited.find(pkg.pkgName) == visited.end()) {
                    togo.push(pkg.pkgName);
                    visited.insert(pkg.pkgName);
                }

                if(reco && pkg.rec.find(togo.front()) != pkg.rec.end() && 
                   visited.find(pkg.pkgName) == visited.end()) {
                    togo.push(pkg.pkgName);
                    visited.insert(pkg.pkgName);
                }
            }     
            togo.pop();
        }

        return visited;
    }
 
private:
    std::set<CPkg> files;
};
#ifndef __PROGTEST__
int main() {
    CPkg pkg("balik");
    CPkg("ssh").addDepend("openssh").addDepend("libstdc++");
 
    CPkgSys t0;
    t0.addPkg(CPkg("ssh").addDepend("openssh").addDepend("libstdc++"));
    t0.addPkg(CPkg("openssh").addDepend("openssh-server").addRecommend("openssh-client"));
    t0.addPkg(
        CPkg("openssh-server").addDepend("libkrb").addRecommend("libaudit").addDepend("libssl").addDepend("libc"));
    t0.addPkg(CPkg("openssh-client").addDepend("libssl").addDepend("libc"));
    t0.addPkg(CPkg("libaudit").addDepend("libc").addDepend("libstdc++"));
    t0.addPkg(CPkg("libssl").addDepend("libc"));
    t0.addPkg(CPkg("libc"));
    t0.addPkg(CPkg("libstdc++").addDepend("libc"));
    t0.addPkg(CPkg("ntpd").addDepend("libc").addDepend("libm"));
    t0.addPkg(CPkg("libssl-dev").addDepend("libc").addDepend("libssl"));
    t0.addPkg(CPkg("libm"));
        // t0.listAffected(std::list<std::string>{"libkrb"}, false);
    assert(
        t0.listAffected(std::list<std::string>{"libkrb"}, false) == (std::set<std::string>{"libkrb", "openssh",
            "openssh-server", "ssh"}));
    assert(
        t0.listAffected(std::list<std::string>{"libssl"}, false) == (std::set<std::string>{"libssl", "libssl-dev",
            "openssh" , "openssh-client", "openssh-server", "ssh"}));
    assert(
        t0.listAffected(std::list<std::string>{"libssl", "libm"}, false) == (std::set<std::string>{"libm", "libssl",
            "libssl-dev", "ntpd", "openssh", "openssh-client", "openssh-server", "ssh"}));
    assert(
        t0.listAffected(std::list<std::string>{"libstdc++"}, false) == (std::set<std::string>{"libaudit", "libstdc++",
            "ssh"}));
    assert(
        t0.listAffected(std::list<std::string>{"libstdc++"}, true) == (std::set<std::string>{"libaudit", "libstdc++",
            "openssh", "openssh-server", "ssh"}));
            t0.addPkg(CPkg(""));
t0.listAffected({""}, false);
 
    return EXIT_SUCCESS;
}
 
#endif /* __PROGTEST__ */

