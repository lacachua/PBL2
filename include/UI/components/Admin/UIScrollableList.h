#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <functional>
#include <memory>
#include <string>

template<typename T>
class UIScrollableList {
public:
    struct Config {
        float itemHeight;
        float scrollbarWidth;
        float padding;
        
        sf::Color backgroundColor;
        sf::Color scrollbarTrackColor;
        sf::Color scrollbarThumbColor;
        sf::Color scrollbarThumbHoverColor;
        sf::Color borderColor;
        bool showBorder;
        
        Config()
            : itemHeight(36.f)
            , scrollbarWidth(6.f)
            , padding(0.f)
            , backgroundColor(255, 255, 255)
            , scrollbarTrackColor(230, 230, 230)
            , scrollbarThumbColor(180, 180, 180)
            , scrollbarThumbHoverColor(150, 150, 150)
            , borderColor(210, 216, 224)
            , showBorder(true)
        {}
    };

    using RenderItemCallback = std::function<void(T& item, size_t index, sf::Vector2f position, bool isHovered, sf::RenderTarget& target)>;
    using ItemClickCallback = std::function<void(T& item, size_t index)>;
    using ItemHoverCallback = std::function<void(T* item, size_t index)>; // nullptr when not hovering any

    UIScrollableList(sf::FloatRect bounds, const Config& config = Config{});
    
    // Data management
    void setItems(const std::vector<T>& items);
    void setItems(std::vector<T>&& items);
    void clearItems();
    const std::vector<T>& getItems() const { return items_; }
    std::vector<T>& getItems() { return items_; }
    size_t getItemCount() const { return items_.size(); }
    
    // Layout
    void setBounds(sf::FloatRect bounds);
    sf::FloatRect getBounds() const { return bounds_; }
    void setItemHeight(float height);
    float getItemHeight() const { return config_.itemHeight; }
    
    // Scroll control
    void setScrollOffset(float offset);
    float getScrollOffset() const { return scrollOffset_; }
    void scrollTo(size_t index);
    void scrollToTop();
    void scrollToBottom();
    float getMaxScrollOffset() const;
    bool isScrollable() const { return getMaxScrollOffset() > 0.f; }
    
    // Event handling
    void handleMouseWheel(float delta);
    bool handleMouseClick(sf::Vector2f mousePos);
    void updateHover(sf::Vector2f mousePos);
    
    // Mouse position translation (screen -> list coordinate)
    sf::Vector2f translateMousePos(sf::Vector2f screenPos) const;
    
    // Get item at mouse position
    T* getItemAtPosition(sf::Vector2f mousePos);
    int getItemIndexAtPosition(sf::Vector2f mousePos) const;
    
    // Callbacks
    void setRenderItem(RenderItemCallback callback) { renderItem_ = std::move(callback); }
    void setOnItemClick(ItemClickCallback callback) { onItemClick_ = std::move(callback); }
    void setOnItemHover(ItemHoverCallback callback) { onItemHover_ = std::move(callback); }
    
    // Rendering
    void render(sf::RenderTarget& target);
    
    // Get visible range for external use
    std::pair<size_t, size_t> getVisibleRange() const;
    
    // Get hovered index
    int getHoveredIndex() const { return hoveredIndex_; }

private:
    sf::FloatRect bounds_;
    Config config_;
    std::vector<T> items_;
    
    float scrollOffset_ = 0.f;
    int hoveredIndex_ = -1;
    bool scrollbarHovered_ = false;
    bool scrollbarDragging_ = false;
    
    RenderItemCallback renderItem_;
    ItemClickCallback onItemClick_;
    ItemHoverCallback onItemHover_;
    
    // Cached renderables
    sf::RectangleShape background_;
    sf::RectangleShape border_;
    sf::RectangleShape scrollbarTrack_;
    sf::RectangleShape scrollbarThumb_;
    
    void updateScrollbar();
    float getContentHeight() const;
    sf::FloatRect getScrollbarThumbBounds() const;
};

// Implementation in header due to template

template<typename T>
UIScrollableList<T>::UIScrollableList(sf::FloatRect bounds, const Config& config)
    : bounds_(bounds)
    , config_(config)
{
    // Background
    background_.setSize(bounds_.size);
    background_.setPosition(bounds_.position);
    background_.setFillColor(config_.backgroundColor);
    
    // Border
    if (config_.showBorder) {
        border_.setSize(bounds_.size);
        border_.setPosition(bounds_.position);
        border_.setFillColor(sf::Color::Transparent);
        border_.setOutlineThickness(1.f);
        border_.setOutlineColor(config_.borderColor);
    }
    
    // Scrollbar track
    scrollbarTrack_.setFillColor(config_.scrollbarTrackColor);
    
    // Scrollbar thumb
    scrollbarThumb_.setFillColor(config_.scrollbarThumbColor);
    
    updateScrollbar();
}

