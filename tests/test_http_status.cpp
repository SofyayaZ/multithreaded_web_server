#include "http_status.hpp"
#include <gtest/gtest.h>


TEST(HttpStatusTest, CodesMatchEnumValues) {
    EXPECT_EQ(200u, codeFromStatus(HttpStatus::OK));
    EXPECT_EQ(400u, codeFromStatus(HttpStatus::BadRequest));
    EXPECT_EQ(503u, codeFromStatus(HttpStatus::ServiceUnavailable));
}

TEST(HttpStatusTest, ReasonPhrases) {
    EXPECT_EQ(reasonPhrase(HttpStatus::OK), "OK");
    EXPECT_EQ(reasonPhrase(HttpStatus::Created), "Created");
    EXPECT_EQ(reasonPhrase(HttpStatus::NoContent), "No Content");
    EXPECT_EQ(reasonPhrase(HttpStatus::BadRequest), "Bad Request");
    EXPECT_EQ(reasonPhrase(HttpStatus::NotFound), "Not Found");
    EXPECT_EQ(reasonPhrase(HttpStatus::ContentTooLarge), "Content Too Large");
    EXPECT_EQ(reasonPhrase(HttpStatus::InternalServerError), "Internal Server Error");
}

TEST(HttpStatusTest, UnknownCodeFallsBack) {
    EXPECT_FALSE(statusFromCode(900u).has_value());
    EXPECT_EQ(reasonPhrase(999u), "Unknown");
}

TEST(HttpStatusTest, StatusFromCode) {
    EXPECT_EQ(statusFromCode(201), HttpStatus::Created);
    EXPECT_EQ(statusFromCode(404), HttpStatus::NotFound);
    EXPECT_FALSE(statusFromCode(999).has_value());
}
