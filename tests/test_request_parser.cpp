#include <gtest/gtest.h>
#include <request_parser.hpp>
#include <http_request.hpp>
#include <http_response.hpp>
#include <iostream>


TEST(RequestParserTest, ParseValidGet) {
    std::string request{};
    request += "GET / HTTP/1.1\r\n";
    request += "Host: :/example.com\r\n";
    request += "User-Agent: Mozilla/5.0\r\n";
    request += "Accept: text/html\r\n\r\n";
    auto realParsedRequest = parseRequest(request);
    HttpRequest expectedParsedRequest{"GET", "/", "HTTP/1.1"};
    expectedParsedRequest.headers = {{"host", ":/example.com"}, 
                                     {"user-agent", "Mozilla/5.0"},
                                     {"accept", "text/html"}};
    ASSERT_TRUE(realParsedRequest.has_value());
    EXPECT_EQ(realParsedRequest->httpMethod, expectedParsedRequest.httpMethod);
    EXPECT_EQ(realParsedRequest->endpoint, expectedParsedRequest.endpoint);
    EXPECT_EQ(realParsedRequest->httpVersion, expectedParsedRequest.httpVersion);
    EXPECT_EQ(realParsedRequest->contentLength, expectedParsedRequest.contentLength);
    EXPECT_EQ(realParsedRequest->headers, expectedParsedRequest.headers);
    EXPECT_EQ(realParsedRequest->body, expectedParsedRequest.body);
}

TEST(RequestParserTest, ParseDifferentCaseHeaders) {
    std::string request{};
    request += "GET / HTTP/1.1\r\n";
    request += "HoSt: :/example.com\r\n";
    request += "USER-Agent: Mozilla/5.0\r\n";
    request += "ACCEPT: text/html\r\n\r\n";
    auto realParsedRequest = parseRequest(request);
    HttpRequest expectedParsedRequest{"GET", "/", "HTTP/1.1"};
    expectedParsedRequest.headers = {{"host", ":/example.com"}, 
                                     {"user-agent", "Mozilla/5.0"},
                                     {"accept", "text/html"}};
    ASSERT_TRUE(realParsedRequest.has_value());
    EXPECT_EQ(realParsedRequest->httpMethod, expectedParsedRequest.httpMethod);
    EXPECT_EQ(realParsedRequest->endpoint, expectedParsedRequest.endpoint);
    EXPECT_EQ(realParsedRequest->httpVersion, expectedParsedRequest.httpVersion);
    EXPECT_EQ(realParsedRequest->contentLength, expectedParsedRequest.contentLength);
    EXPECT_EQ(realParsedRequest->headers, expectedParsedRequest.headers);
    EXPECT_EQ(realParsedRequest->body, expectedParsedRequest.body);
}

TEST(RequestParserTest, ParseValidPost) {
    std::string request{};
    request += "POST / HTTP/1.1\r\n";
    request += "Host: localhost\r\n";
    request += "User-Agent: Chrome/16.0\r\n";
    request += "Accept: text/plain\r\n";
    request += "Content-Length: 12\r\n";
    request += "\r\n";
    request += "Body to POST";
    auto realParsedRequest = parseRequest(request);
    HttpRequest expectedParsedRequest{"POST", "/", "HTTP/1.1"};
    expectedParsedRequest.headers = {{"host", "localhost"}, 
                                     {"user-agent", "Chrome/16.0"},
                                     {"accept", "text/plain"},
                                     {"content-length", "12"}};
    expectedParsedRequest.contentLength = 12;
    expectedParsedRequest.body = "Body to POST";
    ASSERT_TRUE(realParsedRequest.has_value());
    EXPECT_EQ(realParsedRequest->httpMethod, expectedParsedRequest.httpMethod);
    EXPECT_EQ(realParsedRequest->endpoint, expectedParsedRequest.endpoint);
    EXPECT_EQ(realParsedRequest->httpVersion, expectedParsedRequest.httpVersion);
    EXPECT_EQ(realParsedRequest->contentLength, expectedParsedRequest.contentLength);
    EXPECT_EQ(realParsedRequest->headers, expectedParsedRequest.headers);
    EXPECT_EQ(realParsedRequest->body, expectedParsedRequest.body);
}

