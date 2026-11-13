// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

//! Rust interface to the QTAC TACDev dynamic library.
//!
//! Loads `TACDev.dll` (Windows) or `libTACDev.so` (Linux) at runtime using
//! `libloading`, so a new version of QTAC can be installed without recompiling
//! the application.
//!
//! # Quick start
//!
//! ```no_run
//! use tacdev::{TACDev, TACError};
//!
//! fn main() -> Result<(), TACError> {
//!     let tac = TACDev::load()?;
//!     println!("QTAC version: {}", tac.alpaca_version()?);
//!     let count = tac.device_count()?;
//!     for i in 0..count {
//!         let mut dev = tac.get_device(i)?;
//!         dev.open()?;
//!         println!("{}: battery={}", dev.port_name(), dev.battery_state()?);
//!         dev.close()?;
//!     }
//!     Ok(())
//! }
//! ```

use libloading::{Library, Symbol};
use std::ffi::CString;
use std::path::PathBuf;

// ---------------------------------------------------------------------------
// Error type
// ---------------------------------------------------------------------------

#[derive(Debug)]
pub enum TACError {
    /// The TACDev shared library could not be found or loaded.
    LibraryNotFound(String),
    /// A required symbol was absent from the loaded library.
    SymbolNotFound(String),
    /// The library's `InitializeTACDev()` call failed.
    InitFailed,
    /// A TAC API call returned a non-zero error code.
    ApiError(u32),
    /// A string returned by the library was not valid UTF-8.
    InvalidString,
    /// A handle was used after the device was closed, or open failed.
    BadHandle,
}

impl std::fmt::Display for TACError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            TACError::LibraryNotFound(s) => write!(f, "TACDev library not found: {s}"),
            TACError::SymbolNotFound(s) => write!(f, "Symbol not found in TACDev library: {s}"),
            TACError::InitFailed => write!(f, "TACDev initialization failed"),
            TACError::ApiError(code) => write!(f, "TACDev API error {code}: {}", error_string(*code)),
            TACError::InvalidString => write!(f, "Invalid string returned by TACDev"),
            TACError::BadHandle => write!(f, "Bad TAC handle — device not open"),
        }
    }
}

impl std::error::Error for TACError {}

pub fn error_string(code: u32) -> &'static str {
    match code {
        0 => "No error",
        1 => "Buffer too small",
        2 => "Bad TAC handle — is the device open?",
        3 => "Command not found",
        4 => "Bad index",
        5 => "Initialization failed",
        6 => "Script variable not found",
        _ => "Unknown error",
    }
}

// ---------------------------------------------------------------------------
// Library discovery — mirrors Python's _find_lib() logic
// ---------------------------------------------------------------------------

fn find_library() -> Result<PathBuf, TACError> {
    #[cfg(target_os = "windows")]
    let lib_name = "TACDev.dll";
    #[cfg(not(target_os = "windows"))]
    let lib_name = "libTACDev.so";

    // 1) Well-known installed product locations
    #[cfg(target_os = "windows")]
    let install_dirs: &[&str] = &[
        r"C:\Program Files\Qualcomm\QTAC",
        r"C:\Program Files\Qualcomm\Alpaca",
    ];
    #[cfg(not(target_os = "windows"))]
    let install_dirs: &[&str] = &["/opt/qcom/QTAC/lib", "/opt/qcom/Alpaca/lib"];

    for dir in install_dirs {
        let p = PathBuf::from(dir).join(lib_name);
        if p.exists() {
            return Ok(p);
        }
    }

    // 2) Local development build — walk up from CWD up to 8 levels
    #[cfg(all(target_os = "windows", target_arch = "x86_64"))]
    let rel: &str = r"__Builds\x64\Release\bin\TACDev.dll";
    #[cfg(all(target_os = "windows", target_arch = "aarch64"))]
    let rel: &str = r"__Builds\ARM64\Release\bin\TACDev.dll";
    #[cfg(not(target_os = "windows"))]
    let rel: &str = "__Builds/Linux/Release/lib/libTACDev.so";

    let mut candidate = std::env::current_dir().unwrap_or_else(|_| PathBuf::from("."));
    for _ in 0..8 {
        let p = candidate.join(rel);
        if p.exists() {
            return Ok(p);
        }
        if !candidate.pop() {
            break;
        }
    }

    Err(TACError::LibraryNotFound(format!(
        "Searched install dirs and walked up 8 levels from CWD looking for {lib_name}. \
         Install QTAC or build the project first."
    )))
}

