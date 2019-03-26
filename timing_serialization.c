


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <arpa/inet.h>

#include "rdtsc.h"
#include "cJSON/cJSON.h"
#include "cJSON/cJSON_Utils.h"
#include "crt_dbg_leak.h"

#include "rapid-json-wrapper/rapidJSON_api.h"

#define TIME_JSON_C

// `jansson`'s defines conflict with `json-c`'s defines
#ifdef TIME_JANSSON
#include "jansson.h"
#endif // TIME_JANSSON

#ifdef TIME_JSON_C
#include "json-c/json.h"
#endif

struct item
{
    char uuid[36];
    char action[10];
    char version[10];
    uint32_t ip; // 1760484994
};

static void nlog(const char * fmt, ...) __attribute__((format(printf,1,2)));
static void nlog(const char * fmt, ...)
{
    int rc;
    char buf[0x400];
    va_list ap;
    time_t now=0;
    //
    time(&now);
    va_start(ap, fmt);
    rc = vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);

    fprintf(stderr, "[%.*s]%s\n", 19, ctime(&now), buf);
}

// make a struct array
static void make_struct(struct item ** pp, int * arsize)
{
    const int size = 20*100*1024;
    struct item * p = NULL;
    p = calloc(size, sizeof(struct item));
    int i;

    for(i=0; i<size; i++){
        memcpy(p[i].uuid, "ffffffffffffffffffffffffffffffffffff", sizeof(p[i].uuid));
        snprintf(p[i].action, sizeof(p[i].action), "offline");
        snprintf(p[i].version, sizeof(p[i].version), "v1.1.11111");
        p[i].ip = htonl(1760484994);
    }
    *pp = p;
    *arsize = size;
}

/**

[Tue Mar 26 13:46:11]enter time_cJSON() serialization
[Tue Mar 26 13:49:35]leave time_cJSON() serialization
[Tue Mar 26 13:49:35]enter time_cJSON() cJSON_PrintUnformatted
[Tue Mar 26 13:49:35]leave time_cJSON() cJSON_PrintUnformatted

 serialization cost too long, take 3+ minutes (204 sec)
 */

static void time_cJSON(struct item * p, int size)
{
    nlog("enter %s() serialization", __func__);
   cJSON * root = cJSON_CreateObject();
   cJSON * stat = cJSON_CreateArray();
   cJSON_AddItemToObject(root, "stats", stat);
   int i;

   for(i=0; i<size; i++){
       cJSON * itemj = cJSON_CreateObject();
       cJSON_AddItemToArray(stat, itemj);

       char tmp[0x200];
       snprintf(tmp, sizeof(tmp), "%.*s", (int)sizeof(p[i].uuid), p[i].uuid);
       cJSON_AddItemToObject(itemj, "uuid", cJSON_CreateString(tmp));
       cJSON_AddItemToObject(itemj, "action", cJSON_CreateString(p[i].action));
       cJSON_AddItemToObject(itemj, "version", cJSON_CreateString(p[i].version));

       inet_ntop(AF_INET, &p[i].ip, tmp, sizeof(tmp));
       cJSON_AddItemToArray(cJSON_AddArrayToObject(itemj, "ip"), cJSON_CreateString(tmp));
   }
    nlog("leave %s() serialization", __func__);
    nlog("enter %s() cJSON_PrintUnformatted", __func__);
    char * s = cJSON_PrintUnformatted(root);
    //printf("%s\n",s);
    free(s);
    nlog("leave %s() cJSON_PrintUnformatted", __func__);
   cJSON_Delete(root);
}

// rapid json is c++ lib, we not choice it


/**
[Tue Mar 26 14:27:41]enter time_jansson() serialization
[Tue Mar 26 14:27:41]leave time_jansson() serialization
[Tue Mar 26 14:27:41]enter time_jansson() cJSON_PrintUnformatted
[Tue Mar 26 14:27:42]leave time_jansson() cJSON_PrintUnformatted

 serialization take <0 sec

 */

