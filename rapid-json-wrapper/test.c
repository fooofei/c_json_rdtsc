#include <stdio.h>
#include "rapidJSON_api.h"

int main(void) {
        
    // test 1
    tm_json_r_handler_t* rh;
    tm_json_parse(rh,"{\"a\":1}");

    // test 2
    tm_json_parse(rh,"{\"name\":\"Jim Cowart\",\"location\":{\"city\":{\"name\":\"Chattanooga\",\"population\":167674},\"state\":{\"name\":\"Tennessee\",\"abbreviation\":\"TN\",\"population\":6403000}},\"company\":\"appendTo\"}");

    // test 3
    tm_json_w_handler_t W3 = tm_json_write_create();
    tm_json_write_object_start(W3);
    tm_json_write_object_end(W3);
    printf("%s\n",tm_json_write_result(W3));
    tm_json_write_destroy(W3);

    // test 4
    tm_json_w_handler_t W4 = tm_json_write_create();
    tm_json_write_object_start(W4);
    tm_json_write_string(W4,"well");
    tm_json_write_object_start(W4);
    tm_json_write_string(W4,"well");
    tm_json_write_array_start(W4);
    tm_json_write_string(W4,"hello");
    tm_json_write_string(W4,"world");
    tm_json_write_array_end(W4);
    tm_json_write_object_end(W4);
    tm_json_write_object_end(W4);
    printf("%s\n",tm_json_write_result(W4));
    tm_json_write_destroy(W4);

    // test 5
    tm_json_w_handler_t W5 = tm_json_write_create();
    tm_json_write_object_start(W5);
    tm_json_write_string(W5,"hello");
    tm_json_write_string(W5,"world");
    tm_json_write_string(W5,"t");
    tm_json_write_boolean(W5,1);
    tm_json_write_string(W5,"f");
    tm_json_write_boolean(W5,0);
    tm_json_write_string(W5,"n");
    tm_json_write_null(W5);
    tm_json_write_string(W5,"i");
    tm_json_write_number(W5,123);
    tm_json_write_string(W5,"pi");
    tm_json_write_number(W5,3.1416);
    tm_json_write_string(W5,"a");
    tm_json_write_array_start(W5);
    unsigned i;
    for (i = 0; i < 4; ++i)
        tm_json_write_number(W5,i);
    tm_json_write_array_end(W5);
    tm_json_write_object_end(W5);
    printf("%s\n",tm_json_write_result(W5));
    tm_json_write_destroy(W5);

    return 0;
}