// ---------------------------------------------------------------------------
// Raw FFI type aliases — match TACDev.h
// ---------------------------------------------------------------------------

type TacHandle = u32;
type TacResult = u32;

const BAD_TAC_HANDLE: TacHandle = 0;
const NO_TAC_ERROR: TacResult = 0;
const TACDEV_INIT_FAILED: TacResult = 5;

// ---------------------------------------------------------------------------
// TACDev — library handle + global functions
// ---------------------------------------------------------------------------

/// Entry point.  Load the library and call global functions through this.
pub struct TACDev {
    // Safety: `_lib` must outlive all `Symbol`s derived from it.
    // We keep it alive for the lifetime of TACDev.
    _lib: Library,

    // Global functions (not tied to a handle)
    f_alpaca_version:   unsafe extern "C" fn(*mut u8, i32) -> TacResult,
    f_tac_version:      unsafe extern "C" fn(*mut u8, i32) -> TacResult,
    f_last_tac_error:   unsafe extern "C" fn(*mut u8, i32) -> TacResult,
    f_get_logging:      unsafe extern "C" fn(*mut bool) -> TacResult,
    f_set_logging:      unsafe extern "C" fn(bool) -> TacResult,
    f_device_count:     unsafe extern "C" fn(*mut i32) -> TacResult,
    f_port_data:        unsafe extern "C" fn(i32, *mut u8, i32) -> u32,
    f_open_by_desc:     unsafe extern "C" fn(*const u8) -> TacHandle,
    f_close_handle:     unsafe extern "C" fn(TacHandle) -> TacResult,

