#include "lib/rapidjson/include/rapidjson/writer.h"
#include "lib/rapidjson/include/rapidjson/stringbuffer.h"
#include <iostream>

using namespace rapidjson;
using namespace std;

int main() {

    // test 1
    printf("START_OBJECT\nSTRING: a\nUINT: 1\nEND_OBJECT: 1\n");

    // test 2
    printf("START_OBJECT\nSTRING: name\nSTRING: Jim Cowart\nSTRING: location\nSTART_OBJECT\nSTRING: city\nSTART_OBJECT\nSTRING: name\nSTRING: Chattanooga\nSTRING: population\nUINT: 167674\nEND_OBJECT: 2\nSTRING: state\nSTART_OBJECT\nSTRING: name\nSTRING: Tennessee\nSTRING: abbreviation\nSTRING: TN\nSTRING: population\nUINT: 6403000\nEND_OBJECT: 3\nEND_OBJECT: 2\nSTRING: company\nSTRING: appendTo\nEND_OBJECT: 3\n");

    // test 3
    StringBuffer S3;
    Writer<StringBuffer> W3(S3);
    W3.StartObject();
    W3.EndObject();
    cout << S3.GetString() << endl;

    // test 4
    StringBuffer S4;
    Writer<StringBuffer> W4(S4);    
    W4.StartObject();
    W4.String("well");
    W4.StartObject();
    W4.String("well");
    W4.StartArray();
    W4.String("hello");
    W4.String("world");
    W4.EndArray();
    W4.EndObject();
    W4.EndObject();
    cout << S4.GetString() << endl;

    // test 5
    StringBuffer S5;
    Writer<StringBuffer> W5(S5);    
    W5.StartObject();
    W5.String("hello");
    W5.String("world");
    W5.String("t");
    W5.Bool(true);
    W5.String("f");
    W5.Bool(false);
    W5.String("n");
    W5.Null();
    W5.String("i");
    W5.Uint(123);
    W5.String("pi");
    W5.Double(3.1416);
    W5.String("a");
    W5.StartArray();
    for (unsigned i = 0; i < 4; i++)
        W5.Uint(i);
    W5.EndArray();
    W5.EndObject();
    cout << S5.GetString() << endl;

    return 0;
}