#pragma once
#include <string>

namespace models {

/**
 * @brief FoodItem entity - Represents a food/combo item
 * POCO - No dependencies
 */
class FoodItem {
private:
    std::string foodId;
    std::string name;
    std::string category;     // "Combo", "Snack", "Drink"
    int priceVnd;
    std::string imagePath;
    std::string status;       // "Available", "Sold Out"
    int quantity;             // Quantity selected (runtime state)
    
public:
    FoodItem() : priceVnd(0), quantity(0) {}
    
    FoodItem(const std::string& id, 
             const std::string& name,
             const std::string& category,
             int price,
             const std::string& image)
        : foodId(id), name(name), category(category),
          priceVnd(price), imagePath(image), 
          status("Available"), quantity(0) {}
    
    // Getters
    std::string getFoodId() const { return foodId; }
    std::string getName() const { return name; }
    std::string getCategory() const { return category; }
    int getPriceVnd() const { return priceVnd; }
    std::string getImagePath() const { return imagePath; }
    std::string getStatus() const { return status; }
    int getQuantity() const { return quantity; }
    
    // Setters
    void setFoodId(const std::string& id) { foodId = id; }
    void setName(const std::string& n) { name = n; }
    void setCategory(const std::string& cat) { category = cat; }
    void setPriceVnd(int price) { priceVnd = price; }
    void setImagePath(const std::string& path) { imagePath = path; }
    void setStatus(const std::string& s) { status = s; }
    void setQuantity(int qty) { quantity = qty >= 0 ? qty : 0; }
    
    // Actions
    void incrementQuantity() { quantity++; }
    void decrementQuantity() { if (quantity > 0) quantity--; }
    void resetQuantity() { quantity = 0; }
    
    // Calculations
    int getTotalPrice() const {
        return priceVnd * quantity;
    }
    
    // Validation
    bool isAvailable() const {
        return status == "Available" || status == "Có sẵn";
    }
    
    bool isValid() const {
        return !foodId.empty() &&
               !name.empty() &&
               priceVnd >= 0;
    }
};

} // namespace models
