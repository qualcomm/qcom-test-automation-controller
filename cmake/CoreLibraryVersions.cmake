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

# Core library versions
set(QTAC_CORE_VERSION "6.9.1")
set(QCOMMON_CONSOLE_VERSION "3.0.1")
set(QCOMMON_VERSION "2.0.0")
set(UI_COMMON_VERSION "2.0.0")

# Shared component versions
set(ALPACA_VERSION "5.5.2")
set(TAC_VERSION "7.0.1")
set(TAC_LIB_VERSION "3.0.1")
set(EPM_SERVER_VERSION "2.0.0")

# Utility versions
set(EEPROM_COMMAND_VERSION "2.0.0")
set(QCOMMONCOMMAND_VERSION "2.0.0")

# Export all core versions as cache variables for use in other CMake files
set(QTAC_CORE_VERSION "${QTAC_CORE_VERSION}" CACHE STRING "QTAC Core Version")
set(QCOMMON_CONSOLE_VERSION "${QCOMMON_CONSOLE_VERSION}" CACHE STRING "QCommon Console Library Version")
set(QCOMMON_VERSION "${QCOMMON_VERSION}" CACHE STRING "QCommon Library Version")
set(UI_COMMON_VERSION "${UI_COMMON_VERSION}" CACHE STRING "UI Common Library Version")
set(ALPACA_VERSION "${ALPACA_VERSION}" CACHE STRING "Alpaca Version")
set(TAC_VERSION "${TAC_VERSION}" CACHE STRING "TAC Version")
set(TAC_LIB_VERSION "${TAC_LIB_VERSION}" CACHE STRING "TAC Library Version")
set(EPM_SERVER_VERSION "${EPM_SERVER_VERSION}" CACHE STRING "EPM Server Version")
set(EEPROM_COMMAND_VERSION "${EEPROM_COMMAND_VERSION}" CACHE STRING "EEPROM Command Version")
set(QCOMMONCOMMAND_VERSION "${QCOMMONCOMMAND_VERSION}" CACHE STRING "QCommon Command Version")
