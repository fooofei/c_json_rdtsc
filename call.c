
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "rdtsc.h"
#include "cJSON/cJSON.h"
#include "cJSON/cJSON_Utils.h"
#include "crt_dbg_leak.h"

#include "rapid-json-wrapper/rapidJSON_api.h"

#include "jansson.h"



struct json_struct_sub
{
    int number1;
    int number2;
    int number3;
};

struct json_struct
{
    char result[0x100];
    char type[30];
    int number1;
    int number2;
    int number3;
    int sub_size;
    struct json_struct_sub sub[0];
};

// generate struct for the json libs serialization
struct json_struct * make_struct()
{
    struct json_struct * r = calloc(1, sizeof(struct json_struct) + 2 * sizeof(struct json_struct_sub));
    r->sub_size = 2;
    snprintf(r->type, sizeof(r->type), "%s", "json_struct");
    r->number1 = 111;
    r->number2 = 112;
    r->number3 = 113;
    r->sub[0].number1 = 1111;
    r->sub[0].number2 = 1112;
    r->sub[0].number3 = 1113;

    r->sub[1].number1 = 1121;
    r->sub[1].number2 = 1122;
    r->sub[1].number3 = 1123;

    snprintf(r->result, sizeof(r->result), "{\"type\":\"json_struct\",\"number1\":111,\"number2\":112,"
        "\"number3\":113,\"json_struct_sub\":[{\"number1\":1111,\"number2\":1112,\"number3\":1113},"
        "{\"number1\":1121,\"number2\":1122,\"number3\":1123}]}");
    return r;
}

void timepoint_cJSON()
{
    struct json_struct * p = 0;
    cJSON * root=0;
    int i;
    cJSON * array = 0;
    char * str = 0;
    (void)i;
    uint64_t start;
    
    
    p = make_struct();
    start = rdtsc();
    root = cJSON_CreateObject();

    cJSON_AddItemToObject(root, "type", cJSON_CreateString(p->type));
    cJSON_AddItemToObject(root, "number1", cJSON_CreateNumber(p->number1));
    cJSON_AddItemToObject(root, "number2", cJSON_CreateNumber(p->number2));
    cJSON_AddItemToObject(root, "number3", cJSON_CreateNumber(p->number3));

    array = cJSON_CreateArray();
    for (i=0;i<p->sub_size;i+=1)
    {
        cJSON * sub = cJSON_CreateObject();
        cJSON_AddItemToObject(sub, "number1", cJSON_CreateNumber(p->sub[i].number1));
        cJSON_AddItemToObject(sub, "number2", cJSON_CreateNumber(p->sub[i].number2));
        cJSON_AddItemToObject(sub, "number3", cJSON_CreateNumber(p->sub[i].number3));
        cJSON_AddItemToArray(array, sub);

    }
    cJSON_AddItemToObject(root, "json_struct_sub", array);
    
    str = cJSON_PrintUnformatted(root);

    if (0 != strcmp(p->result, str))
    {
        printf("[!] %s() result is unexpected\n", __func__);
    }
    
    free(str);
    free(p);
    cJSON_Delete(root);
    fprintf(stdout, "%s() cost %llu\n", __func__, rdtsc() - start); fflush(stdout);
}

void timepoint_rapidjson()
{
    uint64_t start;
    struct json_struct * p;
    struct tm_json_w_handler wh;
    int rc;
    int i;
    const char * result;
    size_t result_size;

    memset(&wh, 0, sizeof(wh));
    p = make_struct();
    start = rdtsc();
    wh = tm_json_write_create();
    rc = tm_json_write_object_start(wh);
    rc = tm_json_write_key(wh, "type");
    rc = tm_json_write_string(wh, p->type);
    rc = tm_json_write_key(wh, "number1");
    rc = tm_json_write_int(wh, p->number1);
    rc = tm_json_write_key(wh, "number2");
    rc = tm_json_write_int(wh, p->number2);
    rc = tm_json_write_key(wh, "number3");
    rc = tm_json_write_int(wh, p->number3);

    rc = tm_json_write_key(wh, "json_struct_sub");
    rc = tm_json_write_array_start(wh);
    for (i=0;i<p->sub_size;i+=1)
    {
        rc = tm_json_write_object_start(wh);
        rc = tm_json_write_key(wh, "number1");
        rc = tm_json_write_int(wh, p->sub[i].number1);
        rc = tm_json_write_key(wh, "number2");
        rc = tm_json_write_int(wh, p->sub[i].number2);
        rc = tm_json_write_key(wh, "number3");
        rc = tm_json_write_int(wh, p->sub[i].number3);

        rc = tm_json_write_object_end(wh);
    }
    rc = tm_json_write_array_end(wh);
    
    rc = tm_json_write_object_end(wh);
    result = tm_json_write_result(wh);
    result_size = tm_json_write_result_length(wh);

    if (0 != strncmp(p->result, result,result_size))
    {
        printf("[!] %s() result is unexpected\n", __func__);
    }

    free(p);
    tm_json_write_destroy(wh);

    fprintf(stdout, "%s() cost %llu\n", __func__, rdtsc() - start); fflush(stdout);
}

