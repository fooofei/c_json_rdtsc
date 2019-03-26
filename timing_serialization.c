


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

// `jansson`'s defines conflict with `json-c`'s defines
#ifdef TIME_JANSSON
#include "jansson.h"
#endif // TIME_JANSSON

#include "json-c/json.h"


struct item
{
    char uuid[36];
    char action[10];
    char version[10];
    uint32_t ip; // 1760484994
};

static void log(const char * fmt, ...) __attribute__((format(printf,1,2)));
static void log(const char * fmt, ...)
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
    const int size = 100*1024;
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
    log("enter %s() serialization %s:%d", __func__, __FILE__, __LINE__);
   cJSON * root = cJSON_CreateObject();
   cJSON * stat = cJSON_CreateArray();
   cJSON_AddItemToObject(root, "stat", stat);
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
    log("leave %s() serialization %s:%d", __func__, __FILE__, __LINE__);
    log("enter %s() cJSON_PrintUnformatted %s:%d", __func__, __FILE__, __LINE__);
    char * s = cJSON_PrintUnformatted(root);
    //printf("%s\n",s);
    free(s);
    log("leave %s() cJSON_PrintUnformatted %s:%d", __func__, __FILE__, __LINE__);
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
    log("enter %s() serialization %s:%d", __func__, __FILE__, __LINE__);
    json_t * root = json_object();
    json_t * stat = json_array();
    json_object_set_new(root, "stat", stat);
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
    log("leave %s() serialization %s:%d", __func__, __FILE__, __LINE__);
    log("enter %s() cJSON_PrintUnformatted %s:%d", __func__, __FILE__, __LINE__);
    char * s = json_dumps(root, JSON_COMPACT);
    //printf("%s\n",s);
    free(s);
    log("leave %s() cJSON_PrintUnformatted %s:%d", __func__, __FILE__, __LINE__);
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
static void time_json_c(struct item * p, int size)
{
    log("enter %s() serialization %s:%d", __func__, __FILE__, __LINE__);
    struct json_object * root = json_object_new_object();
    struct json_object * stat = json_object_new_array();
    json_object_object_add(root, "stat", stat);
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
    log("leave %s() serialization %s:%d", __func__, __FILE__, __LINE__);
    log("enter %s() cJSON_PrintUnformatted %s:%d", __func__, __FILE__, __LINE__);
    const char * s = json_object_to_json_string_ext(root, JSON_C_TO_STRING_PLAIN);
    printf("%s\n",s);
    log("leave %s() cJSON_PrintUnformatted %s:%d", __func__, __FILE__, __LINE__);
    json_object_put(root); root = 0;
}


int main(){

    log("%s() %s:%d", __func__, __FILE__, __LINE__);

    struct item * c_struct = NULL;
    int size=0;
    make_struct(&c_struct, &size);


    //time_jansson(c_struct, size);
    //time_cJSON(c_struct, size);
    time_json_c(c_struct, size);

    free(c_struct);
    c_struct = NULL;

    log("main exit\n");
    return 0;
}
