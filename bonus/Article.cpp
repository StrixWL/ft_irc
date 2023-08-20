/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Article.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmakboub <mmakboub@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/15 23:03:15 by mmakboub          #+#    #+#             */
/*   Updated: 2023/08/17 18:44:19 by mmakboub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Article.hpp"

Article::Article(const std::string& title, const std::string& content): title(title), content(content), votes(0) {}

std::string Article::getTitle() const {
    return title;
}
Article::Article(const Article &src)
{
    this->title = src.title;
    this->content = src.content;
    this->votes = src.votes;
}

std::string Article::getContent() const {
    return content;
}
