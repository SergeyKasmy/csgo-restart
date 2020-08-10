#include <iostream>
#include <sys/inotify.h>
#include <unistd.h>

const char *restart_file = "/tmp/ino";

int main()
{
	int fd = inotify_init();

	if(fd == -1)
	{
		std::cerr << "ERROR: Failed to create an inotify file descriptor";
		return 1;
	}
	else
	{
		std::cout << "Successfully created an inotify file descriptor: " << fd << std::endl; 
	}

	int wd = inotify_add_watch(fd, restart_file, IN_MODIFY);

	if(wd == -1)
	{
		std::cerr << "ERROR: Failed to create a watch descriptor";
		return 1;
	}
	else
	{
		std::cout << "Successfully created a watch descriptor: " << wd << std::endl; 
	}

	struct inotify_event* event = new inotify_event;
	
	read(fd, event, sizeof(struct inotify_event) + 16);
	
	if(event->mask & IN_MODIFY)
	{
		std::cout << "File modified" << std::endl;
	}

	delete event;
	close(fd);
	return 0;
}