    // Per-device functions (stored here, shared across TACDevice instances)
    f_get_name:         unsafe extern "C" fn(TacHandle, *mut u8, i32) -> TacResult,
    f_get_firmware:     unsafe extern "C" fn(TacHandle, *mut u8, i32) -> TacResult,
    f_get_hardware:     unsafe extern "C" fn(TacHandle, *mut u8, i32) -> TacResult,
    f_get_hw_version:   unsafe extern "C" fn(TacHandle, *mut u8, i32) -> TacResult,
    f_get_uuid:         unsafe extern "C" fn(TacHandle, *mut u8, i32) -> TacResult,
    f_set_ext_power:    unsafe extern "C" fn(TacHandle, bool) -> TacResult,
    f_cmd_count:        unsafe extern "C" fn(TacHandle, *mut u32) -> TacResult,
    f_get_cmd:          unsafe extern "C" fn(TacHandle, u32, *mut u8, i32) -> TacResult,
    f_quick_cmd_count:  unsafe extern "C" fn(TacHandle, *mut u32) -> TacResult,
    f_get_quick_cmd:    unsafe extern "C" fn(TacHandle, u32, *mut u8, i32) -> TacResult,
    f_var_count:        unsafe extern "C" fn(TacHandle, *mut u32) -> TacResult,
    f_get_var:          unsafe extern "C" fn(TacHandle, u32, *mut u8, i32) -> TacResult,
    f_update_var:       unsafe extern "C" fn(TacHandle, *const u8, *const u8) -> TacResult,
    f_cmd_state:        unsafe extern "C" fn(TacHandle, *const u8, *mut bool) -> TacResult,
    f_send_cmd:         unsafe extern "C" fn(TacHandle, *const u8, bool) -> TacResult,
    f_set_pin:          unsafe extern "C" fn(TacHandle, i32, bool) -> TacResult,
    f_queue_clear:      unsafe extern "C" fn(TacHandle, *mut bool) -> TacResult,
    f_help_text:        unsafe extern "C" fn(TacHandle, *mut u8, i32, *mut i32) -> TacResult,
    f_set_battery:      unsafe extern "C" fn(TacHandle, bool) -> TacResult,
    f_get_battery:      unsafe extern "C" fn(TacHandle, *mut bool) -> TacResult,
    f_usb0:             unsafe extern "C" fn(TacHandle, bool) -> TacResult,
    f_get_usb0:         unsafe extern "C" fn(TacHandle, *mut bool) -> TacResult,
    f_usb1:             unsafe extern "C" fn(TacHandle, bool) -> TacResult,
    f_get_usb1:         unsafe extern "C" fn(TacHandle, *mut bool) -> TacResult,
    f_power_key:        unsafe extern "C" fn(TacHandle, bool) -> TacResult,
    f_get_power_key:    unsafe extern "C" fn(TacHandle, *mut bool) -> TacResult,
    f_vol_up:           unsafe extern "C" fn(TacHandle, bool) -> TacResult,
    f_get_vol_up:       unsafe extern "C" fn(TacHandle, *mut bool) -> TacResult,
    f_vol_down:         unsafe extern "C" fn(TacHandle, bool) -> TacResult,
    f_get_vol_down:     unsafe extern "C" fn(TacHandle, *mut bool) -> TacResult,
    f_disc_uim1:        unsafe extern "C" fn(TacHandle, bool) -> TacResult,
    f_get_disc_uim1:    unsafe extern "C" fn(TacHandle, *mut bool) -> TacResult,
    f_disc_uim2:        unsafe extern "C" fn(TacHandle, bool) -> TacResult,
    f_get_disc_uim2:    unsafe extern "C" fn(TacHandle, *mut bool) -> TacResult,
    f_disc_sd:          unsafe extern "C" fn(TacHandle, bool) -> TacResult,
    f_get_disc_sd:      unsafe extern "C" fn(TacHandle, *mut bool) -> TacResult,
    f_primary_edl:      unsafe extern "C" fn(TacHandle, bool) -> TacResult,
    f_get_primary_edl:  unsafe extern "C" fn(TacHandle, *mut bool) -> TacResult,
    f_secondary_edl:    unsafe extern "C" fn(TacHandle, bool) -> TacResult,
    f_get_secondary_edl:unsafe extern "C" fn(TacHandle, *mut bool) -> TacResult,
    f_force_ps_hold:    unsafe extern "C" fn(TacHandle, bool) -> TacResult,
    f_get_force_ps_hold:unsafe extern "C" fn(TacHandle, *mut bool) -> TacResult,
    f_sec_pm_resin:     unsafe extern "C" fn(TacHandle, bool) -> TacResult,
    f_get_sec_pm_resin: unsafe extern "C" fn(TacHandle, *mut bool) -> TacResult,
    f_eud:              unsafe extern "C" fn(TacHandle, bool) -> TacResult,
    f_get_eud:          unsafe extern "C" fn(TacHandle, *mut bool) -> TacResult,
    f_headset_disc:     unsafe extern "C" fn(TacHandle, bool) -> TacResult,
    f_get_headset_disc: unsafe extern "C" fn(TacHandle, *mut bool) -> TacResult,
    f_set_name:         unsafe extern "C" fn(TacHandle, *const u8) -> TacResult,
    f_reset_count:      unsafe extern "C" fn(TacHandle, *mut i32) -> TacResult,
    f_clear_reset:      unsafe extern "C" fn(TacHandle) -> TacResult,
    f_power_on_btn:     unsafe extern "C" fn(TacHandle) -> TacResult,
    f_power_off_btn:    unsafe extern "C" fn(TacHandle) -> TacResult,
    f_fastboot_btn:     unsafe extern "C" fn(TacHandle) -> TacResult,
    f_uefi_btn:         unsafe extern "C" fn(TacHandle) -> TacResult,
    f_edl_btn:          unsafe extern "C" fn(TacHandle) -> TacResult,
    f_sec_edl_btn:      unsafe extern "C" fn(TacHandle) -> TacResult,
}

macro_rules! load_sym {
    ($lib:expr, $name:literal) => {{
        let sym: Symbol<unsafe extern "C" fn()> = $lib
            .get($name)
            .map_err(|_| TACError::SymbolNotFound(String::from_utf8_lossy($name).into_owned()))?;
        // SAFETY: caller must ensure the function signature matches TACDev.h
        std::mem::transmute(sym.into_raw())
    }};
}

