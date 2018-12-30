#ifndef _document_term_matrix_fp_tree_h_
#define _document_term_matrix_fp_tree_h_

#include <iostream>
#include <vector>
#include <set>
#include <list>
#include <functional>
#include <algorithm>

using namespace std;

template<typename T>
using Transaction = vector<T>;

/*!
 * \struct Node
 * \details The Node representation
 */
template<typename T>
struct Node
{
	T _itemValue;
	
    int _order{0};
    int _freq{0};
    Node *_parent{nullptr};
    list<Node *> _children;
    list<Node *> _links;

    explicit Node(T const& p_value, int p_order = 0):_itemValue(p_value), _order(p_order)
	{
		++_freq;
        //cout << " + " << _itemValue << " (" << _order << ")" << endl;
	}

    bool operator ==(Node const& p_node) const
	{
        return _itemValue == p_node._itemValue;
	}		
};


/*!
 * \struct Comparator
 * \details A helper to allow comparison of 2 nodes
 */
template<typename T>
struct Comparator
{
    bool operator()(Node<T> const& p_left, Node<T> const& p_right)
	{
        return (p_left._freq > p_right._freq) ||
        (p_left._freq == p_right._freq && p_left._order < p_right._order);
	}
};


/*!
 * \typedef Itemset
 * \details To define an itemset type
 */
template<typename T>
using ItemSet = vector<Node<T>>;


/*!
 * \typedef OrderedItems
 * \details To define an ordered unique items
 */
template<typename T>
using OrderedItems  = std::set<Node<T>, Comparator<T>>;


/*!
 * \struct ItemSupport
 * \details The tree representation
 */
template<typename T>
struct ItemSupport
{
	
    explicit ItemSupport(int p_minSup){ItemSupport<T>::_minSup = p_minSup;}

	ItemSupport& operator<< (T const& p_itemValue)
	{
        static int order = 0;
        auto inode = find_if(_itemList.begin(), _itemList.end(), [&p_itemValue](Node<T> const& p_node)
        {
            return p_node._itemValue == p_itemValue;
        });

        if(inode == _itemList.end())
        {
            Node<T> node(p_itemValue, order);
            _itemList.push_back(node);
            cout << p_itemValue << " : order " << order << endl;
            ++order;
        }
		else
        {
            auto& node = (*inode);
            ++node._freq;
        }

		return *this;
	}
	
	friend ostream& operator<<( ostream& p_os, ItemSupport const& p_itemSupport)
	{
        OrderedItems<T> itemset = p_itemSupport.getFrequentItems();
		
        for(Node<T> node: itemset)
		{
            p_os << node._itemValue << ": support " << node._freq << ", order " << node._order << endl;
		}
		return p_os;
	}
	
    Node<T>* getItem(T const& p_itemValue)
	{
        auto inode = find_if(_itemList.begin(), _itemList.end(), [&p_itemValue](Node<T> const& p_node)
        {
            return p_node._itemValue == p_itemValue;
        });
        if(inode != _itemList.end())
		{
            Node<T>* node = const_cast<Node<T>*>(&(*inode));
            return node;
		}
		
		return nullptr;
	}
	

    static int getMinSup()
    {
        return _minSup;
    }

    const ItemSet<T>& getItemList() const
    {
        return _itemList;
    }

    static int _minSup;
private:

    ItemSet<T> _itemList;

    OrderedItems<T> getFrequentItems() const
	{
        OrderedItems<T> ordered;
        for(Node<T> const& node: _itemList)
		{
            if(node._freq >= ItemSupport<T>::_minSup)
                ordered.insert(node);
		}
		
		return ordered;
	}
		
    OrderedItems<T> getUnfrequentItems() const
	{
        OrderedItems<T> ordered;
        for(Node<T> const& node: _itemList)
		{
            if(node._freq <= ItemSupport<T>::_minSup)
                ordered.insert(node);
		}
		
		return ordered;
	}
};

template<typename T>
int ItemSupport<T>::_minSup = 0;

template<typename T>
struct FP_Tree
{
    explicit FP_Tree(ItemSupport<T>& p_itemSupport, const T& p_rootValue = T()):_headItemSupport(p_itemSupport)
	{
        _root = new Node<T>(p_rootValue);
	}
	
    void construct( Transaction<T> const& p_itemValues)
	{
		// A. Order items into transaction
        OrderedItems<T> ordered;
        cout << "(transaction) ";
		for(T const& itemValue: p_itemValues)
		{
			cout << itemValue << " ";
            Node<T> *pNode = _headItemSupport.getItem(itemValue);
            if(pNode && pNode->_freq >= ItemSupport<T>::getMinSup())
            {
                ordered.insert((*pNode));
            }
		}
		cout << endl;

        cout << "(ordered) ";
        for(Node<T> const& node: ordered)
		{
            cout << node._itemValue << " ";
		}
		cout << endl;

		// B. Create FP_TREE
        Node<T>* actualNode = _root;
		bool here = true;
		string tab;
        for(Node<T> const& node: ordered)
		{
			tab += "\t-";
			
            auto it = actualNode->_children.begin();
			if(here)
			{
                it = find_if(actualNode->_children.begin(),
                actualNode->_children.end(), [&node](const Node<T>* nodeTmp){
                    return node == (*nodeTmp);
				});
				
                here &= it != actualNode->_children.end();
			}

			if(here)
			{
                actualNode = *it;
                ++actualNode->_freq;
            }
			else
			{
                Node<T>* pNode = new Node<T>(node._itemValue);
                actualNode->_children.push_back(pNode);

                pNode->_parent = actualNode;
                Node<T> *pNodeHead = _headItemSupport.getItem(node._itemValue);
                pNodeHead->_links.push_back(pNode);

                actualNode = pNode;
			}
			
            //cout  << tab << actualNode->_itemValue << "(" << actualNode->_freq << ")" << endl;
		}
		cout << endl;
	}

    void printFPTree()
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

        fctPrint(_root, 0);
    }

    void printConditionalPattern()
    {
        cout  << endl << "--------- Conditional Pattern (p-Conditional) ---------------" << endl;

        for(Node<T> const& node: _headItemSupport.getItemList())
        {
            if(ItemSupport<T>::_minSup)
            cout << endl << node._itemValue << ": ";

            for(Node<T>* link: node._links)
            {
                Node<T>* parent = link->_parent;
                while(parent && parent != _root)
                {
                    cout << parent->_itemValue;
                    parent = parent->_parent;
                }
                cout << ":" << link->_freq << " ";
            }
        }
    }

    void print()
    {
        printFPTree();

        printConditionalPattern();
    }

private:
    ItemSupport<T>& _headItemSupport;
    Node<T>* _root;
};

#endif

