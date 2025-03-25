/*
    Copyright (C) 2022 Modelica Association
    Copyright (C) 2015 Modelon AB

    This program is free software: you can redistribute it and/or modify
    it under the terms of the BSD style license.

     This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    BSD_License.txt file for more details.

    You should have received a copy of the BSD_License.txt file
    along with this program. If not, contact Modelon AB <http://www.modelon.com>.
*/

#define _XOPEN_SOURCE 700
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "mlle_types.h"
#include "mlle_error.h"
#include "mlle_spawn.h"


/********************************************************************
 * Setup pipes and start up connection to LVE (server) on Linux.
 * 
 * We are setting up both FILE and Pipes for communication.
 * The FILE streams are however not in use. They worked when
 * OpenSSL was not used but when the streams was connected to BIO
 * descriptors and a SSL handshake was initiated it just froze.
 * With pipes it worked ok.
 *******************************************************************/
struct mlle_connections *
mlle_spawn(const char *exec_name,
           struct mlle_error **error)
{
    pid_t child_pid = -1;
    int parent_to_child_pipe_fd[2] = { -1, -1 };
    int child_to_parent_pipe_fd[2] = { -1, -1 };
    int status = 0;
    struct mlle_connections *connections = NULL;

    status = access(exec_name, F_OK);
    if (status == -1) {
        mlle_error_set(error, 1, 1, "LVE file does not exist \"%s\": Error: %s", exec_name, strerror(errno));
        return NULL;
    }
    status = access(exec_name, X_OK);
    if (status == -1) {
        mlle_error_set(error, 1, 1, "execute permission is not set on LVE \"%s\": Error: %s", exec_name, strerror(errno));
        return NULL;
    }

    fflush(NULL);
    status = pipe(parent_to_child_pipe_fd);
    if (status == -1) {
        mlle_error_set_literal(error, 1, 1, "pipe() failed.");
        return NULL;
    }
    status = pipe(child_to_parent_pipe_fd);
    if (status == -1) {
        mlle_error_set_literal(error, 1, 1, "pipe() failed.");
        return NULL;
    }

    child_pid = fork();
    switch (child_pid) {
    case -1:
        mlle_error_set_literal(error, 1, 1, "fork() failed.");
        return NULL;

    case 0:
        close(parent_to_child_pipe_fd[PIPE_WRITE_INDEX]);
        close(STDIN_FILENO);
        if (dup2(parent_to_child_pipe_fd[PIPE_READ_INDEX], STDIN_FILENO) == -1) {
            fprintf(stderr, "dup2() failed, can't set up communication with executable.\n");
            _exit(3);
        }
        close(parent_to_child_pipe_fd[PIPE_READ_INDEX]);

        close(child_to_parent_pipe_fd[PIPE_READ_INDEX]);
        close(STDOUT_FILENO);
        if (dup2(child_to_parent_pipe_fd[PIPE_WRITE_INDEX], STDOUT_FILENO) == -1) {
            fprintf(stderr, "dup2() failed, can't set up communication with executable.\n");
            _exit(3);
        }
        close(child_to_parent_pipe_fd[PIPE_WRITE_INDEX]);

        execl(exec_name, exec_name, (char *) NULL);
        fprintf(stderr, "execl() failed for %s. Error: %s\n", exec_name, strerror(errno));
        _exit(4);
        break;

    default:
        close(parent_to_child_pipe_fd[PIPE_READ_INDEX]);
        connections = calloc(1, sizeof(*connections));
        if (connections != NULL) {
            connections->fd_to_child = parent_to_child_pipe_fd[PIPE_WRITE_INDEX];
            connections->fd_from_child = child_to_parent_pipe_fd[PIPE_READ_INDEX];
        } else {
            mlle_error_set_literal(error, 1, 1, "Out of memory.");
        }
    }

    return connections;
}
