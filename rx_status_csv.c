#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <resolv.h>
#include <string.h>
#include <utime.h>
#include <unistd.h>
#include <getopt.h>
#include <pcap.h>
#include <endian.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "lib.h"

wifibroadcast_rx_status_t *status_memory_open(void) {
	int fd = shm_open("/wifibroadcast_rx_status_0", O_RDWR, S_IRUSR | S_IWUSR);

	if(fd < 0) {
		perror("shm_open");
		exit(1);
	}

	if (ftruncate(fd, sizeof(wifibroadcast_rx_status_t)) == -1) {
		perror("ftruncate");
		exit(1);
	}

	void *retval = mmap(NULL, sizeof(wifibroadcast_rx_status_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (retval == MAP_FAILED) {
		perror("mmap");
		exit(1);
	}
	
	
	return (wifibroadcast_rx_status_t*)retval;

}


int main(int argc, char **argv){

	int opt;

	char *path = NULL;

	while ((opt = getopt (argc, argv, ":f:")) != -1){
		switch (opt)
		{
		case 'f':
			path = optarg;
			printf("path: %s",path);
			break;
		case '?':
			if (optopt == 'f')
				fprintf (stderr, "Option -%c requires an argument.\n", optopt);
			else if (isprint (optopt))
				fprintf (stderr, "Unknown option `-%c'.\n", optopt);
			else
				fprintf (stderr,
					"Unknown option character `\\x%x'.\n",
					optopt);
			return 1;
		default:
			abort ();
		}
	}

	printf("%s",path);

	FILE* fp = fopen (path,"w+");

	fprintf(fp,"stamp,");
	fprintf(fp,"wifi_adapter_cnt,");
	fprintf(fp,"received_block_cnt,");
	fprintf(fp,"damaged_block_cnt,");
	fprintf(fp,"fecs_used_cnt,");
	fprintf(fp,"tx_restart_cnt,");
	fprintf(fp,"adapterIdx,");
	fprintf(fp,"current_signal_dbm,");
	fprintf(fp,"received_packet_cnt,");
	fprintf(fp,"wrong_crc_cnt,");
	fprintf(fp,"lost_packets_cnt");
	fprintf(fp,"\n");

	wifibroadcast_rx_status_t *t = status_memory_open();

	for(;;) {
		time_t ttime = time(NULL);
		struct tm tm = *localtime(&ttime);
		
		for (int i = 0; i < t->wifi_adapter_cnt; i++){
			fprintf(fp,"%d-%02d-%02dT%02d:%02d:%02dZ,", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
			fprintf(fp,"%d,", t->wifi_adapter_cnt);
			fprintf(fp,"%d,", t->received_block_cnt);
			fprintf(fp,"%d,", t->damaged_block_cnt);
			fprintf(fp,"%d,", t->fecs_used_cnt);
			fprintf(fp,"%d,", t->tx_restart_cnt);
			fprintf(fp,"%d,", i);
			fprintf(fp,"%d,", t->adapter[i].current_signal_dbm);
			fprintf(fp,"%d,", t->adapter[i].received_packet_cnt);
			fprintf(fp,"%d,", t->adapter[i].wrong_crc_cnt);
			fprintf(fp,"%d", t->adapter[i].lost_packets_cnt);
			fprintf(fp,"\n");
			fflush(fp);
		}
		usleep(1e5);
	}

	return 0;
}

