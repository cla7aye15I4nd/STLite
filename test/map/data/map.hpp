#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP
// only for std::less<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

template<
	class Key,
	class T,
	class Compare = std::less<Key>
> class map {
public:
	/**
	 * the internal type of data.
	 * it should have a default constructor, a copy constructor.
	 * You can use sjtu::map as value_type by typedef.
	 */
	typedef pair<const Key, T> value_type;
private:
	struct node{
        value_type inf;
        bool color;               //true for red and false for black
        node *left,*right,*parent;//right side is greater and left is smaller

        node():inf(),color(true),left(nullptr),right(nullptr),parent(nullptr){}

        node(Key k,T v,node *p):inf(value_type(k,v)),color(true),
		left(nullptr),right(nullptr),parent(p){}

		node(const value_type &value,node *p):inf(value),color(true),
		left(nullptr),right(nullptr),parent(p){}

	    node(node *n,node *p):inf(value_type(n->inf)),color(n->color){
            if (n->left != nullptr) left = new node(n->left,this);
            else left = nullptr;
            if (n->right != nullptr) right = new node(n->right,this);
            else right = nullptr;
            parent = p;
        }
        ~node(){}
    };
    node *root;
	size_t n;
	node *tail;
	Compare com;

    bool RBjudge(node *n){
        if (n == nullptr) return false;
        return n->color;
    }								//0 for black and nullptr, and 1 for red 
    void leftRotate(node *temp_root){
		if (temp_root == root) root = temp_root->right;
        node *p = temp_root->parent,*y = temp_root->right;
        node *beta = y->left;
        y->parent = p;y->left = temp_root;
        temp_root->right = beta;temp_root->parent = y;
        if (beta != nullptr) beta->parent = temp_root;
        if (p != nullptr)
            if (p->left == temp_root) p->left = y;
            else p->right = y;
    }
    void rightRotate(node *temp_root){
		if (temp_root == root) root = temp_root->left;
        node *p = temp_root->parent,*x = temp_root->left;
        node *beta = x->right;
        x->parent = p; x->right = temp_root;
        temp_root->parent = x;temp_root->left = beta;
        if (beta != nullptr) beta->parent = temp_root;
        if (p != nullptr)
            if (p->left == temp_root) p->left = x;
            else p->right = x;
	}
    void clean(node *temp_root){
		if (temp_root == nullptr) return;
        if (temp_root->left != nullptr) clean(temp_root->left);
        if (temp_root->right != nullptr) clean(temp_root->right);
        delete temp_root;
		n = 0;
    }
	node *search_for(const Key &key) const {
		node *temp = root;
		while(temp != nullptr){
			if (com((temp->inf).first, key)) temp = temp->right;
			else if (com(key, (temp->inf).first)) temp = temp->left;
			else break;
		}
		return temp;
	}
	node *find_next(node *it) const {
		if (it == tail) throw(index_out_of_bound());
		if (it->right != nullptr){
			it = it->right;
			while(it->left != nullptr) it = it->left;
			return it;
		}
		while(it->parent != nullptr){
			if (it->parent->left == it) return it->parent;
			it = it->parent;
		}
		return tail;
	}
	node *find_prior(node *it) const{
		if (it == tail){
			it = root;
			if (it == nullptr) throw(runtime_error());
			while (it->right != nullptr) it = it->right;
			return it;
		}
		if (it->left != nullptr){
			it = it->left;
			while (it -> right != nullptr) it = it->right;
			return it;
		}
		while(it->parent != nullptr){
			if (it->parent->right == it) return it->parent;
			it = it->parent;
		}
	}
public:
	/**
	 * see BidirectionalIterator at CppReference for help.
	 *
	 * if there is anything wrong throw invalid_iterator.
	 *     like it = map.begin(); --it;
	 *       or it = map.end(); ++end();
	 */
	class const_iterator;
	class iterator {
	friend map;
	friend const_iterator;
	private:
		node *it;
		map *th;
		iterator(node *where,map *t):it(where),th(t){}
	public:
		iterator():it(nullptr),th(nullptr) {}
		iterator(const iterator &other):it(other.it),th(other.th){}
		/**
		 * return a new iterator which pointer n-next elements
		 *   even if there are not enough elements, just return the answer.
		 * as well as operator-
		 */
		iterator operator++(int) {
			if (it == th->tail) throw(index_out_of_bound());
			node *temp = it;
			it = th->find_next(it);
			return iterator(temp,th);
		}
		iterator & operator++() {
			if (it == th->tail) throw(index_out_of_bound());
			it = th->find_next(it);
			return *this;
		}
		iterator operator--(int) {
			node *temp = it;
			it = th->find_prior(it);
			return iterator(temp,th);
		}
		iterator & operator--() {
			it = th->find_prior(it);
			return *this;
		}
		
		value_type & operator*() const {
			return it->inf;
		}
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory).
		 */
		bool operator==(const iterator &rhs) const {
			return it == rhs.it;
		}
		bool operator==(const const_iterator &rhs) const {
			return it == rhs.it;
		}
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const {
			return it != rhs.it;
		}
		bool operator!=(const const_iterator &rhs) const {
			return it != rhs.it;
		}

		/**
		 * for the support of it->first. 
		 * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
		 */
		value_type* operator->() const noexcept {
			return &(it->inf);
		}
	};
	class const_iterator {
	friend iterator;
	friend map;
	private:
		node *it;
		const map *th;
		const_iterator(node *where,const map *t):it(where),th(t){}
	public:
		const_iterator():it(nullptr){}
		const_iterator(const const_iterator &other):it(other.it),th(other.th){}
		const_iterator(const iterator &other):it(other.it),th(other.th){}
		const_iterator operator++(int) {
			if (it == th->tail) throw(index_out_of_bound());
			node *temp = it;
			it = th->find_next(it);
			return const_iterator(temp,th);
		}
		const_iterator & operator++() {
			if (it == th->tail) throw(index_out_of_bound());
			it = th->find_next(it);
			return *this;
		}
		const_iterator operator--(int) {
			node *temp = it;
			it = th->find_prior(it);
			return const_iterator(temp,th);
		}
		const_iterator & operator--() {
			it = th->find_prior(it);
			return *this;
		}
		const value_type & operator*() const{
			return it->inf;
		}
		bool operator==(const iterator &rhs) const {
			return it == rhs.it;
		}
		bool operator==(const const_iterator &rhs) const {
			return it == rhs.it;
		}
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const {
			return it != rhs.it;
		}
		bool operator!=(const const_iterator &rhs) const {
			return it != rhs.it;
		}
		value_type* operator->() const noexcept {
			return &(it->inf);
		}
	};
	map() {
		tail = (node *)operator new (sizeof(node));
		root = nullptr;
		n = 0;
	}
	map(const map &other):n(other.n) {
		tail = (node *)operator new (sizeof(node));
		node *temp;
		if (other.root != nullptr)
			temp = new node(other.root,nullptr);
		else temp = nullptr;
		root = temp;
        temp = nullptr;
	}
	map & operator=(const map &other){
		n = other.n;
		if (other.root == nullptr) root = nullptr;
		else root = new node(other.root,nullptr);
		tail = (node *)operator new (sizeof(node));
	}
	~map() {
		clear();
		operator delete[]((void *)tail,sizeof(node));
	}
	/**
	 * access specified element with bounds checking
	 * Returns a reference to the mapped value of the element with key equivalent to key.
	 * If no such element exists, an exception of type `index_out_of_bound'
	 */
	T & at(const Key &key) {
		node *temp = search_for(key);
		if (temp == nullptr) throw(index_out_of_bound());
		return (temp->inf).second;
	}
	const T & at(const Key &key) const {
		node *temp = search_for(key);
		if (temp == nullptr) throw(index_out_of_bound());
		return (temp->inf).second;
	}
	/**
	 * access specified element 
	 * Returns a reference to the value that is mapped to a key equivalent to key,
	 *   performing an insertion if such key does not already exist.
	 */
	T & operator[](const Key &key) {
		node *temp = search_for(key);
		if (temp == nullptr) return (insert(value_type(key,T())).first)->second;
		return (temp->inf).second;
	}
	/**
	 * behave like at() throw index_out_of_bound if such key does not exist.
	 */
	const T & operator[](const Key &key) const {
		node *temp = search_for(key);
		if (temp == nullptr) throw(index_out_of_bound());
		return (temp->inf).second;
	}
	/**
	 * return a iterator to the beginning
	 */
	iterator begin() {
		if (root == nullptr) return iterator(tail,this);
		node *temp = root;
		while (temp->left != nullptr) temp = temp->left;
		return iterator(temp,this);
	}
	const_iterator cbegin() const {
		if (root == nullptr) return const_iterator(tail,this);
		node *temp = root;
		while (temp->left != nullptr) temp = temp->left;
		return const_iterator(temp,this);
	}
	/**
	 * return a iterator to the end
	 * in fact, it returns past-the-end.
	 */
	iterator end() {
		return iterator(tail,this);
	}
	const_iterator cend() const {
		return const_iterator(tail,this);
	}
	/**
	 * checks whether the container is empty
	 * return true if empty, otherwise false.
	 */
	bool empty() const {
		return root == nullptr;
	}
	/**
	 * returns the number of elements.
	 */
	size_t size() const {
		return n;
	}
	/**
	 * clears the contents
	 */
	void clear() {
		clean(root);
		root = nullptr;
	}
	/**
	 * insert an element.
	 * return a pair, the first of the pair is
	 *   the iterator to the new element (or the element that prevented the insertion), 
	 *   the second one is true if insert successfully, or false.
	 */
	pair<iterator, bool> insert(const value_type &value) {
		Key k = value.first;
		T v = value.second;
		if (root == nullptr){
			root = new node(value,nullptr);
			root->color = 0;
			n++;
			return pair<iterator, bool>(iterator(root,this),true);
		}
		node *now = root;
		while(now != nullptr){
			if (com(now->inf.first,k))
				if (now->right == nullptr) break;
				else now = now->right;
			else if (com(k,(now->inf).first))
				if (now->left == nullptr) break;
				else now = now->left;
			else return pair<iterator, bool>(iterator(now,this),false);
		}
		n++;
		node *temp,*record;
		if ( com((now->inf).first, k) ){
			record = now->right = new node(k,v,now);
			temp = now->right;
		}
		else{
			record = now->left = new node(k,v,now);
			temp = now->left;
		}
		if (!now->color)
			return pair<iterator, bool>(iterator(record,this),true);
		//rebalance
		node *grandp, *par, *unc;
		while (true){
		//temp and temp's parent are all red(1),
		//but all black heights are right.
			par = temp->parent;
			if (par->parent != nullptr){
				grandp = par->parent;
				if (grandp->left == par) unc = grandp->right;
				else unc = grandp->left;
				bool condition;
				condition = RBjudge(unc);
				if (condition){
				//par and its uncle are red(and thus grandp must be black)
					par->color = unc->color = 0;
					grandp->color = 1;
					temp = grandp;
					if (grandp->parent == nullptr){
						grandp->color = 0;break;
					}
					if (grandp->parent->color) continue;
					else break;
				}
				else{
				//par is red but uncle is black(grandp must be black)
					bool lr = 0;
					if (grandp->right == par) lr = 1;
					if (!lr && temp == par->right){
						leftRotate(par);
						temp = par;
						par = temp->parent;
					}
					else if (lr && temp == par->left){
						rightRotate(par);
						temp = par;
						par = temp->parent;
					}
					par->color = 0;
					grandp->color = 1;
					if (!lr) rightRotate(grandp);
					else leftRotate(grandp);
					break;
				}
			}
			else{
				par->color = 0;
				break;
			}
		}
		return pair<iterator, bool>(iterator(record,this),true);
	}
	/**
	 * erase the element at pos.
	 *
	 * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
	 */
	void erase(iterator pos) {
		if (pos.th != this) throw(invalid_iterator());
		if (pos == this->end()) throw(invalid_iterator());
		if (root == nullptr) throw(container_is_empty());
		n--;
		node *now = root;
		Key k = pos->first;
		while(now != nullptr){
			if (com(k,now->inf.first))
				if (now -> left != nullptr) now = now->left;
				else throw(invalid_iterator());
			else if (com(now->inf.first, k))
				if (now -> right != nullptr) now = now->right;
				else throw(invalid_iterator());
			else break;
		}
		//look for the real deleted node now
		node *d = now;
		bool LR = 0;		//1 for rightmin(rllll) and 0 otherwise(lrrrr)
		if (now->left == nullptr)
			if (now->right == nullptr){}
			else{
				LR = 1;
				d = d->right;
				while(d->left != nullptr) d = d->left;
			}
		else{
			d = d->left;
			while(d->right != nullptr) d = d->right;
		}
		//now move d to now and for some reason, k is const Key so some improvement is needed
		node *p;
		node *dlr, *s;
		bool delr = 0;
		if (now != d){
			now->inf.~value_type();
			new(&(now->inf)) value_type(d->inf);
		}
		p = d->parent;
		
		//concerning on the real deleted node d, "now" is useless now
		//mention that d has at most one child
		if (p == nullptr){
		//when d is the root
			if (d->left != nullptr){
				root = d->left;
				d->left->parent = nullptr;
			}
			else if (d->right != nullptr){
				root = d->right;
				d->right->parent = nullptr;
			}
			else root = nullptr;
			delete d;
			return;
		}
		delr = 0;
		if (p->right == d) delr = 1;//d is the right child
		if (d->color){
		//d is red and thus it cannot have any child.
		//(notice that d has at most one child and d is red)
			if (delr) p->right = nullptr;
			else p->left = nullptr;
			delete d;
			return;
		}
		//now d is black(0) and try to delete it
		if (LR) dlr = d->right;
		else dlr = d->left;
		if (dlr != nullptr){
		//dlr cannot be black(as d only has one child dlr)
			if (delr) p->right = dlr;
			else p->left = dlr;
			dlr->parent = p;
			dlr->color = 0;
			delete d;
			return;
		}
		//now all d's children are nullptr and consider d's brother s
		if (delr){
			s = p->left;        //delr = 1, then d is the right one
			p->right = nullptr;
		}
		else {
			s = p->right;
			p->left = nullptr;
		}
		//d is removed and notice that s cannot be nullptr.Now rebalance it
		delete d;
		bool needRepeat = 1;
		/*
		The situation now is abstract.
		s has 1 more black-height than its brother(so it cannot be nullptr), 
		being the son of p when every repeatation starts.
		delr shows s is which son of p when every repeatation starts.
		delr is 1 when s is the left-son being rightside otherwise.
		*/
		while(needRepeat){
			needRepeat = 0;
			node *sl = s->left, *sr = s->right;
			if (s->color){
				p->color = 1;s->color = 0;
				if (!delr){
					leftRotate(p);
					s = sl;
				}
				else{
					rightRotate(p);
					s = sr;
				}
				sl = s->left;
				sr = s->right;
			}
		//and this comes to one of the following conditions, where s is black
			if (RBjudge(sl)){
				if (!delr){
					rightRotate(s);
					sl->color = p->color;
					p->color = 0;
					leftRotate(p);
				}
				else{
					s->color = p->color;
					p->color = sl->color = 0;
					rightRotate(p);
				}
				break;
			}
			if (RBjudge(sr)){
				if (delr){
					leftRotate(s);
					sr->color = p->color;
					p->color = 0;
					rightRotate(p);
				}
				else{
					s->color = p->color;
					p->color = sr->color = 0;
					leftRotate(p);
				}
				break;
			}
			if (RBjudge(p)){
				p->color = 0;
				s->color = 1;
				break;
			}
			s->color = 1;
			if (p->parent == nullptr) break;
			needRepeat = 1;
			if (p->parent->left == p){
				delr = 0;
				s = p->parent->right;
			}
			else{
				delr = 1;
				s = p->parent->left;
			}
			p = p->parent;
			sl = s->left;
			sr = s->right;
		}
	}
	/**
	 * Returns the number of elements with key 
	 *   that compares equivalent to the specified argument,
	 *   which is either 1 or 0 
	 *     since this container does not allow duplicates.
	 * The default method of check the equivalence is !(a < b || b > a)
	 */
	size_t count(const Key &key) const {
		node *temp = root;
		while(temp != nullptr){
			if (com((temp->inf).first, key)) temp = temp->right;
			else if (com(key, (temp->inf).first)) temp = temp->left;
			else break;
		}
		if (temp == nullptr) return 0;
		return 1;
	}
	/**
	 * Finds an element with key equivalent to key.
	 * key value of the element to search for.
	 * Iterator to an element with key equivalent to key.
	 *   If no such element is found, past-the-end (see end()) iterator is returned.
	 */
	iterator find(const Key &key) {
		node *temp = search_for(key);
		if (temp == nullptr) return iterator(tail,this);
		return iterator(temp,this);
	}
	const_iterator find(const Key &key) const {
		node *temp = search_for(key);
		if (temp == nullptr) return const_iterator(tail,this);
		return const_iterator(temp,this);
	}
};

}
#endif