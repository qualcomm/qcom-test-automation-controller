"""
Build presentation-qt-refactor.pptx from the markdown outline.
Run: python docs/build_pptx.py
Output: docs/presentation-qt-refactor.pptx
"""

from pptx import Presentation
from pptx.util import Inches, Pt, Emu
from pptx.dml.color import RGBColor
from pptx.enum.text import PP_ALIGN
import copy, os

# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------

W = Inches(13.333)   # widescreen 16:9
H = Inches(7.5)

TITLE_FONT   = "Calibri"
BODY_FONT    = "Calibri"
CODE_FONT    = "Courier New"

BLACK  = RGBColor(0x00, 0x00, 0x00)
DARK   = RGBColor(0x1F, 0x1F, 0x1F)
GRAY   = RGBColor(0x44, 0x44, 0x44)
ACCENT = RGBColor(0x00, 0x70, 0xC0)   # blue for headings
CODE_BG = RGBColor(0xF2, 0xF2, 0xF2)
WHITE  = RGBColor(0xFF, 0xFF, 0xFF)


def new_prs():
    prs = Presentation()
    prs.slide_width  = W
    prs.slide_height = H
    return prs


def blank_slide(prs):
    blank_layout = prs.slide_layouts[6]   # completely blank
    return prs.slides.add_slide(blank_layout)


def add_textbox(slide, left, top, width, height, text,
                font_name=BODY_FONT, font_size=18, bold=False,
                color=BLACK, align=PP_ALIGN.LEFT, wrap=True):
    txBox = slide.shapes.add_textbox(left, top, width, height)
    txBox.word_wrap = wrap
    tf = txBox.text_frame
    tf.word_wrap = wrap
    p = tf.paragraphs[0]
    p.alignment = align
    run = p.add_run()
    run.text = text
    run.font.name = font_name
    run.font.size = Pt(font_size)
    run.font.bold = bold
    run.font.color.rgb = color
    return txBox


def add_code_box(slide, left, top, width, height, text, font_size=9):
    """Shaded background box with monospace code."""
    from pptx.util import Pt
    from pptx.oxml.ns import qn
    from lxml import etree

    # Background rectangle
    rect = slide.shapes.add_shape(
        1,  # MSO_SHAPE_TYPE.RECTANGLE
        left, top, width, height
    )
    rect.fill.solid()
    rect.fill.fore_color.rgb = CODE_BG
    rect.line.color.rgb = RGBColor(0xCC, 0xCC, 0xCC)
    rect.line.width = Pt(0.5)

    # Text box on top
    txBox = slide.shapes.add_textbox(
        left + Inches(0.1), top + Inches(0.05),
        width - Inches(0.2), height - Inches(0.1)
    )
    txBox.word_wrap = False
    tf = txBox.text_frame
    tf.word_wrap = False

    lines = text.split("\n")
    for i, line in enumerate(lines):
        if i == 0:
            p = tf.paragraphs[0]
        else:
            p = tf.add_paragraph()
        p.space_before = Pt(0)
        p.space_after  = Pt(0)
        run = p.add_run()
        run.text = line if line else " "
        run.font.name = CODE_FONT
        run.font.size = Pt(font_size)
        run.font.color.rgb = DARK

    return txBox


def add_title(slide, text, subtitle=None):
    """Big centered title, optional subtitle."""
    add_textbox(slide,
                left=Inches(0.6), top=Inches(2.5),
                width=Inches(12.1), height=Inches(1.2),
                text=text,
                font_name=TITLE_FONT, font_size=36, bold=True,
                color=ACCENT, align=PP_ALIGN.CENTER)
    if subtitle:
        add_textbox(slide,
                    left=Inches(0.6), top=Inches(3.9),
                    width=Inches(12.1), height=Inches(0.6),
                    text=subtitle,
                    font_name=TITLE_FONT, font_size=22,
                    color=GRAY, align=PP_ALIGN.CENTER)


def slide_header(slide, title):
    """Slide title bar at the top."""
    add_textbox(slide,
                left=Inches(0.4), top=Inches(0.15),
                width=Inches(12.5), height=Inches(0.6),
                text=title,
                font_name=TITLE_FONT, font_size=24, bold=True,
                color=ACCENT)
    # Thin rule under title
    from pptx.util import Pt as _Pt
    line = slide.shapes.add_shape(1, Inches(0.4), Inches(0.8), Inches(12.5), Inches(0.02))
    line.fill.solid()
    line.fill.fore_color.rgb = ACCENT
    line.line.fill.background()


def add_bullets(slide, left, top, width, height, items, font_size=16, indent=0):
    """Add a list of bullet strings. Prefix '  ' for sub-bullet."""
    txBox = slide.shapes.add_textbox(left, top, width, height)
    txBox.word_wrap = True
    tf = txBox.text_frame
    tf.word_wrap = True

    for i, item in enumerate(items):
        is_sub = item.startswith("  ")
        text   = item.lstrip()
        bullet = "  •  " if is_sub else "•  "

        if i == 0:
            p = tf.paragraphs[0]
        else:
            p = tf.add_paragraph()

        p.space_before = Pt(2)
        run = p.add_run()
        run.text = bullet + text
        run.font.name = BODY_FONT
        run.font.size = Pt(font_size - (2 if is_sub else 0))
        run.font.color.rgb = DARK
        if is_sub:
            run.font.color.rgb = GRAY

    return txBox


def add_table(slide, left, top, width, rows_data, col_widths=None, font_size=13):
    """
    rows_data: list of lists of strings. First row = header.
    col_widths: list of Inches values. If None, divide evenly.
    """
    from pptx.util import Inches, Pt
    from pptx.dml.color import RGBColor

    nrows = len(rows_data)
    ncols = len(rows_data[0])
    row_height = Inches(0.38)

    if col_widths is None:
        cw = width / ncols
        col_widths = [cw] * ncols

    table = slide.shapes.add_table(nrows, ncols, left, top,
                                   width, row_height * nrows).table

    for ci, cw in enumerate(col_widths):
        table.columns[ci].width = cw

    HEADER_BG = RGBColor(0x00, 0x70, 0xC0)
    ROW_BG1   = RGBColor(0xFF, 0xFF, 0xFF)
    ROW_BG2   = RGBColor(0xEE, 0xF4, 0xFB)

    for ri, row in enumerate(rows_data):
        for ci, cell_text in enumerate(row):
            cell = table.cell(ri, ci)
            cell.text = cell_text
            tf = cell.text_frame
            tf.paragraphs[0].font.size = Pt(font_size)
            tf.paragraphs[0].font.name = BODY_FONT
            tf.word_wrap = True

            if ri == 0:
                tf.paragraphs[0].font.bold  = True
                tf.paragraphs[0].font.color.rgb = WHITE
                cell.fill.solid()
                cell.fill.fore_color.rgb = HEADER_BG
            else:
                tf.paragraphs[0].font.color.rgb = DARK
                cell.fill.solid()
                cell.fill.fore_color.rgb = ROW_BG1 if ri % 2 == 1 else ROW_BG2

    return table


# ---------------------------------------------------------------------------
# Slide builders
# ---------------------------------------------------------------------------

def s01_title(prs):
    sl = blank_slide(prs)
    add_title(sl,
              "Removing Qt from the QTAC Core Library",
              "Architecture, Design Decisions, and Demo")
    add_textbox(sl, Inches(0.4), Inches(6.8), Inches(12.5), Inches(0.5),
                "QTAC Refactor  |  2026",
                font_size=12, color=GRAY, align=PP_ALIGN.CENTER)


def s02_what_is_tac(prs):
    sl = blank_slide(prs)
    slide_header(sl, "What is QTAC / TAC?")
    bullets = [
        "TAC = Test Automation Controller",
        "Used by hardware test engineers to control debug boards on DUT (Device Under Test)",
        "Boards supported: FTDI-based ALPACA-LITE, PSoC, PIC32CX",
        "Exposes: pin control, serial communication, scripted quick-commands, device configuration",
        "Deliverables:",
        "  TAC.exe — legacy GUI application",
        "  TACDev.dll — C API used by Python lab automation scripts",
    ]
    add_bullets(sl, Inches(0.5), Inches(1.05), Inches(12.3), Inches(5.8), bullets)