impl TACDev {
    /// Load `TACDev.dll` / `libTACDev.so` and initialize the library.
    pub fn load() -> Result<Self, TACError> {
        let path = find_library()?;
        // SAFETY: we trust the library at the discovered path is a valid TACDev build.
        let lib = unsafe {
            Library::new(&path)
                .map_err(|e| TACError::LibraryNotFound(format!("{path:?}: {e}")))?
        };

        let tac = unsafe {
            let init: Symbol<unsafe extern "C" fn() -> TacResult> = lib
                .get(b"InitializeTACDev")
                .map_err(|_| TACError::SymbolNotFound("InitializeTACDev".into()))?;
            let rc = init();
            if rc == TACDEV_INIT_FAILED {
                return Err(TACError::InitFailed);
            }

            TACDev {
                f_alpaca_version:    load_sym!(lib, b"GetAlpacaVersion"),
                f_tac_version:       load_sym!(lib, b"GetTACVersion"),
                f_last_tac_error:    load_sym!(lib, b"GetLastTACError"),
                f_get_logging:       load_sym!(lib, b"GetLoggingState"),
                f_set_logging:       load_sym!(lib, b"SetLoggingState"),
                f_device_count:      load_sym!(lib, b"GetDeviceCount"),
                f_port_data:         load_sym!(lib, b"GetPortData"),
                f_open_by_desc:      load_sym!(lib, b"OpenHandleByDescription"),
                f_close_handle:      load_sym!(lib, b"CloseTACHandle"),
                f_get_name:          load_sym!(lib, b"GetName"),
                f_get_firmware:      load_sym!(lib, b"GetFirmwareVersion"),
                f_get_hardware:      load_sym!(lib, b"GetHardware"),
                f_get_hw_version:    load_sym!(lib, b"GetHardwareVersion"),
                f_get_uuid:          load_sym!(lib, b"GetUUID"),
                f_set_ext_power:     load_sym!(lib, b"SetExternalPowerControl"),
                f_cmd_count:         load_sym!(lib, b"GetCommandCount"),
                f_get_cmd:           load_sym!(lib, b"GetCommand"),
                f_quick_cmd_count:   load_sym!(lib, b"GetQuickCommandCount"),
                f_get_quick_cmd:     load_sym!(lib, b"GetQuickCommand"),
                f_var_count:         load_sym!(lib, b"GetScriptVariableCount"),
                f_get_var:           load_sym!(lib, b"GetScriptVariable"),
                f_update_var:        load_sym!(lib, b"UpdateScriptVariableValue"),
                f_cmd_state:         load_sym!(lib, b"GetCommandState"),
                f_send_cmd:          load_sym!(lib, b"SendCommand"),
                f_set_pin:           load_sym!(lib, b"SetPinState"),
                f_queue_clear:       load_sym!(lib, b"IsCommandQueueClear"),
                f_help_text:         load_sym!(lib, b"GetHelpText"),
                f_set_battery:       load_sym!(lib, b"SetBatteryState"),
                f_get_battery:       load_sym!(lib, b"GetBatteryState"),
                f_usb0:              load_sym!(lib, b"Usb0"),
                f_get_usb0:          load_sym!(lib, b"GetUsb0State"),
                f_usb1:              load_sym!(lib, b"Usb1"),
                f_get_usb1:          load_sym!(lib, b"GetUsb1State"),
                f_power_key:         load_sym!(lib, b"PowerKey"),
                f_get_power_key:     load_sym!(lib, b"GetPowerKeyState"),
                f_vol_up:            load_sym!(lib, b"VolumeUp"),
                f_get_vol_up:        load_sym!(lib, b"GetVolumeUpState"),
                f_vol_down:          load_sym!(lib, b"VolumeDown"),
                f_get_vol_down:      load_sym!(lib, b"GetVolumeDownState"),
                f_disc_uim1:         load_sym!(lib, b"DisconnectUIM1"),
                f_get_disc_uim1:     load_sym!(lib, b"GetDisconnectUIM1State"),
                f_disc_uim2:         load_sym!(lib, b"DisconnectUIM2"),
                f_get_disc_uim2:     load_sym!(lib, b"GetDisconnectUIM2State"),
                f_disc_sd:           load_sym!(lib, b"DisconnectSDCard"),
                f_get_disc_sd:       load_sym!(lib, b"GetDisconnectSDCardState"),
                f_primary_edl:       load_sym!(lib, b"PrimaryEDL"),
                f_get_primary_edl:   load_sym!(lib, b"GetPrimaryEDLState"),
                f_secondary_edl:     load_sym!(lib, b"SecondaryEDL"),
                f_get_secondary_edl: load_sym!(lib, b"GetSecondaryEDLState"),
                f_force_ps_hold:     load_sym!(lib, b"ForcePSHoldHigh"),
                f_get_force_ps_hold: load_sym!(lib, b"GetForcePSHoldHighState"),
                f_sec_pm_resin:      load_sym!(lib, b"SecondaryPmResinN"),
                f_get_sec_pm_resin:  load_sym!(lib, b"GetSecondaryPmResinNState"),
                f_eud:               load_sym!(lib, b"Eud"),
                f_get_eud:           load_sym!(lib, b"GetEUDState"),
                f_headset_disc:      load_sym!(lib, b"HeadsetDisconnect"),
                f_get_headset_disc:  load_sym!(lib, b"GetHeadsetDisconnectState"),
                f_set_name:          load_sym!(lib, b"SetName"),
                f_reset_count:       load_sym!(lib, b"GetResetCount"),
                f_clear_reset:       load_sym!(lib, b"ClearResetCount"),
                f_power_on_btn:      load_sym!(lib, b"PowerOnButton"),
                f_power_off_btn:     load_sym!(lib, b"PowerOffButton"),
                f_fastboot_btn:      load_sym!(lib, b"BootToFastBootButton"),
                f_uefi_btn:          load_sym!(lib, b"BootToUEFIMenuButton"),
                f_edl_btn:           load_sym!(lib, b"BootToEDLButton"),
                f_sec_edl_btn:       load_sym!(lib, b"BootToSecondaryEDLButton"),
                _lib: lib,
            }
        };
        Ok(tac)
    }