TEST(RequestParserTest, ParsePartialBody) {
    std::string request{};
    request += "POST / HTTP/1.1\r\n";
    request += "Host: localhost\r\n";
    request += "User-Agent: Chrome/16.0\r\n";
    request += "Accept: text/plain\r\n";
    request += "Content-Length: 12\r\n";
    request += "\r\n";
    request += "Body to";
    auto realParsedRequest = parseRequest(request);
    HttpRequest expectedParsedRequest{"POST", "/", "HTTP/1.1"};
    expectedParsedRequest.headers = {{"host", "localhost"}, 
                                     {"user-agent", "Chrome/16.0"},
                                     {"accept", "text/plain"},
                                     {"content-length", "12"}};
    expectedParsedRequest.contentLength = 12;
    expectedParsedRequest.body = "Body to";
    ASSERT_TRUE(realParsedRequest.has_value());
    EXPECT_EQ(realParsedRequest->httpMethod, expectedParsedRequest.httpMethod);
    EXPECT_EQ(realParsedRequest->endpoint, expectedParsedRequest.endpoint);
    EXPECT_EQ(realParsedRequest->httpVersion, expectedParsedRequest.httpVersion);
    EXPECT_EQ(realParsedRequest->contentLength, expectedParsedRequest.contentLength);
    EXPECT_EQ(realParsedRequest->headers, expectedParsedRequest.headers);
    EXPECT_EQ(realParsedRequest->body, expectedParsedRequest.body);
}

TEST(RequestParserTest, ParseZeroContentLength) {
    std::string request{};
    request += "POST / HTTP/1.1\r\n";
    request += "Host: localhost\r\n";
    request += "User-Agent: Chrome/16.0\r\n";
    request += "Accept: text/plain\r\n";
    request += "Content-Length: 0\r\n";
    request += "\r\n";
    auto realParsedRequest = parseRequest(request);
    HttpRequest expectedParsedRequest{"POST", "/", "HTTP/1.1"};
    expectedParsedRequest.headers = {{"host", "localhost"}, 
                                     {"user-agent", "Chrome/16.0"},
                                     {"accept", "text/plain"},
                                     {"content-length", "0"}};
    ASSERT_TRUE(realParsedRequest.has_value());
    EXPECT_EQ(realParsedRequest->httpMethod, expectedParsedRequest.httpMethod);
    EXPECT_EQ(realParsedRequest->endpoint, expectedParsedRequest.endpoint);
    EXPECT_EQ(realParsedRequest->httpVersion, expectedParsedRequest.httpVersion);
    EXPECT_EQ(realParsedRequest->contentLength, expectedParsedRequest.contentLength);
    EXPECT_EQ(realParsedRequest->headers, expectedParsedRequest.headers);
    EXPECT_EQ(realParsedRequest->body, expectedParsedRequest.body);
}

TEST (RequestParserTest, ParseExtraWords) {
    std::string request{};
    request += "GET / HTTP/1.1 garbageeee\r\n";
    std::stringstream buffer;
    std::streambuf* old_cerr = std::cerr.rdbuf(buffer.rdbuf());
    auto realParsedRequest = parseRequest(request);
    std::cerr.rdbuf(old_cerr);
    EXPECT_EQ(buffer.str(), "Extra symbols in request: garbageeee\n");
    EXPECT_EQ(realParsedRequest, std::nullopt);
}

TEST (RequestParserTest, ParseWrongHTTPVersion) {
    std::string request{};
    request += "GET / HTTP/0.1\r\n";
    std::stringstream buffer;
    std::streambuf* old_cerr = std::cerr.rdbuf(buffer.rdbuf());
    auto realParsedRequest = parseRequest(request);
    std::cerr.rdbuf(old_cerr);
    EXPECT_EQ(buffer.str(), "Invalid HTTP request version: HTTP/0.1\n");
    EXPECT_EQ(realParsedRequest, std::nullopt);
}