#ifdef TIME_JANSSON
static void time_jansson(struct item * p, int size)
{
    nlog("enter %s() serialization", __func__);
    json_t * root = json_object();
    json_t * stat = json_array();
    json_object_set_new(root, "stats", stat);
    int i;

    for(i=0; i<size; i++){
        json_t * itemj = json_object();
        json_array_append_new(stat, itemj);

        char tmp[0x200];
        snprintf(tmp, sizeof(tmp), "%.*s", (int)sizeof(p[i].uuid), p[i].uuid);
        json_object_set_new(itemj, "uuid", json_string(tmp));
        json_object_set_new(itemj, "action", json_string(p[i].action));
        json_object_set_new(itemj, "version", json_string(p[i].version));

        inet_ntop(AF_INET, &p[i].ip, tmp, sizeof(tmp));
        json_t * ar = json_array();
        json_object_set_new(itemj, "ip", ar);
        json_array_append_new(ar, json_string(tmp));
    }
    nlog("leave %s() serialization", __func__);
    nlog("enter %s() json_dumps", __func__);
    char * s = json_dumps(root, JSON_COMPACT);
    printf("%s\n",s);
    free(s);
    nlog("leave %s() json_dumps", __func__);
    json_decref(root); root = 0;
}
#endif // TIME_JANSSON


// simdjson only support parse a string to json object, not support modify json object



/**
MD5 (1.log) = 7927132589f5c7b15dbc3d3ab0753939
MD5 (../result.log) = 7927132589f5c7b15dbc3d3ab0753939


[Tue Mar 26 16:16:22]enter time_json_c() serialization
[Tue Mar 26 16:16:23]leave time_json_c() serialization
[Tue Mar 26 16:16:23]enter time_json_c() cJSON_PrintUnformatted
[Tue Mar 26 16:16:23]leave time_json_c() cJSON_PrintUnformatted

 * @param p
 * @param size
 */
#ifdef TIME_JSON_C
static void time_json_c(struct item * p, int size)
{
    nlog("enter %s() serialization", __func__);
    struct json_object * root = json_object_new_object();
    struct json_object * stat = json_object_new_array();
    json_object_object_add(root, "stats", stat);
    int i;

    for(i=0; i<size; i++){
        struct json_object * itemj = json_object_new_object();
        json_object_array_add(stat, itemj);

        char tmp[0x200];
        snprintf(tmp, sizeof(tmp), "%.*s", (int)sizeof(p[i].uuid), p[i].uuid);
        json_object_object_add(itemj, "uuid", json_object_new_string(tmp));
        json_object_object_add(itemj, "action", json_object_new_string(p[i].action));
        json_object_object_add(itemj, "version", json_object_new_string(p[i].version));

        inet_ntop(AF_INET, &p[i].ip, tmp, sizeof(tmp));
        json_object * ar = json_object_new_array();
        json_object_object_add(itemj, "ip", ar);
        json_object_array_add(ar, json_object_new_string(tmp));
    }
    nlog("leave %s() serialization", __func__);
    nlog("enter %s() json_object_to_json_string_ext", __func__);
    const char * s = json_object_to_json_string_ext(root, JSON_C_TO_STRING_PLAIN);
    printf("%s\n",s);
    nlog("leave %s() json_object_to_json_string_ext", __func__);
    json_object_put(root); root = 0;
}
#endif

static void time_rapidjson(struct item * p, int size)
{
    nlog("enter %s() serialization", __func__);
    struct tm_json_w_handler wh = tm_json_write_create();
    int i;

    tm_json_write_object_start(wh);

    tm_json_write_key(wh, "stats");
    tm_json_write_array_start(wh);
    for(i=0;i<size;i++){

        tm_json_write_object_start(wh);
        tm_json_write_key(wh, "uuid");
        char tmp[0x200];
        snprintf(tmp, sizeof(tmp), "%.*s", (int)sizeof(p[i].uuid), p[i].uuid);
        tm_json_write_string(wh, tmp);

        tm_json_write_key(wh, "action");
        tm_json_write_string(wh, p[i].action);

        tm_json_write_key(wh, "version");
        tm_json_write_string(wh, p[i].version);

        inet_ntop(AF_INET, &p[i].ip, tmp, sizeof(tmp));
        tm_json_write_key(wh, "ip");
        tm_json_write_array_start(wh);
        tm_json_write_string(wh, tmp);
        tm_json_write_array_end(wh);


        tm_json_write_object_end(wh);
    }

    tm_json_write_array_end(wh);

    tm_json_write_object_end(wh);

    nlog("leave %s() serialization", __func__);

    nlog("enter %s() to string", __func__);
    const char * s = tm_json_write_result(wh);
    printf("%s\n", s);
    nlog("leave %s() to string", __func__);
    
    tm_json_write_destroy(wh);
    
}


