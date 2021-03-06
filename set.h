//
// Created by Михаил Терентьев on 17/06/2018.
//

#ifndef SET_SET_H
#define SET_SET_H

#include <iostream>
#include <cassert>
template<typename T>
class set;

//typedef std::shared_ptr<Element> node_ptr;
//ROOT is NEUTRAL ELEMENT
template<typename T>
void swap(set<T> &frst, set<T> &sec) noexcept {
    using std::swap;
    swap(frst.root, sec.root);
    if (sec.root.l != nullptr) {
        sec.root.l->p = sec.root_ptr;
    }
    if (frst.root.l != nullptr) {
        frst.root.l->p = frst.root_ptr;
    }
}

template<typename T>
class set {

///////NODES///////
private:
    struct Neutral {
        Neutral *l = nullptr;

        Neutral *r = nullptr;

        Neutral *p;


        Neutral(Neutral *l_child, Neutral *r_child, Neutral *parent) : l(l_child),
                                                                       r(r_child),
                                                                       p(parent) {}

        Neutral() : l(nullptr),
                    r(nullptr),
                    p(nullptr) {}

        virtual ~Neutral() = default;

    };

    struct Element : public Neutral {

        T value;

        Element() = delete;

        ~Element() = default;

        Element(Neutral *l_child, Neutral *r_child, Neutral *parent, const T &_data) : Neutral(l_child,
                                                                                               r_child,
                                                                                               parent),
                                                                                       value(_data) {}

        friend void swap(Element *frst, Element *sec) {
            Element f_node(*frst);
            Element sec_node(*sec);
            if (sec_node.p != nullptr) {
                if (sec_node.p->l == sec) {
                    sec_node.p->l = frst;
                } else {
                    sec_node.p->r = frst;
                }
            }
            if (f_node.p != nullptr) {
                if (f_node.p->l == frst) {
                    f_node.p->l = sec;
                } else {
                    f_node.p->r = sec;
                }
            }
            if (f_node.r != nullptr)
                f_node.r->p = sec;

            if (sec_node.r != nullptr)
                sec_node.r->p = frst;

            if (f_node.l != nullptr)
                f_node.l->p = sec;

            if (sec_node.l != nullptr)
                sec_node.l->p = frst;

            std::swap(sec->p, frst->p);
            std::swap(sec->r, frst->r);
            std::swap(sec->l, frst->l);
        }
    };

    Neutral root;
    Neutral *root_ptr = &root;

public:
    friend void swap<T>(set<T> &frst, set<T> &sec) noexcept;

/////ITERATOR////
    template<typename Z>
    struct Set_iterator : public std::iterator<std::bidirectional_iterator_tag, Z> {
        friend struct set<T>;

        Set_iterator() = delete;

//LIKE FUNC_NEXT
        Set_iterator &operator++() {
            if (it_ptr->r != nullptr) {
                it_ptr = get_min(it_ptr->r);
                return *this;
            }
            Neutral *y = it_ptr->p;
            while (y != nullptr && it_ptr == y->r) {
                it_ptr = y;
                y = y->p;
            }
            it_ptr = y;
            return *this;
        }

        template<typename C>
        Set_iterator(const Set_iterator<C> &other)
                : it_ptr(other.it_ptr) {
        }

//LIKE FUNC PREV
        Set_iterator &operator--() {
            if (it_ptr->l != nullptr) {
                it_ptr = get_max(it_ptr->l);
                return *this;
            }
            Neutral *par_node = it_ptr->p;
            while (par_node != nullptr && it_ptr == par_node->l) {
                it_ptr = par_node;
                par_node = par_node->p;
            }
            it_ptr = par_node;
            return *this;
        }

        Set_iterator operator++(int) {
            Set_iterator tmp(*this);
            ++(*this);
            return tmp;
        }

        Set_iterator operator--(int) {
            Set_iterator tmp(*this);
            --(*this);
            return tmp;
        }

        Z const &operator*() const {
            return dynamic_cast<Element *>(it_ptr)->value;
        }

        Z const *operator->() const {
            return &(dynamic_cast<Element *>(it_ptr)->value);
        }

        bool operator==(Set_iterator const &other) const {
            return other.it_ptr == it_ptr;
        }

        bool operator!=(Set_iterator const &other) const {
            return other.it_ptr != it_ptr;
        }

        typedef std::ptrdiff_t difference_type;
        typedef Z value_type;
        typedef Z const *pointer;
        typedef Z const &reference;
        typedef std::bidirectional_iterator_tag iterator_category;

    private:
        explicit Set_iterator(Neutral *node) : it_ptr(node) {}

        Neutral *it_ptr;
    };

    typedef Set_iterator<T> iterator;
    typedef Set_iterator<const T> const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
//////////

public:
    friend void swap<T>(set &frst, set &sec) noexcept;

    set() noexcept {}

    set(set const &other) : set() {
        for (const auto &node : other) {
            insert(node);
        }
    }

    set &operator=(set const &other) {
        set temp(other);
        swap(*this, temp);
        return *this;
    }

