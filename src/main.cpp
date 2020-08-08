// for std::cout & std::cerr
#include <iostream>
// for stoi()
#include <string>
// for strlen()
#include <string.h>
#include <sys/inotify.h>
//for fgets() and FILE
#include <stdio.h>
#include <unistd.h>

#define EVENT_SIZE ( sizeof(struct inotify_event) )

// approx 16 event
#define BUF_SIZE ( 16 * (EVENT_SIZE + strlen(restart_file)) )

const char *restart_file = "/home/gray/.local/share/Steam/userdata/163030748/730/local/cfg/.restart-watch.cfg";

int main()
{
	std::cout << "EVENT_SIZE: " << EVENT_SIZE << '\n';
	std::cout << "Filename length: " << strlen(restart_file) << '\n';
	std::cout << "BUF_SIZE: " << BUF_SIZE << std::endl;

	int fd = inotify_init();
	if(fd == -1)
	{
		std::cerr << "ERROR: Failed to create an inotify file descriptor";
		return 1;
	}
	else std::cout << "Successfully created an inotify file descriptor: " << fd << std::endl; 

	while(true)
	{
		{
			int wd = inotify_add_watch(fd, restart_file, IN_MODIFY);
			if(wd == -1)
			{
				std::cerr << "ERROR: Failed to create a watch descriptor";
				return 2;
			}
			else std::cout << "Successfully created a watch descriptor: " << wd << std::endl; 

			char buf[BUF_SIZE];
			ssize_t len = read(fd, buf, BUF_SIZE);
			if(len == -1)
			{
				std::cerr << "ERROR: read() failed";
				return 3;
			}
			
			inotify_rm_watch(fd, wd);
		}

		{
			pid_t pid;
			{
				FILE *cmd = popen("pidof csgo_linux64", "r");
				std::cout << "FILE *cmd: " << cmd << std::endl; 

				char pid_c[10];
				fgets(pid_c, 10, cmd);

				pid = std::stoi(pid_c);
			}

			std::cout << "PID: " << pid << std::endl;
		}

		break;
	}

	close(fd);
	return 0;
}
