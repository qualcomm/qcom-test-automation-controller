# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries. 
#  
# Redistribution and use in source and binary forms, with or without
# modification, are permitted (subject to the limitations in the
# disclaimer below) provided that the following conditions are met:
#  
# 	* Redistributions of source code must retain the above copyright
# 	  notice, this list of conditions and the following disclaimer.
#  
# 	* Redistributions in binary form must reproduce the above
# 	  copyright notice, this list of conditions and the following
# 	  disclaimer in the documentation and/or other materials provided
# 	  with the distribution.
#  
# 	* Neither the name of Qualcomm Technologies, Inc. nor the names of its
# 	  contributors may be used to endorse or promote products derived
# 	  from this software without specific prior written permission.
#  
# NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
# GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
# HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
# GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
# IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
# IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# Author: Biswajit Roy <biswroy@qti.qualcomm.com>

# Main applications
set(TEST_AUTOMATION_CONTROLLER_VERSION "5.5.2")
set(DEVICE_CATALOG_VERSION "3.0.0")
set(TAC_CONFIGURATION_EDITOR_VERSION "3.0.0")

# Utility applications
set(DEVLIST_VERSION "2.0.0")
set(TACDUMP_VERSION "2.0.0")
set(FTDI_CHECK_VERSION "3.0.0")
set(UPDATE_DEVICE_LIST_VERSION "2.0.0")

# Programming tools
set(LITE_PROGRAMMER_VERSION "2.0.0")
set(FW_UPDATE_VERSION "3.0.0")

# Additional tools
set(BUG_WRITER_VERSION "2.0.0")
set(EPM_SCOPE_VERSION "2.0.0")
set(ALPACA_VIEWER_VERSION "2.0.0")
set(EPM_VERSION "3.0.0")
set(EPM_EDITOR_VERSION "2.0.0")
set(EEPROM_UTL_VERSION "2.0.0")
set(TACCOM_VERSION "2.0.0")
set(TAC_TERMINAL_VERSION "2.0.0")

# Export all application versions as cache variables for use in other CMake files
set(TEST_AUTOMATION_CONTROLLER_VERSION "${TEST_AUTOMATION_CONTROLLER_VERSION}" CACHE STRING "Test Automation Controller Version")
set(DEVICE_CATALOG_VERSION "${DEVICE_CATALOG_VERSION}" CACHE STRING "Device Catalog Version")
set(TAC_CONFIGURATION_EDITOR_VERSION "${TAC_CONFIGURATION_EDITOR_VERSION}" CACHE STRING "TAC Configuration Editor Version")
set(DEVLIST_VERSION "${DEVLIST_VERSION}" CACHE STRING "DevList Version")
set(TACDUMP_VERSION "${TACDUMP_VERSION}" CACHE STRING "TACDump Version")
set(FTDI_CHECK_VERSION "${FTDI_CHECK_VERSION}" CACHE STRING "FTDI Check Version")
set(UPDATE_DEVICE_LIST_VERSION "${UPDATE_DEVICE_LIST_VERSION}" CACHE STRING "Update Device List Version")
set(LITE_PROGRAMMER_VERSION "${LITE_PROGRAMMER_VERSION}" CACHE STRING "Lite Programmer Version")
set(FW_UPDATE_VERSION "${FW_UPDATE_VERSION}" CACHE STRING "Firmware Update Version")
set(BUG_WRITER_VERSION "${BUG_WRITER_VERSION}" CACHE STRING "Bug Writer Version")
set(EPM_SCOPE_VERSION "${EPM_SCOPE_VERSION}" CACHE STRING "EPM Scope Version")
set(ALPACA_VIEWER_VERSION "${ALPACA_VIEWER_VERSION}" CACHE STRING "Alpaca Viewer Version")
set(EPM_VERSION "${EPM_VERSION}" CACHE STRING "EPM Version")
set(EPM_EDITOR_VERSION "${EPM_EDITOR_VERSION}" CACHE STRING "EPM Editor Version")
set(EEPROM_UTL_VERSION "${EEPROM_UTL_VERSION}" CACHE STRING "EEPROM Utility Version")
set(TACCOM_VERSION "${TACCOM_VERSION}" CACHE STRING "TACCOM Version")
set(TAC_TERMINAL_VERSION "${TAC_TERMINAL_VERSION}" CACHE STRING "TAC Terminal Version")
