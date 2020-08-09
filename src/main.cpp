// for std::cout & std::cerr
#include <iostream>
#include <poll.h>
// for fgets() and FILE
#include <stdio.h>
// for system()
#include <stdlib.h>
// for strlen()
#include <string.h>
// for stoi()
#include <string>
#include <sys/inotify.h>
#include <sys/syscall.h>
#include <unistd.h>

#define EVENT_SIZE ( sizeof(struct inotify_event) )
// approx 16 event
#define BUF_SIZE ( 16 * (EVENT_SIZE + restart_file.length()) )

const bool DEBUG = true;

template <class... Args>
void debug_print(Args... args)
{
	if(DEBUG) (std::cout << ... << args) << std::endl;
}

int main(int argc, char **argv)
{
	debug_print("Debug output enabled");

	if(argc < 2)
	{
		std::cerr << "ERROR: filename not provided";
		return 5;
	}
	std::string restart_file = argv[1];

	debug_print("EVENT_SIZE: ", EVENT_SIZE);
	debug_print("Filename: ", restart_file);
	debug_print("Filename length: ", restart_file.length());
	debug_print("BUF_SIZE: ", BUF_SIZE);

	{
		int fd = inotify_init();
		if(fd == -1)
		{
			std::cerr << "ERROR: Failed to create an inotify file descriptor";
			return 1;
		}
		else debug_print("Successfully created an inotify file descriptor: ", fd); 

		{
			int wd = inotify_add_watch(fd, restart_file.c_str(), IN_MODIFY);
			if(wd == -1)
			{
				std::cerr << "ERROR: Failed to create a watch descriptor";
				return 2;
			}
			else debug_print("Successfully created a watch descriptor: ",  wd); 

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

			char pid_c[10];
			fgets(pid_c, 10, cmd);

			pid = std::stoi(pid_c);
			pclose(cmd);
		}

		debug_print("PID: ", pid);
		
		int pidfd = syscall(SYS_pidfd_open, pid, 0);
		if(pidfd == -1)
		{
			std::cerr << "ERROR: pidfd_open() failed";
			return 4;
		}
		else debug_print("Successfully got a pidfd: ", pidfd);

		{
			struct pollfd pidfd_poll[1];

			pidfd_poll[0].fd = pidfd;
			pidfd_poll[0].events = POLLIN;

			poll(pidfd_poll, 1, -1);

			debug_print("Process has stopped");
		}
	}

	// TODO: find a better way to start the game
	system("xdg-open steam://run/730");

	return 0;
}
