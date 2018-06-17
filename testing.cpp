#include <algorithm>
#include "my_set.h"
#include <gtest/gtest.h>

using namespace std;

template <typename It1, typename It2>
void assert_range_equality(It1 f1, It1 l1, It2 f2, It2 l2) {
    for (; f1 != l1 and f2 != l2; ++f1, ++f2) {
        cout<<*f1<<" "<<*f2<<"\n";
        ASSERT_EQ(*f1, *f2);
    }
    ASSERT_TRUE(f1 == l1 and f2 == l2);
}

TEST(correctness, empty) {
    my_set<int> s;
    ASSERT_TRUE(s.empty());
}

TEST(correctness, 3_4_5) {
    my_set<int> a;
    std::set<int> b;
    a.insert(5);
    a.insert(3);
    a.insert(4);
    b.insert({5});
    b.insert({3});
    b.insert({4});

    auto a_it = a.begin();
    auto b_it = b.begin();
    for (; a_it != a.end() && b_it != b.end(); ++a_it, ++b_it) {
        ASSERT_EQ(*a_it, *b_it);
    }
}


TEST(correctness, reverse_iterator_rbeg_to_rend) {
    my_set<int> a;
    std::set<int> b;
    a.insert(5);
    a.insert(3);
    a.insert(4);
    a.insert(11);
    a.insert(8);
    a.insert(20);
    b.insert(5);
    b.insert(3);
    b.insert(4);
    b.insert(11);
    b.insert(8);
    b.insert(20);
    auto a_it = a.rbegin();
    auto b_it = b.rbegin();
    for (; a_it != a.rend() && b_it != b.rend(); ++a_it, ++b_it) {
        ASSERT_EQ(*a_it, *b_it);
    }
}


TEST(correctness, reverse_iterator_rend_to_rbeg) {
    my_set<int> a;
    std::set<int> b;
    a.insert(5);
    a.insert(3);
    a.insert(4);
    a.insert(11);
    a.insert(8);
    a.insert(20);
    b.insert(5);
    b.insert(3);
    b.insert(4);
    b.insert(11);
    b.insert(8);
    b.insert(20);
    auto a_it = a.rend();
    auto b_it = b.rend();
    --a_it;
    --b_it;
    for (; a_it != a.rbegin() && b_it != b.rbegin(); --a_it, --b_it) {
        ASSERT_EQ(*a_it, *b_it);
    }
}


TEST(correctness, iter_down) {
    my_set<int> a;
    std::set<int> b;
    a.insert(5);
    a.insert(3);
    a.insert(4);
    a.insert(11);
    a.insert(8);
    a.insert(20);
    b.insert(5);
    b.insert(3);
    b.insert(4);
    b.insert(11);
    b.insert(8);
    b.insert(20);

    auto a_it = a.end();
    auto b_it = b.end();

    do {
        --a_it;
        --b_it;
        ASSERT_EQ(*a_it, *b_it);
    } while (a_it != a.begin() || b_it != b.begin());
}

TEST(correctness, find) {
    my_set<int> a;
    a.insert(5);
    a.insert(3);
    a.insert(4);

    ASSERT_EQ(a.find(6), a.end());
    ASSERT_EQ(a.find(-120), a.end());
    ASSERT_EQ(*a.find(5), 5);
    ASSERT_EQ(*a.find(3), 3);
    ASSERT_EQ(*a.find(4), 4);
}

TEST(correctness, erase) {
    my_set<int> a;
    a.insert(5);
    a.insert(3);
    a.insert(4);
    a.insert(10);
    a.insert(20);
    a.insert(30);

    ASSERT_EQ(*a.erase(a.find(10)), 20);
    ASSERT_EQ(*a.erase(a.find(20)), 30);
    ASSERT_EQ(a.erase(a.find(30)), a.end());
    ASSERT_EQ(*a.erase(a.find(3)), 4);
    ASSERT_EQ(a.find(3), a.end());
    ASSERT_EQ(*a.erase(a.find(4)), 5);
    ASSERT_EQ(a.find(4), a.end());
    ASSERT_EQ(a.erase(a.find(5)), a.end());
    ASSERT_EQ(a.find(5), a.end());
}
/*
TEST(correctness, size)
{
    my_set a;
    ASSERT_EQ(a.size(), 0);
    a.insert(5);
    ASSERT_EQ(a.size(), 1);
    a.insert(3);
    ASSERT_EQ(a.size(), 2);
    a.insert(4);
    ASSERT_EQ(a.size(), 3);
}*/

TEST(correctness, random) {
    std::set<int> a;
    my_set<int> b;

    for (int i = 0,j=100; i < 100; i++,j--) {
        a.insert(i);
        a.insert(j);
        b.insert(i);
        b.insert(j);
    }
    for (int i = 0; i < 10; i++) {
        int pos = rand() % a.size();
        auto it = a.begin();
        for (int j = 0; j < pos; j++) {
            ++it;
        }
        int x = *it;
        ASSERT_EQ(*(a.find(x)),*(b.find(x)));
        //cout<<x<<"|";
        a.erase(a.find(x));
        b.erase(b.find(x));
    }
    auto a_it = a.begin();
    auto b_it = b.begin();
    for (; a_it != a.end() && b_it != b.end(); ++a_it, ++b_it) {
   //     std::cout<<*a_it<<" ";
        ASSERT_EQ(*a_it, *b_it);
    }
   // std::cout<<endl;
}

TEST(iterators, const_iterator) {
    my_set<int> x;

    x.insert(5);
    x.insert(3);
    x.insert(4);
    x.insert(10);
    x.insert(20);
    x.insert(30);

    my_set<int>::const_iterator it(x.begin());
    ASSERT_EQ(*it, 3);
    it++;
    ASSERT_EQ(*it, 4);
}

TEST(iterators, const_iterator_cast) {
    my_set<int> x;

    x.insert(5);
    x.insert(3);
    x.insert(4);
    x.insert(10);
    x.insert(20);
    x.insert(30);

    my_set<int>::iterator z(x.begin());
    my_set<int>::const_iterator n(z);
    //(*n) = 4;
}
TEST(iterators, const_iterator_equality) {
    my_set<int> x;

    x.insert(5);
    x.insert(3);
    x.insert(4);
    x.insert(10);
    x.insert(20);
    x.insert(30);

    my_set<int>::iterator i(x.begin());
    my_set<int>::const_iterator j(x.begin());
    EXPECT_TRUE(i == j && j == i);
    EXPECT_FALSE(++i == j || j == ++i);
}

TEST(swaps, swap_empty) {
    my_set<int> x;
    my_set<int> y;
    x.insert(2);
    swap(x,y);
    EXPECT_FALSE(y.empty());
    swap(x,y);
    EXPECT_FALSE(x.empty());
}