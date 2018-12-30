#include <iostream>
#include <string>
#include "fptree_headeronly.h"

using namespace std;

template<typename T>
void test(vector<vector<T>>& DB, T p_root, int p_minSup)
{
    ItemSupport<T> itemsupport(p_minSup);
    using Transactions = vector<Transaction<T>>;

    Transactions tr;
    for(vector<T> v: DB)
    {
        tr.push_back(v);

        for(T c:v)
        {
            itemsupport << c;
        }
    }

    cout << endl << "--------- itemsupport ---------------" << endl;
    cout << itemsupport;
    cout << endl;

    T root = p_root;
    FP_Tree<T> fptree(itemsupport, root);
    for(Transaction<T>& item : tr)
    {
        fptree.construct(item);
    }

    fptree.print();
}

void test1()
{
   cout  << endl << "\t================ [ Test with char database ] ================" << endl;

   vector<vector<char>> DB = {
       {'f','a','c','d','g','i','m','p'},
       {'a','b','c','f','l','m','o'},
       {'b','f','h','j','o'},
       {'b','c','k','s','p'},
       {'a','f','c','e','l','p','m','n'}
   };

   test<char>(DB, '*', 3);
}

void test2()
{
   cout  << endl << "\t================ [ Test with string database ] ================" << endl;

   vector<vector<string>> DB = {
       {"I1", "I2", "I5"},
       {"I2", "I4"},
       {"I2", "I3"},
       {"I1", "I2", "I4"},
       {"I1", "I3"},
       {"I2", "I3"},
       {"I1", "I3"},
       {"I1", "I2", "I3", "I5"},
       {"I1", "I2", "I3"}
   };

   test<string>(DB, "*", 1);
}

int main()
{
    test1();
    test2();

    return 0;
}
