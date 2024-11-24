#ifndef _CLIENT_H_
#define _CLIENT_H_

#define DEFAULT_PLAYERCMD	""/usr/bin/mpg123 -q -""


struct client_conf_st
{
	char *rcvport;
	char *mgroup;
	char *player_cmd;
};

extern struct client_conf_st client_conf;

#endif

