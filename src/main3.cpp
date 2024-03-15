#include "../inc/Headers.hpp"
#include "../inc/Request.hpp"


int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	int   fd = open("request.txt", O_CREAT | O_RDWR, 0666);
	std::ofstream outFile;
    
    outFile.open("request.txt");
	outFile << "GET / HTTP/1.1\r\n\
Host:www.google.com\r\nUser-Agent: Mozilla/5.0\r\n\
Accept: text/html\r\nHost: ochko\r\nAccept-Language: en\r\nAccept-Encoding: gzip, deflate\r\nConnection: keep-alive\r\n\r\n\
hui pizda\npidor suka\ngovvnooooooooooooooooooooooooo\n";

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
/*
	try
	{
		if (argc == 2)
			Config config(argv[1]);
		else
			Config config;
	}
	catch (const std::exception &e)
	{
		std::cerr << "error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
		(void)argc;
	(void)argv;
	// if (argc != 2)
	// {
	// 	std::cerr << "Error. Invalid number of arguments." << std::endl;
	// 	std::cerr << "Usage: " << argv[0] << " [config file <.conf>]" << std::endl;
	// 	return 1;
	// }
	ServerInfo server_info;
	server_info.set_port(8080);
	server_info.set_type(SERVER_NODE);

	Director director;
	director.add_server_info(server_info);
	if(director.init_servers() < 0)
	{
		std::cerr << "Error initializing servers." << std::endl;
		return (1);
	}
	if (director.run_servers() < 0)
	{
		std::cerr << "Error." << std::endl;
		return (1);
	}
	return 0;
}
*/