/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krocha-h <krocha-h@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/26 15:26:49 by rocky             #+#    #+#             */
/*   Updated: 2025/03/26 12:40:57 by krocha-h         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#define RESET	"\e[0m"
#define RED		"\e[31m"
#define GREEN	"\e[32m"
#define YELLOW	"\e[33m"
#define BLUE	"\e[34m"
#define PURPLE	"\e[35m"
#define CYAN	"\e[36m"

/* Standard Input/Output */
#include <iostream>
#include <cstdio>

/* Standard Library */
#include <new>
#include <cmath>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <csignal>
#include <cassert>

/* String & Stream */
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <fstream>

/* Algorithms & Utilities */
#include <numeric>
#include <utility>
#include <iterator>
#include <algorithm>
#include <functional>

/* Containers */
#include <set>
#include <map>
#include <list>
#include <deque>
#include <queue>
#include <stack>
#include <bitset>
#include <vector>
#include <iterator>

/* Exception Handling */
#include <limits>
#include <signal.h>
#include <stdexcept>
#include <exception>

/* Time */
#include <ctime>

/* System & Networking */
#include <poll.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

void	        handleCtrlD(void);
unsigned int    getUnsignedInt(const std::string& prompt);
void            printStr(const std::string& text, const std::string& colour = "RESET");
int             isDigits(const std::string& s);
int             isValidPort(const std::string& s);