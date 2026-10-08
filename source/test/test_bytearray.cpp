#include <qtac/ByteArray.h>
#include <cassert>
#include <cstdio>

int main() {
    using qtac::ByteArray;

    // Null vs empty
    ByteArray null;
    assert(null.isNull());
    assert(null.isEmpty());
    ByteArray empty("");
    assert(!empty.isNull());
    assert(empty.isEmpty());

    // Iterators
    ByteArray ba("hello");
    int n = 0;
    for (auto it = ba.begin(); it != ba.end(); ++it) ++n;
    assert(n == 5);

    // Insert/Remove
    ByteArray ins("ac");
    ins.insert(1, "b");
    assert(ins == "abc");
    ins.remove(1, 1);
    assert(ins == "ac");

    // Chop/truncate
    ByteArray ch("hello");
    ch.chop(2);
    assert(ch == "hel");
    ch.truncate(2);
    assert(ch == "he");

    // Count
    ByteArray cnt("abcabc");
    assert(cnt.count('a') == 2);
    assert(cnt.count(ByteArray("abc")) == 2);

    // Numeric
    ByteArray num("42");
    bool ok = false;
    assert(num.toUInt(&ok) == 42u && ok);
    ByteArray big("18446744073709551615");
    assert(big.toULongLong(&ok) == 18446744073709551615ULL && ok);

    // Compare
    ByteArray a("Hello"), b("hello");
    assert(a.compare(b, true) != 0);
    assert(a.compare(b, false) == 0);

    // Hex
    ByteArray raw("\xDE\xAD", 2);
    assert(raw.toHex() == "dead");
    ByteArray decoded = ByteArray::fromHex(ByteArray("DEAD"));
    assert(decoded.size() == 2);

    // Base64
    ByteArray plain("Hello");
    ByteArray encoded = plain.toBase64();
    assert(encoded == "SGVsbG8=");
    ByteArray roundtrip = ByteArray::fromBase64(encoded);
    assert(roundtrip == "Hello");

    printf("ALL TESTS PASSED\n");
    return 0;
}
