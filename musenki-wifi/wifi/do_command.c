/* -*- c-basic-offset: 8; -*- */
#pragma GCC optimize("O0")
/*
 * Copyright 2016 Rubicon Communications, LLC.
 */

/* netgate coding-style-patch-verification: on */

#include <unistd.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <syslog.h>

#include <cligen/cligen.h>
#include <clixon/clixon.h>
#include <clixon/clixon_cli.h>
#include <clixon/clixon_err.h>

#ifndef MAX
#define MAX(a,b)	((a) > (b) ? (a) : (b))
#endif

#define SYSCMD_PATH "/bin:/sbin:/usr/bin:/usr/sbin:/usr/local/bin:/usr/local/sbin"

/**
 * Execute a command as a new process.
 *
 *    cmd -- the command to be executed
 *    argv -- an array of parameter strings for cmd, NULL terminated.
 *    time_max_sec -- The maximum time, in seconds, that the command
 *		may execute, or that it may fail to produce output.
 *		If 0, the command is not limited in time.
 *    cb -- if passed cbuf, place output into it, if NULL write to
 *          console
 */

int do_command(const char *cmd, char *const argv[], int time_max_sec, cbuf *cb)
{
	pid_t pid;
	int pipefd[2];		/* Write on [1], read on [0] */
	uint8_t buf[PIPE_BUF];
	int status;
	time_t time_start_sec;
	int time_delta_sec;

#if 0
	int i;

	for (i = 0; argv[i]; ++i) {
		clicon_log (LOG_NOTICE, "%s: [%d] %s", __func__, i, argv[i]);
	}
#endif
	
	if (pipe(pipefd) < 0) {
		clicon_err(OE_UNIX, errno, "pipe");
		return -1;
	}

	/*
	 * Make parent read of child output non-blocking.
	 * Explicit polling so stuck children can be terminated.
	 */
	int mode = 1;

	ioctl(pipefd[0], FIONBIO, &mode);

	fflush(stdout);
	fflush(stderr);

	pid = fork();

	if (pid == -1) {
		clicon_err(OE_UNIX, errno, "fork");
		return -1;
	}

	if (pid == 0) {
		/*
		 * Child.
		 * Redirect child stdout/stderr to this pipe.
		 * Exec new command.
		 */
		while ((dup2(pipefd[1], STDOUT_FILENO) < 0) && errno == EINTR) {
			/* try again */
		}
		while ((dup2(pipefd[1], STDERR_FILENO) < 0) && errno == EINTR) {
			/* try again */
		}

		close(pipefd[1]);
		close(pipefd[0]);

		/*
		 * Commands on different distros can sit at different paths.
		 * E.g. CentOS's /usr/sbin/lspci vs Ubuntu's /usr/bin/lspci.
		 * Set a reasonable path so commands can be executed
		 * by filename rather than providing the full path everywhere.
		 */
		setenv("PATH", SYSCMD_PATH, 1);

		execvp(cmd, argv);

		/*
		 * Shouldn't be reached.
		 */
		clicon_err(OE_UNIX, errno, "execvp");
		return -1;
	}

	/*
	 * Parent.
	 * Start a timer to limit max child time.
	 * Scrape child output and either collect it into cbuf cb,
	 * if that was supplied, or pass it along to parent stdout.
	 */
	close(pipefd[1]);	/* Close write side */

	time_delta_sec = 0;
	time(&time_start_sec);

	while (time_max_sec == 0 || time_delta_sec < time_max_sec) {
		ssize_t count;
		time_t time_now_sec;

		count = read(pipefd[0], buf, sizeof(buf) - 1);
		if (count == -1) {
			if (errno == EINTR || errno == EAGAIN) {
				time_now_sec = time(0) - time_start_sec;
				time_delta_sec = MAX(0, time_now_sec);
				usleep(5000);
				continue;
			} else {
				clicon_err(OE_UNIX, errno, "read");
				return -1;
			}
		} else if (count == 0) {
			break;
		} else {
			if (cb) {
				cprintf(cb, "%s", buf);
			} else {
				/*
				 * FIXME: Does this need to go through
				 * cligen_output()?  If so, a cli_write()
				 * might be needed.
				 */
				write(STDOUT_FILENO, buf, count);
			}
		}
		time_now_sec = time(0) - time_start_sec;
		time_delta_sec = MAX(0, time_now_sec);
	}

	close(pipefd[0]);

	/*
	 * Check for slow or recalcitrant children.
	 */
	if (time_max_sec && time_delta_sec >= time_max_sec) {
		/*
		 * FIXME: Should this be a clicon_err() and return -1?
		 */

		cligen_output(stdout,
			   "Time limit %d seconds exceeded.\n", time_max_sec);
		cligen_output(stdout, "Sub-process '%s' killed.\n", cmd);
		kill(pid, SIGKILL);
	}

	int waited_pid;

	while ((waited_pid = waitpid(pid, &status, WNOHANG)) != pid) {
		/* Try again */
	}

	/*
	 * Return exit code returned by a child
	 */
	if (WIFEXITED(status)) {
		return WEXITSTATUS(status);
	}

	return 0;
}
