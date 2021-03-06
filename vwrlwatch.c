#include <stdio.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <err.h>

struct Page {
    char * buf;
    size_t size;
    char * address;
};

static size_t download(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct Page *mem = (struct Page *) userp;
 
  char *ptr = realloc(mem->buf, mem->size + realsize + 1);
  if(ptr == NULL) {
    /* out of memory! */ 
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }
 
  mem->buf = ptr;
  memcpy(&(mem->buf[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->buf[mem->size] = 0;
  mem->address = ptr;
 
  return realsize;
}

struct Page download_html(char url[]) {
    CURL *curl;
    CURLcode res;

    struct Page chunk;
    chunk.buf = malloc(1);  /* will be grown as needed by the realloc above */ 
    chunk.size = 0;    /* no data at this point */ 
    chunk.address = chunk.buf; 

    curl = curl_easy_init();
    if (!curl)
    {
        printf("Error initialising curl\n");
        return chunk;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, download);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) &chunk);
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1);
    curl_easy_setopt(curl, CURL_SOCKET_TIMEOUT, 10);
    #ifdef __APPLE__
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
    #endif

    res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
    }

    curl_easy_cleanup(curl);
    return chunk;        
}

/* read (double) value between start and end string */
double find_value(struct Page *page, char needle_s[], char needle_e) {
    char *str = strstr(page->buf, needle_s);
    if (str == NULL) {
        return 0.00;
    }

    str += strlen(needle_s);

    // read everything up to needle_e into buf 
    char buf[100];
    size_t j = 0;
    for (size_t i=0; str[i] != '\0' && str[i] != needle_e && i < 100; i++) {
        // replace comma with dot so we can parse floats
        if (str[i] == ',') {
            buf[j++] = '.';
        } else {
            buf[j++] = str[i];
        }
    }
    buf[j++] = '\0';
    return atof(buf);
}

// <span id="ctl00_ctl00_Content_TopContent_IssueDetailBar_RangeBlock52W_lblDayHighPrice">88,11</span>
// basevalues['61114463LastPrice'] = 87.48;
// basevalues['61114463LowPrice'] = 87.48;
// basevalues['61114463HighPrice'] = 87.9;
// basevalues['61114463LastTime'] = 1581591241000; 
// basevalues['61114463AbsoluteDifference'] = -0.59;
// basevalues['61114463RelativeDifference'] = -0.6699;
int main(void)
{
    struct Page page = download_html("https://www.iex.nl/Beleggingsfonds-Koers/61114463/Vanguard-FTSE-All-World-UCITS-ETF.aspx");
    double ath, last_price, change_today;
    ath = find_value(&page, "<dt class=\"keylist__term\">Hoog 52 weeks</dt>\r\n                <dd class=\"keylist__value\"><span>", '<');
    last_price = find_value(&page, "basevalues['61114463LastPrice'] = ", ';');
    change_today = find_value(&page, "basevalues['61114463RelativeDifference'] = ", ';');
    free(page.address);

    double change = 0.0;
    double recovery = 0.0;
    if (ath > 0.0 && last_price > 0.0) {
        change = (last_price / ath - 1.0) * 100.0;      
        recovery = (ath / last_price - 1.0) * 100.0;
    }
        
    printf("VWRL €%.2f\n", last_price);
    printf("---\n");
    printf("Today's change: %.1f%%\n", change_today);
    printf("%.1f%% from 52w high of €%.2f (%.1f%% to recover)\n", change, ath, recovery);
    printf("---\n");
    printf("Google Finance | href='https://www.google.com/search?site=finance&tbm=fin&q=AMS:+VWRL'\n");
    printf("Yahoo Finance | href='https://finance.yahoo.com/quote/VWRL.AS'\n");
    printf("Morningstar | href='https://www.morningstar.com/etfs/XAMS/VWRL/quote.html'\n");
    printf("---\n");

    char time_str[20];
    time_t curtime = time (NULL);
    struct tm * loc_time = localtime (&curtime);
    strftime(time_str, 100, "%I:%M %p", loc_time);
    printf("Last updated at %s | refresh=true \n", time_str);

    return 0;
}

