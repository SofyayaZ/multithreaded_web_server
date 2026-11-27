#include "headers.hpp"

#include <gtest/gtest.h>
#include <vector>


TEST (HeadersTest, AddStoresLowercaseName) {
    Headers headers;
    headers.add("UPPer", "lOcAl");
    headers.add("lower", "local");
    ASSERT_TRUE(headers.contains("upper"));
    ASSERT_TRUE(headers.contains("lower"));
    auto upVal = headers.get("upper");
    auto loVal = headers.get("lower");
    ASSERT_TRUE(upVal.has_value());
    ASSERT_TRUE(loVal.has_value());
    EXPECT_EQ(*upVal, "lOcAl");
    EXPECT_EQ(*loVal, "local");
}

TEST (HeadersTest, GetIsCaseInsensitive) {
    Headers headers;
    headers.add("lower", "local");
    auto val = headers.get("LoWer");
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(*val, "local");
}

TEST (HeadersTest, ContainsAndCountAreCaseInsensitive) {
    Headers headers;
    headers.add("host", "value");
    ASSERT_TRUE(headers.contains("HOsT"));
    EXPECT_EQ(headers.count("hOst"), 1u);
    ASSERT_FALSE(headers.contains("acceR"));
    EXPECT_EQ(headers.count("ACcept"), 0u);
}

TEST (HeadersTest, AddPreservesDuplicatedLines) {
    Headers headers;
    headers.add("header", "duplicate");
    headers.add("header", "duplicate");
    headers.add("header", "no");
    EXPECT_EQ(headers.count("header"), 3u);
    EXPECT_EQ(headers.getAll("header").size(), 3u);
    const std::vector<Header> expectedEntries = {{"header", "duplicate"},
                                                 {"header", "duplicate"},
                                                 {"header", "no"}};
    const std::vector<Header> entries(headers.entries().begin(), headers.entries().end());
    EXPECT_EQ(expectedEntries, entries);
}

TEST (HeadersTest, SetReplacesAllLines) {
    Headers headers;
    headers.add("Header", "val_1");
    headers.add("Header", "val_2");
    headers.add("Header", "val_3");
    EXPECT_EQ(headers.size(), 3u);
    headers.set("Header", "new");
    EXPECT_EQ(headers.size(), 1u);
    auto val = headers.get("Header");
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(*val, "new");

}

TEST (HeadersTest, RemoveDeleteAllMatchingLines) {
    Headers headers;
    headers.add("Header", "val_1");
    headers.add("Header", "val_2");
    headers.add("Header", "val_1");
    EXPECT_EQ(headers.size(), 3u);
    headers.remove("Header");
    EXPECT_EQ(headers.size(), 0u);
}

TEST (HeadersTest, GetAllReturnsValuesInOrder) {
    Headers headers;
    headers.add("Header", "val_1");
    headers.add("Header", "val_2");
    headers.add("Head", "v");
    headers.add("Header", "val_3");
    std::vector<std::string> strings = headers.getAll("header");
    std::vector<std::string> expectedStrings{"val_1", "val_2", "val_3"};
    EXPECT_EQ(strings, expectedStrings);
}

TEST (HeadersTest, GetCombinedJoinsValues) {
    Headers headers;
    headers.add("Header", "val_1");
    headers.add("Header", "val_2");
    headers.add("Head", "v");
    headers.add("Header", "val_3");
    auto combinedVals = headers.getCombined("Header");
    ASSERT_TRUE(combinedVals.has_value());
    EXPECT_EQ(*combinedVals, "val_1, val_2, val_3");
}

TEST (HeadersTest, GetCombinedMissingReturnsNullopt) {
    Headers headers;
    headers.add("Header", "val_1");
    headers.add("Header", "val_2");
    headers.add("Head", "v");
    headers.add("Header", "val_3");
    auto combinedVals = headers.getCombined("missing");
    ASSERT_FALSE(combinedVals.has_value());
    EXPECT_EQ(combinedVals, std::nullopt);
}

TEST (HeadersTest, EntriesPreserveOrder) {
    Headers headers;
    headers.add("Host", "localhost");
    headers.add("User-Agent", "Chrome/190.0");
    headers.add("Content-Length", "0");
    headers.add("Content-Type", "text");
    EXPECT_EQ(headers.entries().size(), 4u);
    const std::vector<Header> expectedEntries {{"host", "localhost"},
                                               {"user-agent", "Chrome/190.0"},
                                               {"content-length", "0"},
                                               {"content-type", "text"}};
    const std::vector<Header> entries(headers.entries().begin(), headers.entries().end());
    EXPECT_EQ(expectedEntries, entries);
}

TEST (HeadersTest, EmptyAndSize) {
    Headers headers;
    EXPECT_TRUE(headers.empty());
    EXPECT_EQ(headers.size(), 0u);
    EXPECT_EQ(headers.entries().size(), 0u);
    headers.add("header", "value");
    EXPECT_FALSE(headers.empty());
    EXPECT_EQ(headers.size(), 1u);
    EXPECT_EQ(headers.entries().size(), 1u);
}

TEST (HeadersTest, EmptyValuesAreAllowed) {
    Headers headers;
    headers.add("name", "");
    auto value = headers.get("name");
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(*value, "");
}

TEST(HeadersTest, SetOnMissingAdds) {
    Headers headers;
    headers.set("host", "newValue");
    EXPECT_TRUE(headers.contains("host"));
    auto value = headers.get("host");
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(*value, "newValue");
}

TEST(HeadersTest, RemoveMissingIsNoOp) {
    Headers headers;
    headers.add("header", "val");
    headers.remove("missing");
    EXPECT_EQ(headers.size(), 1u);
    EXPECT_TRUE(headers.contains("header"));
}
