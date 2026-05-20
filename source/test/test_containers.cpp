#include <qtac/List.h>
#include <qtac/Map.h>
#include <qtac/Variant.h>
#include <qtac/Pair.h>
#include <qtac/String.h>
#include <cassert>
#include <cstdio>

int main() {
    using qtac::List;
    using qtac::Map;
    using qtac::Variant;
    using qtac::String;

    // ===== List tests =====

    // Basic operations
    List<int> list;
    assert(list.isEmpty());
    assert(list.size() == 0);

    list.append(10);
    list.append(20);
    list.append(30);
    assert(list.size() == 3);
    assert(!list.isEmpty());
    assert(list.at(0) == 10);
    assert(list.at(1) == 20);
    assert(list.at(2) == 30);
    assert(list.first() == 10);
    assert(list.last() == 30);

    // Insert
    list.insert(1, 15);
    assert(list.size() == 4);
    assert(list.at(1) == 15);
    assert(list.at(2) == 20);

    // RemoveAt
    list.removeAt(1);
    assert(list.size() == 3);
    assert(list.at(1) == 20);

    // Contains / indexOf
    assert(list.contains(20));
    assert(!list.contains(99));
    assert(list.indexOf(20) == 1);
    assert(list.indexOf(99) == -1);

    // RemoveOne / RemoveAll
    List<int> dupList = {1, 2, 3, 2, 4, 2};
    assert(dupList.removeOne(2));
    assert(dupList.size() == 5);
    assert(dupList.at(1) == 3); // first 2 removed
    int removed = dupList.removeAll(2);
    assert(removed == 2);
    assert(dupList.size() == 3);

    // TakeAt
    List<int> takeList = {10, 20, 30};
    int taken = takeList.takeAt(1);
    assert(taken == 20);
    assert(takeList.size() == 2);

    // Clear
    list.clear();
    assert(list.isEmpty());

    // Range-for
    List<int> rList = {1, 2, 3, 4, 5};
    int sum = 0;
    for (int val : rList) sum += val;
    assert(sum == 15);

    // Operators
    List<int> l1 = {1, 2};
    List<int> l2 = {3, 4};
    List<int> l3 = l1 + l2;
    assert(l3.size() == 4);
    assert(l3.at(2) == 3);

    // ===== Map tests =====

    // Basic operations
    Map<String, int> map;
    assert(map.isEmpty());
    map.insert(String("one"), 1);
    map.insert(String("two"), 2);
    map.insert(String("three"), 3);
    assert(map.size() == 3);
    assert(!map.isEmpty());

    // Value with default
    assert(map.value(String("one")) == 1);
    assert(map.value(String("two")) == 2);
    assert(map.value(String("missing"), -1) == -1);
    assert(map.value(String("missing")) == 0); // default int

    // Contains
    assert(map.contains(String("one")));
    assert(!map.contains(String("four")));

    // Remove
    assert(map.remove(String("two")) == 1);
    assert(map.size() == 2);
    assert(!map.contains(String("two")));
    assert(map.remove(String("nonexist")) == 0);

    // Keys / Values
    Map<String, int> kvMap;
    kvMap.insert(String("a"), 1);
    kvMap.insert(String("b"), 2);
    auto keys = kvMap.keys();
    assert(keys.size() == 2);
    auto values = kvMap.values();
    assert(values.size() == 2);

    // Operator[]
    Map<String, String> smap;
    smap[String("key")] = String("value");
    assert(smap[String("key")] == "value");

    // Range-for
    Map<int, int> imap;
    imap.insert(1, 10);
    imap.insert(2, 20);
    int mapSum = 0;
    for (const auto& [k, v] : imap) mapSum += v;
    assert(mapSum == 30);

    // ===== Variant tests =====

    // Invalid
    Variant invalid;
    assert(!invalid.isValid());
    assert(invalid.type() == Variant::Type::Invalid);

    // Bool
    Variant vBool(true);
    assert(vBool.isValid());
    assert(vBool.type() == Variant::Type::Bool);
    assert(vBool.toBool() == true);
    assert(vBool.toInt() == 1);

    // Int
    Variant vInt(42);
    assert(vInt.type() == Variant::Type::Int);
    assert(vInt.toInt() == 42);
    assert(vInt.toString() == "42");
    assert(vInt.toDouble() == 42.0);
    assert(vInt.toBool() == true);

    Variant vZero(0);
    assert(vZero.toBool() == false);

    // UInt
    Variant vUInt(100u);
    assert(vUInt.type() == Variant::Type::UInt);
    assert(vUInt.toUInt() == 100u);

    // LongLong
    Variant vLL(static_cast<long long>(123456789012345LL));
    assert(vLL.type() == Variant::Type::LongLong);
    assert(vLL.toLongLong() == 123456789012345LL);

    // ULongLong
    Variant vULL(static_cast<unsigned long long>(18446744073709551615ULL));
    assert(vULL.type() == Variant::Type::ULongLong);
    assert(vULL.toULongLong() == 18446744073709551615ULL);

    // Double
    Variant vDouble(3.14);
    assert(vDouble.type() == Variant::Type::Double);
    assert(vDouble.toDouble() > 3.13 && vDouble.toDouble() < 3.15);

    // String
    Variant vStr(String("hello"));
    assert(vStr.type() == Variant::Type::String);
    assert(vStr.toString() == "hello");
    assert(vStr.toBool() == true);

    Variant vEmptyStr(String(""));
    assert(vEmptyStr.toBool() == false);

    // String from numeric
    Variant vNum(123);
    assert(vNum.toString() == "123");

    // String to numeric
    Variant vStrNum(String("456"));
    bool convOk = false;
    assert(vStrNum.toInt(&convOk) == 456);
    assert(convOk);

    // ByteArray
    Variant vBa(qtac::ByteArray("bytes"));
    assert(vBa.type() == Variant::Type::ByteArray);
    assert(vBa.toByteArray() == "bytes");

    // Template value<>
    assert(vInt.value<int>() == 42);
    assert(vStr.value<String>() == "hello");

    // Comparison
    assert(Variant(42) == Variant(42));
    assert(Variant(42) != Variant(43));
    assert(Variant(String("a")) != Variant(String("b")));

    // ===== Pair tests =====

    auto p = qtac::makePair(String("key"), 42);
    assert(p.first == "key");
    assert(p.second == 42);

    qtac::Pair<int, int> p2 = {1, 2};
    assert(p2.first == 1 && p2.second == 2);

    std::printf("All container tests passed!\n");
    return 0;
}
