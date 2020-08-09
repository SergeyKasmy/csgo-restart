#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <poll.h>
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

template <class... Args>
void error(int exit_code, Args... args)
{
	std::cerr << "ERROR: ";
	(std::cerr << ... << args);
	exit(exit_code);
}

int main(int argc, char **argv)
{
	debug_print("Debug output enabled");

	if(argc < 2) error(5, "filename not provided");
	std::string restart_file = argv[1];

	debug_print("EVENT_SIZE: ", EVENT_SIZE);
	debug_print("Filename: ", restart_file);
	debug_print("Filename length: ", restart_file.length());
	debug_print("BUF_SIZE: ", BUF_SIZE);

	{
		int fd = inotify_init();
		if(fd == -1) error(1, "failed to create an inotify file descriptor");
		else debug_print("Successfully created an inotify file descriptor: ", fd); 

		{
			int wd = inotify_add_watch(fd, restart_file.c_str(), IN_MODIFY);
			if(wd == -1) error(2, "failed to create a watch descriptor");
			else debug_print("Successfully created a watch descriptor: ",  wd); 

			char buf[BUF_SIZE];
			ssize_t len = read(fd, buf, BUF_SIZE);
			if(len == -1) error(3, "read() failed on inotify fd");
			inotify_rm_watch(fd, wd);
		}

		close(fd);
	}


	{
		pid_t pid;
		{
			FILE *cmd = popen("pidof csgo_linux64", "r");

			// buffer size of 10 should be enough
			char pid_c[10];
			std::fgets(pid_c, 10, cmd);

			pid = std::stoi(pid_c);
			pclose(cmd);
		}

		debug_print("PID: ", pid);
		
		int pidfd = syscall(SYS_pidfd_open, pid, 0);
		if(pidfd == -1) error(4, "pidfd_open() failed");
		else debug_print("Successfully got a pidfd: ", pidfd);

		{
			struct pollfd pidfd_poll[1];

			pidfd_poll[0].fd = pidfd;
			pidfd_poll[0].events = POLLIN;

			poll(pidfd_poll, 1, -1);

			debug_print("Process has stopped");
		}
	}

	//std::system("xdg-open steam://run/730");
	std::system("notify-send test");

	return 0;
}