def s03_problem(prs):
    sl = blank_slide(prs)
    slide_header(sl, "The Problem: Qt Entangled in the Core")

    before_diag = """\
┌──────────────────────────────────────────────────────────┐
│               QCommonConsole (Qt-dependent)              │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐      │
│  │ Device      │  │ Protocols   │  │ Serial      │      │
│  │ Logic       │  │ TACLite     │  │ QSerialPort │      │
│  │ AlpacaDevice│  │ PSOC        │  │             │      │
│  └──────┬──────┘  └──────┬──────┘  └──────┬──────┘      │
│         └────────────────┴─────────────────┘             │
│                          │                               │
│               ┌──────────▼─────────┐                    │
│               │    Qt6::Core       │ ← required by ALL  │
│               │    QString         │                    │
│               │    QObject / moc   │                    │
│               └──────────┬─────────┘                    │
└─────────────────────────┬────────────────────────────────┘
                           │  Qt6 required by every consumer
             ┌─────────────┼─────────────┐
             ▼             ▼             ▼
        TAC.exe       TACDev.dll      CLI tools
                  (lab scripts — must install Qt6 ~600 MB)"""

    add_code_box(sl, Inches(0.4), Inches(0.9), Inches(7.6), Inches(5.9),
                 before_diag, font_size=7.5)

    # Right column: bullet points
    bullets = [
        "Device logic, protocols, serial all Qt-dependent",
        "TACDev.dll forced to carry Qt6 runtime (~38 MB)",
        "Every lab machine needed Qt6 installed",
        "Qt LGPL prohibits redistribution — blocker for open-sourcing QTAC",
        "Outside developers couldn't build or contribute without a full Qt6 install",
    ]
    add_textbox(sl, Inches(8.2), Inches(0.9), Inches(4.9), Inches(0.4),
                "Problems:", font_size=15, bold=True, color=ACCENT)
    add_bullets(sl, Inches(8.2), Inches(1.35), Inches(4.9), Inches(5.4),
                bullets, font_size=14)


def s04_goal(prs):
    sl = blank_slide(prs)
    slide_header(sl, "Goal and Scope")

    after_diag = """\
          ┌──────────────────────────────┐
          │    qtac-app.exe  (GUI)       │  Qt6 here only
          │    TACWindow, TACPinFrame    │
          └──────────────┬───────────────┘
                         │
          ┌──────────────▼───────────────┐
          │   qt-adapter  (bridge)       │  Qt6::Core only
          │   TACDeviceBridge            │
          └──────────────┬───────────────┘
                         │  C++20 only below this line
  ┌──────────────────────▼──────────────────────┐
  │          qtac-core  (static lib)             │
  │  AlpacaDevice · TACDriveThread · AlpacaScript│
  │  qtac::String · qtac::Signal<> · SerialPort  │
  │           zero Qt dependency                 │
  └──────────────────┬───────────────────────────┘
                     │
            ┌────────▼────────┐
            │  TACDev.dll     │  no Qt · ~300 KB
            └─────────────────┘"""

    add_code_box(sl, Inches(0.4), Inches(0.9), Inches(6.6), Inches(5.9),
                 after_diag, font_size=8.5)

    rows = [
        ["Layer", "Qt?", "Status"],
        ["source/library  (qtac-core)", "No", "✅ Refactored"],
        ["source/tacdev  (TACDev.dll)", "No", "✅ Refactored — Qt-free"],
        ["source/libraries/qt-adapter", "Bridge only", "✅ New bridge"],
        ["source/app  (qtac-app.exe)", "Yes (GUI)", "✅ New Qt6 GUI"],
    ]
    col_widths = [Inches(3.7), Inches(1.3), Inches(2.1)]
    add_table(sl, Inches(7.1), Inches(1.1), Inches(6.0), rows,
              col_widths=col_widths, font_size=13)


def s05_approaches(prs):
    sl = blank_slide(prs)
    slide_header(sl, "Three Approaches Considered")

    rows = [
        ["Approach", "Call-site changes", "Risk", "Readability"],
        ["A. qtac:: wrappers  ✅ chosen", "Minimal — type names only", "Low", "High — Qt-like API"],
        ["B. Direct std:: replacement", "Extensive — every method call", "Medium", "Medium"],
        ["C. Centralized StringUtils", "Extensive — every call site", "Medium", "Medium"],
    ]
    col_widths = [Inches(3.5), Inches(3.2), Inches(1.3), Inches(2.5)]
    add_table(sl, Inches(0.5), Inches(1.1), Inches(12.3), rows,
              col_widths=col_widths, font_size=14)

    add_textbox(sl, Inches(0.5), Inches(3.6), Inches(12.3), Inches(0.4),
                "Why wrappers won:", font_size=15, bold=True, color=ACCENT)
    bullets = [
        "159 call sites would need rewriting with Approach B (str.contains() → str.find() != npos, etc.)",
        "Wrapper approach: change only type names — logic and method names stay identical",
        "Smaller diffs → easier review, lower bug risk, familiar to engineers who know Qt",
        "~2000 lines of wrapper infrastructure up front, saves thousands of call-site rewrites",
    ]
    add_bullets(sl, Inches(0.5), Inches(4.05), Inches(12.3), Inches(3.0),
                bullets, font_size=15)


def s06_wrappers(prs):
    sl = blank_slide(prs)
    slide_header(sl, "Approach A: qtac:: Wrappers (Chosen)")

    add_textbox(sl, Inches(0.5), Inches(0.95), Inches(3.9), Inches(0.35),
                "Before (Qt):", font_size=13, bold=True, color=GRAY)
    add_code_box(sl, Inches(0.5), Inches(1.3), Inches(3.9), Inches(1.1),
                 "QString processInput(const QString& input) {\n"
                 "    QStringList parts =\n"
                 "        input.trimmed().toLower().split(',');\n"
                 "    return parts.join(\" | \");\n"
                 "}", font_size=9.5)

    add_textbox(sl, Inches(0.5), Inches(2.55), Inches(3.9), Inches(0.35),
                "After (qtac:: wrappers):", font_size=13, bold=True, color=ACCENT)
    add_code_box(sl, Inches(0.5), Inches(2.9), Inches(3.9), Inches(1.1),
                 "qtac::String processInput(\n"
                 "        const qtac::String& input) {\n"
                 "    qtac::StringList parts =\n"
                 "        input.trimmed().toLower().split(',');\n"
                 "    return parts.join(\" | \");\n"
                 "}", font_size=9.5)

    add_textbox(sl, Inches(0.5), Inches(4.15), Inches(3.9), Inches(0.35),
                "With direct std:: (Approach B):", font_size=13, bold=True, color=GRAY)
    add_code_box(sl, Inches(0.5), Inches(4.5), Inches(3.9), Inches(1.55),
                 "std::string processInput(\n"
                 "        const std::string& input) {\n"
                 "    // manual trim (~4 lines)\n"
                 "    // manual toLower (std::transform)\n"
                 "    // manual split (istringstream loop)\n"
                 "    // manual join (loop + accumulate)\n"
                 "    // ~20 lines total\n"
                 "}", font_size=9.5)

    add_textbox(sl, Inches(4.7), Inches(0.95), Inches(8.1), Inches(0.4),
                "Key insight:", font_size=15, bold=True, color=ACCENT)
    bullets = [
        "qtac:: version = 2-word change (type names only)",
        "All method calls (.trimmed, .toLower, .split, .join) unchanged",
        "Logic is identical to the original Qt code",
        "",
        "Approach B rewrites every string operation site:",
        "  str.contains()   →   str.find() != npos",
        "  str.split(',')   →   8 lines of istringstream",
        "  str.startsWith() →   str.substr(0,n) == prefix",
        "  …159 such sites across the codebase",
    ]
    add_bullets(sl, Inches(4.7), Inches(1.4), Inches(8.1), Inches(5.2),
                bullets, font_size=14)


def s07_types(prs):
    sl = blank_slide(prs)
    slide_header(sl, "Key Qt Replacement Types")

    rows = [
        ["Qt Type", "qtac:: Equivalent", "Backed by"],
        ["QString",               "qtac::String",       "std::string"],
        ["QByteArray",            "qtac::ByteArray",     "std::string"],
        ["QStringList",           "qtac::StringList",    "std::vector<qtac::String>"],
        ["QList<T>",              "qtac::List<T>",       "std::vector<T>"],
        ["QMap<K,V>",             "qtac::Map<K,V>",      "std::map<K,V>"],
        ["QVariant",              "qtac::Variant",       "std::variant"],
        ["QSharedPointer<T>",     "std::shared_ptr<T>",  "C++11 stdlib"],
        ["QMutex / QWaitCondition","std::mutex / std::condition_variable","C++11 stdlib"],
        ["QPair<A,B>",            "qtac::Pair<A,B>",     "std::pair<A,B>"],
        ["QSerialPort / QSerialPortInfo","qtac::SerialPort / SerialPortInfo","libserialport"],
    ]
    col_widths = [Inches(3.4), Inches(4.5), Inches(4.0)]
    add_table(sl, Inches(0.4), Inches(0.95), Inches(12.4), rows,
              col_widths=col_widths, font_size=13)

    add_textbox(sl, Inches(0.4), Inches(6.55), Inches(12.4), Inches(0.6),
                "All wrappers expose the same method names as Qt: toLower(), contains(), split(), join(), append(), isEmpty() …",
                font_size=13, color=GRAY)