TEST (RequestParserTest, ParseWrongLF) {
    std::string request{};
    request += "GET / HTTP/1.1\r\n";
    request += "Host: :/example.com\n";
    request += "User-Agent: Mozilla/5.0\n";
    request += "Accept: text/html\r\n";
    request += "\r\n";
    std::stringstream buffer;
    std::streambuf* old_cerr = std::cerr.rdbuf(buffer.rdbuf());
    auto realParsedRequest = parseRequest(request);
    std::cerr.rdbuf(old_cerr);
    EXPECT_EQ(buffer.str(), "Invalid CRLF\n");
    EXPECT_EQ(realParsedRequest, std::nullopt);
}

TEST (RequestParserTest, ParseNoColonInHeader) {
    std::string request{};
    request += "GET / HTTP/1.1\r\n";
    request += "Host: :/example.com\r\n";
    request += "User-AgentMozilla/5.0\r\n";
    request += "Accept: text/html\r\n";
    request += "Content-Length: 0\r\n";
    request += "\r\n";
    std::stringstream buffer;
    std::streambuf* old_cerr = std::cerr.rdbuf(buffer.rdbuf());
    auto realParsedRequest = parseRequest(request);
    std::cerr.rdbuf(old_cerr);
    EXPECT_EQ(buffer.str(), "Header without a colon is invalid\n");
    EXPECT_EQ(realParsedRequest, std::nullopt);
}

TEST (RequestParserTest, ParseEmptyHeader) {
    std::string request{};
    request += "GET / HTTP/1.1\r\n";
    request += "Host: :/example.com\r\n";
    request += ": content...\r\n";
    request += "Content-Length: 0\r\n";
    request += "\r\n";
    std::stringstream buffer;
    std::streambuf* old_cerr = std::cerr.rdbuf(buffer.rdbuf());
    auto realParsedRequest = parseRequest(request);
    std::cerr.rdbuf(old_cerr);
    EXPECT_EQ(buffer.str(), "Empty header\n");
    EXPECT_EQ(realParsedRequest, std::nullopt);
}

TEST (RequestParserTest, ParseInvalidHeaderFormat) {
    std::string request{};
    request += "GET / HTTP/1.1\r\n";
    request += "Host: :/example.com\r\n";
    request += "User-Agent : Mozilla/5.0\r\n";
    request += "Content-Length: 0\r\n";
    request += "\r\n";
    std::stringstream buffer;
    std::streambuf* old_cerr = std::cerr.rdbuf(buffer.rdbuf());
    auto realParsedRequest = parseRequest(request);
    std::cerr.rdbuf(old_cerr);
    EXPECT_EQ(buffer.str(), "Invalid header format\n");
    EXPECT_EQ(realParsedRequest, std::nullopt);
}

TEST (RequestParserTest, ParseHeaderWithInvalidSpecialSymbols) {
    std::string request{};
    request += "GET / HTTP/1.1\r\n";
    request += "Ho\tst: :/example.com\r\n";
    request += "User-Agent: Mozilla/5.0\r\n";
    request += "Content-Length: 0\r\n";
    request += "\r\n";
    std::stringstream buffer;
    std::streambuf* old_cerr = std::cerr.rdbuf(buffer.rdbuf());
    auto realParsedRequest = parseRequest(request);
    std::cerr.rdbuf(old_cerr);
    EXPECT_EQ(buffer.str(), "Invalid header format\n");
    EXPECT_EQ(realParsedRequest, std::nullopt);
}

TEST (RequestParserTest, ParseHeaderWithValidSpecialSymbols) {
    std::string request{};
    request += "GET / HTTP/1.1\r\n";
    request += "Host: :/example.com\r\n";
    request += "User-Agent%: Mozilla/5.0\r\n";
    request += "Content-Leng_9012th#1*: 0\r\n";
    request += "\r\n";
    auto realParsedRequest = parseRequest(request);
    HttpRequest expectedParsedRequest = HttpRequest("GET", "/", "HTTP/1.1");
    expectedParsedRequest.headers = {{"host", ":/example.com"},
                                     {"user-agent%", "Mozilla/5.0"},
                                     {"content-leng_9012th#1*", "0"}};
    ASSERT_TRUE(realParsedRequest.has_value());
    EXPECT_EQ(realParsedRequest->headers, expectedParsedRequest.headers);
}

