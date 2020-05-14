#include <ctime>
#include <string>
#include <iostream>

#include <bits/stdc++.h> 

#include "doudizhu_game.h"
#include "doudizhu_card.h"

#include <Eigen/Dense>
using namespace Eigen;
using namespace std;

using namespace caishen::doudizhu;

void Combi(char a[], int reqLen, int s, int currLen, bool check[], int l)
{
   if(currLen > reqLen)
   return;
   else if (currLen == reqLen) {
      cout<<"\t";
      for (int i = 0; i < l; i++) {
         if (check[i] == true) {
            cout<<a[i]<<" ";
         }
      }
      cout<<"\n";
      return;
   }
   if (s == l) {
      return;
   }
   check[s] = true;
   Combi(a, reqLen, s + 1, currLen + 1, check, l);
   check[s] = false;
   Combi(a, reqLen, s + 1, currLen, check, l);
}

void comb(vector<int> & a, vector<int> o, int n, int r, int s)
{
  if (r == 0)
  {
    copy(o.begin(), o.end(), ostream_iterator<int>(cout, " "));
    cout << endl;
    return;
  }

  if (n - s >= r)
  {
    o.push_back(a[s]);
    comb(a, o, n, r - 1, s + 1);
    o.pop_back();

    int i;
    for (i = s + 1; i < n; i++)
    {
      if (a[i] != a[s]) break;
    }
    if (i != n)
    {
      comb(a, o, n, r, i);
    }
  }
}

int main(int argc, char** argv) {

  std::vector<int> a({0, 0, 1, 2, 2});
  vector<int> o;

  comb(a, o, 5, 2, 0);

  return 0;
}


