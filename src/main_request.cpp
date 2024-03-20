#include "../inc/Request.hpp"
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	int   fd = open("request.txt", O_CREAT | O_RDWR, 0666);
	std::ofstream outFile;
    
    outFile.open("request.txt");
	outFile << "GET http://danil%20gutak@website%20super.com:8080/pat%20h2/?q=fi%20nd&p=2#hea%20der1 HTTP/1.1\r\n\
Host:www.google.com\r\nUser-Agent: Mozilla/5.0\r\n\
Accept: text/html\r\nHost: ochko\r\nAccept-Language: en\r\nAccept-Encoding: gzip, deflate\r\nConnection: keep-alive\r\n\r\nhuuuuuu";

	outFile.close();
	fd = open("request.txt", O_RDONLY);

	char *buf = new char[1000];

	int bufer_size = read(fd, buf, 1000);
	buf[bufer_size] = '\0';

	std::string str(buf);

	delete[] buf;
	Request req(str);
	std::cout << req;
}