def s08_signals_problem(prs):
    sl = blank_slide(prs)
    slide_header(sl, "The Biggest Design Challenge: Signals & Slots")

    add_textbox(sl, Inches(0.5), Inches(0.95), Inches(5.8), Inches(0.4),
                "Legacy Qt signals in TACDriveThread:", font_size=14, bold=True, color=GRAY)
    add_code_box(sl, Inches(0.5), Inches(1.35), Inches(5.8), Inches(2.0),
                 "// Q_OBJECT macro — moc generates glue code\n"
                 "signals:\n"
                 "    void pinStateChanged(quint64 pin, bool state);\n"
                 "    void deviceOpen();\n"
                 "    void firmwareVersionUpdate(QString firmware);\n"
                 "    // ... 14 signals total", font_size=10)

    add_textbox(sl, Inches(0.5), Inches(3.5), Inches(5.8), Inches(0.4),
                "Why std::function alone is not enough:", font_size=14, bold=True, color=GRAY)
    add_code_box(sl, Inches(0.5), Inches(3.9), Inches(5.8), Inches(1.35),
                 "// std::function holds ONE callback only:\n"
                 "std::function<void(uint64_t,bool)> onPinChanged;\n\n"
                 "// Second connect silently overwrites the first!\n"
                 "dt->onPinChanged = [](auto p, auto s){ /* GUI */ };\n"
                 "dt->onPinChanged = [](auto p, auto s){ /* log */ };  // ← BUG", font_size=10)

    add_textbox(sl, Inches(6.6), Inches(0.95), Inches(6.3), Inches(0.4),
                "Qt signals do three things at once:", font_size=14, bold=True, color=ACCENT)
    rows = [
        ["Property", "Why it matters"],
        ["Multi-subscriber", "GUI + logger both connect to one signal"],
        ["Thread-safe dispatch", "Device thread fires; Qt delivers on GUI thread"],
        ["Auto-disconnect", "Connection breaks when object is destroyed"],
    ]
    col_widths = [Inches(2.8), Inches(3.3)]
    add_table(sl, Inches(6.6), Inches(1.4), Inches(6.1), rows,
              col_widths=col_widths, font_size=13)

    add_textbox(sl, Inches(6.6), Inches(3.5), Inches(6.3), Inches(0.4),
                "None of these are provided by std::function.", font_size=14,
                bold=True, color=GRAY)
    bullets = [
        "Missing any one of these causes real bugs:",
        "  • Silent subscriber overwrite → GUI stops updating",
        "  • Wrong-thread callback → GUI corruption / crash",
        "  • No auto-disconnect → dangling pointer on object destruction",
    ]
    add_bullets(sl, Inches(6.6), Inches(3.95), Inches(6.3), Inches(2.8),
                bullets, font_size=14)


def s09_signal_solution(prs):
    sl = blank_slide(prs)
    slide_header(sl, "Solution: qtac::Signal<Args...>")

    add_textbox(sl, Inches(0.5), Inches(0.95), Inches(5.8), Inches(0.4),
                "Template design:", font_size=14, bold=True, color=GRAY)
    add_code_box(sl, Inches(0.5), Inches(1.35), Inches(5.8), Inches(2.4),
                 "template<typename... Args>\n"
                 "class Signal {\n"
                 "public:\n"
                 "    // Returns token — subscriber lives while token held\n"
                 "    std::shared_ptr<void> connect(\n"
                 "        std::function<void(Args...)> fn);\n\n"
                 "    // Fire all live subscribers\n"
                 "    void operator()(Args... args);\n"
                 "};", font_size=10)

    add_textbox(sl, Inches(0.5), Inches(3.9), Inches(5.8), Inches(0.4),
                "Usage — identical feel to Qt signals:", font_size=14, bold=True, color=GRAY)
    add_code_box(sl, Inches(0.5), Inches(4.3), Inches(5.8), Inches(2.55),
                 "// Declared in TACDriveThread:\n"
                 "qtac::Signal<uint64_t, bool>      onPinStateChanged;\n"
                 "qtac::Signal<const qtac::String&> onFirmwareVersionUpdate;\n\n"
                 "// Subscriber (auto-disconnects when token drops):\n"
                 "_token = thread->onPinStateChanged.connect(\n"
                 "    [this](uint64_t pin, bool state) {\n"
                 "        // handle pin change\n"
                 "    });", font_size=10)

    add_textbox(sl, Inches(6.6), Inches(0.95), Inches(6.3), Inches(0.4),
                "How it works:", font_size=14, bold=True, color=ACCENT)
    bullets = [
        "Stores a list of weak_ptr-wrapped callbacks",
        "connect() wraps the lambda in a shared_ptr — the returned token keeps it alive",
        "Token out of scope → weak_ptr expires → auto-disconnect on next fire",
        "operator() iterates list, promotes weak_ptrs, fires live ones, cleans up dead ones",
        "Internal mutex makes connect/fire thread-safe",
        "",
        "Thread marshaling to GUI thread is handled separately by TACDeviceBridge via QMetaObject::invokeMethod(Qt::QueuedConnection)",
        "",
        "Separation of concerns:",
        "  Core library: what happened",
        "  Bridge: where to deliver it",
    ]
    add_bullets(sl, Inches(6.6), Inches(1.4), Inches(6.3), Inches(5.8),
                bullets, font_size=13)


def s10_bridge(prs):
    sl = blank_slide(prs)
    slide_header(sl, "The Qt Bridge: Clean Separation")

    diag = """\
┌──────────────────────────────────────────────┐
│          source/app  (Qt6 GUI)               │  ← uses Qt freely
│  TACWindow · TACPinFrame · TACDeviceSelection│
└────────────────────┬─────────────────────────┘
                     │  Qt signals → GUI widgets
┌────────────────────▼─────────────────────────┐
│   source/libraries/qt-adapter                │  ← thin bridge
│   TACDeviceBridge  (Q_OBJECT)                │
│   • Wraps qtac::TACDriveThread               │
│   • Receives qtac::Signal<> callbacks        │
│   • Re-emits as Qt signals via               │
│     QMetaObject::invokeMethod                │
│     (Qt::QueuedConnection — thread-safe)     │
└────────────────────┬─────────────────────────┘
                     │  C++20 only below this line
┌────────────────────▼─────────────────────────┐
│   source/library  (qtac-core)                │  ← zero Qt
│   AlpacaDevice · TACDriveThread · ...        │
└──────────────────────────────────────────────┘"""

    add_code_box(sl, Inches(0.4), Inches(0.9), Inches(6.5), Inches(6.1),
                 diag, font_size=9)

    add_textbox(sl, Inches(7.1), Inches(0.95), Inches(5.8), Inches(0.4),
                "Why the bridge pattern?", font_size=15, bold=True, color=ACCENT)
    bullets = [
        "TACDeviceBridge is the ONLY class allowed to know about both worlds",
        "Device library (below) has zero knowledge of Qt",
        "GUI code (above) has zero knowledge of threading",
        "Bridge handles cross-thread delivery safely",
        "",
        "Without the bridge:",
        "  • qtac::Signal<> fires on device thread",
        "  • GUI widget update from wrong thread → crash",
        "",
        "With the bridge:",
        "  • Signal fires on device thread",
        "  • invokeMethod queues event onto GUI thread",
        "  • Qt delivers it safely to the widget",
    ]
    add_bullets(sl, Inches(7.1), Inches(1.45), Inches(5.8), Inches(5.7),
                bullets, font_size=13)


def s11_tacdev(prs):
    sl = blank_slide(prs)
    slide_header(sl, "TACDev.dll: Refactored to Qt-Free")

    add_textbox(sl, Inches(0.5), Inches(0.95), Inches(6.0), Inches(0.4),
                "C API (unchanged from caller's perspective):", font_size=14, bold=True, color=GRAY)
    add_code_box(sl, Inches(0.5), Inches(1.35), Inches(6.0), Inches(3.05),
                 "// TACDev.h — pure C, no Qt, no C++ exceptions\n"
                 "TACDEV_API TACDevHandle\n"
                 "    TACDev_Open(const char* portName);\n"
                 "TACDEV_API void\n"
                 "    TACDev_Close(TACDevHandle h);\n"
                 "TACDEV_API int\n"
                 "    TACDev_SendCommand(TACDevHandle h,\n"
                 "                      const char* cmd, bool on);\n"
                 "TACDEV_API int\n"
                 "    TACDev_SetPinState(TACDevHandle h,\n"
                 "                      uint64_t pin, bool state);\n"
                 "TACDEV_API int\n"
                 "    TACDev_QuickCommand(TACDevHandle h,\n"
                 "                       const char* name);",
                 font_size=9.5)

    rows = [
        ["", "Before", "After"],
        ["Qt runtime", "Qt6Core.dll + Qt6SerialPort.dll + ICU  (~38 MB)", "None"],
        ["DLL footprint", "~2 MB + 38 MB Qt DLLs", "~300 KB"],
        ["Lab machine setup", "Install Qt6 runtime", "Drop DLL next to script"],
        ["Usable from Python/C#/LabVIEW", "Yes (with Qt installed)", "Yes (no Qt needed)"],
    ]
    col_widths = [Inches(2.5), Inches(3.8), Inches(2.6)]
    add_table(sl, Inches(0.5), Inches(4.55), Inches(8.9), rows,
              col_widths=col_widths, font_size=12)

    bullets = [
        "Lab scripts unchanged — same API",
        "No Qt6 install on lab machines",
        "Any language via C API: Python, C#, LabVIEW",
        "Built entirely from qtac-core",
    ]
    add_textbox(sl, Inches(9.6), Inches(1.1), Inches(3.5), Inches(0.4),
                "Benefits:", font_size=14, bold=True, color=ACCENT)
    add_bullets(sl, Inches(9.6), Inches(1.55), Inches(3.5), Inches(3.0),
                bullets, font_size=14)


