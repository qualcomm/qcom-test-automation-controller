# qtac-core Library

Qt-free replacement types for the QTAC project. This static library provides drop-in alternatives to common Qt classes used throughout QCommonConsole and TACDev, eliminating the Qt dependency from core library code.

## Directory Structure

```
source/library/
├── cmake/
│   └── QtacCommon.cmake       # Shared build settings (platform flags, output paths)
├── include/qtac/
│   ├── ByteArray.h            # QByteArray replacement (header-only)
│   ├── String.h               # QString replacement
│   ├── StringList.h           # QStringList replacement
│   ├── Variant.h              # QVariant replacement
│   ├── Map.h                  # QMap replacement (header-only template)
│   ├── List.h                 # QList replacement (header-only template)
│   └── Pair.h                 # QPair replacement (header-only alias)
├── src/
│   ├── qtac_core.cpp          # Library version info
│   ├── String.cpp             # String method definitions
│   ├── StringList.cpp         # StringList + String::split() definitions
│   └── Variant.cpp            # Variant type conversion definitions
└── CMakeLists.txt             # Builds qtac-core static library
```

## Classes

### `qtac::ByteArray` (replaces `QByteArray`)
Header-only. Wraps `std::string` with Qt-compatible API: append, prepend, insert, remove, hex/base64 encoding, numeric conversions, split, search, and comparison.

### `qtac::String` (replaces `QString`)
UTF-8 string class wrapping `std::string`. Key features:
- **`arg()`** — Qt-style `%1`..`%9` placeholder substitution
- **`split()`** — split by character or string separator
- **`number()`** — static int/double-to-string conversion
- **`toInt()`, `toUInt()`, `toULongLong()`** — numeric parsing
- **`toUpper()`, `toLower()`, `simplified()`, `trimmed()`** — text transforms
- **`toLatin1()`, `toUtf8()`** — conversion to `ByteArray`
- **`fromStdWString()`** — Windows wide-string conversion
- **`startsWith()`, `endsWith()`, `contains()`, `indexOf()`** — search

### `qtac::StringList` (replaces `QStringList`)
Wraps `std::vector<String>`. Provides:
- **`join(separator)`** — concatenate with delimiter
- **`filter(pattern)`** — substring filter
- Append, insert, remove, contains, indexOf

### `qtac::Variant` (replaces `QVariant`)
Uses `std::variant` internally. Stores: `bool`, `int`, `unsigned int`, `long long`, `unsigned long long`, `double`, `String`, `ByteArray`.
- **`toString()`, `toInt()`, `toBool()`, `toDouble()`** — type conversions
- **`value<T>()`** — template accessor
- **`isValid()`, `type()`** — type introspection

### `qtac::Map<K, V>` (replaces `QMap`)
Header-only template wrapping `std::map`. Provides:
- **`value(key, defaultValue)`** — safe lookup with fallback
- **`insert()`, `remove()`, `contains()`**
- **`keys()`, `values()`** — extract key/value vectors

### `qtac::List<T>` (replaces `QList`)
Header-only template wrapping `std::vector`. Provides:
- **`append()`, `prepend()`, `insert()`, `removeAt()`**
- **`at()`** — bounds-checked access
- **`contains()`, `indexOf()`**
- **`first()`, `last()`, `takeAt()`, `takeFirst()`, `takeLast()`**

### `qtac::Pair<T1, T2>` (replaces `QPair`)
Type alias for `std::pair<T1, T2>` plus `makePair()` helper.

## Building

```bash
cmake -S source/library -B source/library/build
cmake --build source/library/build
```

Requires C++20. No external dependencies.

## Testing

```bash
cmake -S source/test -B source/test/build
cmake --build source/test/build

# Run tests
source/test/build/Debug/test_bytearray
source/test/build/Debug/test_string
source/test/build/Debug/test_containers
```

## Usage

```cpp
#include <qtac/String.h>
#include <qtac/List.h>
#include <qtac/Map.h>
#include <qtac/Variant.h>

using namespace qtac;

String msg = String("Device %1 returned code %2").arg(name).arg(code);
List<String> items = msg.split(' ');
Map<String, Variant> config;
config.insert(String("timeout"), Variant(5000));
```

## Design Principles

1. **API compatibility** — Method names and signatures match Qt equivalents for easy migration
2. **No Qt headers** — Zero Qt includes; pure C++20
3. **Header-only where possible** — Templates (Map, List, Pair) and ByteArray are header-only
4. **Namespace isolation** — Everything in `namespace qtac` to avoid conflicts during migration
5. **Minimal footprint** — Only implements methods actually used in the codebase
