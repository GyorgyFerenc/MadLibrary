#include <functional>
#include <iostream>
#include <vector>

template <class T>
struct MyIter {
    virtual bool has_next() = 0;
    virtual T    next() = 0;
};

template <class Container>
struct IterOverSTD
    : public MyIter<typename Container::value_type> {
    IterOverSTD(Container& c) {
        this->begin = c.begin();
        this->end = c.end();
    }

    bool has_next() override {
        return begin != end;
    }

    Container::value_type next() override {
        return *(begin++);
    }

   private:
    Container::iterator begin;
    Container::iterator end;
};

template <class A, class B>
struct MapIter : public MyIter<B> {
    MapIter(MyIter<A>& iter, std::function<B(A)> function)
        : iter{iter}, function{function} {
    }
    bool has_next() override {
        return iter.has_next();
    }
    virtual B next() {
        return function(iter.next());
    }

   private:
    MyIter<A>&          iter;
    std::function<B(A)> function;
};

template <class Iter>
struct Stream {
    Stream(Iter begin, Iter end) {
        this->begin = begin;
        this->end = end;
    }

    template <class Function>
    void for_each(Function function) {
        for (; this->begin != this->end; this->begin++) {
            function(*this->begin);
        }
    }

    template <class Function>
    Stream map(Function func) {
    }

   private:
    Iter begin;
    Iter end;
};

int main(int argc, char const* argv[]) {
    std::vector<int> v{{1, 2, 3, 4}};
    IterOverSTD      a{v};
    auto asd = [](int a) -> int { return a + 1; };
    MapIter<int, int> iter{
        a, [](int a) -> int { return a + 1; }};

    std::cout << iter.next();
}
