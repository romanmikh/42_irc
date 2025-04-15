#ifndef FILE_HPP
#define FILE_HPP

#include "irc.hpp"

class File
{
	private:
		std::string	_fileName;
		std::string	_filePath;
		std::string	_sender;
		std::string	_receiver;

	public:
		File();
		File(std::string fileName, std::string filePath, std::string sender, std::string receiver);
		~File();

		std::string	getFileName(void) const;
		std::string	getFilePath(void) const;
		std::string	getSender(void) const;
		std::string	getReceiver(void) const;

		void	setFileName(std::string fileName);
		void	setFilePath(std::string filePath);
		void	setSender(std::string sender);
		void	setReceiver(std::string receiver);

		// void	sendFile(void);
};

#endif