def s12_waves(prs):
    sl = blank_slide(prs)
    slide_header(sl, "Implementation: Waves / Phases")

    rows = [
        ["Wave", "Work"],
        ["W1",     "Platform configs, PSoC/PIC32CX device classes, devicelist.json loader"],
        ["W2",     "qtac::SerialPort/SerialPortInfo (libserialport); Qt-free drive thread hierarchy"],
        ["W3",     "qtac::Signal<> — multi-subscriber, lifetime-safe; migrate all callbacks"],
        ["W4",     "qt-adapter library — string/container converters + TACDeviceBridge"],
        ["W5–6",   "Wire FTDIDevice::open() to TACDriveThread; fix FTDI hardware bugs; VEH crash logger"],
        ["W7",     "Device Info tab, Terminal tab in new GUI"],
        ["W8",     "qtac::AlpacaScript engine; Quick Settings buttons + Variables UI"],
        ["W9",     "Preferences dialog, auto-shutdown, open-last-device"],
        ["W10",    "TACDev.dll — Qt-free C API"],
        ["W11",    "PSoC GUI path; PSoC command hash fix; decouple source/ from src/"],
        ["Fixes",  "PSoC platform ID fallback; getPins() override; TC_READY_N init; battery state; serial number"],
    ]
    col_widths = [Inches(1.1), Inches(11.1)]
    add_table(sl, Inches(0.4), Inches(0.95), Inches(12.4), rows,
              col_widths=col_widths, font_size=12)


def s13_results(prs):
    sl = blank_slide(prs)
    slide_header(sl, "Results: Before vs. After")

    rows = [
        ["Metric", "Before (legacy)", "After (qtac-core + TACDev.dll)"],
        ["Qt dependency in device core",   "Yes (Qt6::Core, Qt6::SerialPort)", "None"],
        ["TACDev.dll runtime deps",        "Qt6Core.dll (~38 MB)",             "ftd2xx.dll only"],
        ["TACDev.dll size",                "~2 MB + ~38 MB Qt DLLs",          "~300 KB"],
        ["Lab machine Qt install",         "Required",                         "Not required"],
        ["QObject / moc in core",          "Pervasive",                        "Zero"],
        ["Multi-subscriber callbacks",     "Via Qt signals",                   "Via qtac::Signal<>"],
        ["Thread-safe GUI delivery",       "Qt auto-marshaling",               "Bridge + QueuedConnection"],
        ["Test coverage",                  "Manual only",                      "Unit tests + hardware integration tests"],
        ["PSoC / PIC32CX support",         "Legacy Qt path only",              "New Qt-free path"],
    ]
    col_widths = [Inches(3.5), Inches(3.8), Inches(4.9)]
    add_table(sl, Inches(0.4), Inches(0.95), Inches(12.4), rows,
              col_widths=col_widths, font_size=13)


def s14_demo(prs):
    sl = blank_slide(prs)
    slide_header(sl, "Demo: New GUI vs. Legacy TAC.exe")

    add_textbox(sl, Inches(0.5), Inches(0.95), Inches(5.5), Inches(0.4),
                "Legacy TAC.exe", font_size=16, bold=True, color=GRAY)
    steps_legacy = [
        "Launch TAC.exe",
        "Connect to ALPACA-LITE MTP8975 (platform 13)",
        "Show device info: hardware type, firmware, serial, platform ID",
        "Show pin tabs: General, FTDI layout, Terminal",
        "Show Quick Settings buttons and Variables",
        "Toggle a pin — show LED / physical response",
    ]
    add_bullets(sl, Inches(0.5), Inches(1.35), Inches(5.5), Inches(4.5),
                steps_legacy, font_size=14)

    add_textbox(sl, Inches(6.8), Inches(0.95), Inches(6.1), Inches(0.4),
                "New qtac-app.exe", font_size=16, bold=True, color=ACCENT)
    steps_new = [
        "Launch qtac-app.exe",
        "Connect to same device (device selection dialog)",
        "Show Device Info tab — same fields",
        "Show pin tabs — General, tcnf-defined tabs, Terminal",
        "Show Quick Settings buttons, Variables — same functionality",
        "Toggle same pin — verify parity",
        "Show Preferences → auto-shutdown, open last device",
        "Disconnect and reconnect — open last device on startup",
    ]
    add_bullets(sl, Inches(6.8), Inches(1.35), Inches(6.1), Inches(4.5),
                steps_new, font_size=14)

    add_textbox(sl, Inches(0.5), Inches(6.2), Inches(12.3), Inches(0.5),
                "TACDev.dll Python demo available in Appendix G if time allows.",
                font_size=13, color=GRAY)


def s15_gaps(prs):
    sl = blank_slide(prs)
    slide_header(sl, "Known Gaps / Future Work")

    bullets = [
        "Some legacy GUI features not yet ported (help system, some device-type-specific dialogs)",
        "Linux support: SerialPortInfo_posix.cpp is stubbed",
        "  libserialport is cross-platform — Win32 path fully tested, Linux build not yet validated on hardware",
        "AlpacaScript validateScript() not yet exposed in new API",
        "Full automated test suite against all hardware variants still in progress",
    ]
    add_bullets(sl, Inches(0.5), Inches(1.1), Inches(12.3), Inches(5.5),
                bullets, font_size=18)


def s16_summary(prs):
    sl = blank_slide(prs)
    slide_header(sl, "Summary")

    add_textbox(sl, Inches(0.5), Inches(1.0), Inches(12.3), Inches(0.65),
                "We removed Qt from the QTAC device core without breaking any existing functionality.",
                font_name=TITLE_FONT, font_size=20, bold=True, color=ACCENT)

    rows = [
        ["Decision", "What it gave us"],
        ["qtac:: wrappers",       "Minimal call-site changes, Qt-like API, low migration risk"],
        ["qtac::Signal<>",        "Multi-subscriber, lifetime-safe callbacks — no Qt meta-object system"],
        ["Bridge pattern",        "Clean separation: device core knows nothing about Qt or threads"],
        ["Preserve legacy code",  "Zero risk to production — new code coexists in same repo"],
        ["Qt-free TACDev.dll",    "No Qt runtime on lab machines, 100× smaller footprint, open-source ready"],
    ]
    col_widths = [Inches(3.0), Inches(9.3)]
    add_table(sl, Inches(0.5), Inches(1.85), Inches(12.3), rows,
              col_widths=col_widths, font_size=14)

    add_textbox(sl, Inches(0.5), Inches(6.3), Inches(12.3), Inches(0.6),
                "The foundation is in place to migrate off the legacy stack incrementally, at the team's pace.",
                font_size=14, color=GRAY, align=PP_ALIGN.CENTER)


# ---------------------------------------------------------------------------
# Appendix slides
# ---------------------------------------------------------------------------

def sA_file_map(prs):
    sl = blank_slide(prs)
    slide_header(sl, "Appendix A — File Map")

    tree = """\
source/
├── library/              ← qtac-core  (Qt-free static library)
│   ├── include/qtac/     ← Public headers  (qtac:: namespace)
│   │     String.h · ByteArray.h · Signal.h · SerialPort.h
│   │     AlpacaScript.h · Variant.h · Map.h · List.h · Pair.h
│   └── src/              ← Implementations
│         AlpacaDevice.cpp · FTDIDevice.cpp · PSOCDevice.cpp
│         TACDriveThread.cpp · AlpacaScript.cpp · SerialPort_win32.cpp
│
├── libraries/
│   └── qt-adapter/       ← Bridge: qtac-core ↔ Qt
│         include/TACDeviceBridge.h
│         include/qt_string_convert.h
│         include/qt_container_convert.h
│         src/TACDeviceBridge.cpp
│
├── app/                  ← qtac-app.exe  (Qt6 GUI, uses only qtac-core + adapter)
│     TACWindow · TACPinFrame · TACDeviceSelection · TACApplication
│
└── tacdev/               ← TACDev.dll  (C API, Qt-free)
      TACDev.h · TACDev.cpp

third-party/              ← vendored dependencies
├── nlohmann/json/        ← header-only JSON (MIT)
└── libserialport/        ← cross-platform serial (LGPL v3)

docs/                     ← planning documents, this presentation"""

    add_code_box(sl, Inches(0.4), Inches(0.9), Inches(12.5), Inches(6.1),
                 tree, font_size=9)


