#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <signal.h>
#include <getopt.h>
#include <sys/sysinfo.h>

#define PROC "/proc"
#define MEMINFO PROC "/meminfo"
#define STATM "statm"
#define CMDLINE "cmdline"
#define MEMINFO_CACHED "Cached:"
#define MEMINFO_SRECLAIMABLE "SReclaimable:"
#define PROCPATHLEN 64

#define DEFAULT_RAM_LIMIT 93
#define DEFAULT_SWAP_LIMIT 93

static struct option long_options[] = {
    { "ram", required_argument, NULL, 'r' },
    { "swap", required_argument, NULL, 's' },
    { "non-buffered-stdout", no_argument, NULL, 'b' },
    { "no-date", no_argument, NULL, 'd' },
    { "no-tick-log", no_argument, NULL, 't' },
    { "help", no_argument, NULL, 'h' },
    { NULL, 0, NULL, 0 },
};

void read_arguments(int argc, char* argv[], int* ram_limit, int* swap_limit, int* no_date, int* no_tick_log) {
    char ch;
    *ram_limit = DEFAULT_RAM_LIMIT;
    *swap_limit = DEFAULT_SWAP_LIMIT;
    *no_date = 0;
    *no_tick_log = 0;

    while ((ch = getopt_long(argc, argv, "r:s:bdth", long_options, NULL)) != -1) {
        switch (ch) {
            case 'r':
                *ram_limit = strtoul(optarg, NULL, 10);
                if (*ram_limit < 1 && 99 < *ram_limit)
                    *ram_limit = DEFAULT_RAM_LIMIT;
                break;

            case 's':
                *swap_limit = strtoul(optarg, NULL, 10);
                if (*swap_limit < 1 && 99 < *swap_limit)
                    *swap_limit = DEFAULT_SWAP_LIMIT;
                break;

            case 'b':
                setvbuf(stdout, NULL, _IOLBF, 0);
                break;

            case 'd':
                *no_date = 1;
                break;

            case 't':
                *no_tick_log = 1;
                break;

            case 'h':
                printf("%s [-r | --ram <limit>] [-s | --swap <limit>] [-h | --help] [-b | --non-buffered-stdout] [-d | --no-date] [-t | --no-tick-log]\n", argv[0]);
                exit(1);
                break;

            default:
                break;
        }
    }
}

long read_entry(FILE* file, const char* key, size_t length) {
    long value = 0;
    int i = 0;
    int c;

    while ((c = fgetc(file)) != EOF) {
        if (c == key[i]) {
            i++;
        } else if (i == length) {
            if ('0' <= c && c <= '9') {
                value = value * 10 + c - '0';
            } else if (c == '\n') {
                break;
            }
        } else {
            i = 0;
        }
    }

    return value;
}

long get_cached() {
    // Cached + SReclaimable
    long cached = 0;
    FILE* file = fopen(MEMINFO, "r");

    if (file) {
        cached = read_entry(file, MEMINFO_CACHED, sizeof(MEMINFO_CACHED) - 1)
            + read_entry(file, MEMINFO_SRECLAIMABLE, sizeof(MEMINFO_SRECLAIMABLE) - 1);
        fclose(file);
    }

    return cached;
}

long get_resident(const char* path) {
    long resident = 0;
    FILE* file = fopen(path, "r");

    if (file) {
        int space = 0;
        int c;
        while ((c = fgetc(file)) != EOF) {
            if (space > 0) {
                if ('0' <= c && c <= '9') {
                    resident = resident * 10 + c - '0';
                } else if (c == ' ') {
                    break;
                }
            } else {
                if (c == ' ')
                    space++;
            }
        }

        fclose(file);
    }

    return resident;
}

void read_cmdline(const char* path, char* output, size_t length) {
    FILE* file = fopen(path, "r");

    output[0] = 0;

    if (file) {
        size_t l = fread(output, sizeof(char), length - 1, file);
        output[l] = 0;
        fclose(file);
    }
}

int main(int argc, char* argv[]) {
    struct sysinfo info;
    long total;
    long cached;
    long used;
    long current;
    long total_swap;
    long used_swap;
    long current_swap;
    char datetime[sizeof("YYYY-MM-DD HH:mm:ss")] = {0};
    time_t t;
    struct tm* tm_info;
    char path[PROCPATHLEN] = {0};
    int ram_limit;
    int swap_limit;
    int no_date;
    int no_tick_log;

    read_arguments(argc, argv, &ram_limit, &swap_limit, &no_date, &no_tick_log);

    for (;;) {
        if (sysinfo(&info) < 0)
            return -1;

#define to_unit(x) ((x) * info.mem_unit / 1024)
        total = to_unit(info.totalram);
        cached = get_cached();
        used = total - to_unit(info.freeram) - to_unit(info.bufferram) - cached;
        current = used * 100 / total;
        total_swap = to_unit(info.totalswap);
        used_swap  = to_unit(info.totalswap - info.freeswap);
        current_swap = used_swap * 100 / total_swap;
#undef to_unit

        if (!no_tick_log) {
            if (!no_date) {
                t = time(NULL);
                tm_info = localtime(&t);
                strftime(datetime, 20, "%Y-%m-%d %H:%M:%S", tm_info);
                printf("[%s] ", datetime);
            }

            printf("Memory %ld%% (%ld / %ld) Swap %ld%% (%ld / %ld)\n", current, used, total, current_swap, used_swap, total_swap);
        }

        if (current > ram_limit || current_swap > swap_limit) {
            DIR* dir = opendir(PROC);
            long resident;
            long max_resident = 0;
            int max_pid = 0;

            if (dir) {
                struct dirent* ent;

                while ((ent = readdir(dir))) {
                    if (!ent->d_name[0]) break;

                    if (*ent->d_name > '0' && *ent->d_name <= '9') {
                        int pid = strtoul(ent->d_name, NULL, 10);

                        if (errno == 0) {
                            snprintf(path, PROCPATHLEN, PROC "/%d/" STATM, pid);
                            resident = get_resident(path);

                            if (resident > max_resident) {
                                max_resident = resident;
                                max_pid = pid;
                            }

                        }
                    }
                }

                closedir(dir);
                snprintf(path, PROCPATHLEN, PROC "/%d/" CMDLINE, max_pid);
                read_cmdline(path, path, PROCPATHLEN);

                if (!no_date) {
                    t = time(NULL);
                    tm_info = localtime(&t);
                    strftime(datetime, 20, "%Y-%m-%d %H:%M:%S", tm_info);
                    printf("[%s] ", datetime);
                }
                printf("Kill %s (pid %d) %ld [ram: %ld > %d] [swap: %ld > %d]\n", path, max_pid, max_resident, current, ram_limit, current_swap, swap_limit);
                if (max_pid)
                    kill(max_pid, SIGTERM);
            }

        }

        sleep(10);
    }

    return 0;
}
