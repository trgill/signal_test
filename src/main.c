#include <stddef.h>
#include <alloca.h>
#include <endian.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
 
#include <systemd/sd-daemon.h>
#include <systemd/sd-event.h>

typedef struct test_event_source {
	sd_event_source *sd_es;
        sd_event *event_loop;
	const char *name;
} test_event_source_t;

test_event_source_t *new_es = NULL;

int start_worker(test_event_source_t *es) 
{
    pid_t pid = -1;
	if ((pid = fork()) < 0) {
		goto out;
	}

	if (pid == 0) {
		/*
		 *  Child Process
		 */
               sd_event_unref(es->event_loop);
               sd_event_source_unref(es->sd_es);
                exit(0);
    }
    out:

    return 0;
}
static int _sd_signal_event_handler(sd_event_source *sd_es, const struct signalfd_siginfo *si, void *data)
{
    test_event_source_t *es = (test_event_source_t *) data;

	switch (si->ssi_signo) {
		case SIGTERM:
                sd_event_exit(es->event_loop, 0);
	        break;
        case SIGINT:
                start_worker(es);
		case SIGPIPE:
			break;
		case SIGHUP:
			break;
		case SIGCHLD:
			break;
		default:
			break;
	};

	return 1;
}


int main(int argc, char *argv[]) {
        static const char unnamed[] = "unnamed";
        const char* name = "test name";
        int  r;
        sigset_t ss;

        new_es = malloc(sizeof(test_event_source_t));

        r = sd_event_default(&new_es->event_loop);
        if (r < 0)
                goto finish;


        if (sigemptyset(&ss) < 0 ||
            sigaddset(&ss, SIGTERM) < 0 ||
            sigaddset(&ss, SIGINT) < 0) {
                r = -errno;
                goto finish;
        }
        signal(SIGCHLD, SIG_IGN);

        if (sigprocmask(SIG_BLOCK, &ss, NULL) < 0) {
                r = -errno;
                goto finish;
        }

        r = sd_event_add_signal(new_es->event_loop, &new_es->sd_es, SIGTERM, _sd_signal_event_handler, new_es);
        if (r < 0)
                goto finish;

 
        r = sd_event_add_signal(new_es->event_loop, &new_es->sd_es, SIGINT, _sd_signal_event_handler, new_es);
        if (r < 0)
                goto finish;

        r = sd_event_loop(new_es->event_loop);

finish:
       sd_event_unref(new_es->event_loop);

        if (r < 0)
                fprintf(stderr, "Failure: %s\n", strerror(-r));

        return r < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
