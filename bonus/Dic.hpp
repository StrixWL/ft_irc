/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Dic.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmakboub <mmakboub@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/09 22:55:42 by mmakboub          #+#    #+#             */
/*   Updated: 2023/08/17 20:10:01 by mmakboub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DIC_HPP
#define DIC_HPP

#include <unistd.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <vector>
#include "Page.hpp"
#include <thread>
#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <string.h>

class Dic {
private:
    int sock;;
    void bot_Cmdpart();
    void  initialize_articles();
    std::vector<std::string> getArgs(std::string commandLine) ;
    void showCommande(std::string buff);
    void addCommande(std::string buff);
    void downVoteCommande(std::string buff);
    void upVoteCommande(std::string buff);
    void addArticleToPageWithTitle(const std::string& title, const std::string& articleContent);
    void handle_commande(std::string command);
    void helpCommand();
    int	ft_atoi(const char *str);
    std::vector<Page> pages;
public:
    Dic();
    void topics(std::string cmd);
    void start(void);
    void addArticleToPage(const Article& articles);
    void displayPages();

};


#endif