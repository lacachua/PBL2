#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <unordered_map>
#include <algorithm>

/**
 * @brief Application event base class for the Observer pattern
 * 
 * All application events should inherit from this class.
 * Named AppEvent to avoid conflict with sf::Event.
 */
class AppEvent {
public:
    virtual ~AppEvent() = default;
    virtual std::string getType() const = 0;
};

/**
 * @brief Event fired when a new user registers
 */
class UserRegisteredEvent : public AppEvent {
public:
    std::string email;
    std::string fullName;
    
    UserRegisteredEvent(const std::string& email, const std::string& fullName);
    std::string getType() const override;
};

/**
 * @brief Event fired when a voucher is created
 */
class VoucherCreatedEvent : public AppEvent {
public:
    std::string code;
    bool autoProvision;  // If true, should be auto-provisioned to new users
    int defaultDays;     // Default days to expire for auto-provision
    int defaultQuantity; // Default quantity for auto-provision
    
        VoucherCreatedEvent(const std::string& code, bool autoProvision = false,
                                                int defaultDays = 30, int defaultQuantity = 1);
        std::string getType() const override;
};

/**
 * @brief Observer interface for application event handling
 */
class IAppEventObserver {
public:
    virtual ~IAppEventObserver() = default;
    virtual void onAppEvent(const AppEvent& event) = 0;
    virtual std::vector<std::string> getSubscribedEvents() const = 0;
};

/**
 * @brief Singleton Event System for publishing and subscribing to application events
 * 
 * Implements the Observer pattern for loose coupling between components.
 * Components can publish events without knowing who will handle them,
 * and observers can subscribe to specific event types.
 */
class AppEventSystem {
public:
    static AppEventSystem& getInstance();
    
    // Delete copy constructor and assignment operator
    AppEventSystem(const AppEventSystem&) = delete;
    AppEventSystem& operator=(const AppEventSystem&) = delete;
    
    /**
     * @brief Subscribe an observer to events
     * @param observer The observer to register
     */
    void subscribe(std::shared_ptr<IAppEventObserver> observer);
    
    /**
     * @brief Unsubscribe an observer from all events
     * @param observer The observer to unregister
     */
    void unsubscribe(std::shared_ptr<IAppEventObserver> observer);
    
    /**
     * @brief Publish an event to all subscribed observers
     * @param event The event to publish
     */
    void publish(const AppEvent& event);
    
private:
    AppEventSystem() = default;
    
    std::unordered_map<std::string, std::vector<std::weak_ptr<IAppEventObserver>>> observers;
};