def sB_signal_design(prs):
    sl = blank_slide(prs)
    slide_header(sl, "Appendix B — qtac::Signal<> Design")

    design = """\
Signal<Args...>
├── connect(fn) → shared_ptr<token>
│     • wraps fn in a weak_ptr entry in the subscriber list
│     • subscriber stays alive while caller holds the returned token
│     • token destroyed → weak_ptr expires → auto-disconnect on next fire
│
└── operator()(args...)
      • acquires internal mutex
      • iterates subscriber list
      • promotes each weak_ptr; skips expired ones
      • calls all live subscribers (mutex released during call to avoid deadlock)
      • cleans up dead entries in-place after iteration"""

    add_code_box(sl, Inches(0.4), Inches(0.9), Inches(7.8), Inches(4.0),
                 design, font_size=9.5)

    add_textbox(sl, Inches(0.4), Inches(5.05), Inches(7.8), Inches(0.35),
                "Thread safety:", font_size=13, bold=True, color=ACCENT)
    add_textbox(sl, Inches(0.4), Inches(5.4), Inches(7.8), Inches(0.8),
                "connect/disconnect and fire can happen concurrently via internal mutex.\n"
                "GUI thread marshaling is handled separately by TACDeviceBridge via QMetaObject::invokeMethod(Qt::QueuedConnection).",
                font_size=12, color=GRAY)

    add_textbox(sl, Inches(8.4), Inches(0.9), Inches(4.7), Inches(0.35),
                "Lifetime example:", font_size=13, bold=True, color=ACCENT)
    add_code_box(sl, Inches(8.4), Inches(1.3), Inches(4.7), Inches(3.3),
                 "class MyWidget {\n"
                 "    // Token held as member — lives as long as widget\n"
                 "    std::shared_ptr<void> _tok;\n\n"
                 "    void connectSignals(TACDriveThread* dt) {\n"
                 "        _tok = dt->onPinStateChanged.connect(\n"
                 "            [this](uint64_t pin, bool s) {\n"
                 "                updateUI(pin, s);\n"
                 "            });\n"
                 "    }\n"
                 "    // ~MyWidget() destroys _tok\n"
                 "    // → weak_ptr expires\n"
                 "    // → next Signal fire skips it\n"
                 "};", font_size=9.5)

    add_textbox(sl, Inches(8.4), Inches(4.75), Inches(4.7), Inches(0.35),
                "Properties at a glance:", font_size=13, bold=True, color=ACCENT)
    rows = [
        ["Property", "Provided?"],
        ["Multi-subscriber",    "Yes"],
        ["Auto-disconnect",     "Yes (token RAII)"],
        ["Thread-safe fire",    "Yes (internal mutex)"],
        ["GUI thread marshal",  "Via bridge"],
        ["Qt dependency",       "None"],
    ]
    col_widths = [Inches(2.9), Inches(1.65)]
    add_table(sl, Inches(8.4), Inches(5.15), Inches(4.55), rows,
              col_widths=col_widths, font_size=12)


def sC_commits(prs):
    sl = blank_slide(prs)
    slide_header(sl, "Appendix C — Key Commits Reference")

    rows = [
        ["Commit", "Description"],
        ["W2: 88def5c", "qtac::SerialPort + qtac::SerialPortInfo (libserialport)"],
        ["W2: b4ebf76", "Qt-free drive thread hierarchy (TACLite, PSoC, PIC32CX)"],
        ["W2: 490edaf", "Migrate FramePackage + protocol layers to qtac::ByteArray"],
        ["d70370e",     "qtac::Signal<> + migrate all callbacks"],
        ["c560bb6",     "qt-adapter library (TACDeviceBridge + converters)"],
        ["f99befd",     "PSoC/PIC32CX platform configs + device classes"],
        ["ee313d9",     "devicelist.json loader + .tcnf config loader"],
        ["987b98e",     "W6: Fix FTDI connect for ALPACA-LITE MTP8975"],
        ["13a95d3",     "W8: qtac::AlpacaScript + Quick Settings UI"],
        ["501ba84",     "W9: Preferences dialog + auto-shutdown"],
        ["e1d0939",     "W10: Qt-free TACDev.dll"],
        ["a5013c8",     "W11: Decouple source/ from src/"],
        ["84880df",     "PSoC platform ID fallback (firmware_chip)"],
        ["05dff91",     "Fix composite USB serial number (CM_Get_Parent)"],
        ["7b36a3e",     "Fix TC_READY_N disabled-pin initialization"],
    ]
    col_widths = [Inches(2.0), Inches(10.3)]
    add_table(sl, Inches(0.4), Inches(0.9), Inches(12.5), rows,
              col_widths=col_widths, font_size=12)


def sD_alpacascript(prs):
    sl = blank_slide(prs)
    slide_header(sl, "Appendix D — AlpacaScript: Qt-Free Script Engine")

    add_textbox(sl, Inches(0.5), Inches(0.9), Inches(12.3), Inches(0.35),
                "AlpacaScript is the mini-language embedded in .tcnf device-config files that drives the Quick Settings buttons.",
                font_size=13, color=GRAY)

    add_textbox(sl, Inches(0.5), Inches(1.35), Inches(5.8), Inches(0.35),
                "Legacy (Qt-dependent):", font_size=13, bold=True, color=GRAY)
    add_code_box(sl, Inches(0.5), Inches(1.7), Inches(5.8), Inches(1.55),
                 "// qcommon-console\n"
                 "typedef QSharedPointer<_CommandEntry> CommandEntry;\n"
                 "typedef QList<CommandEntry>           CommandEntries;\n"
                 "QMap<QString, ScriptCommand>          _scriptCommands;\n"
                 "QList<ButtonEntry>                    _buttons;\n"
                 "QMap<QString, VariableEntry>          _variables;",
                 font_size=9.5)

    add_textbox(sl, Inches(0.5), Inches(3.35), Inches(5.8), Inches(0.35),
                "Qt-free (source/library):", font_size=13, bold=True, color=ACCENT)
    add_code_box(sl, Inches(0.5), Inches(3.7), Inches(5.8), Inches(1.55),
                 "// AlpacaScript.h\n"
                 "using CommandEntry   = std::shared_ptr<_CommandEntry>;\n"
                 "using CommandEntries = std::vector<CommandEntry>;\n"
                 "qtac::Map<qtac::String, ScriptCommand> _scriptCommands;\n"
                 "qtac::List<ButtonEntry>                _buttons;\n"
                 "qtac::Map<qtac::String, VariableEntry> _variables;",
                 font_size=9.5)

    add_textbox(sl, Inches(6.6), Inches(0.9), Inches(6.3), Inches(0.35),
                "What AlpacaScript does:", font_size=13, bold=True, color=ACCENT)
    bullets = [
        "Parses the \"script\" section of .tcnf device-config files",
        "Resolves named sub-functions (up to 7 levels of nesting)",
        "Substitutes $VariableName tokens at execution time",
        "Powers Quick Settings buttons and user-configurable variables",
        "",
        "New additions vs legacy:",
        "  Explicit ButtonEntry and VariableEntry structs",
        "  GUI enumerates buttons/variables without knowing script internals",
        "  Identical parsing logic and execution semantics",
        "  No Qt types — pure C++20 + qtac:: containers",
    ]
    add_bullets(sl, Inches(6.6), Inches(1.3), Inches(6.3), Inches(5.8),
                bullets, font_size=13)

    add_textbox(sl, Inches(0.5), Inches(5.4), Inches(5.8), Inches(0.35),
                "Execution flow:", font_size=13, bold=True, color=GRAY)
    add_code_box(sl, Inches(0.5), Inches(5.75), Inches(5.8), Inches(1.3),
                 "parseScript(json)           // load .tcnf \"script\" block\n"
                 "  → resolves sub-function calls recursively\n"
                 "  → builds CommandEntries per button\n\n"
                 "replaceTokens(cmd, vars)    // at button-press time\n"
                 "  → substitutes $VarName with current variable values",
                 font_size=9.5)