TEST (RequestParserTest, ParseEmptyHeaderContent) {
    std::string request{};
    request += "GET / HTTP/1.1\r\n";
    request += "Host: :/example.com\r\n";
    request += "User-Agent: \r\n";
    request += "Content-Length: 0\r\n";
    request += "\r\n";
    std::stringstream buffer;
    std::streambuf* old_cerr = std::cerr.rdbuf(buffer.rdbuf());
    auto realParsedRequest = parseRequest(request);
    std::cerr.rdbuf(old_cerr);
    EXPECT_EQ(buffer.str(), "Empty header content\n");
    EXPECT_EQ(realParsedRequest, std::nullopt);
}

TEST (RequestParserTest, ParseEndOfHeadersNotFound) {
    std::string request{};
    request += "GET / HTTP/1.1\r\n";
    request += "Host: :/example.com\r\n";
    request += "User-Agent: Mozilla/5.0\r\n";
    request += "Accept: text/html\r\n";
    std::stringstream buffer;
    std::streambuf* old_cerr = std::cerr.rdbuf(buffer.rdbuf());
    auto realParsedRequest = parseRequest(request);
    std::cerr.rdbuf(old_cerr);
    EXPECT_EQ(buffer.str(), "End of headers not found\n");
    EXPECT_EQ(realParsedRequest, std::nullopt);
}

TEST(RequestParserTest, ParseDoubleContentLength) {
    std::string request{};
    request += "POST / HTTP/1.1\r\n";
    request += "Host: localhost\r\n";
    request += "User-Agent: Chrome/16.0\r\n";
    request += "Accept: text/plain\r\n";
    request += "Content-Length: 12\r\n";
    request += "Content-Length: 20\r\n";
    request += "\r\n";
    request += "Body to POST\r\n\r\n";
    std::stringstream buffer;
    std::streambuf* old_cerr = std::cerr.rdbuf(buffer.rdbuf());
    auto realParsedRequest = parseRequest(request);
    std::cerr.rdbuf(old_cerr);
    EXPECT_EQ(buffer.str(), "Duplicated Content-Length header is invalid\n");
    EXPECT_EQ(realParsedRequest, std::nullopt);
}

TEST(RequestParserTest, ParseDoubleHost) {
    std::string request{};
    request += "POST / HTTP/1.1\r\n";
    request += "Host: localhost\r\n";
    request += "Host: malwarehost\r\n";
    request += "User-Agent: Chrome/16.0\r\n";
    request += "Accept: text/plain\r\n";
    request += "Content-Length: 12\r\n";
    request += "\r\n";
    request += "Body to POST\r\n\r\n";
    std::stringstream buffer;
    std::streambuf* old_cerr = std::cerr.rdbuf(buffer.rdbuf());
    auto realParsedRequest = parseRequest(request);
    std::cerr.rdbuf(old_cerr);
    EXPECT_EQ(buffer.str(), "Duplicated Host header is invalid\n");
    EXPECT_EQ(realParsedRequest, std::nullopt);
}

TEST(RequestParserTest, ParseDoubleUserAgent) {
    std::string request{};
    request += "POST / HTTP/1.1\r\n";
    request += "Host: localhost\r\n";
    request += "User-Agent: Chrome/16.0\r\n";
    request += "User-Agent: malwareagent\r\n";
    request += "Accept: text/plain\r\n";
    request += "Content-Length: 12\r\n";
    request += "\r\n";
    request += "Body to POST\r\n\r\n";
    std::stringstream buffer;
    std::streambuf* old_cerr = std::cerr.rdbuf(buffer.rdbuf());
    auto realParsedRequest = parseRequest(request);
    std::cerr.rdbuf(old_cerr);
    EXPECT_EQ(buffer.str(), "Duplicated User-Agent header is invalid\n");
    EXPECT_EQ(realParsedRequest, std::nullopt);
}

