# TAC Configuration File Format

A TAC configuration is stored on disk as **two sibling JSON files**:

| File | Purpose | Audience |
| :-- | :-- | :-- |
| `TAC_<CHIP>_<ID>.pinout.json` | Hardware pinout + automation script | **Shared** — this repo *and* external/3rd-party tools |
| `TAC_<CHIP>_<ID>.tcnf` | UI layout (buttons, tabs, on-screen labels) | The QTAC application |

The `.tcnf` references its pinout file via a relative `pinout_ref` field. The QTAC
app loads both and merges them in memory; external tools only need the
`*.pinout.json` file, which is fully self-contained.

This split lets the hardware description (which physical pin drives which logical
function, and the script that sequences them) be reused outside QTAC without
dragging UI concerns along.

## Pinout file (`*.pinout.json`) — the shared artifact

Self-describing: tools should verify `format == "tac-pinout"` before parsing. A
formal JSON Schema (Draft 2020-12) is provided at [`schemas/pinout-1.0.json`](../schemas/pinout-1.0.json)
(published as `https://qualcomm.github.io/tac/schemas/pinout-1.0.json`); validate a
pinout file against it with any standard JSON Schema validator, e.g.:

```bash
python3 -m pip install jsonschema
python3 -c "import json,jsonschema,sys; jsonschema.validate(json.load(open(sys.argv[1])), json.load(open('schemas/pinout-1.0.json')))" configurations/TAC_FTDI_15.pinout.json
```

```json
{
  "$schema": "https://qualcomm.github.io/tac/schemas/pinout-1.0.json",
  "format": "tac-pinout",
  "schema_version": "1.0",

  "platform_type": "FTDI",
  "platform_id": 81,
  "name": "Arduino Ventuno Q",
  "description": "Arduino Ventuno Q",
  "usb_descriptor": "VentunoQ BugHopper",
  "reset_enabled": false,

  "chip_count": 1,
  "bus": [
    { "chip_index": 0, "bus": "C", "bus_function": 2 }
  ],
  "pins": [
    {
      "chip_index": 0, "bus": "C", "pin_number": "0",
      "command": "edl",
      "input": false,
      "inverted": false,
      "initial_value": false,
      "priority": 0
    }
  ],
  "variables": [
    { "name": "edl", "default_value": "100" }
  ],
  "script": "def powerOff()\n\tbattery 1\n..."
}
```

### Field reference

- **Envelope** — `$schema`, `format` (always `"tac-pinout"`), `schema_version`.
- **Identity** — `platform_type`, `platform_id`, `name`, `description`. Also present
  in the `.tcnf`; the `.tcnf` is authoritative when edited (QTAC writes both, so
  they never diverge).
- **`usb_descriptor`** — USB string used to match a connected debug board.
- **`reset_enabled`** — whether the board exposes a hardware reset line.
- **`chip_count`** / **`bus[]`** — FTDI only. `bus[]` maps each `chip_index`+`bus`
  letter to a `bus_function` (e.g. VCP vs D2XX).
- **`pins[]`** — one entry per physical pin. Identity key is `chip_index`+`bus`+
  `pin_number` for FTDI, just `pin_number` for PSOC/PIC32CX, and `pin` for STM32.
  Hardware fields:
  - `command` — the logical name the script drives (e.g. `edl`, `battery`, `pkey`).
  - `input` — pin direction (FTDI).
  - `inverted` — active-low logic.
  - `initial_value` — state applied at initialization.
  - `priority` / `initialization_priority` — initialization order.
  - `classic_action` — PSOC/PIC32CX built-in action binding.
- **`variant`** / **`slaves[]`** / **`i2c_addr[]`** — PSOC "GPIO with I2C" boards only.
  `variant` selects the board flavour, `slaves[]` describes the fitted I2C GPIO
  expanders (`reg_addr`, `config_addr`, `port_count` and the chip `variant`), and
  `i2c_addr[]` holds one entry per expander pin. An expander pin is identified by
  `pin_number`+`slave_addr`+`write_addr` and carries the same hardware fields as a
  direct pin (`command`, `inverted`, `classic_action`); its labels and placement live
  in the `.tcnf` alongside a matching `ref`.
- **`variables[]`** — `name` + `default_value`. The script substitutes `$name`
  tokens (e.g. `delay $edl`). Labels/tooltips/layout for these live in the `.tcnf`.
- **`script`** — the "Alpaca" automation language. `def <fn>()` blocks made of:
  - `<command> <0|1>` — drive a pin by its `command` name.
  - `delay <ms | $var>` — wait.
  - `logComment <text>` — annotate the log.
  - `<fn>` — call another function.

## UI overlay file (`*.tcnf`)

```json
{
  "pinout_ref": "TAC_FT232H_81.pinout.json",
  "name": "Arduino Ventuno Q",
  "platform_type": "FT232H", "platform_id": 81,
  "tabs": [ ... ],
  "buttons": [ ... ],
  "variables": [
    { "name": "edl", "label": "EDL timing (ms)", "tooltip": "...", "type": 1, "cellLocation": "0,0" }
  ],
  "pins": [
    {
      "ref": { "chip_index": 0, "bus": "C", "pin_number": "0" },
      "enabled": true,
      "name": "EDL",
      "help_hint": "Boot mode select...",
      "group": "General",
      "command_group": 3,
      "run_priority": "0,0"
    }
  ]
}
```

Each overlay pin keys back to its hardware pin via the `ref` object (the same
identity fields used in the pinout file). UI-only pin fields: `enabled`, `name`
(display label), `help_hint` (tooltip), `group`/`tab_name`, `command_group`,
`run_priority` (grid cell).

PSOC "GPIO with I2C" boards carry a second such array, `i2c_addr[]`, holding the UI
half of each expander pin. It follows the same pattern: a `ref` of
`pin_number`+`slave_addr`+`write_addr` joining back to the pinout file, plus
`enabled`, `name`, `help_hint`, `tab_name`, `command_group` and `run_priority`.

## Backward compatibility & migration

QTAC still loads **legacy combined `.tcnf`** files (everything inline, no
`pinout_ref`). Opening and saving such a file in the Configuration Editor upgrades
it to the two-file layout automatically.

The Configuration Editor always writes the two-file layout, both when saving an
existing configuration and when creating one from scratch (**File -> New**); the
pinout file is named after the `.tcnf` and referenced from it, so the pair stays
together. Because a configuration is now two files, both must travel together: the
editor refuses to open a `.tcnf` whose `pinout_ref` cannot be resolved, and reports
which file is missing rather than silently presenting an empty board.

To migrate a folder of legacy configs in bulk, use the `TACConfigSplit` utility:

```bash
TACConfigSplit /path/to/configurations            # split in place
TACConfigSplit /path/to/configurations --dry-run  # preview only
```

Already-split files are skipped. Migrate under version control so the diff is
reviewable.
