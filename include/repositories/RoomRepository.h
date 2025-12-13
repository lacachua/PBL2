#pragma once

#include <SFML/System/String.hpp>

#include <unordered_map>

class RoomRepository {
private:
    sf::String filePath;
    std::unordered_map<std::string, sf::String> roomNameById;

    void loadRooms();

public:
    explicit RoomRepository(const sf::String& path = "../data/rooms.txt");

    // Returns roomId if not found
    sf::String getRoomNameById(const sf::String& roomId) const;
};