def sE_platforms(prs):
    sl = blank_slide(prs)
    slide_header(sl, "Appendix E — Platform Support: PSoC & PIC32CX")

    add_textbox(sl, Inches(0.5), Inches(0.9), Inches(5.8), Inches(0.35),
                "Platform identification chain (PSoC):", font_size=13, bold=True, color=ACCENT)
    chain = """\
PSOCDevice::open()
  │
  ├─ 1. "Get Platform ID" command (hardware response)
  │       ↓ if unsupported / fails
  ├─ 2. USB bcdDevice revision
  │       PlatformContainer::fromRevision(usbRevision, ePSOC)
  │       ↓ if not found
  └─ 3. Firmware chip field (2nd component of "major.chip.minor.rev")
          PlatformContainer::fromFirmwareChip(chip, ePSOC)"""

    add_code_box(sl, Inches(0.5), Inches(1.3), Inches(5.8), Inches(2.7),
                 chain, font_size=9.5)

    add_textbox(sl, Inches(0.5), Inches(4.15), Inches(5.8), Inches(0.35),
                "Composite USB serial number fix:", font_size=13, bold=True, color=ACCENT)
    add_code_box(sl, Inches(0.5), Inches(4.55), Inches(5.8), Inches(2.5),
                 "// PSoC COM ports: Windows auto-generates instance ID\n"
                 "// containing '&' — not the real device serial.\n"
                 "// Real serial is on the parent USB device.\n\n"
                 "// SerialPortInfo_win32.cpp fix:\n"
                 "if (instanceId.contains('&')) {\n"
                 "    DEVINST parent;\n"
                 "    CM_Get_Parent(&parent, devInst, 0);\n"
                 "    CM_Get_Device_IDA(parent, buf, size, 0);\n"
                 "    // extract serial from parent device ID\n"
                 "}",
                 font_size=9.5)

    add_textbox(sl, Inches(6.6), Inches(0.9), Inches(6.3), Inches(0.35),
                "Drive thread classes:", font_size=13, bold=True, color=ACCENT)
    rows = [
        ["Class", "Transport", "Platform"],
        ["TACLiteDriveThread",   "FTDI D2XX",    "ALPACA-LITE"],
        ["TACPSOCDriveThread",   "Serial (COM)", "PSoC"],
        ["TACPIC32CXDriveThread","Serial (COM)", "PIC32CX"],
    ]
    col_widths = [Inches(3.4), Inches(1.6), Inches(1.1)]
    add_table(sl, Inches(6.6), Inches(1.3), Inches(6.1), rows,
              col_widths=col_widths, font_size=13)

    add_textbox(sl, Inches(6.6), Inches(3.0), Inches(6.3), Inches(0.35),
                "Key PSoC fixes:", font_size=13, bold=True, color=ACCENT)
    bullets = [
        "getPins() override missing — base returned {} — TACPinFrame showed no pins; fixed by delegating to platform config",
        "TC_READY_N init — disabled pins with initial_value:true were never written HIGH at open; fix: iterate getAllPins() not getActivePins()",
        "Battery state race — waitForCompletion() called without setWaitForCompletion(); CV returned instantly; fix: set flag before creating TACPSOCCommand",
        "devicelist.json platform 66 — added firmware_chip:4 for Hawi MTP8975 PSoC fallback lookup",
    ]
    add_bullets(sl, Inches(6.6), Inches(3.4), Inches(6.3), Inches(3.7),
                bullets, font_size=12)


def sF_build(prs):
    sl = blank_slide(prs)
    slide_header(sl, "Appendix F — Build Architecture")

    graph = """\
CMake dependency graph:

  qtac-core  (static lib)
  ├── depends on:
  │     libserialport  (vendored, LGPL v3)
  │     nlohmann/json  (vendored, MIT, header-only)
  │     ftd2xx.lib     (FTDI SDK, from __Builds/x64/)
  │     C++20 standard library
  │
  ├── TACDev.dll  (shared lib — C API)
  │     no Qt dependency
  │     runtime: ftd2xx.dll  (FTDI devices)
  │              serialport.dll  (serial devices)
  │
  └── qt-adapter  (static lib)
        depends on: qtac-core + Qt6::Core  only
        │
        └── qtac-app.exe
              depends on: qt-adapter + Qt6::Core + Qt6::Widgets + Qt6::SerialPort
              runtime:    windeployqt output (Qt6 DLLs + plugins)"""

    add_code_box(sl, Inches(0.4), Inches(0.9), Inches(7.0), Inches(5.9),
                 graph, font_size=9)

    add_textbox(sl, Inches(7.6), Inches(0.9), Inches(5.5), Inches(0.35),
                "Build commands:", font_size=13, bold=True, color=ACCENT)
    add_code_box(sl, Inches(7.6), Inches(1.3), Inches(5.5), Inches(1.35),
                 "# Configure + build qtac-app:\n"
                 ".\\build_app.ps1\n\n"
                 "# Deploy (windeployqt + copy DLLs):\n"
                 ".\\deploy_app.ps1",
                 font_size=10)

    add_textbox(sl, Inches(7.6), Inches(2.8), Inches(5.5), Inches(0.35),
                "Key CMake targets:", font_size=13, bold=True, color=ACCENT)
    rows = [
        ["Target", "Type", "Qt?"],
        ["qtac-core",   "static lib",  "No"],
        ["TACDev",      "shared lib",  "No"],
        ["qt-adapter",  "static lib",  "Core only"],
        ["qtac-app",    "executable",  "Full Qt6"],
    ]
    col_widths = [Inches(1.9), Inches(1.5), Inches(1.9)]
    add_table(sl, Inches(7.6), Inches(3.2), Inches(5.4), rows,
              col_widths=col_widths, font_size=13)

    add_textbox(sl, Inches(7.6), Inches(5.15), Inches(5.5), Inches(0.35),
                "Generator / compiler:", font_size=13, bold=True, color=ACCENT)
    bullets = [
        "CMake 3.16+, C++20",
        "MSVC 2022 BuildTools + Ninja",
        "Output: build\\Release\\qtac-app.exe",
    ]
    add_bullets(sl, Inches(7.6), Inches(5.55), Inches(5.5), Inches(1.5),
                bullets, font_size=13)


def sG1_licensing_overview(prs):
    sl = blank_slide(prs)
    slide_header(sl, "Appendix G — Licensing Overview")

    rows = [
        ["Dependency", "License", "How used", "Key obligation"],
        ["nlohmann/json",
         "MIT",
         "Header-only, compiled into qtac-core",
         "Include copyright notice in distributed binaries"],
        ["libserialport",
         "LGPL v3",
         "Static lib compiled into qtac-core / TACDev.dll",
         "Provide relinkable object files OR switch to dynamic linking"],
        ["Qt6",
         "LGPL v3",
         "Dynamic-linked — GUI app only; not in qtac-core or TACDev.dll",
         "Include Qt copyright; allow user to relink against modified Qt"],
        ["FTDI D2XX (ftd2xx.dll)",
         "FTDI proprietary",
         "Dynamic-linked at runtime",
         "Redistribute unmodified; cannot modify"],
    ]
    col_widths = [Inches(2.2), Inches(1.5), Inches(4.2), Inches(4.4)]
    add_table(sl, Inches(0.4), Inches(0.95), Inches(12.5), rows,
              col_widths=col_widths, font_size=12)

    add_textbox(sl, Inches(0.4), Inches(5.45), Inches(12.5), Inches(0.35),
                "Action items:", font_size=13, bold=True, color=ACCENT)
    rows2 = [
        ["Item", "Priority", "Action"],
        ["Add NOTICES / THIRD_PARTY_LICENSES file",
         "High",
         "Include MIT + LGPL v3 notices for nlohmann/json, libserialport, Qt"],
        ["Evaluate libserialport link mode",
         "Medium",
         "Switch to dynamic linking (serialport.dll) to simplify LGPL compliance"],
        ["Verify Qt DLL distribution",
         "Low",
         "Already handled by windeployqt in deploy_app.ps1 — confirm LGPL notice bundled"],
    ]
    col_widths2 = [Inches(4.0), Inches(1.3), Inches(7.0)]
    add_table(sl, Inches(0.4), Inches(5.85), Inches(12.5), rows2,
              col_widths=col_widths2, font_size=12)


