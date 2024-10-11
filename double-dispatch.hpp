#include<iostream>
//double-dispatch是动态多态性技术,用于根据两个对象的实际类型决定调用那个方法, single-dispatch方法的调用依赖于
//调用对象的实际类型,而double-dispatch则考虑到调用对象和参数对象的实际类型.
//常见的实现方式是使用虚函数和接受不同参数类型的方法，
//从而在运行时根据这两个对象的类型选择合适的方法。这在实现复杂的行为（如图形处理或游戏开发中的碰撞检测）时尤其有用。
class Bear;
class Fish;
class Animal
{
public:
    virtual ~Animal() = default;

    virtual bool eat(const Animal& animal) const = 0;
    
    virtual bool eatby(const Bear&) const = 0;
    virtual bool eatby(const Fish&) const = 0;
};

class Bear : public Animal
{
public:
    virtual bool eat(const Animal& animal) const override {return animal.eatby(*this);}

    virtual bool eatby(const Bear&) const override {return false;}
    virtual bool eatby(const Fish&) const override {return false;}
};

class Fish : public Animal
{
public:
    virtual bool eat(const Animal& animal) const override {return animal.eatby(*this);}

    virtual bool eatby(const Bear&) const override {return true;}
    virtual bool eatby(const Fish&) const override {return false;}
};

void testing()
{
    Bear bear1, bear2;
    Fish fish1, fish2;

    const Animal& animal_bear{bear1};
    const Animal& animal_fish{fish1};

    std::cout<<"bear eat fish "<<bear1.eat(fish1)<<std::endl;
    std::cout<<"bear eat fish "<<bear1.eat(animal_fish)<<std::endl;

    std::cout<<"fish eat bear "<<animal_fish.eat(bear1)<<std::endl;
    std::cout<<"fish eat fish "<<animal_fish.eat(animal_fish)<<std::endl;
}