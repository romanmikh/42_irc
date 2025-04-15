#include "File.hpp"

File::File()
{
	_fileName = "";
	_filePath = "";
}

File::File(std::string fileName, std::string filePath, std::string sender, std::string receiver) :
	_fileName(fileName), _filePath(filePath), _sender(sender), _receiver(receiver) {}

File::~File() {}

std::string	File::getFileName(void) const { return _fileName; };

std::string	File::getFilePath(void) const { return _filePath; };

std::string	File::getSender(void) const { return _sender; };

std::string	File::getReceiver(void) const { return _receiver; };

void	File::setFileName(std::string fileName) { _fileName = fileName; };

void	File::setFilePath(std::string filePath) { _filePath = filePath; };

void	File::setSender(std::string sender) { _sender = sender; };

void	File::setReceiver(std::string receiver) { _receiver = receiver; };


