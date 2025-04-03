#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <climits>
#include <cmath>
#include <cfloat>
#include <cassert>
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <span>
#include <algorithm>
#include <memory>
#include <compare>
#include <complex>
#endif /* __PROGTEST__ */

// keep this dummy version if you do not implement a real manipulator
std::ios_base & ( * poly_var ( const std::string & name ) ) ( std::ios_base & x )
{
    auto in = name;
  return [] ( std::ios_base & ios ) -> std::ios_base & { return ios; };
}

class CPolynomial
{
  public:
    CPolynomial() {
        p.push_back(0);
    }
    CPolynomial(double x) {
        p.push_back(x);
        i.push_back(0);
    }

    friend CPolynomial operator*(const CPolynomial &x,const CPolynomial &y) {
        CPolynomial result;
        if (!x || !y) {
            result.i.clear();
            return result;
        }
        if (x.i.begin() == x.i.end() || y.i.begin() == y.i.end() ) return result;
        auto itx = x.i.begin();
        auto ity = y.i.begin();
        result.p.resize(*itx + *ity + 1, 0);

        for (auto itx = x.i.begin();itx != x.i.end(); itx++) {
            for (auto ity = y.i.begin(); ity != y.i.end(); ity++) {
        result[*itx+*ity] += x.p[*itx] * y.p[*ity];
            }
        }
        return result;
    }
    CPolynomial& operator*=(const CPolynomial&other) {
        CPolynomial temp_result = *this * other;
        if (!static_cast<bool>(temp_result)) {
            this->i.clear();
            this->p.clear();
        }
        this->i = temp_result.i;
        this->p = temp_result.p;
        return *this;
    }
    friend bool operator==(const CPolynomial & x, const CPolynomial & y) {
        size_t posX = 0, posY = 0;

        while (posX < x.i.size() && posY < y.i.size()) {
            int xExp = x.i[posX];
            int yExp = y.i[posY];

            if (yExp == xExp) {
                if (x.p[xExp] != y.p[yExp]) return false;
                posY++;
                posX++;
            }
            else if (yExp < xExp) {
                if (x.p[xExp] != 0) return false;
                posX++;
            }
            else if (yExp > xExp) {
                if (y.p[yExp] != 0) return false;
                posY++;
            }
        }

        while (posX < x.i.size()) {
            int xExp = x.i[posX];
            if (x.p[xExp] != 0) return false;
            posX++;
        }

        while (posY < y.i.size()) {
            int yExp = y.i[posY];
            if (y.p[yExp] != 0) return false;
            posY++;
        }
        return true;
    }

    friend bool operator!=(const CPolynomial &x, const CPolynomial &y) {
        if (x == y) return false;
        return true;
    }
    
    double& operator[](std::size_t index) {
        if (p.size() <= index) {
            p.resize(index+1,0);
        }
        auto it = std::lower_bound(i.begin(), i.end(), index, [] 
        (const int &a, const int &b)  {
            return a > b;
        });
        if (it == i.end() || *it != static_cast<int>(index))
        i.insert(it, index);

        return p[index];
    }
    double operator[](std::size_t index) const
    {
        if (index >= p.size()) 
            return 0.0;

        auto it = std::lower_bound(i.begin(), i.end(), static_cast<int>(index),
                                [] (int a, int b) { return a > b; });
        if (it == i.end() || *it != static_cast<int>(index))
            return 0.0;

        return p[index];
    }

    double operator()(const double x) const {
        double sum = 0;
        for (auto idx : i) { 
            sum += p[idx] * std::pow(x, idx);
        }
        return sum;
    }
	friend std::ostream& operator<<(std::ostream& os, const CPolynomial& x) {
        bool first = true;
        if (x.i.size() == 0) {
            os << 0;
            return os;
        }
        for (auto idx = x.i.begin(); idx != x.i.end(); ++idx) {

            if (x.p[*idx] < 0) os << "- ";
            else if (!first) os << "+ ";

            if (x.p[*idx] != 1 && x.p[*idx] != -1 && *idx != 0) {
            if (x.p[*idx] < 0 && x.p[*idx] != -1) os << -x.p[*idx] << "*";
            else if (x.p[*idx] > 0 && x.p[*idx] != 1) os << x.p[*idx] << "*";
            }
            else {
                if (x.p[*idx] < 0 && x.p[*idx] != -1) os << -x.p[*idx];
                else if (x.p[*idx] > 0 && x.p[*idx] != 1) os << x.p[*idx];
            }

            if(*idx != 0) os << "x^" << *idx;
            if(idx != x.i.end()-1) os << " ";
            first = false;
        }
        
        
		return os;
	}
    friend bool dumpMatch ( const CPolynomial & x, const std::vector<double> & ref );

