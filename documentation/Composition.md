# Composition

This module makes it easier to use [composition](https://en.wikipedia.org/wiki/Object_composition) inside C++.

## Usage

By struct we gonna refer to __struct__ and __classes__ as well.

There is 4 important macros:
- __COMPOSE__ -> It is used to compose a struct out of another struct
- __AS__ -> Cast the object to a component type
- __THIS_AS__ -> Casts the "this" keyword to a component type. It must be used inside struct methods.
- __CONSTRUCT__-> Constructs the underlying component. It must be used inside an initializer list

Let us see an easy example.
Check out the full [example](../example/Composition/basic_use.cpp).

We create struct A, and create a method say_hi.

```c++
struct A {
    void say_hi() {
        std::cout << "A says hi!" << std::endl;
    }
};
```

Now we create struct B, which we compose out of A.
After this we create a method say_hi as well.

```c++
struct B {
    // Compose B out of A
    COMPOSE(A);

    void say_hi() {
        std::cout << "B says helloo!" << std::endl;
    }
};
```

Now we can instanstiate an object of B.

```c++
B b{};
```

Now we can call B's say_hi method and A's say_hi method as well.

```c++
b.say_hi();
// Casts variable b into type A
AS(b, A).say_hi();
```

Let us see antoher example.
Check out the full [example](../example/Composition/more_complex_example.cpp).

We create a struct LifeComponent.
```c++
struct LifeComponent {
    LifeComponent(int hp) {
        this->hp = hp;
    }

    void heal(int heal) {
        this->hp += heal;
        std::cout << "Heal received: " << heal << std::endl;
        std::cout << "New hp: " << this->hp << std::endl;
    }

   private:
    int hp;
};
```

After this we can create a Cat which has a LifeComponent.

```c++
struct Cat {
    // Compose Cat out of LifeComponent
    COMPOSE(LifeComponent);

    // Calls the constructor of the LifeComponent with the paramater 10
    Cat() : CONSTRUCT(LifeComponent, 10) {
    }

    // Heals one tenth of the food amount
    void feed(int food_amout) {
        // We can cast this to LifeComponent
        THIS_AS(LifeComponent).heal(food_amout / 10);
    }
};
```

As we can see when wee construct the cat we need to construct the LifeComponent as well.
We create the feed method with the intation of healing the Cat. 

After this we can create the cat and feed it.

```c++
Cat cat;

// Feed the cat 50 grams of food thus healing it with 5
cat.feed(50);
```