#include "UI/components/TicketBooking/BookingState.h"

#include <algorithm>

std::string BookingData::getFormattedComboDisplay(int maxPerLine) const {
    if (selectedCombos.empty()) return "Không có";

    std::string result;
    for (size_t i = 0; i < selectedCombos.size(); i++) {
        if (i > 0) {
            // Add line break every maxPerLine items
            if (maxPerLine > 0 && i % (size_t)maxPerLine == 0) {
                result += "\n";
            }
            else {
                result += ", ";
            }
        }
        result += selectedCombos[i].comboName;
        if (selectedCombos[i].quantity > 1) {
            result += " (x" + std::to_string(selectedCombos[i].quantity) + ")";
        }
    }
    return result;
}

std::string BookingData::getComboDisplayForHistory() const {
    if (selectedCombos.empty()) return "Không có";

    std::string result;
    size_t displayCount = std::min(selectedCombos.size(), size_t(2));

    for (size_t i = 0; i < displayCount; i++) {
        if (i > 0) result += ", ";
        result += selectedCombos[i].comboName;
        if (selectedCombos[i].quantity > 1) {
            result += " (x" + std::to_string(selectedCombos[i].quantity) + ")";
        }
    }

    if (selectedCombos.size() > 2) {
        result += "...";
    }

    return result;
}

void BookingData::clear() {
    showtimeId.clear();
    movieId.clear();
    movieName.clear();
    roomId.clear();
    roomName.clear();
    date.clear();
    time.clear();
    ticketPrice = 0;

    selectedSeats.clear();
    seatsDisplay.clear();
    totalSeats = 0;

    selectedCombos.clear();
    combosDisplay.clear();
    comboTotalPrice = 0;

    voucherCode.clear();
    voucherDiscount = 0;

    customerName.clear();
    customerEmail.clear();
    customerPhone.clear();

    ticketId.clear();
    totalPrice = 0;
}
