/******************************************************************************
 * Objective: Interface between the C functions needed in Lua to the C++
 *            functions that rapidJSON uses. This acts as the actual interface
 * Author:    Kenneth Nierenhausen
 * Date:      July 11, 2014
 * Date:      May 13, 2018
 *****************************************************************************/
#include <iostream>
#include <inttypes.h>

#include "./rapidJSON_api.h"
#include "rapidjson/reader.h"
#include "rapidjson/writer.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/stringbuffer.h"

/* Used to call functions in the rapidJSON namespace */
using namespace rapidjson;

/* Converts string to input stream and feeds it to rapidJSON Parse function */
extern "C" bool tm_json_parse(tm_json_r_handler_t* rh,const char* json_s) {

	// allocate memory for the struct
	rh = (tm_json_r_handler_t*)malloc(sizeof(tm_json_r_handler_t));

	// link the function pointers to the callbacks
	rh->Default = tm_json_read_default;
	rh->Null = tm_json_read_null;
	rh->Bool = tm_json_read_boolean;
	rh->Int = tm_json_read_int;
	rh->Uint = tm_json_read_uint;
	rh->Int64 = tm_json_read_int64;
	rh->Uint64 = tm_json_read_uint64;
	rh->Double = tm_json_read_double;
	rh->String = tm_json_read_string;
	rh->StartObject = tm_json_read_startObject;
	rh->EndObject = tm_json_read_endObject;
	rh->StartArray = tm_json_read_startArray;
	rh->EndArray = tm_json_read_endArray;
    rh->Key = tm_json_read_key;
    rh->RawNumber = tm_json_read_rawnumber;

	// create an input stream from the stringified JSON input
	StringStream is(json_s);

	// create a defaults flags GenericReader object
	Reader reader;

	// get a copy of the reader handler
	tm_json_r_handler_t rh_copy = *rh;

	// get rid of the allocated space
	free(rh);

	// call rapidJSON's Parser using the input stream and the given handler
	return reader.Parse(is,rh_copy);

}

/* Collection of callbacks that get invoked by rapidJSON when it parses */
extern "C" bool tm_json_read_default(void) { printf("DEFAULT\n"); return true; }
extern "C" bool tm_json_read_null(void) { printf("NULL\n"); return true;  }
extern "C" bool tm_json_read_boolean(bool val) { printf("BOOL: %s\n", val ? "true" : "false");  return true; }
extern "C" bool tm_json_read_int(int val) { printf("INT: %d\n", val); return true; }
extern "C" bool tm_json_read_uint(unsigned val) { printf("UINT: %d\n", val); return true; }

/* MSVC 2017 error :
If is "INT_64: %"PRId64"\n"
error C3688: 文本后缀“PRIu64”无效；未找到文文本运算符或文本运算符模板“operator """"PRIu64”
*/
extern "C" bool tm_json_read_int64(int64_t val) { printf("INT_64: %" PRId64"\n", val); return true; }

extern "C" bool tm_json_read_uint64(uint64_t val) { printf("UINT_64: %" PRIu64"\n", val); return true; }
extern "C" bool tm_json_read_double(double val) { printf("DOUBLE: %f\n", val); return true; }
extern "C" bool tm_json_read_string(const char* val, size_t len, bool set) { printf("STRING: %s\n", val); return true; }
extern "C" bool tm_json_read_startObject(void) { printf("START_OBJECT\n"); return true; }
extern "C" bool tm_json_read_endObject(size_t val) { printf("END_OBJECT: %llu\n", (unsigned long long)val); return true; }
extern "C" bool tm_json_read_startArray(void) { printf("START_ARRAY\n"); return true; }
extern "C" bool tm_json_read_endArray(size_t val) { printf("END_ARRAY: %llu\n", (unsigned long long)val); return true; }
extern "C" bool tm_json_read_key(const char * str, size_t size, bool)
{
    printf("KEY: %.*s\n", (int)size, str);
    return true;
}
extern "C" bool tm_json_read_rawnumber(const char * str, size_t size, bool)
{
    printf("RAWNumber: %.*s\n", (int)size, str);
    return true;
}

/* Creates a new Writer object and returns it to C as a void pointer */
extern "C" tm_json_w_handler_t tm_json_write_create() {

	// create the writer handler
	tm_json_w_handler_t wh;
	
	// allocate the writer and assugn in in the struct
	StringBuffer* sb = new StringBuffer();
	wh.stringBuffer = static_cast<tm_json_stringbuffer_t>(sb);

	// allocate the string buffer and assign it in the struct
	Writer<StringBuffer>* w = new Writer<StringBuffer>(*sb);
	wh.writer = static_cast<tm_json_writer_t>(w);

	// return the actual write handler containing void pointers
	return wh;
}

/* Writes out a String using rapidJSON functions */
extern "C" int tm_json_write_string(tm_json_w_handler_t wh, const char* value) {
	Writer<StringBuffer>* w = static_cast<Writer<StringBuffer>*>(wh.writer);
    return w->String(value) ? 0 : -1;
}