TEST(RequestParserTest, ParseTransferEncoding) {
    std::string request{};
    request += "POST / HTTP/1.1\r\n";
    request += "Host: localhost\r\n";
    request += "User-Agent: Chrome/16.0\r\n";
    request += "Accept: text/plain\r\n";
    request += "Content-Length: 12\r\n";
    request += "Transfer-Encoding: chunked\r\n";
    request += "\r\n";
    request += "Body to POST\r\n\r\n";
    std::stringstream buffer;
    std::streambuf* old_cerr = std::cerr.rdbuf(buffer.rdbuf());
    auto realParsedRequest = parseRequest(request);
    std::cerr.rdbuf(old_cerr);
    EXPECT_EQ(buffer.str(), "Transfer-Encoding => ban\n");
    EXPECT_EQ(realParsedRequest, std::nullopt);
}

TEST(RequestParserTest, ParseInvalidContentLengthFormat) {
    std::string request{};
    request += "POST / HTTP/1.1\r\n";
    request += "Host: localhost\r\n";
    request += "User-Agent: Chrome/16.0\r\n";
    request += "Accept: text/plain\r\n";
    request += "Content-Length: 12s\r\n";
    request += "\r\n";
    std::stringstream buffer;
    std::streambuf* old_cerr = std::cerr.rdbuf(buffer.rdbuf());
    auto realParsedRequest = parseRequest(request);
    std::cerr.rdbuf(old_cerr);
    EXPECT_EQ(buffer.str(), "Invalid Content-Length value\n");
    EXPECT_EQ(realParsedRequest, std::nullopt);
}

TEST(RequestParserTest, ParseInvalidContentLengtSize) {
    std::string request{};
    request += "POST / HTTP/1.1\r\n";
    request += "Host: localhost\r\n";
    request += "User-Agent: Chrome/16.0\r\n";
    request += "Accept: text/plain\r\n";
    request += "Content-Length: 99999999999999999999999999999\r\n";
    request += "\r\n";
    std::stringstream buffer;
    std::streambuf* old_cerr = std::cerr.rdbuf(buffer.rdbuf());
    auto realParsedRequest = parseRequest(request);
    std::cerr.rdbuf(old_cerr);
    EXPECT_EQ(buffer.str(), "Getting Content-Length from stream failed\n");
    EXPECT_EQ(realParsedRequest, std::nullopt);
}

TEST(RequestParserTest, ParseRealBodySizeIsBiggerThanContentLength) {
    std::string request{};
    request += "POST / HTTP/1.1\r\n";
    request += "Host: localhost\r\n";
    request += "User-Agent: Chrome/16.0\r\n";
    request += "Accept: text/plain\r\n";
    request += "Content-Length: 3\r\n";
    request += "\r\n";
    request += "body";
    std::stringstream buffer;
    std::streambuf* old_cerr = std::cerr.rdbuf(buffer.rdbuf());
    auto realParsedRequest = parseRequest(request);
    std::cerr.rdbuf(old_cerr);
    EXPECT_EQ(buffer.str(), "Real body size is bigger than the Content-Length\n");
    EXPECT_EQ(realParsedRequest, std::nullopt);
}

TEST(RequestParserTest, ParseBodyWithNoContentLength) {
    std::string request{};
    request += "POST / HTTP/1.1\r\n";
    request += "Host: localhost\r\n";
    request += "\r\n";
    request += "body";
    std::stringstream buffer;
    std::streambuf* old_cerr = std::cerr.rdbuf(buffer.rdbuf());
    auto realParsedRequest = parseRequest(request);
    std::cerr.rdbuf(old_cerr);
    EXPECT_EQ(buffer.str(), "Need Content-Length\n");
    EXPECT_EQ(realParsedRequest, std::nullopt);
}