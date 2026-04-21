#pragma once
#include <string>

class Product {
    private:
    int price; //  amount of money required to acquire a product
    double rating; // valuation, or measurement of something's quality,
    int popularity; // value given to the product to show its popularity
    std::string name;
    int id;
    public:
    Product(int num1, double num2, int num3, std::string txt1,int num4): 
    price(num1), rating(num2), popularity(num3),name(txt1),id(num4) {}
    int getPrice()      const { return price; }
    double getRating() const { return rating; }
    int getPopularity() const { return popularity; }
    std::string getName() const { return name; }
    int getID() const { return id; }
};