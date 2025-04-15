#include "File.hpp"

File::File()
{
	_fileName = "";
	_filePath = "";
	_Sender = NULL;
	_Receiver = NULL;
}

File::File(std::string fileName, std::string filePath, Client* sender, Client* receiver) :
	_fileName(fileName), _filePath(filePath), _Sender(sender), _Receiver(receiver) {}

File::~File() {}

std::string	File::getFileName(void) const { return _fileName; };

std::string	File::getFilePath(void) const { return _filePath; };

Client*		File::getSender(void) const { return _Sender; };

Client*		File::getReceiver(void) const { return _Receiver; };

void	File::setFileName(std::string fileName) { _fileName = fileName; };

void	File::setFilePath(std::string filePath) { _filePath = filePath; };

void	File::setSender(Client* sender) { _Sender = sender; };

void	File::setReceiver(Client* receiver) { _Receiver = receiver; };


