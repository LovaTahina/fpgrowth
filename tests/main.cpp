#include <iostream>
#include <string>
#include "../lib/fptree_headeronly.h"

using namespace std;

template<typename T>
void printFPTree(Node<T>* p_root)
{
    cout  << endl << "--------- FP-Tree ---------------" << endl;

    function<void(Node<T> *, int)> fctPrint;
    fctPrint = [&fctPrint](Node<T> *p_actualNode, int p_level)
    {
        string tab;
        for(int l=0; l<p_level; ++l)
        {
            tab += "  ";
        }

        cout << tab << '-' << p_actualNode->_itemValue << " (freq " << p_actualNode->_freq << ")" << endl;
        if(p_actualNode && !p_actualNode->_children.empty())
        {
            ++p_level;
            for(Node<T>* node : p_actualNode->_children)
            {
                fctPrint(node, p_level);
            }
        }
    };

    fctPrint(p_root, 0);
}

template<typename T>
void printConditionalPattern(Node<T>* p_root, ItemSupport<T>& p_headItemSupport)
{
    cout  << endl << "--------- Conditional Pattern (p-Conditional) ---------------" << endl;

    for(Node<T> const& node: p_headItemSupport.getItemList())
    {
        if(ItemSupport<T>::_minSup)
        {
            cout << endl << node._itemValue << ": ";
        }

        for(Node<T>* link: node._links)
        {
            Node<T>* parent = link->_parent;
            while(parent && parent != p_root)
            {
                cout << parent->_itemValue;
                parent = parent->_parent;
            }
            cout << ":" << link->_freq << " ";
        }
    }
}

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

    printFPTree<T>(fptree.root());
    printConditionalPattern<T>(fptree.root(), fptree.headItemSupport());
}

void test1()
{
   cout  << endl << "\t================ [ Test 1. With char database ] ================" << endl;

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
   cout  << endl << "\t================ [ Test 2. With string database ] ================" << endl;

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
