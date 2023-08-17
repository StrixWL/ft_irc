/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Page.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmakboub <mmakboub@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/15 23:07:12 by mmakboub          #+#    #+#             */
/*   Updated: 2023/08/17 20:09:54 by mmakboub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Page.hpp"
Page::Page() {}
void Page::addArticle(const Article& article) {
        articles[article.getTitle()] = article.getContent();
        votes[article.getTitle()] = 0;
}

#include <cassert>
std::string Page::display() {    
    std::string topics;
    topics.clear();
    topics = "Page Contents: \r\n";
    for (std::map<std::string , std::string>::iterator it = articles.begin(); it != articles.end(); it++) {
        topics += "PRIVMSG #myChannel :Title: " + it->first + "\r\n";
    }
    return topics;
}
int Page::getNumArticles() const {
    return articles.size();
}

bool Page::isFull() const {
    return articles.size() >= 10;
}

bool Page::isEmpty() const {
    return articles.empty();
}

const std::map<std::string, std::string>& Page::getArticle() const
{
   return this->articles;
}