    // -----------------------------------------------------------------------
    // Global functions
    // -----------------------------------------------------------------------

    pub fn alpaca_version(&self) -> Result<String, TACError> {
        read_string(|buf, len| unsafe { (self.f_alpaca_version)(buf, len) })
    }

    pub fn tac_version(&self) -> Result<String, TACError> {
        read_string(|buf, len| unsafe { (self.f_tac_version)(buf, len) })
    }

    pub fn last_tac_error(&self) -> Result<String, TACError> {
        read_string(|buf, len| unsafe { (self.f_last_tac_error)(buf, len) })
    }

    pub fn logging_state(&self) -> Result<bool, TACError> {
        let mut state = false;
        check(unsafe { (self.f_get_logging)(&mut state) })?;
        Ok(state)
    }

    pub fn set_logging_state(&self, enabled: bool) -> Result<(), TACError> {
        check(unsafe { (self.f_set_logging)(enabled) })
    }

    pub fn device_count(&self) -> Result<i32, TACError> {
        let mut count = 0i32;
        check(unsafe { (self.f_device_count)(&mut count) })?;
        Ok(count)
    }

    /// Return a `TACDevice` for `device_index`.  The device is not yet open.
    pub fn get_device(&self, device_index: i32) -> Result<TACDevice<'_>, TACError> {
        let mut buf = vec![0u8; 1024];
        let rc = unsafe { (self.f_port_data)(device_index, buf.as_mut_ptr(), buf.len() as i32) };
        if rc == 0 {
            return Err(TACError::ApiError(0));
        }
        let s = cstr_to_string(&buf)?;
        let parts: Vec<&str> = s.splitn(4, ';').collect();
        if parts.len() < 3 {
            return Err(TACError::ApiError(0));
        }
        Ok(TACDevice {
            tac: self,
            port_name: parts[0].to_owned(),
            description: parts[1].to_owned(),
            serial_number: parts[2].to_owned(),
            handle: BAD_TAC_HANDLE,
        })
    }
}

// ---------------------------------------------------------------------------
// TACDevice — per-device handle
// ---------------------------------------------------------------------------

/// A single connected TAC device.  Obtain one from [`TACDev::get_device`].
pub struct TACDevice<'a> {
    tac: &'a TACDev,
    port_name: String,
    description: String,
    serial_number: String,
    handle: TacHandle,
}

impl<'a> TACDevice<'a> {
    pub fn port_name(&self) -> &str { &self.port_name }
    pub fn description(&self) -> &str { &self.description }
    pub fn serial_number(&self) -> &str { &self.serial_number }
    pub fn is_open(&self) -> bool { self.handle != BAD_TAC_HANDLE }

    pub fn open(&mut self) -> Result<(), TACError> {
        let name = CString::new(self.port_name.as_str()).map_err(|_| TACError::InvalidString)?;
        let h = unsafe { (self.tac.f_open_by_desc)(name.as_ptr() as *const u8) };
        if h == BAD_TAC_HANDLE {
            return Err(TACError::BadHandle);
        }
        self.handle = h;
        Ok(())
    }

    pub fn close(&mut self) -> Result<(), TACError> {
        check(unsafe { (self.tac.f_close_handle)(self.handle) })?;
        self.handle = BAD_TAC_HANDLE;
        Ok(())
    }

    pub fn name(&self) -> Result<String, TACError> {
        read_string_h(self.handle, |h, buf, len| unsafe { (self.tac.f_get_name)(h, buf, len) })
    }

