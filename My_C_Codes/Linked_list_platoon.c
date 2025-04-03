#ifndef __PROGTEST__
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct TSoldier
{
  struct TSoldier   * m_Next;
  int                 m_PersonalID;
  char                m_SecretRecord[64];
} TSOLDIER;

#endif /* __PROGTEST__ */

TSOLDIER         * mergePlatoons     ( TSOLDIER        * p1,
                                       TSOLDIER        * p2 )
{
  TSOLDIER* mergedPlatoon = NULL;
  TSOLDIER* current = NULL;

  if (p1 && p2) {
    mergedPlatoon = p1;
    p1 = p1->m_Next;
    current = mergedPlatoon;
    
    current->m_Next = p2;
    p2 = p2->m_Next;
    
    current = current->m_Next;

  }
  else if (p1) {
    mergedPlatoon = p1;
    p1 = p1->m_Next;
    current = mergedPlatoon;
  }
  else if (p2) {
    mergedPlatoon = p2;
    p2 = p2->m_Next;
    current = mergedPlatoon;
  }
  else return NULL;

  while (p1 || p2) {
    if (p1) {
      current->m_Next = p1;
      current = current->m_Next;
      p1=p1->m_Next;
    }

    if (p2) {
      current->m_Next = p2;
      current = current->m_Next;
      p2 = p2->m_Next;
    }
  }

  return mergedPlatoon;
}


void               splitPlatoon      ( TSOLDIER        * src, 
                                       TSOLDIER       ** p1,
                                       TSOLDIER       ** p2 )
{
  *p1 = NULL;
  *p2 = NULL;
  if (!src) return;

  int Len = 0;
  TSOLDIER* current = src;
  while ( current) {
    current = current->m_Next;
    Len++;
  }

  if(Len == 1) return; 

  *p1 = src;
  TSOLDIER* currentNode = src;
  for(int i = 0; i < Len/2 - 1; i++) {
    currentNode = currentNode->m_Next;
  }

    *p2 = currentNode->m_Next;
  currentNode->m_Next = NULL;
  if (Len % 2 != 0) {
    currentNode = *p2;
    for (int i = 0; i < Len / 2 - 1; i++) {
      currentNode = currentNode->m_Next;
    }
    free(currentNode->m_Next);
    currentNode->m_Next = NULL;
  }
  
}

