#pragma once
#include <string>
#include <vector>

struct FoodItem {
    std::string item_id;
    std::string name;
    int price;
    std::string category;
    std::string image_path;
    int quantity;  // For cart
    
    FoodItem() : price(0), quantity(0) {}
    FoodItem(const std::string& id, const std::string& n, int p, const std::string& cat, const std::string& img)
        : item_id(id), name(n), price(p), category(cat), image_path(img), quantity(0) {}
};

class FoodsRepository {
public:
    /**
     * Load all food items from foods.csv
     * @param path Path to foods.csv
     * @return Vector of FoodItem
     */
    static std::vector<FoodItem> loadAll(const std::string& path);

private:
    static std::vector<std::string> parseCsvLine(const std::string& line);
};
