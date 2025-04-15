#ifndef FILE_HPP
#define FILE_HPP


#include "irc.hpp"

class File
{
	private:
		std::string	_fileName;
		std::string	_filePath;
		Client*		_Sender;
		Client*		_Receiver;

	public:
		File();
		File(std::string fileName, std::string filePath, Client* sender, Client* receiver);
		~File();

		std::string	getFileName(void) const;
		std::string	getFilePath(void) const;
		Client*		getSender(void) const;
		Client*		getReceiver(void) const;

		void	setFileName(std::string fileName);
		void	setFilePath(std::string filePath);
		void	setSender(Client* sender);
		void	setReceiver(Client* receiver);

		// void	sendFile(void);
};

#endif