def sG2_licensing_detail(prs):
    sl = blank_slide(prs)
    slide_header(sl, "Appendix G — Licensing Detail")

    add_textbox(sl, Inches(0.5), Inches(0.9), Inches(5.8), Inches(0.35),
                "nlohmann/json — MIT", font_size=14, bold=True, color=ACCENT)
    bullets_json = [
        "Most permissive common open-source license",
        "No copyleft, no source disclosure, no linking restrictions",
        "Only obligation: include copyright notice in binary distribution",
        "Compatible with closed-source and open-source projects",
    ]
    add_bullets(sl, Inches(0.5), Inches(1.3), Inches(5.8), Inches(1.6),
                bullets_json, font_size=13)
    add_code_box(sl, Inches(0.5), Inches(2.95), Inches(5.8), Inches(0.95),
                 "nlohmann/json  (https://github.com/nlohmann/json)\n"
                 "Copyright (c) 2013-2023 Niels Lohmann\n"
                 "Licensed under the MIT License.",
                 font_size=9.5)

    add_textbox(sl, Inches(0.5), Inches(4.05), Inches(5.8), Inches(0.35),
                "FTDI D2XX — Proprietary (freely redistributable)", font_size=14, bold=True, color=ACCENT)
    bullets_ftdi = [
        "Redistribution permitted for products using FTDI devices",
        "Modification not permitted — closed-source binary",
        "Must be distributed unmodified; cannot be statically linked",
        "No additional notice required beyond standard acknowledgment",
    ]
    add_bullets(sl, Inches(0.5), Inches(4.45), Inches(5.8), Inches(2.4),
                bullets_ftdi, font_size=13)

    add_textbox(sl, Inches(6.6), Inches(0.9), Inches(6.3), Inches(0.35),
                "libserialport — LGPL v3 (static link)", font_size=14, bold=True, color=ACCENT)
    bullets_lsp = [
        "Currently statically linked into qtac-core → TACDev.dll",
        "LGPL v3 §4: static link is allowed, but distributor must do ONE of:",
        "  Option A: provide relinkable .obj files for qtac-core / TACDev.dll",
        "  Option B: switch to shared/dynamic linking (serialport.dll)",
        "Option B is simpler — user can replace serialport.dll with modified version",
        "Recommended: build libserialport as shared lib and ship serialport.dll",
    ]
    add_bullets(sl, Inches(6.6), Inches(1.3), Inches(6.3), Inches(2.6),
                bullets_lsp, font_size=13)

    add_textbox(sl, Inches(6.6), Inches(4.05), Inches(6.3), Inches(0.35),
                "Qt6 — LGPL v3 (dynamic, GUI layer only)", font_size=14, bold=True, color=ACCENT)
    bullets_qt = [
        "Qt used only in qtac-app.exe and qt-adapter — dynamically linked",
        "NOT present in qtac-core or TACDev.dll (primary goal of the refactor)",
        "Dynamic linking satisfies LGPL v3 trivially — DLLs are separate files",
        "Actions for distribution:",
        "  Distribute Qt DLLs unmodified (windeployqt handles this)",
        "  Include Qt copyright + LGPL v3 notice in NOTICES file",
    ]
    add_bullets(sl, Inches(6.6), Inches(4.45), Inches(6.3), Inches(2.6),
                bullets_qt, font_size=13)


def _commit_dot(slide, cx, cy, color, r=Inches(0.13)):
    """Draw a filled circle (commit node)."""
    shape = slide.shapes.add_shape(9, cx - r, cy - r, r * 2, r * 2)  # 9 = OVAL
    shape.fill.solid()
    shape.fill.fore_color.rgb = color
    shape.line.fill.background()
    return shape


def _hline(slide, x1, x2, cy, color, width_pt=2.0):
    """Horizontal connector (branch line)."""
    from pptx.util import Pt
    line = slide.shapes.add_shape(1, x1, cy - Inches(0.01), x2 - x1, Inches(0.02))
    line.fill.solid()
    line.fill.fore_color.rgb = color
    line.line.fill.background()
    return line


def _vline(slide, cx, y1, y2, color, width_pt=2.0):
    """Vertical connector."""
    line = slide.shapes.add_shape(1, cx - Inches(0.01), y1, Inches(0.02), y2 - y1)
    line.fill.solid()
    line.fill.fore_color.rgb = color
    line.line.fill.background()
    return line


def _diag_line(slide, x1, y1, x2, y2, color):
    """Diagonal line via connector shape using lxml."""
    from pptx.util import Emu
    from pptx.oxml.ns import qn
    from lxml import etree

    sp_tree = slide.shapes._spTree
    cx = abs(x2 - x1)
    cy_h = abs(y2 - y1)
    left = min(x1, x2)
    top  = min(y1, y2)

    # Build a <p:cxnSp> straight connector
    cxnSp_xml = (
        f'<p:cxnSp xmlns:p="http://schemas.openxmlformats.org/presentationml/2006/main"'
        f' xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main"'
        f' xmlns:r="http://schemas.openxmlformats.org/officeDocument/2006/relationships">'
        f'<p:nvCxnSpPr>'
        f'  <p:cNvPr id="99" name="Line"/>'
        f'  <p:cNvCxnSpPr/>'
        f'  <p:nvPr/>'
        f'</p:nvCxnSpPr>'
        f'<p:spPr>'
        f'  <a:xfrm>'
        f'    <a:off x="{left}" y="{top}"/>'
        f'    <a:ext cx="{cx}" cy="{cy_h}"/>'
        f'  </a:xfrm>'
        f'  <a:prstGeom prst="line"><a:avLst/></a:prstGeom>'
        f'  <a:ln w="20000">'
        f'    <a:solidFill>'
        f'      <a:srgbClr val="{color.rgb if hasattr(color,"rgb") else "%02X%02X%02X" % (color[0],color[1],color[2])}"/>'
        f'    </a:solidFill>'
        f'  </a:ln>'
        f'</p:spPr>'
        f'<p:style>'
        f'  <a:lnRef idx="1"><a:schemeClr clr="accent1"/></a:lnRef>'
        f'  <a:fillRef idx="0"><a:schemeClr clr="accent1"/></a:fillRef>'
        f'  <a:effectRef idx="0"><a:schemeClr clr="accent1"/></a:effectRef>'
        f'  <a:fontRef idx="minor"><a:schemeClr clr="dk2"/></a:fontRef>'
        f'</p:style>'
        f'</p:cxnSp>'
    )
    sp_tree.append(etree.fromstring(cxnSp_xml))


def _label(slide, cx, cy, text, font_size=9, color=None, bold=False, align=PP_ALIGN.LEFT, width=Inches(2.5)):
    if color is None:
        color = DARK
    add_textbox(slide,
                left=cx, top=cy - Inches(0.17),
                width=width, height=Inches(0.34),
                text=text,
                font_size=font_size, color=color, bold=bold,
                align=align)