template<typename T>
void UIScrollableList<T>::setItems(const std::vector<T>& items) {
    items_ = items;
    scrollOffset_ = 0.f;
    hoveredIndex_ = -1;
    updateScrollbar();
}

template<typename T>
void UIScrollableList<T>::setItems(std::vector<T>&& items) {
    items_ = std::move(items);
    scrollOffset_ = 0.f;
    hoveredIndex_ = -1;
    updateScrollbar();
}

template<typename T>
void UIScrollableList<T>::clearItems() {
    items_.clear();
    scrollOffset_ = 0.f;
    hoveredIndex_ = -1;
    updateScrollbar();
}

template<typename T>
void UIScrollableList<T>::setBounds(sf::FloatRect bounds) {
    bounds_ = bounds;
    background_.setSize(bounds_.size);
    background_.setPosition(bounds_.position);
    border_.setSize(bounds_.size);
    border_.setPosition(bounds_.position);
    updateScrollbar();
}

template<typename T>
void UIScrollableList<T>::setItemHeight(float height) {
    config_.itemHeight = height;
    updateScrollbar();
}

template<typename T>
void UIScrollableList<T>::setScrollOffset(float offset) {
    float maxOffset = getMaxScrollOffset();
    scrollOffset_ = std::max(0.f, std::min(offset, maxOffset));
    updateScrollbar();
}

template<typename T>
float UIScrollableList<T>::getMaxScrollOffset() const {
    float contentHeight = getContentHeight();
    float viewHeight = bounds_.size.y;
    return std::max(0.f, contentHeight - viewHeight);
}

template<typename T>
void UIScrollableList<T>::scrollTo(size_t index) {
    if (index >= items_.size()) return;
    
    float itemTop = static_cast<float>(index) * config_.itemHeight;
    float itemBottom = itemTop + config_.itemHeight;
    float viewTop = scrollOffset_;
    float viewBottom = scrollOffset_ + bounds_.size.y;
    
    if (itemTop < viewTop) {
        setScrollOffset(itemTop);
    } else if (itemBottom > viewBottom) {
        setScrollOffset(itemBottom - bounds_.size.y);
    }
}

template<typename T>
void UIScrollableList<T>::scrollToTop() {
    setScrollOffset(0.f);
}

template<typename T>
void UIScrollableList<T>::scrollToBottom() {
    setScrollOffset(getMaxScrollOffset());
}

template<typename T>
void UIScrollableList<T>::handleMouseWheel(float delta) {
    float scrollAmount = config_.itemHeight * 3.f * delta;
    setScrollOffset(scrollOffset_ - scrollAmount);
}

template<typename T>
bool UIScrollableList<T>::handleMouseClick(sf::Vector2f mousePos) {
    if (!bounds_.contains(mousePos)) return false;
    
    int index = getItemIndexAtPosition(mousePos);
    if (index >= 0 && index < static_cast<int>(items_.size())) {
        if (onItemClick_) {
            onItemClick_(items_[static_cast<size_t>(index)], static_cast<size_t>(index));
        }
        return true;
    }
    return false;
}

template<typename T>
void UIScrollableList<T>::updateHover(sf::Vector2f mousePos) {
    int prevHovered = hoveredIndex_;
    
    if (bounds_.contains(mousePos)) {
        hoveredIndex_ = getItemIndexAtPosition(mousePos);
    } else {
        hoveredIndex_ = -1;
    }
    
    if (prevHovered != hoveredIndex_ && onItemHover_) {
        if (hoveredIndex_ >= 0 && hoveredIndex_ < static_cast<int>(items_.size())) {
            onItemHover_(&items_[static_cast<size_t>(hoveredIndex_)], static_cast<size_t>(hoveredIndex_));
        } else {
            onItemHover_(nullptr, 0);
        }
    }
}

template<typename T>
sf::Vector2f UIScrollableList<T>::translateMousePos(sf::Vector2f screenPos) const {
    return sf::Vector2f(
        screenPos.x - bounds_.position.x,
        screenPos.y - bounds_.position.y + scrollOffset_
    );
}

template<typename T>
T* UIScrollableList<T>::getItemAtPosition(sf::Vector2f mousePos) {
    int index = getItemIndexAtPosition(mousePos);
    if (index >= 0 && index < static_cast<int>(items_.size())) {
        return &items_[static_cast<size_t>(index)];
    }
    return nullptr;
}

