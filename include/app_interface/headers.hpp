#pragma once

#include <algorithm>
#include <cctype>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>


struct Header {
    std::string name;
    std::string value;
    bool operator==(const Header& other) const = default;
};

class Headers {
    std::vector<Header> headers{};

    void normalizeHeader(std::string& header) {
        for (auto& c : header) {
            c = static_cast<unsigned char>(tolower(static_cast<unsigned char>(c)));
        }
    }

    bool equalsIgnoreCase(std::string_view a, std::string_view b) const noexcept {
        return std::equal(a.begin(), a.end(), b.begin(), b.end(),
                          [](unsigned char c1, unsigned char c2) {
                                return std::tolower(c1) == std::tolower(c2);
                          });
    }

public:
    bool operator==(const Headers& other) const = default;

    void add(std::string name, std::string value);      // add new header
    void set(std::string name, std::string value);      // delete all existing headers + add new one
    void remove(std::string_view target);               // remove existing header
    
    bool contains(std::string_view target) const;       // check if headers contains target header
    std::size_t count(std::string_view target) const;   // return number of string with t-header

    std::optional<std::string> get(std::string_view target) const;          // return first target header string
    std::vector<std::string> getAll(std::string_view target) const;         // return all strings with target header
    std::optional<std::string> getCombined(std::string_view target) const;  // return strings with t-header combined

    std::span<const Header> entries() const noexcept;
    bool empty() const noexcept;
    std::size_t size() const noexcept;
};

inline void Headers::add(std::string newHeader, std::string newHeaderContent) {
    normalizeHeader(newHeader);
    headers.push_back({std::move(newHeader), std::move(newHeaderContent)});
}

inline void Headers::set(std::string target, std::string newContent) {
    normalizeHeader(target);
    remove(target);
    add(target, newContent);
}

inline void Headers::remove(std::string_view target) {
    std::erase_if(headers, [&](const auto& h) {
        return equalsIgnoreCase(h.name, target);
    });
    return;
}

inline bool Headers::contains(std::string_view target) const {
    for (const auto& h : headers) {
        if (equalsIgnoreCase(h.name, target)) return true;
    }
    return false;
}

inline std::size_t Headers::count(std::string_view target) const {
    std::size_t count{0};
    for (const auto& h : headers) {
        if (equalsIgnoreCase(h.name, target)) count++;
    }
    return count;
}

inline std::optional<std::string> Headers::get(std::string_view target) const {
    for (const auto& h : headers) {
        if (equalsIgnoreCase(h.name, target)) return h.value;
    }
    return std::nullopt;
} 

inline std::vector<std::string> Headers::getAll(std::string_view target) const {
    std::vector<std::string> all{};
    for (const auto& h : headers) {
        if (equalsIgnoreCase(h.name, target)) all.push_back(h.value);
    }
    return all;
}

inline std::optional<std::string> Headers::getCombined(std::string_view target) const {
    std::string combined{};
    const std::vector<std::string> all = getAll(target);
    if (all.size() == 0) return std::nullopt;
    for (size_t i = 0; i < all.size(); i++) {
        if (i != 0) combined += ", ";
        combined.append(all[i]);
    }
    return combined;
}

inline std::span<const Header> Headers::entries() const noexcept {
    return headers;
}

inline bool Headers::empty() const noexcept {
    return headers.size() == 0;
}

inline std::size_t Headers::size() const noexcept {
    return headers.size();
}