    explicit operator bool() const{
		if (i.size() == 0) return false;
        for (size_t it = 0; it < i.size(); it++) {
            if (p[i[it]] != 0) return true;
        }
        return false;
	}

    bool operator!() const {
        return !static_cast<bool>(*this);
    }

    int degree() const {
        return i.empty() ? 0 : *i.begin();
    }

  private:
    std::vector<double> p;
    std::vector<int> i;
};


#ifndef __PROGTEST__
bool smallDiff(double a, double b)
{
    if(a == b)
        return true;
    double maxVal = std::max(std::fabs(a), std::fabs(b));
    if(maxVal < 1e-12) 
        return std::fabs(a - b) < 1e-6;
    return (std::fabs(a - b) / maxVal) < 1e-6;
}

bool dumpMatch ( const CPolynomial & x,
                 const std::vector<double> & ref ) 
{
return x.p==ref;
}

int main ()
{
  CPolynomial a, b, c;
  std::ostringstream out, tmp;

  a[0] = -10;
  a[1] = 3.5;
  a[3] = 1;
  assert ( smallDiff ( a ( 2 ), 5 ) );
out . str ("");
  out << a;
  assert ( out . str () == "x^3 + 3.5*x^1 - 10" );
  c = -2 * a;
   assert ( c . degree () == 3
           && dumpMatch ( c, std::vector<double>{ 20.0, -7.0, -0.0, -2.0 } ) );

  out . str ("");
  out << c;
  assert ( out . str () == "- 2*x^3 - 7*x^1 + 20" );
  out . str ("");
  out << b;
  assert ( out . str () == "0" );
  b[5] = -1;
  b[2] = 3;
  out . str ("");
  out << b;
  assert ( out . str () == "- x^5 + 3*x^2" );
  c = a * b;
  assert ( c . degree () == 8
          && dumpMatch ( c, std::vector<double>{ -0.0, -0.0, -30.0, 10.5, -0.0, 13.0, -3.5, 0.0, -1.0 } ) );

  out . str ("");
  out << c;
  //std::cout << c << std::endl;
  assert ( out . str () == "- x^8 - 3.5*x^6 + 13*x^5 + 10.5*x^3 - 30*x^2" );
  a *= 5;
  assert ( a . degree () == 3
           && dumpMatch ( a, std::vector<double>{ -50.0, 17.5, 0.0, 5.0 } ) );

  a *= b;
  assert ( a . degree () == 8
           && dumpMatch ( a, std::vector<double>{ 0.0, 0.0, -150.0, 52.5, -0.0, 65.0, -17.5, -0.0, -5.0 } ) );

  assert ( a != b );
  b[5] = 0;
  assert ( static_cast<bool> ( b ) );
  assert ( ! ! b );
  b[2] = 0;
  assert ( !(a == b) );
//   std::cout << a << std::endl; 
//   std::cout << a.degree() << std::endl; 
//   a.print_vector();
  a *= 0;
//   std::cout << "-------------" << std::endl;
// //   a.print_vector();
//   std::cout << a << std::endl;
//     std::cout << a.degree() << std::endl;

  assert ( a . degree () == 0);
           //&& dumpMatch ( a, std::vector<double>{ 0.0 } ) );

  assert ( a == b );
  assert ( ! static_cast<bool> ( b ) );
  assert ( ! b );

//   // bonus - manipulators

//   out . str ("");
//   out << poly_var ( "y" ) << c;
//   assert ( out . str () == "- y^8 - 3.5*y^6 + 13*y^5 + 10.5*y^3 - 30*y^2" );
//   out . str ("");
//   tmp << poly_var ( "abc" );
//   out . copyfmt ( tmp );
//   out << c;
//   assert ( out . str () == "- abc^8 - 3.5*abc^6 + 13*abc^5 + 10.5*abc^3 - 30*abc^2" );
  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