template<typename T>
int UIScrollableList<T>::getItemIndexAtPosition(sf::Vector2f mousePos) const {
    if (!bounds_.contains(mousePos)) return -1;
    
    // Translate to list coordinate
    float relativeY = mousePos.y - bounds_.position.y + scrollOffset_;
    int index = static_cast<int>(relativeY / config_.itemHeight);
    
    if (index < 0 || index >= static_cast<int>(items_.size())) {
        return -1;
    }
    
    return index;
}

template<typename T>
std::pair<size_t, size_t> UIScrollableList<T>::getVisibleRange() const {
    if (items_.empty()) return {0, 0};
    
    size_t firstVisible = static_cast<size_t>(scrollOffset_ / config_.itemHeight);
    size_t lastVisible = static_cast<size_t>((scrollOffset_ + bounds_.size.y) / config_.itemHeight) + 1;
    
    firstVisible = std::min(firstVisible, items_.size());
    lastVisible = std::min(lastVisible, items_.size());
    
    return {firstVisible, lastVisible};
}

template<typename T>
void UIScrollableList<T>::render(sf::RenderTarget& target) {
    // Draw background
    target.draw(background_);
    
    if (items_.empty() || !renderItem_) {
        if (config_.showBorder) {
            target.draw(border_);
        }
        return;
    }
    
    // Save current view
    sf::View defaultView = target.getView();
    
    // Create clipped view
    sf::View clippedView;
    clippedView.setCenter({
        bounds_.position.x + bounds_.size.x / 2.f,
        bounds_.position.y + bounds_.size.y / 2.f + scrollOffset_
    });
    clippedView.setSize(bounds_.size);
    
    // Calculate viewport (normalized 0-1)
    sf::Vector2u windowSize = target.getSize();
    sf::FloatRect viewport(
        sf::Vector2f(bounds_.position.x / static_cast<float>(windowSize.x),
                     bounds_.position.y / static_cast<float>(windowSize.y)),
        sf::Vector2f(bounds_.size.x / static_cast<float>(windowSize.x),
                     bounds_.size.y / static_cast<float>(windowSize.y))
    );
    clippedView.setViewport(viewport);
    
    target.setView(clippedView);
    
    // View culling: only render visible items
    auto [firstVisible, lastVisible] = getVisibleRange();
    
    for (size_t i = firstVisible; i < lastVisible; ++i) {
        sf::Vector2f itemPos(
            bounds_.position.x + config_.padding,
            bounds_.position.y + static_cast<float>(i) * config_.itemHeight + scrollOffset_
        );
        
        bool isHovered = (static_cast<int>(i) == hoveredIndex_);
        renderItem_(items_[i], i, itemPos, isHovered, target);
    }
    
    // Restore view
    target.setView(defaultView);
    
    // Draw border over content
    if (config_.showBorder) {
        target.draw(border_);
    }
    
    // Draw scrollbar (outside clipped view)
    if (isScrollable()) {
        target.draw(scrollbarTrack_);
        target.draw(scrollbarThumb_);
    }
}

template<typename T>
float UIScrollableList<T>::getContentHeight() const {
    return static_cast<float>(items_.size()) * config_.itemHeight;
}

template<typename T>
void UIScrollableList<T>::updateScrollbar() {
    if (!isScrollable()) return;
    
    float contentHeight = getContentHeight();
    float viewHeight = bounds_.size.y;
    float maxScroll = getMaxScrollOffset();
    
    // Scrollbar track (right side of list)
    float trackX = bounds_.position.x + bounds_.size.x - config_.scrollbarWidth - 2.f;
    scrollbarTrack_.setSize({config_.scrollbarWidth, viewHeight - 4.f});
    scrollbarTrack_.setPosition({trackX, bounds_.position.y + 2.f});
    
    // Scrollbar thumb
    float thumbHeight = std::max(30.f, (viewHeight / contentHeight) * viewHeight);
    float thumbY = bounds_.position.y + 2.f + (scrollOffset_ / maxScroll) * (viewHeight - thumbHeight - 4.f);
    
    scrollbarThumb_.setSize({config_.scrollbarWidth, thumbHeight});
    scrollbarThumb_.setPosition({trackX, thumbY});
    
    if (scrollbarHovered_ || scrollbarDragging_) {
        scrollbarThumb_.setFillColor(config_.scrollbarThumbHoverColor);
    } else {
        scrollbarThumb_.setFillColor(config_.scrollbarThumbColor);
    }
}

template<typename T>
sf::FloatRect UIScrollableList<T>::getScrollbarThumbBounds() const {
    return sf::FloatRect(scrollbarThumb_.getPosition(), scrollbarThumb_.getSize());
}
