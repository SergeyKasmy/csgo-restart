// for std::cout & std::cerr
#include <iostream>
#include <poll.h>
// for fgets() and FILE
#include <stdio.h>
// for strlen()
#include <string.h>
// for stoi()
#include <string>
#include <sys/inotify.h>
#include <sys/syscall.h>
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

	{
		int fd = inotify_init();
		if(fd == -1)
		{
			std::cerr << "ERROR: Failed to create an inotify file descriptor";
			return 1;
		}
		else std::cout << "Successfully created an inotify file descriptor: " << fd << std::endl; 

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
				std::cerr << "ERROR: read() failed on inotify fd";
				return 3;
			}
			
			inotify_rm_watch(fd, wd);
		}

		close(fd);
	}


	{
		pid_t pid;
		{
			FILE *cmd = popen("pidof csgo_linux64", "r");
			std::cout << "FILE *cmd: " << cmd << std::endl; 

			char pid_c[10];
			fgets(pid_c, 10, cmd);

			pid = std::stoi(pid_c);
			pclose(cmd);
		}

		std::cout << "PID: " << pid << std::endl;
		
		int pidfd = syscall(SYS_pidfd_open, pid, 0);
		if(pidfd == -1)
		{
			std::cerr << "ERROR: pidfd_open() failed";
			return 4;
		}
		else std::cout << "Successfully got a pidfd: " << pidfd << std::endl;

		{
			struct pollfd pidfd_poll[1];

			pidfd_poll[0].fd = pidfd;
			pidfd_poll[0].events = POLLIN;

			poll(pidfd_poll, 1, -1);

			std::cout << "Process has stopped" << std::endl;
		}
	}

	return 0;
}
