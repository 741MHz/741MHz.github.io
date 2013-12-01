#include <cstdio>
#include <cstring>
#include <string>
#include <unistd.h>
#include <sys/inotify.h>

int main() {
    const std::string directory = "/tmp";
    const std::string filename = "test.txt";
    const std::string fullpath = directory + "/" + filename;

    int fd = inotify_init();
    int watch = inotify_add_watch(
        fd, directory.c_str(),
        IN_MODIFY | IN_CREATE | IN_MOVED_TO
    );

    if (access(fullpath.c_str(), F_OK) == 0) {
        printf("File %s exists.\n", fullpath.c_str());
        return 0;
    }

    char    buf[1024 * (sizeof(inotify_event) + 16)];
    ssize_t length;
    bool    is_created = false;

    while (!is_created) {
        length = read(fd, buf, sizeof(buf));
        if (length < 0)
            break;
        inotify_event *event;
        for (size_t i = 0; i < static_cast<size_t>(length);
             i += sizeof(inotify_event) + event->len)
        {
            event = reinterpret_cast<inotify_event *>(&buf[i]);
            if (event->len > 0 && filename == event->name) {
                printf("The file %s was created.\n", event->name);
                is_created = true;
                break;
            }
        }
    }

    inotify_rm_watch(fd, watch);
    close(fd);
}
