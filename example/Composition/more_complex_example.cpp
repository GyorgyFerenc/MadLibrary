#include <iostream>

#include "../../include/Composition.hpp"

struct LifeComponent {
    LifeComponent(int hp) {
        this->hp = hp;
    }

    void damage(int damage) {
        this->hp -= damage;
        std::cout << "Damage taken: " << damage << std::endl;
        std::cout << "New hp: " << this->hp << std::endl;
    }

    void heal(int heal) {
        this->hp += heal;
        std::cout << "Heal received: " << heal << std::endl;
        std::cout << "New hp: " << this->hp << std::endl;
    }

   private:
    int hp;
};

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

void fight(LifeComponent& life_component) {
    life_component.damage(2);
}

int main(int argc, char const* argv[]) {
    Cat cat;

    auto& cat_life = AS(cat, LifeComponent);

    // Casts cat to the LifeComponent
    cat_life.damage(3);

    fight(cat_life);

    // Feed the cat 50 grams of food thus healing it with 5
    cat.feed(50);
}
