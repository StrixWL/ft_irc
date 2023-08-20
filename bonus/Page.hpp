
#include <unistd.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
#include "Article.hpp"
#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <string.h>
#include <map>
#include <string>


#ifndef PAGE_HPP
#define PAGE_HPP
class Page {
    private:
    public:
        std::map<std::string, std::string> articles;
        std::map<std::string, int> votes;
        Page();
        void addArticle(const Article& articles);
        std::string display();
        const std::map<std::string, std::string> &getArticle() const;
        int getNumArticles() const ;
};
#endif