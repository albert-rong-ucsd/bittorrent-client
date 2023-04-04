#ifndef TRACKER_ANNOUNCE_H
#define TRACKER_ANNOUNCE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <curl/curl.h>

int tannounce_connect(char *url, uint16_t port, char *buf) {
	CURL *curl = curl_easy_init();
	CURLcode res;
	curl_easy_setopt(curl, CURLOPT_URL, buf);
	res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        fprintf(stderr,"[-] Could Not Fetch Webpage\n[+] Error : %s\n",curl_easy_strerror(res));
        exit(-2);
    }
 
    curl_easy_cleanup(curl);
    return 0;
}

#endif
