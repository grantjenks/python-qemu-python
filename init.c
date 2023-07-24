#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/stat.h>

char *const default_environment[] = {
    "PATH=/usr/local/bin:/usr/local/sbin:/usr/bin:/usr/sbin:/bin:/sbin",
    NULL,
};

void mount_check(
    const char *source, const char *target,
    const char *filesystemtype, unsigned long mountflags,
    const void *data
) {
    struct stat info;

    if (stat(target, &info) == -1 && errno == ENOENT) {
        // printf("Creating %s\n", target);
        if (mkdir(target, 0755) < 0) {
            perror("Creating directory failed");
            exit(1);
        }
    }

    // printf("Mounting %s\n", target);
    if (mount(source, target, filesystemtype, mountflags, data) < 0) {
        perror("Mount failed");
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    mount_check("none", "/proc", "proc", 0, "");
    mount_check("none", "/dev/pts", "devpts", 0, "");
    mount_check("none", "/dev/mqueue", "mqueue", 0, "");
    mount_check("none", "/dev/shm", "tmpfs", 0, "");
    mount_check("none", "/sys", "sysfs", 0, "");
    mount_check("none", "/sys/fs/cgroup", "cgroup", 0, "");

    sethostname("microvm", sizeof("microvm"));

    execle("/usr/local/bin/python", "/usr/local/bin/python", NULL, default_environment);
    perror("Exec failed");
    return 1;
}
