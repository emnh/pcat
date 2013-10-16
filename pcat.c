#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <inttypes.h>

#define ull unsigned long long
const unsigned int updint = 500; //update interval in milliseconds
ull bytescopied = 0;
pthread_t tid;
int isnetwork = 0, iscd = 0;
int done = 0;

void printsize (unsigned int kb) {
    if (kb <= 999) {
        fprintf(stderr, "%5.1u KiB/s", kb);
    } else {
        if (kb <= 999 * 1024) {
            fprintf(stderr, "%5.1f MiB/s", (float) kb / 1024);
        } else {
            fprintf(stderr, "%5.1f GiB/s", (float) kb / 1024 / 1024);
        }
    }
}

void status () {
    struct timeval starttime, nowtime;
    unsigned int secs, kbcopied, okbcopied = 0;
    unsigned int deltakb;
    gettimeofday(&starttime, 0);
    while (done < 2) {
        if (done == 1) done++;
        if (bytescopied == 0) gettimeofday(&starttime, 0);
        gettimeofday(&nowtime, 0);
        secs = nowtime.tv_sec - starttime.tv_sec + 1;
        kbcopied = bytescopied / 1024;
        deltakb = (kbcopied - okbcopied) * (1000 / updint);
        
        //constant width breaks at:
        //999:59 999%
        
        fprintf(stderr, " Acc: ");
        //print total MiB copied and time in mmm:ss
        printsize(kbcopied);
        //print time in mmm:ss, 
        fprintf(stderr, "  %3u:%.2u", secs / 60, secs % 60);
        //print current transfer speed
        fprintf(stderr, "   Cur: ");
        printsize(deltakb);
        //print average transfer speed
        fprintf(stderr, "   Avg: ");
        printsize(kbcopied / secs);
        if (isnetwork) {
            //print transfer speed in percentage of 100Mbit network
            fprintf(stderr, "   %3u%%", deltakb / 125); // * 100% / 12500KiB/s
        }
        if (iscd) {
            //print transfer speed in cdrom X
            fprintf(stderr, "   %3uX", deltakb / 150); // 1X = 150KB/s
        }

        //write a few spaces, just in case
        fprintf(stderr, "     ");
        
        if (done) {
            fprintf(stderr, "\n");
        } else {
            //print carriage return (moves the cursor to beginning of line)
            fprintf(stderr, "\r");
        }
        okbcopied = kbcopied;
        usleep(updint * 1000);
    }
}

int main (int argc, char* argv[]) {
    int c, insize, outsize;
    struct stat stat_buf;
    char* buf; char ch;

    if (argc > 1) {
        if (!strcmp(argv[1], "--help")) {
            puts("pcat is a fast threaded data flow statistics generator for piped data");
            puts("insert pcat in pipeline where appropriate");
            puts("example: tar -cz kernel | pcat | tar -xz -C /tmp");
            puts("");
            puts("Usage: pcat [-n] [-c]");
            puts("       -n print transfer speed in percentage of 100Mbit network");
            puts("       -c print transfer speed in cdrom X");
            exit(1);
        }
        while ((ch = getopt(argc, argv, "ncv")) != -1) {
            switch (ch) {
                case 'n':
                    isnetwork = 1;
                    break;
                case 'c':
                    iscd = 1;
                    break;
                case 'v':
                    puts(PACKAGE_STRING);
                    exit(0);
            }
        }
    }
    pthread_create(&tid, NULL, (void*) status, NULL);
    fstat(STDIN_FILENO, &stat_buf);
    insize = stat_buf.st_blksize;
    fstat(STDOUT_FILENO, &stat_buf);
    outsize = stat_buf.st_blksize;
    if (insize < outsize) insize = outsize;
    if ((buf = malloc(insize)) == NULL) { perror("malloc"); exit(1); }
    while ((c = read(STDIN_FILENO, buf, insize)) > 0) {
        write((int) STDOUT_FILENO, buf, c);
        bytescopied += c;
    }
    done++;
    while (done < 2) usleep(10);
    return 0;
}