/* Writes out a Bool using rapidJSON functions */
extern "C" int tm_json_write_boolean (tm_json_w_handler_t wh, int value) {
	Writer<StringBuffer>* w = static_cast<Writer<StringBuffer>*>(wh.writer);
    return w->Bool(value) ? 0 : -1;
}

/* Writes out a Number using rapidJSON functions */
extern "C" int tm_json_write_number (tm_json_w_handler_t wh, double value) {
	Writer<StringBuffer>* w = static_cast<Writer<StringBuffer>*>(wh.writer);
    return w->Double(value) ? 0 : -1;
}

extern "C" int tm_json_write_int(tm_json_w_handler_t wh, int value) {
    Writer<StringBuffer>* w = static_cast<Writer<StringBuffer>*>(wh.writer);
    return w->Int(value) ? 0 : -1;
}
extern "C" int tm_json_write_uint(tm_json_w_handler_t wh, uint32_t value) {
    Writer<StringBuffer>* w = static_cast<Writer<StringBuffer>*>(wh.writer);
    return w->Uint(value) ? 0 : -1;
}
extern "C" int tm_json_write_int64(tm_json_w_handler_t wh, int64_t value) {
    Writer<StringBuffer>* w = static_cast<Writer<StringBuffer>*>(wh.writer);
    return w->Int64(value) ? 0 : -1;
}

extern "C" int tm_json_write_uint64(tm_json_w_handler_t wh, uint64_t value) {
    Writer<StringBuffer>* w = static_cast<Writer<StringBuffer>*>(wh.writer);
    return w->Uint64(value) ? 0 : -1;
}

/* Writes out Null using rapidJSON functions */
extern "C" int tm_json_write_null (tm_json_w_handler_t wh) {
	Writer<StringBuffer>* w = static_cast<Writer<StringBuffer>*>(wh.writer);
    return w->Null() ? 0 : -1;
}

/* Writes out an object start using rapidJSON functions */
extern "C" int tm_json_write_object_start (tm_json_w_handler_t wh) {
	Writer<StringBuffer>* w = static_cast<Writer<StringBuffer>*>(wh.writer);
    return w->StartObject() ? 0 : -1;
}

/* Writes out an object end using rapidJSON functions */
extern "C" int tm_json_write_object_end (tm_json_w_handler_t wh) {
	Writer<StringBuffer>* w = static_cast<Writer<StringBuffer>*>(wh.writer);
    return w->EndObject()? 0:-1;
}

/* Writes out an array start using rapidJSON functions */
extern "C" int tm_json_write_array_start (tm_json_w_handler_t wh) {
	Writer<StringBuffer>* w = static_cast<Writer<StringBuffer>*>(wh.writer);
    return w->StartArray() ? 0 : -1;
}

/* Writes out an array end using rapidJSON functions */
extern "C" int tm_json_write_array_end (tm_json_w_handler_t wh) {
	Writer<StringBuffer>* w = static_cast<Writer<StringBuffer>*>(wh.writer);
    return w->EndArray() ? 0 : -1;
}

/* returns Writer<StringBuffer>::getString() */
extern "C" const char* tm_json_write_result (tm_json_w_handler_t wh) {
	StringBuffer* sb = static_cast<StringBuffer*>(wh.stringBuffer);
	return sb->GetString();
}

extern "C" size_t tm_json_write_result_length(tm_json_w_handler_t wh) {
    StringBuffer* sb = static_cast<StringBuffer*>(wh.stringBuffer);
    return sb->GetLength();
}

/* frees the writer, string buffer, and the struct holding them all */
extern "C" int tm_json_write_destroy(tm_json_w_handler_t wh) {
	delete static_cast<Writer<StringBuffer>*>(wh.writer);
	delete static_cast<StringBuffer*>(wh.stringBuffer);
	return 0;
}

extern "C" int tm_json_write_flush(tm_json_w_handler_t wh) {
    Writer<StringBuffer>* w = static_cast<Writer<StringBuffer>*>(wh.writer);
    w->Flush();
    return 0;
}

extern "C" int tm_json_write_key(tm_json_w_handler_t wh, const char * value){
    Writer<StringBuffer>* w = static_cast<Writer<StringBuffer>*>(wh.writer);
    return w->Key(value) ? 0 : -1;
}
extern "C" int tm_json_write_keyn(tm_json_w_handler_t wh, const char * value, size_t value_size){
    Writer<StringBuffer>* w = static_cast<Writer<StringBuffer>*>(wh.writer);
    return w->Key(value,value_size) ? 0 : -1;
}
extern "C" int tm_json_write_stringn(tm_json_w_handler_t wh, const char * value, size_t value_size) {
    Writer<StringBuffer>* w = static_cast<Writer<StringBuffer>*>(wh.writer);
    return w->String(value,value_size) ? 0 : -1;
}
