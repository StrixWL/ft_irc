/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Dic.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmakboub <mmakboub@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/09 23:14:25 by mmakboub          #+#    #+#             */
/*   Updated: 2023/08/18 00:01:34 by mmakboub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include"Dic.hpp"
#include <sstream>

const int PORT = 6667;
const char* HOST = "127.0.0.1";
const std::string NICKNAME = "mmakboub"; 
const std::string CHANNEL = "#myChannel";
const std::string PASSWORD = "123";

Dic::Dic() 
{
    this->sock = -1;
}
void Dic::addArticleToPage(const Article& articles)
{
    size_t i = 0;
    size_t len = pages.size();
    bool check = false;
    while (i < len)
    {
        if (pages[i].getNumArticles() < 10)
        {
            check = true;
            pages[i].addArticle(articles);
            break;
        }
        i++;
    }
    if (!check)
    {
        pages.push_back(Page());
        pages[i].addArticle(articles);
    }
    
}
void Dic::bot_Cmdpart()
{
    std::string passCmd = "PASS " + PASSWORD +"\r\n";
    std::string nicknameCmd = "NICK "  + NICKNAME + "\r\n";
    std::string userCmd = "USER " + NICKNAME + " hostname servername :realname Bot\r\n";
    std::string joinCmd = "JOIN " + CHANNEL + "\r\n";
    send(this->sock, passCmd.c_str(), passCmd.size(), 0);
    send(this->sock, nicknameCmd.c_str(), nicknameCmd.size(), 0);
    send(this->sock, userCmd.c_str(), userCmd.size(), 0);
    send(this->sock, joinCmd.c_str(), joinCmd.size(), 0);
}
void Dic::initialize_articles()
{   
    this->addArticleToPage(Article("What is Array Decay", "The loss of type and dimensions of an array is known as decay of an array.This generally occurs when we pass the array into function by value or pointer. What it does is, it sends first address to the array which is a pointer, hence the size of array is not the original one, but the one occupied by the pointer in the memory.\r\n"));
   this->addArticleToPage(Article("Can Static Functions Be Virtual in C++?", "In C++, a static member function of a class cannot be virtual. Virtual functions are invoked when you have a pointer or reference to an instance of a class. Static functions aren’t tied to the instance of a class but they are tied to the class. C++ doesn’t have pointers-to-class, so there is no scenario in which you could invoke a static function virtually.\r\n"));
   this->addArticleToPage(Article("What is malloc ?", "The malloc or memory allocation method in C is used to dynamically allocate a single large block of memory with the specified size. It returns a pointer of type void which can be cast into a pointer of any form. It doesn’t Initialize memory at execution time so that it has initialized each block with the default garbage value initially.\r\n"));
   this->addArticleToPage(Article("What is a Responsive Website", "A responsive website, simply improves the viewing experience; it fits into any device irrespective of resolution. It means a responsive website virtually fits into any device that uses a web browser. Other than being compatible with a unique variety of resolutions, a responsive website works flawlessly across a range of devices, including smartphones, tablets and smartphones.\r\n"));

}
void Dic::helpCommand() {
    std::string helpMessage;
    helpMessage += "/help -- List available commands \r\n";
    helpMessage += "PRIVMSG #myChannel :/topics [Page index] -- List a page \r\n";
    helpMessage += "PRIVMSG #myChannel :/show [title] -- Show a specific topic \r\n";
    helpMessage += "PRIVMSG #myChannel :/upvote [title] -- Upvote a topic \r\n";
    helpMessage += "PRIVMSG #myChannel :/downvote [title] -- Downvote a topic \r\n";
    helpMessage += "PRIVMSG #myChannel :/add <[Topic title]> [Topic content] \r\n";
    std::string invitmsg = "PRIVMSG #myChannel :"  + helpMessage + "\r\n";
    send(this->sock, invitmsg.c_str(), invitmsg.length(), 0);
}
std::vector<std::string> Dic::getArgs(std::string commandLine) {
	std::vector<std::string> args;
	while (commandLine.length()) {
		std::string nextArg;
		if (commandLine[0] == ':') {
			commandLine.erase(0, 1);
			nextArg = commandLine;
		}
		else
			nextArg = commandLine.substr(0, commandLine.find(" "));
		commandLine.erase(0, nextArg.length());
		while (commandLine[0] == ' ')
			commandLine.erase(0, 1);
		if (nextArg.length())
			args.push_back(nextArg);
	}
	return args;
}

void Dic::showCommande(std::string command) {
    while (command[0] == ' ')
        command.erase(0, 1);
    if (command.substr(0, command.find(" ")) != "!show") {
         std::string msg = "PRIVMSG #myChannel : invalide argument\r\n";
        send(this->sock, msg.c_str(),msg.length(), 0);
        return;
    }
    command.erase(0, command.find(" "));
    while (command[0] == ' ')
        command.erase(0, 1);
    std::string title = command;
    for (size_t i = 0; i < pages.size() ; i++)
    {
        std::map<std::string, std::string>::const_iterator article = pages[i].getArticle().find(title);
        if (article != pages[i].getArticle().end())
        {
            std::string titleMessage = "PRIVMSG #myChannel :" + title + "\r\n";
            send(this->sock, titleMessage.c_str(), titleMessage.length(), 0);

            std::string articleMessage = "PRIVMSG #myChannel :" + article->second + "\r\n";
            send(this->sock, articleMessage.c_str(), articleMessage.length(), 0);

            
            std::string votesMessage = "PRIVMSG #myChannel :Votes: " + std::to_string(pages[i].votes[title]) + "\r\n";
            send(this->sock, votesMessage.c_str(), votesMessage.length(), 0);
            return;
        }
    }
    std::string formattedMessage = "PRIVMSG #myChannel :doesn't exist\n";
    send(this->sock, formattedMessage.c_str(), formattedMessage.length(), 0);

}  

void Dic::addArticleToPageWithTitle(const std::string& title, const std::string& articleContent) {
    Article newArticle(title, articleContent); 

    size_t i = 0;
    size_t len = pages.size();
    bool check = false;
    while (i < len) {
        if (pages[i].getNumArticles() < 10) {
            check = true;
            pages[i].addArticle(newArticle);
            break;
        }
        i++;
    }

    if (!check) {
        pages.push_back(Page());
        pages[i].addArticle(newArticle);
    }
}
void Dic::addCommande(std::string buff) {
     while (buff[0] == ' ')
        buff.erase(0, 1);
    if (buff.substr(0, buff.find(" ")) != "!add") {
       std::string msg = "PRIVMSG #myChannel : invalide argument\r\n";
        send(this->sock, msg.c_str(),msg.length(), 0);
        return ;
    }
    buff.erase(0, buff.find(" "));
    while (buff[0] == ' ')
        buff.erase(0, 1);
    if (buff[0] != '<') {
         std::string msg = "PRIVMSG #myChannel : invalide argument\r\n";
        send(this->sock, msg.c_str(),msg.length(), 0);
        return ;
    }
    buff.erase(0, 1);
    if (buff.find(">") == std::string::npos) {
        std::cerr << "> not found" << std::endl;
        return;
    }
    std::string title = buff.substr(0, buff.find(">"));
    buff.erase(0, title.length() + 1);
    while (buff[0] == ' ')
        buff.erase(0, 1);
    std::string content = buff;
    addArticleToPageWithTitle(title, content);
    std::string msg = "PRIVMSG #myChannel : article has been added succecfuly \r\n";
}
int	Dic::ft_atoi(const char *str)

{
	int	i;
	int	result;
	int	sign;

	sign = 1;
	result = 0;
	i = 0;
	while (str[i] == 32 || (str[i] >= 9 && str[i] <= 13))
		i++;
	while (str[i] == '+' || str[i] == '-')
	{
		if (str[i] == '-')
			sign = sign * (-1);
		i++;
		if (str[i] == '+' || str[i] == '-')
			return (0);
	}
	while (str[i] >= '0' && str[i] <= '9')
	{
		result = (result * 10) + (str[i] - 48);
		i++;
	}
	return (sign * result);
}


#include <cassert>
void Dic::topics(std::string cmd)
{
    std::string invitmsg;
    size_t i;
    std::vector<std::string> args = getArgs(cmd);
    std::string formattedMessage;
    if (args.size() != 2 || args[0] != "!topics") {
        std::string msg = "PRIVMSG #myChannel : invalide argument\r\n";
        send(this->sock, msg.c_str(),msg.length(), 0);
        return;
    }
    i = ft_atoi(args[1].c_str());
    if (pages.size() <= i)
    {
        std::string msg = "PRIVMSG #myChannel : invalide page\r\n";
        send(this->sock, msg.c_str(),msg.length(), 0);
        return ;
    }
     invitmsg = "PRIVMSG #myChannel :" + pages[i].display() + "\r\n";
    send(this->sock, invitmsg.c_str(), invitmsg.length(), 0);
        // assert(false);
}
void Dic::displayPages() {
    
        for (size_t i = 0; i < pages.size(); i++)
        {
            pages[i].display();
        }
    }
void Dic::downVoteCommande(std::string buff)
{
    while (buff[0] == ' ')
        buff.erase(0, 1);
    if (buff.substr(0, buff.find(" ")) != "!downvote") {
         std::string msg = "PRIVMSG #myChannel : invalide argument\r\n";
        send(this->sock, msg.c_str(),msg.length(), 0);
        return;
    }
    buff.erase(0, buff.find(" "));
    while (buff[0] == ' ')
        buff.erase(0, 1);
    std::string title = buff;
    for (std::vector<Page>::iterator it = pages.begin(); it != pages.end(); it++) {
        if (it->articles[title].length()) {
            it->votes[title]--;
        }
    }
}
void Dic::upVoteCommande(std::string buff){
   while (buff[0] == ' ')
        buff.erase(0, 1);
    if (buff.substr(0, buff.find(" ")) != "!upvote") {
         std::string msg = "PRIVMSG #myChannel : invalide argument\r\n";
        send(this->sock, msg.c_str(),msg.length(), 0);
        return;
    }
    buff.erase(0, buff.find(" "));
    while (buff[0] == ' ')
        buff.erase(0, 1);
    std::string title = buff;
    for (std::vector<Page>::iterator it = pages.begin(); it != pages.end(); it++) {
        if (it->articles[title].length()) {
            it->votes[title]++;
        }
    
}
}
void Dic::handle_commande(std::string command)
{
    if (command[0] != '!')
        return ;
    if (command == "!help") 
        helpCommand();
    else if (command.find("!topics") != std::string::npos)
        topics(command);
    else if (command.find("!show") != std::string::npos) 
        showCommande(command);
    else if (command.find("!add") != std::string::npos) 
       addCommande(command);
    else if (command.find("!downvote") != std::string::npos)        
       downVoteCommande(command);
    else if(command.find("!upvote") != std::string::npos) 
        upVoteCommande(command);
    else 
    {
        std::string msg = "PRIVMSG #myChannel :ERROR -> missing command\r\n";
        send(this->sock, msg.c_str(), msg.length(), 0);
    }
}

std::string removeSpace(std::string cmd)
{
    std::string command = "";
    size_t i = 0;
    size_t len = cmd.length();
    while (cmd[i] == ' ' && i < len)
        i++;
    while (cmd[len -1] == ' ' && len >= 0)
        len--;

    return cmd.substr(i , len - i);
}

void Dic::start(void) {
    this->sock = socket(AF_INET, SOCK_STREAM, 0);
    char buff[1024];
    if (this->sock< 0) {
        std::cerr << "can't creat a socket" << std::endl;
        exit (0);
    }
    struct sockaddr_in serverinf;
    serverinf.sin_family = AF_INET;
    serverinf.sin_port = htons(PORT);
    if (inet_pton(AF_INET, HOST, &serverinf.sin_addr) <= 0) {
		std::cerr << "Invalid address"<< std::endl;
		return ;
	}
    if (connect(this->sock, (struct sockaddr*)&serverinf, sizeof(serverinf)) < 0) {
        std::cerr << "connexion to server failed" << std::endl;
        exit (0);
    }
    
    bot_Cmdpart();
    size_t pos;
    std::string command;
    initialize_articles();

        while(true)
        {
            memset(buff, 0, sizeof(buff));
            recv(sock, buff, sizeof(buff), 0);
            pos = std::string(buff).find(":", std::string(buff).find("PRIVMSG #"));
            if (pos != std::string::npos)
            {
                command = removeSpace(std::string(buff).substr(pos + 1, std::string(buff).find("\r\n") - pos - 1));
                handle_commande(command);  
   
            }
    }
    close(this->sock);
}

int main() {
    Dic bot;
    bot.start();
    return 0;
}