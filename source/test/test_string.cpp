#include <qtac/String.h>
#include <qtac/StringList.h>
#include <cassert>
#include <cstdio>

int main() {
    using qtac::String;
    using qtac::StringList;

    // --- Constructors ---
    String null;
    assert(null.isNull());
    assert(null.isEmpty());

    String empty("");
    assert(!empty.isNull());
    assert(empty.isEmpty());

    String hello("hello");
    assert(!hello.isNull());
    assert(!hello.isEmpty());
    assert(hello.size() == 5);

    String fromStd(std::string("world"));
    assert(fromStd == "world");

    String fromChar('x');
    assert(fromChar == "x");

    // --- toLatin1 / toUtf8 ---
    auto ba = hello.toLatin1();
    assert(ba == "hello");
    assert(ba.size() == 5);

    // --- Numeric conversions ---
    bool ok = false;
    assert(String("42").toInt(&ok) == 42 && ok);
    assert(String("ff").toInt(&ok, 16) == 255 && ok);
    assert(String("abc").toInt(&ok) == 0 && !ok);
    assert(String("100").toUInt(&ok) == 100u && ok);
    assert(String("18446744073709551615").toULongLong(&ok) == 18446744073709551615ULL && ok);
    assert(String("3.14").toDouble(&ok) > 3.13 && ok);

    // --- Case conversion ---
    assert(String("Hello").toUpper() == "HELLO");
    assert(String("Hello").toLower() == "hello");

    // --- Trimming ---
    assert(String("  hi  ").trimmed() == "hi");
    assert(String("  hello   world  ").simplified() == "hello world");
    assert(String("").simplified() == "");
    assert(String("   ").simplified() == "");

    // --- Search ---
    assert(String("hello world").contains("world"));
    assert(!String("hello world").contains("xyz"));
    assert(String("Hello").contains("hello", false));
    assert(String("hello world").indexOf("world") == 6);
    assert(String("hello world").indexOf("xyz") == -1);
    assert(String("hello").startsWith("hel"));
    assert(!String("hello").startsWith("xyz"));
    assert(String("hello").endsWith("llo"));

    // --- Substrings ---
    assert(String("hello world").mid(6) == "world");
    assert(String("hello world").mid(0, 5) == "hello");
    assert(String("hello").left(3) == "hel");
    assert(String("hello").right(3) == "llo");

    // --- Split / Join ---
    StringList parts = String("a,b,c").split(',');
    assert(parts.size() == 3);
    assert(parts.at(0) == "a");
    assert(parts.at(1) == "b");
    assert(parts.at(2) == "c");

    String joined = parts.join(String(","));
    assert(joined == "a,b,c");

    StringList parts2 = String("hello world foo").split(String(" "));
    assert(parts2.size() == 3);
    assert(parts2.at(2) == "foo");

    // --- Replace ---
    String s("hello world");
    s.replace(String("world"), String("earth"));
    assert(s == "hello earth");

    // --- Arg ---
    String tmpl("File %1 has %2 lines");
    String result = tmpl.arg("test.cpp").arg(42);
    assert(result == "File test.cpp has 42 lines");

    String tmpl2("%1 + %1 = %2");
    String result2 = tmpl2.arg("x").arg("2x");
    assert(result2 == "x + x = 2x");

    String tmpl3("%1 %2 %3");
    String result3 = tmpl3.arg("a", "b", "c");
    assert(result3 == "a b c");

    // --- Compare ---
    assert(String("abc").compare("abc") == 0);
    assert(String("ABC").compare("abc", false) == 0);
    assert(String("ABC").compare("abc", true) != 0);

    // --- Static number ---
    assert(String::number(42) == "42");
    assert(String::number(255, 16) == "ff");
    assert(String::number(0u) == "0");

    // --- Operators ---
    String a("hello");
    String b(" world");
    assert(a + b == "hello world");
    a += b;
    assert(a == "hello world");

    // --- StringList operations ---
    StringList sl;
    sl.append(String("one"));
    sl.append(String("two"));
    sl.append(String("three"));
    assert(sl.size() == 3);
    assert(sl.contains(String("two")));
    assert(!sl.contains(String("four")));
    assert(sl.indexOf(String("two")) == 1);
    sl.removeAt(1);
    assert(sl.size() == 2);
    assert(sl.at(1) == "three");

    StringList filtered = StringList({"apple", "banana", "apricot"}).filter(String("ap"));
    assert(filtered.size() == 2);

    std::printf("All String tests passed!\n");
    return 0;
}