void               destroyPlatoon    ( TSOLDIER        * src )
{
    TSOLDIER* currentNode = src;
    TSOLDIER* temp;
while (currentNode != NULL) {
    temp = currentNode;
    currentNode = currentNode->m_Next;
    free(temp);
}
}
#ifndef __PROGTEST__
TSOLDIER         * createSoldier     ( int               id,
                                       TSOLDIER        * next )
{
  TSOLDIER * r = (TSOLDIER *) malloc ( sizeof ( *r ) );
  r -> m_PersonalID = id;
  r -> m_Next = next;
  /* r -> m_SecretRecord will be filled by someone with a higher security clearance */
  return r;  
}
int main ( void )
{
  TSOLDIER * a, *b, *c;
  a = createSoldier ( 0,
        createSoldier ( 1,
          createSoldier ( 2,
            createSoldier ( 3,
              createSoldier ( 4, NULL )))));
  b = createSoldier ( 10,
        createSoldier ( 11,
          createSoldier ( 12,
            createSoldier ( 13,
              createSoldier ( 14, NULL )))));
  c = mergePlatoons ( a, b );
  assert ( c != NULL
           && c -> m_PersonalID == 0
           && c -> m_Next != NULL
           && c -> m_Next -> m_PersonalID == 10
           && c -> m_Next -> m_Next != NULL
           && c -> m_Next -> m_Next -> m_PersonalID == 1
           && c -> m_Next -> m_Next -> m_Next != NULL
           && c -> m_Next -> m_Next -> m_Next -> m_PersonalID == 11
           && c -> m_Next -> m_Next -> m_Next -> m_Next != NULL
           && c -> m_Next -> m_Next -> m_Next -> m_Next -> m_PersonalID == 2
           && c -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next != NULL
           && c -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_PersonalID == 12
           && c -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next != NULL
           && c -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_PersonalID == 3
           && c -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next != NULL
           && c -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_PersonalID == 13
           && c -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next != NULL
           && c -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_PersonalID == 4
           && c -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next != NULL
           && c -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_PersonalID == 14
           && c -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next == NULL );
  splitPlatoon ( c, &a, &b );
  assert ( a != NULL
           && a -> m_PersonalID == 0
           && a -> m_Next != NULL
           && a -> m_Next -> m_PersonalID == 10
           && a -> m_Next -> m_Next != NULL
           && a -> m_Next -> m_Next -> m_PersonalID == 1
           && a -> m_Next -> m_Next -> m_Next != NULL
           && a -> m_Next -> m_Next -> m_Next -> m_PersonalID == 11
           && a -> m_Next -> m_Next -> m_Next -> m_Next != NULL
           && a -> m_Next -> m_Next -> m_Next -> m_Next -> m_PersonalID == 2
           && a -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next == NULL );
  assert ( b != NULL
           && b -> m_PersonalID == 12
           && b -> m_Next != NULL
           && b -> m_Next -> m_PersonalID == 3
           && b -> m_Next -> m_Next != NULL
           && b -> m_Next -> m_Next -> m_PersonalID == 13
           && b -> m_Next -> m_Next -> m_Next != NULL
           && b -> m_Next -> m_Next -> m_Next -> m_PersonalID == 4
           && b -> m_Next -> m_Next -> m_Next -> m_Next != NULL
           && b -> m_Next -> m_Next -> m_Next -> m_Next -> m_PersonalID == 14
           && b -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next == NULL );
  destroyPlatoon ( a );
  destroyPlatoon ( b );

  a = createSoldier ( 0,
        createSoldier ( 1,
          createSoldier ( 2, NULL )));
  b = createSoldier ( 10,
        createSoldier ( 11,
          createSoldier ( 12,
            createSoldier ( 13,
              createSoldier ( 14, NULL )))));
  c = mergePlatoons ( a, b );
  assert ( c != NULL
           && c -> m_PersonalID == 0
           && c -> m_Next != NULL
           && c -> m_Next -> m_PersonalID == 10
           && c -> m_Next -> m_Next != NULL
           && c -> m_Next -> m_Next -> m_PersonalID == 1
           && c -> m_Next -> m_Next -> m_Next != NULL
           && c -> m_Next -> m_Next -> m_Next -> m_PersonalID == 11
           && c -> m_Next -> m_Next -> m_Next -> m_Next != NULL
           && c -> m_Next -> m_Next -> m_Next -> m_Next -> m_PersonalID == 2
           && c -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next != NULL
           && c -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_PersonalID == 12
           && c -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next != NULL
           && c -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_PersonalID == 13
           && c -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next != NULL
           && c -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_PersonalID == 14
           && c -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next == NULL );
  splitPlatoon ( c, &a, &b );
  assert ( a != NULL
           && a -> m_PersonalID == 0
           && a -> m_Next != NULL
           && a -> m_Next -> m_PersonalID == 10
           && a -> m_Next -> m_Next != NULL
           && a -> m_Next -> m_Next -> m_PersonalID == 1
           && a -> m_Next -> m_Next -> m_Next != NULL
           && a -> m_Next -> m_Next -> m_Next -> m_PersonalID == 11
           && a -> m_Next -> m_Next -> m_Next -> m_Next == NULL );
  assert ( b != NULL
           && b -> m_PersonalID == 2
           && b -> m_Next != NULL
           && b -> m_Next -> m_PersonalID == 12
           && b -> m_Next -> m_Next != NULL
           && b -> m_Next -> m_Next -> m_PersonalID == 13
           && b -> m_Next -> m_Next -> m_Next != NULL
           && b -> m_Next -> m_Next -> m_Next -> m_PersonalID == 14
           && b -> m_Next -> m_Next -> m_Next -> m_Next == NULL );
  destroyPlatoon ( a );
  destroyPlatoon ( b );

  a = createSoldier ( 0,
        createSoldier ( 1,
          createSoldier ( 2, NULL )));
  b = createSoldier ( 10,
        createSoldier ( 11,
          createSoldier ( 12,
            createSoldier ( 13, NULL ))));
  c = mergePlatoons ( a, b );
  assert ( c != NULL
           && c -> m_PersonalID == 0
           && c -> m_Next != NULL
           && c -> m_Next -> m_PersonalID == 10
           && c -> m_Next -> m_Next != NULL
           && c -> m_Next -> m_Next -> m_PersonalID == 1
           && c -> m_Next -> m_Next -> m_Next != NULL
           && c -> m_Next -> m_Next -> m_Next -> m_PersonalID == 11
           && c -> m_Next -> m_Next -> m_Next -> m_Next != NULL
           && c -> m_Next -> m_Next -> m_Next -> m_Next -> m_PersonalID == 2
           && c -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next != NULL
           && c -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_PersonalID == 12
           && c -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next != NULL
           && c -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_PersonalID == 13
           && c -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next == NULL );
  splitPlatoon ( c, &a, &b );
  assert ( a != NULL
           && a -> m_PersonalID == 0
           && a -> m_Next != NULL
           && a -> m_Next -> m_PersonalID == 10
           && a -> m_Next -> m_Next != NULL
           && a -> m_Next -> m_Next -> m_PersonalID == 1
           && a -> m_Next -> m_Next -> m_Next == NULL );
  assert ( b != NULL
           && b -> m_PersonalID == 11
           && b -> m_Next != NULL
           && b -> m_Next -> m_PersonalID == 2
           && b -> m_Next -> m_Next != NULL
           && b -> m_Next -> m_Next -> m_PersonalID == 12
           && b -> m_Next -> m_Next -> m_Next == NULL );
  destroyPlatoon ( a );
  destroyPlatoon ( b );

  return 0;
}
#endif /* __PROGTEST__ */
