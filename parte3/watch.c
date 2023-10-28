#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/inotify.h>

#define EVENT_SIZE  (sizeof(struct inotify_event))
#define BUF_SIZE    (1024 * (EVENT_SIZE + 16))

int main() {
    int fd, wd;
    char buffer[BUF_SIZE];

    fd = inotify_init();
    if (fd < 0) {
        perror("inotify_init");
        exit(EXIT_FAILURE);
    }

    const char *path = "/home/so/trabalho-2022-2023/utils/parte-3/validators/so-2022-trab3-validator/";  // Replace with the path to your target directory

    wd = inotify_add_watch(fd, path, IN_CREATE | IN_MODIFY | IN_DELETE);
    if (wd < 0) {
        perror("inotify_add_watch");
        exit(EXIT_FAILURE);
    }

    printf("Monitoring directory: %s\n", path);

    while (1) {
        int i, length = read(fd, buffer, BUF_SIZE);
        if (length < 0) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        i = 0;
        while (i < length) {
            struct inotify_event *event = (struct inotify_event *)&buffer[i];
            if (event->len) {
                if (event->mask & IN_CREATE)
                    printf("File created: %s\n", event->name);
                else if (event->mask & IN_MODIFY)
                    printf("File modified: %s\n", event->name);
                else if (event->mask & IN_DELETE)
                    printf("File deleted: %s\n", event->name);
            }
            i += EVENT_SIZE + event->len;
        }
    }

    inotify_rm_watch(fd, wd);
    close(fd);

    return 0;
}
