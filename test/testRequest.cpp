#include <gtest/gtest.h>
#include <string>
#include "../inc/Request.hpp"
  // Include your URI parser header file

// Test case for parsing a valid URI with scheme, authority, path, query, and fragment
#define DEFAULT_method "GET"
#define DEFAULT_host "example.com"
#define DEFAULT_port "80"
#define DEFAULT_path "/path1/path2/"
#define DEFAULT_query "q=find&p=2"
#define DEFAULT_fragment "header1"
#define DEFAULT_userinfo "danilgutak"
#define DEFAULT_scheme "http"
#define DEFAULT_body "This is a body\n with multiple lines\n and some special characters: !@#$%^&*()_+{}|:\"<>?`-=[]\\;',./\n"
#define DEFAULT_headers "Host: example.com\r\nUser-Agent: Mozilla/5.0\r\nAccept: text/html\r\nAccept-Language: en\r\nAccept-Encoding: gzip, deflate\r\nConnection: keep-alive\r\n"


std::string create_string(std::string method, std::string scheme, std::string userinfo, std::string host, std::string port, std::string path, std::string query, std::string fragment, std::string headers, std::string body) {
  std::string str = method + " " + scheme + "://" + userinfo + "@" + host + ":" + port + path + "?" + query + "#" + fragment + " HTTP/1.1\r\n" + headers + "\r\n" + body;
  return str;
}

TEST(RequestTest, RequestDefaultTest) {
  std::string request = create_string(DEFAULT_method, DEFAULT_scheme, DEFAULT_userinfo, DEFAULT_host, DEFAULT_port, DEFAULT_path, DEFAULT_query, DEFAULT_fragment, DEFAULT_headers, DEFAULT_body);
  Request parser(request);

  EXPECT_EQ(parser.get_method(), "GET");
  EXPECT_EQ(parser.get_userinfo(), "danilgutak");
  EXPECT_EQ(parser.get_host(), "example.com");
  EXPECT_EQ(parser.get_port(), "80");
  EXPECT_EQ(parser.get_path(), "/path1/path2/");
  EXPECT_EQ(parser.get_query(), "q=find&p=2");
  EXPECT_EQ(parser.get_fragment(), "header1");
  EXPECT_EQ(parser.get_body(), "This is a body\n with multiple lines\n and some special characters: !@#$%^&*()_+{}|:\"<>?`-=[]\\;',./\n");

}

TEST(RequestTest, WrongMethodTest) {
  std::string request = create_string(
  "got",// method
  DEFAULT_scheme, // scheme
  DEFAULT_userinfo, // userinfo
  DEFAULT_host, // host
  DEFAULT_port, // port
  DEFAULT_path, // path
  DEFAULT_query, // query
  DEFAULT_fragment, // fragment
  DEFAULT_headers, // headers
  DEFAULT_body); // body

  EXPECT_THROW(Request parser(request),std::runtime_error);


}

TEST(RequestTest, WrongHostTest) {
  std::string request = create_string(
  DEFAULT_method,// method
  DEFAULT_scheme, // scheme
  DEFAULT_userinfo, // userinfo
  "229\r29", // host
  DEFAULT_port, // port
  DEFAULT_path, // path
  DEFAULT_query, // query
  DEFAULT_fragment, // fragment
  DEFAULT_headers, // headers
  DEFAULT_body); // body

  EXPECT_THROW(Request parser(request),std::runtime_error);


}

TEST(RequestTest, WrongPortTest) {
  std::string request = create_string(
  DEFAULT_method,// method
  DEFAULT_scheme, // scheme
  DEFAULT_userinfo, // userinfo
  DEFAULT_host, // host
  "100000a1", // port
  DEFAULT_path, // path
  DEFAULT_query, // query
  DEFAULT_fragment, // fragment
  DEFAULT_headers, // headers
  DEFAULT_body); // body

  EXPECT_THROW(Request parser(request),std::runtime_error);


}

