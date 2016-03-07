#include <cassert>
#include <cmath>
#include <iostream>
#include <limits>
#include <stack>
#include <string>

using std::cout;
using std::numeric_limits;
using std::stack;
using std::string;
using std::to_string;

const bool kInArray = true;

struct Level;

// TODO(dannas): Add hasErrors() method.

// This class creates a "minimized" JSON string.
//
// Before you use this you should probably understand the basic
// concepts in the JSON type system:
//
//    Value  : String | Bool | Null | Object | Array | Number
//    String : UTF-8 sequence
//    Object : (String, Value) pairs, with unique String keys
//    Array  : ordered list of Values
//    Null   : null
//    Bool   : true | false
//    Number : (representation unspecified)
//
// ... That's about it.  For more information see http://json.org or
// look up RFC 4627.
//
// Caller is responsible for order of startX()/endX() calls and setX() calls.
//
// The json string is expected to be utf-8 encoded. Caller must only provide utf-8
// encoded indata.
//
// Delimiters between object members and array elements are automatically inserted.
//
// Use JsonWriter::str() for retrieving the string.
//
// Example usage:
//
//    JsonWriter w;
//    w.startObject();
//        w.setstr("foo");
//        w.setstr("bar");
//        w.setstr("baz");
//        w.startObject();
//            w.setstr("\\");
//            w.setstr("Δ");
//            w.setstr("\"");
//            w.setstr("\x01");
//            w.setstr("\x1f");
//            w.setstr("\x20");
//            w.startObject();
//                w.setstr("foo");
//                w.setnumber(42);
//                w.setnumber(3.14);
//            w.endObject();
//        w.endObject();
//    w.endObject();
//    cout << w.str() << "\n";
//
// Output:
//    {"foo":"bar","baz":{"\\":"Δ","\"":"\u0001","\u001f":" ",{"foo":42,3.14}}}
//
class JsonWriter
{
public:
    // Constructs an empty JsonWriter.
    JsonWriter();

    void startObject();
    void endObject();
    void startArray();
    void endArray();

    // The string must be utf-8 encoded.
    bool setstr(const string &s);

    // The number must be representable as a sequence of digits. Infinity and Nan
    // are not permitted.
    // TODO(dannas): What range for the number? The spec doesn't say.
    // I've read that javascript only supports 2^51 bits...
    bool setnumber(int x);
    bool setnumber(double x);

    // Returns a reference to the constructed json string.
    // The caller must copy the string if they want to modify it.
    const string& str() const;

private:
    // Inserts delimiter for arrays and objects when needed.
    void prefix();

    string escape(const string &s);
    string tohex(unsigned char c);

    string json_;           // the product
    stack<Level> levels_;   // keeps track of array/object nesting
};

// Keeps track of nesting of Object and Arrays.
struct Level
{
    Level(bool in = false) : n(0), inarray(in) { }

    int n;          // number of items in this object or array
    bool inarray;   // inside array?
};

JsonWriter::JsonWriter()
    : json_(""), levels_()
{
}

void JsonWriter::startObject()
{
    prefix();
    levels_.push(Level());
    json_ += '{';
}

void JsonWriter::endObject()
{
    levels_.pop();
    json_ += '}';
}

void JsonWriter::startArray()
{
    prefix();
    levels_.push(Level(kInArray));
    json_ += '[';
}

void JsonWriter::endArray()
{
    levels_.pop();
    json_ += ']';
}

const string& JsonWriter::str() const
{
    return json_;
}

bool JsonWriter::setstr(const string &s)
{
    prefix();
    json_ += '"' + escape(s) + '"';
    return true;
}

bool JsonWriter::setnumber(int x)
{
    prefix();
    json_ += to_string(x);
    return true;
}

bool JsonWriter::setnumber(double x)
{
    // RFC 4627. Section 2.4:
    // Numeric values that cannot be represented as sequences of digits
    // (such as Infinity and NaN) are not permitted.
    if (isnan(x) || isinf(x))
        return false;

    prefix();

    // Remove trailing zeroes.
    string s = to_string(x);
    s.erase(s.find_last_not_of('0') + 1, string::npos);
    if (!s.empty() && s.back() == '.')
        s.erase(s.end() - 1);

    json_ += s;
    return true;
}

void JsonWriter::prefix()
{
    if (levels_.empty())
        return;

    auto &l = levels_.top();
    if (l.n > 0) {
        if (l.inarray)
            json_ += ',';
        else
            json_ += ( l.n % 2 == 0 ? ',' : ':');
    }
    l.n++;
}

string JsonWriter::escape(const string &s)
{
    // RFC 4627. Section 2.5
    string out;
    for (unsigned char c : s) {
        if      (c == '"')  out += "\\\"";
        else if (c == '\\') out += "\\\\";
        else if (c == '/')  out += "\\/";
        else if (c == '\b') out += "\\b";
        else if (c == '\f') out += "\\f";
        else if (c == '\n') out += "\\n";
        else if (c == '\r') out += "\\r";
        else if (c == '\t') out += "\\t";
        else if (c <= 0x1f) out += "\\u00" + tohex(c);
        else                out += c;
    }
    return out;
}

string JsonWriter::tohex(unsigned char c)
{
    static char digits[] = "0123456789abcdef";
    string out;
    out += digits[c >> 4];
    out += digits[c & 0x0f];
    return out;
}


// Test helper. Takes JsonWriter and expected json string as input.
#define TEST(w, expected) do { \
    if (w.str() != expected)  \
        cout << __FILE__ << ":" << __LINE__  \
             << " FAIL " << w.str() << " != " << expected << "\n"; \
    } while (0)

int main()
{
    // Empty
    JsonWriter w;
    TEST(w, "");

    // Empty object
    w = JsonWriter();
    w.startObject();
    w.endObject();
    TEST(w, "{}");

    // Empty array
    w = JsonWriter();
    w.startArray();
    w.endArray();
    TEST(w, "[]");

    // Array, one element
    w = JsonWriter();
    w.startArray();
    w.setstr("foo");
    w.endArray();
    TEST(w, "[\"foo\"]");

    // Object, one key-val
    w = JsonWriter();
    w.startObject();
    w.setstr("key");
    w.setstr("val");
    w.endObject();
    TEST(w, "{\"key\":\"val\"}");

    // Array, numbers
    w = JsonWriter();
    w.startArray();
    w.setnumber(42);
    w.setnumber(3.14);
    w.endArray();
    TEST(w, "[42,3.14]");

    // String, non-ascii
    w = JsonWriter();
    w.setstr("Δ");
    TEST(w, "\"Δ\"");

    // String array, visible control chars
    w = JsonWriter();
    w.startArray();
    w.setstr("\\");
    w.setstr("\"");
    w.setstr("/");
    w.endArray();
    TEST(w, "[\"\\\\\", \"\\\", \"\\/\"]");

    // TODO(dannas): add test for NaN and infinity
}