    pub fn firmware_version(&self) -> Result<String, TACError> {
        read_string_h(self.handle, |h, buf, len| unsafe { (self.tac.f_get_firmware)(h, buf, len) })
    }

    pub fn hardware(&self) -> Result<String, TACError> {
        read_string_h(self.handle, |h, buf, len| unsafe { (self.tac.f_get_hardware)(h, buf, len) })
    }

    pub fn hardware_version(&self) -> Result<String, TACError> {
        read_string_h(self.handle, |h, buf, len| unsafe { (self.tac.f_get_hw_version)(h, buf, len) })
    }

    pub fn uuid(&self) -> Result<String, TACError> {
        read_string_h(self.handle, |h, buf, len| unsafe { (self.tac.f_get_uuid)(h, buf, len) })
    }

    pub fn set_external_power_control(&self, state: bool) -> Result<(), TACError> {
        check(unsafe { (self.tac.f_set_ext_power)(self.handle, state) })
    }

    pub fn command_count(&self) -> Result<u32, TACError> {
        let mut n = 0u32;
        check(unsafe { (self.tac.f_cmd_count)(self.handle, &mut n) })?;
        Ok(n)
    }

    pub fn command(&self, index: u32) -> Result<String, TACError> {
        read_string_h(self.handle, |h, buf, len| unsafe { (self.tac.f_get_cmd)(h, index, buf, len) })
    }

    pub fn quick_command_count(&self) -> Result<u32, TACError> {
        let mut n = 0u32;
        check(unsafe { (self.tac.f_quick_cmd_count)(self.handle, &mut n) })?;
        Ok(n)
    }

    pub fn quick_command(&self, index: u32) -> Result<String, TACError> {
        read_string_h(self.handle, |h, buf, len| unsafe { (self.tac.f_get_quick_cmd)(h, index, buf, len) })
    }

    pub fn script_variable_count(&self) -> Result<u32, TACError> {
        let mut n = 0u32;
        check(unsafe { (self.tac.f_var_count)(self.handle, &mut n) })?;
        Ok(n)
    }

    pub fn script_variable(&self, index: u32) -> Result<String, TACError> {
        read_string_h(self.handle, |h, buf, len| unsafe { (self.tac.f_get_var)(h, index, buf, len) })
    }

    pub fn update_script_variable(&self, name: &str, value: &str) -> Result<(), TACError> {
        let n = CString::new(name).map_err(|_| TACError::InvalidString)?;
        let v = CString::new(value).map_err(|_| TACError::InvalidString)?;
        check(unsafe {
            (self.tac.f_update_var)(self.handle, n.as_ptr() as *const u8, v.as_ptr() as *const u8)
        })
    }

    pub fn command_state(&self, command: &str) -> Result<bool, TACError> {
        let c = CString::new(command).map_err(|_| TACError::InvalidString)?;
        let mut state = false;
        check(unsafe { (self.tac.f_cmd_state)(self.handle, c.as_ptr() as *const u8, &mut state) })?;
        Ok(state)
    }

    pub fn send_command(&self, command: &str, state: bool) -> Result<(), TACError> {
        let c = CString::new(command).map_err(|_| TACError::InvalidString)?;
        check(unsafe { (self.tac.f_send_cmd)(self.handle, c.as_ptr() as *const u8, state) })
    }

    pub fn set_pin_state(&self, pin: i32, state: bool) -> Result<(), TACError> {
        check(unsafe { (self.tac.f_set_pin)(self.handle, pin, state) })
    }

    pub fn is_command_queue_clear(&self) -> Result<bool, TACError> {
        let mut state = false;
        check(unsafe { (self.tac.f_queue_clear)(self.handle, &mut state) })?;
        Ok(state)
    }

    /// Returns the full help text for this device.  Uses the two-call pattern
    /// (first call probes the required buffer size).
    pub fn help_text(&self) -> Result<String, TACError> {
        let mut actual = 0i32;
        let mut buf = vec![0u8; 1];
        unsafe { (self.tac.f_help_text)(self.handle, buf.as_mut_ptr(), 1, &mut actual) };
        buf.resize((actual + 1) as usize, 0);
        check(unsafe {
            (self.tac.f_help_text)(self.handle, buf.as_mut_ptr(), buf.len() as i32, &mut actual)
        })?;
        cstr_to_string(&buf)
    }