int main(){

    nlog("%s()", __func__);

    struct item * c_struct = NULL;
    int size=0;
    make_struct(&c_struct, &size);


    // time_jansson(c_struct, size);
    //time_cJSON(c_struct, size);
    //time_json_c(c_struct, size);
    time_rapidjson(c_struct, size);

    free(c_struct);
    c_struct = NULL;

    nlog("main exit");
    return 0;
}

/**
 * 
 * when struct count turn to 20*100*1024 build with release
 * jansson serialization take 4 sec, to string take 5 sec, free take 1 sec, take memory less than `json-c`
 * json-c serialization take 5 sec, to string take 2 sec, free take 1 sec, take more memory
 * rapidjson are better than both.


我的 macOS 和 linux 差别好大
struct count 20*100*1024

jansson serialization take 7 sec  to string take 7 sec, mem free take 2-4 sec
[Tue Mar 26 19:36:43]main()
[Tue Mar 26 19:36:43]enter time_jansson() serialization
[Tue Mar 26 19:36:50]leave time_jansson() serialization
[Tue Mar 26 19:36:50]enter time_jansson() json_dumps
[Tue Mar 26 19:36:57]leave time_jansson() json_dumps
[Tue Mar 26 19:36:59]main exit
-------------
[Tue Mar 26 19:37:00]main()
[Tue Mar 26 19:37:01]enter time_jansson() serialization
[Tue Mar 26 19:37:07]leave time_jansson() serialization
[Tue Mar 26 19:37:07]enter time_jansson() json_dumps
[Tue Mar 26 19:37:14]leave time_jansson() json_dumps
[Tue Mar 26 19:37:18]main exit


json-c serialization take 7 sec, to string take 3-6 sec, mem free take 4 sec
[Tue Mar 26 19:38:08]main()
[Tue Mar 26 19:38:08]enter time_json_c() serialization
[Tue Mar 26 19:38:15]leave time_json_c() serialization
[Tue Mar 26 19:38:15]enter time_json_c() json_object_to_json_string_ext
[Tue Mar 26 19:38:21]leave time_json_c() json_object_to_json_string_ext
[Tue Mar 26 19:38:25]main exit
--------------
[Tue Mar 26 19:38:28]main()
[Tue Mar 26 19:38:29]enter time_json_c() serialization
[Tue Mar 26 19:38:36]leave time_json_c() serialization
[Tue Mar 26 19:38:36]enter time_json_c() json_object_to_json_string_ext
[Tue Mar 26 19:38:39]leave time_json_c() json_object_to_json_string_ext
[Tue Mar 26 19:38:42]main exit


rapidjson serialization take 8 sec, to string take 0 sec, mem free take 0 sec
[Tue Mar 26 19:39:35]main()
[Tue Mar 26 19:39:36]enter time_rapidjson() serialization
[Tue Mar 26 19:39:43]leave time_rapidjson() serialization
[Tue Mar 26 19:39:43]enter time_rapidjson() to string
[Tue Mar 26 19:39:43]leave time_rapidjson() to string
[Tue Mar 26 19:39:43]main exit
---------
[Tue Mar 26 19:40:14]main()
[Tue Mar 26 19:40:14]enter time_rapidjson() serialization
[Tue Mar 26 19:40:22]leave time_rapidjson() serialization
[Tue Mar 26 19:40:22]enter time_rapidjson() to string
[Tue Mar 26 19:40:22]leave time_rapidjson() to string
[Tue Mar 26 19:40:22]main exit



*/
