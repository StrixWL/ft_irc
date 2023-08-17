#ifndef ARTICLE_H
#define ARTICLE_H
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <string.h>


class Article {
    private:
    std::string title;
    std::string content;
    public:
    int votes;
    Article(const std::string& title, const std::string& content);
    Article(const Article &src);
    std::string getTitle(void) const;
    std::string getContent(void) const;
    void upvote();
    void downvote() ;
};

#endif