def sI_fork_diagram(prs):
    sl = blank_slide(prs)
    slide_header(sl, "Appendix I — Repository Fork & Merge-Back Plan")

    # -----------------------------------------------------------------------
    # Layout constants
    # -----------------------------------------------------------------------
    # Two horizontal swim-lanes:
    #   Top row    y=UPSTREAM_Y  : qualcomm/qcom-test-automation-controller
    #   Bottom row y=FORK_Y      : bryantf-qc/qtac-refactor

    UPSTREAM_Y = Inches(2.1)
    FORK_Y     = Inches(4.5)

    # Commit x positions on the upstream lane
    X_ORIGIN   = Inches(0.9)   # first upstream commit (project start)
    X_FORK_PT  = Inches(2.8)   # fork point on upstream
    X_MERGE    = Inches(11.6)  # merge-back point on upstream

    # Commit x positions on the fork lane
    X_FORK_START = Inches(2.8)  # same column as fork point
    X_FORK_END   = Inches(11.6)  # tip of fork branch (aligns with merge)

    # Upstream continues past merge
    X_UPSTREAM_END = Inches(12.5)

    C_UPSTREAM = RGBColor(0x00, 0x70, 0xC0)   # blue  – upstream commits
    C_FORK     = RGBColor(0xE0, 0x60, 0x00)   # orange – fork commits
    C_MERGE    = RGBColor(0x10, 0x80, 0x30)   # green  – merge-back commit

    # -----------------------------------------------------------------------
    # Repo name labels (left side)
    # -----------------------------------------------------------------------
    add_textbox(sl, Inches(0.1), UPSTREAM_Y - Inches(0.5),
                Inches(2.5), Inches(0.38),
                "qualcomm/\nqcom-test-automation-controller",
                font_size=9, bold=True, color=C_UPSTREAM)
    add_textbox(sl, Inches(0.1), FORK_Y - Inches(0.45),
                Inches(2.5), Inches(0.38),
                "bryantf-qc/\nqtac-refactor",
                font_size=9, bold=True, color=C_FORK)

    # -----------------------------------------------------------------------
    # Upstream branch line  (origin → past merge)
    # -----------------------------------------------------------------------
    _hline(sl, X_ORIGIN, X_UPSTREAM_END, UPSTREAM_Y, C_UPSTREAM)

    # Upstream commits
    for x in [X_ORIGIN, X_FORK_PT]:
        _commit_dot(sl, x, UPSTREAM_Y, C_UPSTREAM)

    # Merge-back commit (green dot on upstream)
    _commit_dot(sl, X_MERGE, UPSTREAM_Y, C_MERGE)

    # Upstream label at end
    add_textbox(sl, X_UPSTREAM_END - Inches(0.1), UPSTREAM_Y - Inches(0.27),
                Inches(0.9), Inches(0.28),
                "main  ▶", font_size=9, bold=True, color=C_UPSTREAM)

    # -----------------------------------------------------------------------
    # Fork branch line
    # -----------------------------------------------------------------------
    _hline(sl, X_FORK_START, X_FORK_END, FORK_Y, C_FORK)

    # Start and end dots only
    _commit_dot(sl, X_FORK_START, FORK_Y, C_FORK)
    _commit_dot(sl, X_FORK_END,   FORK_Y, C_FORK)

    # Shaded "Qt Removal / Refactoring" section bar beneath the fork line
    BAR_TOP    = FORK_Y + Inches(0.22)
    BAR_HEIGHT = Inches(0.52)
    bar = sl.shapes.add_shape(1,
                              X_FORK_START, BAR_TOP,
                              X_FORK_END - X_FORK_START, BAR_HEIGHT)
    bar.fill.solid()
    bar.fill.fore_color.rgb = RGBColor(0xFF, 0xE8, 0xCC)   # light orange
    bar.line.color.rgb = C_FORK
    bar.line.width = Pt(0.75)

    add_textbox(sl,
                X_FORK_START + Inches(0.1), BAR_TOP + Inches(0.05),
                X_FORK_END - X_FORK_START - Inches(0.2), Inches(0.4),
                "Qt Removal / Refactoring",
                font_size=13, bold=True, color=C_FORK, align=PP_ALIGN.CENTER)

    # -----------------------------------------------------------------------
    # Fork connector (diagonal down from upstream fork point to fork start)
    # -----------------------------------------------------------------------
    _diag_line(sl, X_FORK_PT, UPSTREAM_Y, X_FORK_START, FORK_Y, C_FORK)

    # Merge connector (diagonal up from fork tip to merge point on upstream)
    _diag_line(sl, X_FORK_END, FORK_Y, X_MERGE, UPSTREAM_Y, C_MERGE)

    # -----------------------------------------------------------------------
    # Commit labels — upstream
    # -----------------------------------------------------------------------
    _label(sl, X_ORIGIN   - Inches(0.1), UPSTREAM_Y - Inches(0.42),
           "project\nstart", font_size=8, color=C_UPSTREAM)
    _label(sl, X_FORK_PT  - Inches(0.1), UPSTREAM_Y - Inches(0.42),
           "sync\npoint", font_size=8, color=C_UPSTREAM)
    _label(sl, X_MERGE    - Inches(0.55), UPSTREAM_Y - Inches(0.55),
           "merge-back\nPR", font_size=9, bold=True, color=C_MERGE)

    # -----------------------------------------------------------------------
    # Annotation boxes
    # -----------------------------------------------------------------------
    # "fork point" annotation under upstream lane
    add_textbox(sl, X_FORK_PT - Inches(0.9), UPSTREAM_Y + Inches(0.2),
                Inches(1.8), Inches(0.3),
                "fork", font_size=9, color=GRAY, align=PP_ALIGN.CENTER)

    # What lands in merge-back
    add_textbox(sl, X_MERGE - Inches(1.5), UPSTREAM_Y + Inches(0.2),
                Inches(3.1), Inches(0.55),
                "adds source/ alongside src/\nzero changes to legacy code",
                font_size=9, color=C_MERGE, align=PP_ALIGN.CENTER)

    # -----------------------------------------------------------------------
    # Legend
    # -----------------------------------------------------------------------
    lx = Inches(0.5)
    ly = Inches(6.35)
    _commit_dot(sl, lx + Inches(0.15), ly, C_UPSTREAM, r=Inches(0.1))
    add_textbox(sl, lx + Inches(0.35), ly - Inches(0.16), Inches(3.0), Inches(0.32),
                "upstream (qualcomm/qcom-test-automation-controller)",
                font_size=9, color=C_UPSTREAM)

    _commit_dot(sl, lx + Inches(4.5), ly, C_FORK, r=Inches(0.1))
    add_textbox(sl, lx + Inches(4.7), ly - Inches(0.16), Inches(3.0), Inches(0.32),
                "refactor fork (bryantf-qc/qtac-refactor)",
                font_size=9, color=C_FORK)

    _commit_dot(sl, lx + Inches(8.5), ly, C_MERGE, r=Inches(0.1))
    add_textbox(sl, lx + Inches(8.7), ly - Inches(0.16), Inches(2.5), Inches(0.32),
                "merge-back PR",
                font_size=9, color=C_MERGE)


def sH_python_demo(prs):
    sl = blank_slide(prs)
    slide_header(sl, "Appendix H — TACDev.dll Demo (Python)")

    add_textbox(sl, Inches(0.5), Inches(0.9), Inches(12.3), Inches(0.35),
                "Device control from Python with no GUI and no Qt installed:",
                font_size=14, color=GRAY)

    add_code_box(sl, Inches(0.5), Inches(1.3), Inches(7.0), Inches(4.55),
                 "import ctypes\n\n"
                 "tacdev = ctypes.CDLL(\"TACDev.dll\")\n\n"
                 "# Set return types for handle-returning functions\n"
                 "tacdev.TACDev_Open.restype          = ctypes.c_void_p\n"
                 "tacdev.TACDev_GetFirmwareVersion.restype = ctypes.c_char_p\n\n"
                 "# Open device\n"
                 "h = tacdev.TACDev_Open(b\"COM5\")\n"
                 "assert h, \"Failed to open device\"\n\n"
                 "# Read firmware version\n"
                 "fw = tacdev.TACDev_GetFirmwareVersion(h)\n"
                 "print(f\"Firmware: {fw.decode()}\")\n\n"
                 "# Power on sequence\n"
                 "tacdev.TACDev_QuickCommand(h, b\"POWER_ON\")\n\n"
                 "# Toggle a pin directly\n"
                 "PIN_POWER = 0x0001\n"
                 "tacdev.TACDev_SetPinState(h, PIN_POWER, True)\n\n"
                 "# Close\n"
                 "tacdev.TACDev_Close(h)",
                 font_size=9.5)

    add_textbox(sl, Inches(7.8), Inches(0.9), Inches(5.2), Inches(0.35),
                "Prerequisites:", font_size=14, bold=True, color=ACCENT)
    bullets = [
        "Python 3.x (standard library only — ctypes is built-in)",
        "TACDev.dll in the same directory or on PATH",
        "ftd2xx.dll alongside TACDev.dll (FTDI devices)",
        "No Qt6 installation required",
        "No C++ build toolchain required",
    ]
    add_bullets(sl, Inches(7.8), Inches(1.3), Inches(5.2), Inches(2.4),
                bullets, font_size=13)

    add_textbox(sl, Inches(7.8), Inches(3.85), Inches(5.2), Inches(0.35),
                "Deployment footprint:", font_size=14, bold=True, color=ACCENT)
    rows = [
        ["File", "Size", "Required?"],
        ["TACDev.dll",      "~300 KB",  "Yes"],
        ["ftd2xx.dll",      "~800 KB",  "FTDI only"],
        ["serialport.dll",  "~150 KB",  "Serial only"],
        ["Qt6*.dll",        "~38 MB",   "No (not needed)"],
    ]
    col_widths = [Inches(2.3), Inches(1.2), Inches(1.55)]
    add_table(sl, Inches(7.8), Inches(4.25), Inches(5.05), rows,
              col_widths=col_widths, font_size=12)

    add_textbox(sl, Inches(7.8), Inches(6.3), Inches(5.2), Inches(0.5),
                "Same C API works from C#, LabVIEW, or any language with FFI.",
                font_size=12, color=GRAY)


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main():
    prs = new_prs()

    s01_title(prs)
    s02_what_is_tac(prs)
    s03_problem(prs)
    s04_goal(prs)
    s05_approaches(prs)
    s06_wrappers(prs)
    s07_types(prs)
    s08_signals_problem(prs)
    s09_signal_solution(prs)
    s10_bridge(prs)
    s11_tacdev(prs)
    s12_waves(prs)
    s13_results(prs)
    s14_demo(prs)
    s15_gaps(prs)
    s16_summary(prs)

    # Appendices
    sA_file_map(prs)
    sB_signal_design(prs)
    sC_commits(prs)
    sD_alpacascript(prs)
    sE_platforms(prs)
    sF_build(prs)
    sG1_licensing_overview(prs)
    sG2_licensing_detail(prs)
    sI_fork_diagram(prs)
    sH_python_demo(prs)

    out = os.path.join(os.path.dirname(__file__), "presentation-qt-refactor.pptx")
    prs.save(out)
    print(f"Saved: {out}  ({len(prs.slides)} slides)")


if __name__ == "__main__":
    main()