    pub fn battery_state(&self) -> Result<bool, TACError> {
        read_bool(self.handle, |h, s| unsafe { (self.tac.f_get_battery)(h, s) })
    }
    pub fn set_battery_state(&self, state: bool) -> Result<(), TACError> {
        check(unsafe { (self.tac.f_set_battery)(self.handle, state) })
    }

    pub fn usb0_state(&self) -> Result<bool, TACError> {
        read_bool(self.handle, |h, s| unsafe { (self.tac.f_get_usb0)(h, s) })
    }
    pub fn set_usb0(&self, state: bool) -> Result<(), TACError> {
        check(unsafe { (self.tac.f_usb0)(self.handle, state) })
    }

    pub fn usb1_state(&self) -> Result<bool, TACError> {
        read_bool(self.handle, |h, s| unsafe { (self.tac.f_get_usb1)(h, s) })
    }
    pub fn set_usb1(&self, state: bool) -> Result<(), TACError> {
        check(unsafe { (self.tac.f_usb1)(self.handle, state) })
    }

    pub fn power_key_state(&self) -> Result<bool, TACError> {
        read_bool(self.handle, |h, s| unsafe { (self.tac.f_get_power_key)(h, s) })
    }
    pub fn set_power_key(&self, state: bool) -> Result<(), TACError> {
        check(unsafe { (self.tac.f_power_key)(self.handle, state) })
    }

    pub fn volume_up_state(&self) -> Result<bool, TACError> {
        read_bool(self.handle, |h, s| unsafe { (self.tac.f_get_vol_up)(h, s) })
    }
    pub fn set_volume_up(&self, state: bool) -> Result<(), TACError> {
        check(unsafe { (self.tac.f_vol_up)(self.handle, state) })
    }

    pub fn volume_down_state(&self) -> Result<bool, TACError> {
        read_bool(self.handle, |h, s| unsafe { (self.tac.f_get_vol_down)(h, s) })
    }
    pub fn set_volume_down(&self, state: bool) -> Result<(), TACError> {
        check(unsafe { (self.tac.f_vol_down)(self.handle, state) })
    }

    pub fn disconnect_uim1_state(&self) -> Result<bool, TACError> {
        read_bool(self.handle, |h, s| unsafe { (self.tac.f_get_disc_uim1)(h, s) })
    }
    pub fn set_disconnect_uim1(&self, state: bool) -> Result<(), TACError> {
        check(unsafe { (self.tac.f_disc_uim1)(self.handle, state) })
    }

    pub fn disconnect_uim2_state(&self) -> Result<bool, TACError> {
        read_bool(self.handle, |h, s| unsafe { (self.tac.f_get_disc_uim2)(h, s) })
    }
    pub fn set_disconnect_uim2(&self, state: bool) -> Result<(), TACError> {
        check(unsafe { (self.tac.f_disc_uim2)(self.handle, state) })
    }

    pub fn disconnect_sd_card_state(&self) -> Result<bool, TACError> {
        read_bool(self.handle, |h, s| unsafe { (self.tac.f_get_disc_sd)(h, s) })
    }
    pub fn set_disconnect_sd_card(&self, state: bool) -> Result<(), TACError> {
        check(unsafe { (self.tac.f_disc_sd)(self.handle, state) })
    }

    pub fn primary_edl_state(&self) -> Result<bool, TACError> {
        read_bool(self.handle, |h, s| unsafe { (self.tac.f_get_primary_edl)(h, s) })
    }
    pub fn set_primary_edl(&self, state: bool) -> Result<(), TACError> {
        check(unsafe { (self.tac.f_primary_edl)(self.handle, state) })
    }

    pub fn secondary_edl_state(&self) -> Result<bool, TACError> {
        read_bool(self.handle, |h, s| unsafe { (self.tac.f_get_secondary_edl)(h, s) })
    }
    pub fn set_secondary_edl(&self, state: bool) -> Result<(), TACError> {
        check(unsafe { (self.tac.f_secondary_edl)(self.handle, state) })
    }

    pub fn force_ps_hold_high_state(&self) -> Result<bool, TACError> {
        read_bool(self.handle, |h, s| unsafe { (self.tac.f_get_force_ps_hold)(h, s) })
    }
    pub fn set_force_ps_hold_high(&self, state: bool) -> Result<(), TACError> {
        check(unsafe { (self.tac.f_force_ps_hold)(self.handle, state) })
    }

