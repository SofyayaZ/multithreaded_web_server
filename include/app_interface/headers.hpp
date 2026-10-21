#pragma once

#include <string>
#include <vector>
#include <optional>


class Headers {
    std::vector<std::pair<std::string, std::string>> headers{};
    void normalizeHeader(std::string& header) {
        for (auto& c : header) {
            c = static_cast<unsigned char>(tolower(static_cast<unsigned char>(c)));
        }
    }

public:
    std::optional<std::string> get(std::string target) {
        normalizeHeader(target);
        for (const auto& header : headers) {
            if (header.first == target) {
                return header.second;
            }
        }
        return std::nullopt;
    }

    bool contains(std::string target) {
        normalizeHeader(target);
        for (const auto& header : headers) {
            if (header.first == target) {
                return true;
            }
        }
        return false;
    }

    void set(std::string target, std::string newContent) {
        normalizeHeader(target);
        for (size_t i = 0; i < headers.size(); i++) {
            if (headers[i].first == target) {
                headers[i].second = newContent;
                return;
            }
        }
        headers.push_back({target, newContent});
        return;
    }

    void add(std::string newHeader, std::string newHeaderContent) {
        normalizeHeader(newHeader);
        for (const auto& header : headers) {
            if (header.first == newHeader) {
                if (header.second == newHeaderContent) {
                    return;
                }
            }
        }
        headers.push_back({newHeader, newHeaderContent});
    }

    void remove(std::string target) {
        normalizeHeader(target);
        for(size_t i = 0; i < headers.size(); i++) {
            if (headers[i].first == target) {
                headers.erase(headers.begin() + i);
            }
        }
        return;
    }
};