void timepoint_jansson()
{

    uint64_t start;
    struct json_struct * p;
    int rc;
    int i;
    char * result;
    json_t * root;
    json_t * array;
    json_t * obj;

    p = make_struct();
    start = rdtsc();
    root = json_object();

    rc =  json_object_set_new(root, "type", json_string(p->type));
    rc = json_object_set_new(root, "number1", json_integer(p->number1));
    rc = json_object_set_new(root, "number2", json_integer(p->number2));    
    rc = json_object_set_new(root, "number3", json_integer(p->number3));

    array = json_array();
    rc = json_object_set_new(root, "json_struct_sub", array);
    for (i=0;i<p->sub_size;i+=1)
    {
        obj = json_object();
        rc= json_array_append_new(array, obj);
        rc = json_object_set_new(obj, "number1", json_integer(p->sub[i].number1));
        rc = json_object_set_new(obj, "number2", json_integer(p->sub[i].number2));
        rc = json_object_set_new(obj, "number3", json_integer(p->sub[i].number3));
    }

    result = json_dumps(root, JSON_COMPACT);

    if (0 != strcmp(p->result, result))
    {
        printf("[!] %s() result is unexpected\n", __func__);
    }

    free(p);
    json_decref(root); root = 0;
    free(result);

    fprintf(stdout, "%s() cost %llu\n", __func__, rdtsc() - start); fflush(stdout);
}


int main()
{
    struct _crt_dbg_leak cdl;
    memset(&cdl, 0, sizeof(cdl));
    crt_dbg_leak_lock(&cdl);
    timepoint_cJSON();
    timepoint_rapidjson();
    timepoint_jansson();
    crt_dbg_leak_unlock(&cdl);

    return 0;
}


/*
看 rapidjson 提供的数据
https://github.com/miloyip/nativejson-benchmark
以为 rapidjson 在这里的测试会优于 cJSON，但是结果不是这样。
-------------------------------------
on a VirtualBox Windows x64
timepoint_cJSON() cost 365520
timepoint_rapidjson() cost 837615

timepoint_cJSON() cost 327702
timepoint_rapidjson() cost 763839

timepoint_cJSON() cost 617322
timepoint_rapidjson() cost 2466603

timepoint_cJSON() cost 12161178
timepoint_rapidjson() cost 4461243
timepoint_jansson() cost 14578179

timepoint_cJSON() cost 848694
timepoint_rapidjson() cost 4415040
timepoint_jansson() cost 3874536

timepoint_cJSON() cost 349515
timepoint_rapidjson() cost 1104249
timepoint_jansson() cost 5859333

--------------------------------------
on macOS 8GB i5
timepoint_cJSON() cost 343344
timepoint_rapidjson() cost 139077

timepoint_cJSON() cost 303678
timepoint_rapidjson() cost 150321

timepoint_cJSON() cost 252306
timepoint_rapidjson() cost 132498

timepoint_cJSON() cost 198231
timepoint_rapidjson() cost 134577
timepoint_jansson() cost 328386

timepoint_cJSON() cost 248658
timepoint_rapidjson() cost 125205
timepoint_jansson() cost 287448

timepoint_cJSON() cost 246441
timepoint_rapidjson() cost 192180
timepoint_jansson() cost 383229

timepoint_cJSON() cost 190023
timepoint_rapidjson() cost 124821
timepoint_jansson() cost 296121

timepoint_cJSON() cost 315387
timepoint_rapidjson() cost 132606
timepoint_jansson() cost 370143
-------------------------------

*/