    pub fn secondary_pm_resin_n_state(&self) -> Result<bool, TACError> {
        read_bool(self.handle, |h, s| unsafe { (self.tac.f_get_sec_pm_resin)(h, s) })
    }
    pub fn set_secondary_pm_resin_n(&self, state: bool) -> Result<(), TACError> {
        check(unsafe { (self.tac.f_sec_pm_resin)(self.handle, state) })
    }

    pub fn eud_state(&self) -> Result<bool, TACError> {
        read_bool(self.handle, |h, s| unsafe { (self.tac.f_get_eud)(h, s) })
    }
    pub fn set_eud(&self, state: bool) -> Result<(), TACError> {
        check(unsafe { (self.tac.f_eud)(self.handle, state) })
    }

    pub fn headset_disconnect_state(&self) -> Result<bool, TACError> {
        read_bool(self.handle, |h, s| unsafe { (self.tac.f_get_headset_disc)(h, s) })
    }
    pub fn set_headset_disconnect(&self, state: bool) -> Result<(), TACError> {
        check(unsafe { (self.tac.f_headset_disc)(self.handle, state) })
    }

    pub fn set_name(&self, name: &str) -> Result<(), TACError> {
        let n = CString::new(name).map_err(|_| TACError::InvalidString)?;
        check(unsafe { (self.tac.f_set_name)(self.handle, n.as_ptr() as *const u8) })
    }

    pub fn reset_count(&self) -> Result<i32, TACError> {
        let mut count = 0i32;
        check(unsafe { (self.tac.f_reset_count)(self.handle, &mut count) })?;
        Ok(count)
    }

    pub fn clear_reset_count(&self) -> Result<(), TACError> {
        check(unsafe { (self.tac.f_clear_reset)(self.handle) })
    }

    pub fn power_on_button(&self) -> Result<(), TACError> {
        check(unsafe { (self.tac.f_power_on_btn)(self.handle) })
    }

    pub fn power_off_button(&self) -> Result<(), TACError> {
        check(unsafe { (self.tac.f_power_off_btn)(self.handle) })
    }

    pub fn boot_to_fastboot_button(&self) -> Result<(), TACError> {
        check(unsafe { (self.tac.f_fastboot_btn)(self.handle) })
    }

    pub fn boot_to_uefi_menu_button(&self) -> Result<(), TACError> {
        check(unsafe { (self.tac.f_uefi_btn)(self.handle) })
    }

    pub fn boot_to_edl_button(&self) -> Result<(), TACError> {
        check(unsafe { (self.tac.f_edl_btn)(self.handle) })
    }

    pub fn boot_to_secondary_edl_button(&self) -> Result<(), TACError> {
        check(unsafe { (self.tac.f_sec_edl_btn)(self.handle) })
    }
}

impl Drop for TACDevice<'_> {
    fn drop(&mut self) {
        if self.handle != BAD_TAC_HANDLE {
            unsafe { (self.tac.f_close_handle)(self.handle) };
            self.handle = BAD_TAC_HANDLE;
        }
    }
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

fn check(rc: TacResult) -> Result<(), TACError> {
    if rc == NO_TAC_ERROR { Ok(()) } else { Err(TACError::ApiError(rc)) }
}

fn cstr_to_string(buf: &[u8]) -> Result<String, TACError> {
    let end = buf.iter().position(|&b| b == 0).unwrap_or(buf.len());
    String::from_utf8(buf[..end].to_vec()).map_err(|_| TACError::InvalidString)
}

/// Call a global string-returning function (no handle).
fn read_string<F>(f: F) -> Result<String, TACError>
where
    F: Fn(*mut u8, i32) -> TacResult,
{
    let mut buf = vec![0u8; 1024];
    check(f(buf.as_mut_ptr(), buf.len() as i32))?;
    cstr_to_string(&buf)
}

/// Call a per-device string-returning function.
fn read_string_h<F>(handle: TacHandle, f: F) -> Result<String, TACError>
where
    F: Fn(TacHandle, *mut u8, i32) -> TacResult,
{
    let mut buf = vec![0u8; 1024];
    check(f(handle, buf.as_mut_ptr(), buf.len() as i32))?;
    cstr_to_string(&buf)
}

/// Call a per-device bool-returning function.
fn read_bool<F>(handle: TacHandle, f: F) -> Result<bool, TACError>
where
    F: Fn(TacHandle, *mut bool) -> TacResult,
{
    let mut state = false;
    check(f(handle, &mut state))?;
    Ok(state)
}
