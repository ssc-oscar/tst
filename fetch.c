#include "common.h"
#include <git2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
# include <pthread.h>
# include <unistd.h>
#endif

struct dl_data {
	git_remote *remote;
	git_fetch_options *fetch_opts;
	int ret;
	int finished;
};

static int progress_cb(const char *str, int len, void *data)
{
	(void)data;
	printf("remote: %.*s", len, str);
	fflush(stdout); /* We don't have the \n to force the flush */
	return 0;
}

/**
 * This function gets called for each remote-tracking branch that gets
 * updated. The message we output depends on whether it's a new one or
 * an update.
 */
static int update_cb (const char *refname, const git_oid *a, const git_oid *b, void *data)
{
	char a_str[GIT_OID_HEXSZ+1], b_str[GIT_OID_HEXSZ+1];
	(void)data;

	git_oid_fmt(b_str, b);
	b_str[GIT_OID_HEXSZ] = '\0';

	if (git_oid_iszero(a)) {
		printf("[new]     %.20s %s\n", b_str, refname);
	} else {
		git_oid_fmt(a_str, a);
		a_str[GIT_OID_HEXSZ] = '\0';
		printf("[updated] %.10s..%.10s %s\n", a_str, b_str, refname);
	}

	return 0;
}

/**
 * This gets called during the download and indexing. Here we show
 * processed and total objects in the pack and the amount of received
 * data. Most frontends will probably want to show a percentage and
 * the download rate.
 */
static int transfer_progress_cb(const git_transfer_progress *stats, void *payload)
{
	if (stats->received_objects == stats->total_objects) {
		printf("Resolving deltas %d/%d\r",
		       stats->indexed_deltas, stats->total_deltas);
	} else if (stats->total_objects > 0) {
		printf("Received %d/%d objects (%d) in %" PRIuZ " bytes\r",
		       stats->received_objects, stats->total_objects,
		       stats->indexed_objects, stats->received_bytes);
	}
	return 0;
}

/** Entry point for this command */
int fetch(git_repository *repo, int argc, char **argv)
{
	git_remote *remote = NULL;
	const git_transfer_progress *stats;
	struct dl_data data;
	git_fetch_options fetch_opts = GIT_FETCH_OPTIONS_INIT;
	git_remote_callbacks callbacks = GIT_REMOTE_CALLBACKS_INIT;
	const git_remote_head **refs;
	size_t refs_len, i;
	int error;
	git_strarray array;
	
	if (argc < 2) {
		fprintf(stderr, "usage: %s fetch <repo>\n", argv[-1]);
		return EXIT_FAILURE;
	}
	//git_remote_list(&array, repo);
	//printf("Got %d remotes\n", (int)array.count);
	//  for (int i = 0; i < array.count; i++)
	//    printf("%d %s\n", i, array .strings [i]);

	  
	// Figure out whether it's a named remote or a URL
	if (git_remote_lookup(&remote, repo, "origin") < 0)
	  return -1;
	
	// Set up the callbacks (only update_tips for now)
	fetch_opts.callbacks.update_tips = &update_cb;
	fetch_opts.callbacks.sideband_progress = &progress_cb;
	fetch_opts.callbacks.transfer_progress = transfer_progress_cb;
	fetch_opts.callbacks.credentials = cred_acquire_cb;
	
	/**
	 * Perform the fetch with the configured refspecs from the
	 * config. Update the reflog for the updated references with
	 * "fetch".
	 */
	//if (git_remote_fetch (remote, NULL, &fetch_opts, "fetch") < 0)
	//	return -1;
	//if (git_remote_download (remote, NULL, &fetch_opts) < 0)
	//	return -1;
	/**
	 * If there are local objects (we got a thin pack), then tell
	 * the user how many objects we saved from having to cross the
	 * network.
	 */
	//printf("refspeccount %ld\n", git_remote_refspec_count(remote));
	//if (git_remote_get_fetch_refspecs(&array, remote) < 0){
	//  return -1;
	//}else{
	//  printf("Got %d refspec\n", (int)array.count);
	//  for (int i = 0; i < array.count; i++)
	//    printf("%d %s\n", i, array .strings [i]);
	//}

	
	callbacks.credentials = cred_acquire_cb;
	fprintf(stderr,"Fetching %s for repo %p\n", "origin", repo);
	//error = git_remote_connect(remote, GIT_DIRECTION_FETCH, &callbacks, NULL);
	//if (error < 0)
	//  return -1;
	//if (git_remote_connected(remote)==0)
	//  return -1;
	//if (git_remote_ls (&refs, &refs_len, remote) < 0)
	//  return -1;
	//printf("refs_len %ld\n", refs_len);
	//for (i = 0; i < refs_len; i++) {
	//  char oid[GIT_OID_HEXSZ + 1] = {0};
	//  git_oid_fmt(oid, &refs[i]->oid);
	//  printf("%s\t%s\n", oid, refs[i]->name);
	//}	

	git_remote_download (remote, NULL, NULL);

	fprintf(stderr, "downloaded\n");
	//if (git_remote_fetch (remote, NULL, &fetch_opts, "fetch") < 0)
	//  return -1;
	//stats = git_remote_stats (remote);
	//if (stats->local_objects > 0) {
	//	printf("\rReceived %d/%d objects in %" PRIuZ " bytes (used %d local objects)\n",
	//	       stats->indexed_objects, stats->total_objects, stats->received_bytes, stats->local_objects);
	//} else{
	//	printf("\rReceived %d/%d objects in %" PRIuZ "bytes\n",
	//		stats->indexed_objects, stats->total_objects, stats->received_bytes);
	//}

	git_remote_free(remote);

	return 0;

 on_error:
	git_remote_free(remote);
	return -1;
}
