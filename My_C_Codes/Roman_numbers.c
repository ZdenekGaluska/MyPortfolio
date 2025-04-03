#ifndef __PROGTEST__
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <assert.h>
#endif  /* __PROGTEST__ */

char * ConvertToRoman (long number) { /*Konvertuje cislo na rimske*/
static char roman[20];  
    const char* romanNumerals[] = {
        "M", "CM", "D", "CD", "C", "XC", "L", "XL", "X", "IX", "V", "IV", "I" /*Soubor rimske cisel*/
    };
    int arabicValues[] = {1000, 900, 500, 400, 100, 90, 50, 40, 10, 9, 5, 4, 1}; /*Soubor arabskych cisel*/
    
    roman[0] = '\0'; 
    for (int j = 0; j < 13; j++) { /*for cyklus pro prevedeni cisla*/
        while (number >= arabicValues[j]) {
            strcat(roman, romanNumerals[j]);
            number -= arabicValues[j];
        }
    }
    return roman; /*vrati prevede cislo v rimske podobe*/
}

char * arabicToRoman ( const char * str ) {
  int len = strlen(str);
  int i = 0, j = 0;
  char *result = (char*)malloc(sizeof(char) * (len * 2 + 1)); /*dostatecne velka dynamicka alokace pro prevod*/

  while (str[i] != '\0') { /*while cyklus projizdejici cele pole */
      if ( isdigit(str[i]) && ( i == 0 ||  (isspace(str[i-1]) || ispunct(str[i-1])) ) /*pokud hodnota cislo jako prvni prvek v pilo,  nebo pred sebou ma bily znak nebo oddelovac*/
         ) {
        long number = 0;

        while ( i < len && isdigit(str[i]) ) { /*vypise cele cislo do hodnoty number*/
          number = 10 * number + (str[i] - '0');
          i++;
        }
      
        if (number > 0 && number < 4000 && ( str[i] == '\0' || (isspace(str[i]) || ispunct(str[i]) ) )) { /*hleda cislo od 1-3999 ktere je posledni prvek pole nebo za sebou ma oddelovac nebo bily znak*/
          char* roman = ConvertToRoman(number);

          while (*roman) { /*vypisuje prevedene cislo do pole result*/
            result[j++] = *roman++;
          }
        }

        else {
          sprintf(result + j, "%ld", number); /*vypisuje puvodni cislo nesplnujici podminky pro prevod*/
          j += strlen(result + j);
          
        }
      }
      else {
        result[j++] = str[i++]; /*opisuje ostatni hodnoty*/
      }
  }
result[j] = '\0'; /*pridava ukoncovaci znak na konec pole*/
return result;
}

#ifndef __PROGTEST__
int main ()
{
  char * r;

 r = arabicToRoman ( "CVUT FIT was founded on July 1-st 2009" );
  assert ( ! strcmp ( r, "CVUT FIT was founded on July I-st MMIX" ) );
  free ( r );

  r = arabicToRoman ( "PA1 is my favorite subject, rating 5 out of 5stars" );
  assert ( ! strcmp ( r, "PA1 is my favorite subject, rating V out of 5stars" ) );
  free ( r );

  r = arabicToRoman ( "The range of int data type is -2147483648 to 2147483647 inclusive." );
  assert ( ! strcmp ( r, "The range of int data type is -2147483648 to 2147483647 inclusive." ) );
  free ( r );

  r = arabicToRoman ( "There are 11 integers in closed interval 10-20" );
  assert ( ! strcmp ( r, "There are XI integers in closed interval X-XX" ) );
  free ( r );

  r = arabicToRoman ( "Chuck Norris and agent 007 are able to solve all Progtest homework problems on the 1-st try" );
  assert ( ! strcmp ( r, "Chuck Norris and agent VII are able to solve all Progtest homework problems on the I-st try" ) );
  free ( r );

  r = arabicToRoman ( "0" );
  assert ( ! strcmp ( r, "0" ) );
  free ( r );  


  return EXIT_SUCCESS;
}
#endif  /* __PROGTEST__ */
