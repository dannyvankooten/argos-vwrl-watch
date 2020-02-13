#include <stdio.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>

struct Page {
    char * buf;
    size_t size;
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
 
  return realsize;
}

struct Page download_html() {
    CURL *curl;
    CURLcode res;

    struct Page chunk;
    chunk.buf = malloc(1);  /* will be grown as needed by the realloc above */ 
    chunk.size = 0;    /* no data at this point */ 

    curl = curl_easy_init();
    if (!curl)
    {
        printf("Error initialising curl\n");
        return chunk;
    }

    curl_easy_setopt(curl, CURLOPT_URL, "https://www.iex.nl/Beleggingsfonds-Koers/61114463/Vanguard-FTSE-All-World-UCITS-ETF.aspx");
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, download);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) &chunk);
    res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
    }

    curl_easy_cleanup(curl);
    return chunk;        
}


// <span id="ctl00_ctl00_Content_TopContent_IssueDetailBar_RangeBlock52W_lblDayHighPrice">88,11</span>
double find_ath(struct Page page) {
    char needle[1024] = "<span id=\"ctl00_ctl00_Content_TopContent_IssueDetailBar_RangeBlock52W_lblDayHighPrice\">";
    int len = strlen(needle);
    while (strncmp(needle, page.buf, len) != 0) {
        page.buf++;
    }
    page.buf += len;

    char buf[100];
    int j = 0;
    while (page.buf[j] != '<') {
        if (page.buf[j] == ',') {
            buf[j] = ',';
        } else {
            buf[j] = page.buf[j];
        }
        
        j++;
    }

    return atof(buf);
}

// basevalues['61114463LastPrice'] = 87.48;
// basevalues['61114463LowPrice'] = 87.48;
// basevalues['61114463HighPrice'] = 87.9;
// basevalues['61114463LastTime'] = 1581591241000; 
// basevalues['61114463AbsoluteDifference'] = -0.59;
// basevalues['61114463RelativeDifference'] = -0.6699;
double find_price(struct Page page, char needle[]) {
    int len = strlen(needle);
    while (strncmp(needle, page.buf, len) != 0) {
        page.buf++;
    }
    page.buf += len;

    char buf[100];
    int j = 0;
    while (page.buf[j] != ';') {
        buf[j] = page.buf[j];
        j++;
    }

    return atof(buf);
};

int main(void)
{
    struct Page page = download_html();
    double ath = find_ath(page);
    double last_price = find_price(page, "basevalues['61114463LastPrice'] = ");
    double change_today = find_price(page, "basevalues['61114463RelativeDifference'] = ");
    free(page.buf);

    double change = (last_price / ath - 1.0) * 100.0;
    double recovery = (ath / last_price - 1.0) * 100.0;
        
    printf("VWRL: €%.2f\n", last_price);
    printf("---\n");
    printf("Today's change: %.1f%%\n", change_today);
    printf("%1.f%% from 52w high of €%.2f (%.1f%% to recover)\n", change, ath, recovery);
    printf("---\n");
    printf("Google Finance | href='https://www.google.com/search?site=finance&tbm=fin&q=AMS:+VWRL'\n");
    printf("Yahoo Finance | href='https://finance.yahoo.com/quote/VWRL.AS'\n");
    printf("Morningstar | href='https://www.morningstar.com/etfs/XAMS/VWRL/quote.html'\n");

    return 0;
}