    iterator erase_impl(Neutral *ersed_node) {
        iterator tmp_it(ersed_node);
        tmp_it++;
        if (ersed_node->r == nullptr) {
            if (ersed_node->p != nullptr) {
                if (ersed_node->p->r == ersed_node) {
                    ersed_node->p->r = ersed_node->l;
                } else {
                    ersed_node->p->l = ersed_node->l;
                }
            }
            if (ersed_node->l != nullptr) {
                ersed_node->l->p = ersed_node->p;
            }
            delete ersed_node;
        } else if (ersed_node->l == nullptr) {
            if (ersed_node->p != nullptr) {
                if (ersed_node->p->r == ersed_node) {
                    ersed_node->p->r = ersed_node->r;
                } else {
                    ersed_node->p->l = ersed_node->r;
                }
            }
            ersed_node->r->p = ersed_node->p;
            delete ersed_node;
        } else {
            Neutral *goer = ersed_node->l;
            while (goer->r != nullptr) {
                goer = goer->r;
            }
            swap(static_cast<Element *>(goer), static_cast<Element *>(ersed_node));
            erase_impl(ersed_node);
        }
        return tmp_it;
    }

    ~set() {
        clear();
    }

    iterator erase(const_iterator itror) {
        assert(itror != end());
        return erase_impl(static_cast<Element *>(itror.it_ptr));
    }

    void clear() noexcept {
        for (auto e = begin(); e != end();) {
            e = erase(e);
        }
    }

    bool empty() const {
        return root.l == nullptr;
    }

    std::pair<iterator, bool> insert(T const &x) {
        if (empty()) {
            root.l = new Element(nullptr, nullptr, root_ptr, x);
            return {
                    iterator(root.l), 1};
        }
        auto result = go_insert(dynamic_cast<Element *>(root.l), x);
        if (result.first == nullptr) {
            return {iterator(root_ptr), result.second};
        }
        return {iterator(result.first), result.second};
    }


    iterator find(T const &x) const {
        Element *result = search(dynamic_cast<Element *>(root.l), x);
        if (result == nullptr) {
            return iterator(root_ptr);
        }
        return iterator(result);
    }

    iterator lower_bound(T const &x) const {
        if (empty()) {
            return iterator(root_ptr);
        }
        Element *result = go_lowerBound(dynamic_cast<Element *>(root.l), x);
        if (result == nullptr) {
            return iterator(root_ptr);
        }
        return iterator(result);
    }

    iterator upper_bound(T const &x) const {
        if (empty()) {
            return iterator(root_ptr);
        }
        iterator result = find(x);
        if (result != end()) {
            return ++result;
        }
        return lower_bound(x);
    }

    iterator begin() {
        if (empty()) {
            return iterator(root_ptr);
        }
        Element *result = dynamic_cast<Element *>(get_min(root.l));
        return iterator(result);
    }

    const_iterator begin() const {
        if (empty()) {
            return const_iterator(root_ptr);
        }
        Element *result = dynamic_cast<Element *>(get_min(root.l));
        return const_iterator(result);
    }

    reverse_iterator rbegin() {
        return reverse_iterator(end());
    }

    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(end());
    }

    iterator end() {
        return iterator(root_ptr);
    }

    const_iterator end() const {
        return const_iterator(root_ptr);
    }

    reverse_iterator rend() {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rend() const {
        return const_reverse_iterator(begin());
    }

private:
    void deleteNode(Neutral *node) {
        if (node == nullptr) {
            return;
        }
        deleteNode(node->l);
        deleteNode(node->r);
        delete node;
    };

    Element *makeNode(Element *node, Neutral *parent) {
        if (node == nullptr) {
            return nullptr;
        }
        Element *cur = new Element(parent, nullptr, nullptr, node->value);
        cur->l = makeNode(dynamic_cast<Element *>(node->l), cur);
        cur->r = makeNode(dynamic_cast<Element *>(node->r), cur);
        return cur;
    };

//+
    Element *search(Element *node, T const &v) const {
        if (node == nullptr || v == node->value) {
            return node;
        }
        if ((v >= node->value)) return search(dynamic_cast<Element *>(node->r), v);
        return search(dynamic_cast<Element *>(node->l), v);
    };

//+
    std::pair<Element *, bool> go_insert(Element *node, T const &x) {
        while (node != nullptr) {
            if (x < node->value) {
                if (node->l != nullptr) {
                    node = dynamic_cast<Element *>(node->l);
                } else {
                    Element *inserted = new Element(nullptr, nullptr, node, x);
                    node->l = inserted;
                    return {inserted, 1};
                }
            }

            if (x > node->value) {
                if (node->r != nullptr) {
                    node = dynamic_cast<Element *>(node->r);
                } else {
                    Element *inserted = new Element(nullptr, nullptr, node, x);
                    node->r = inserted;
                    return {inserted, 1};
                }
            }
            if (x == node->value) {
                return {nullptr, 0};
            }
        }
        return {nullptr, 0};
        return {nullptr, 0};
    };

//+


    Element *go_lowerBound(Element *node, T const &x) const {
        if (node == nullptr || node->value == x)
            return node;
        if (node->value < x)
            return go_lowerBound(dynamic_cast<Element *>(node->r), x);

        Element *ans = go_lowerBound(dynamic_cast<Element *>(node->l), x);

        if (ans != nullptr && ans->value >= x) return ans;
        return node;
    };

//+
    static Neutral *get_min(Neutral *node) {
        if (node->l == nullptr) return node;
        return get_min(node->l);
    };

//+
    static Neutral *get_max(Neutral *node) {
        if (node->r == nullptr) return node;
        return get_max(node->r);
    };

};


#endif //SET_SET_H