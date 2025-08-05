#ifndef TABS_H
#define TABS_H
/*
	Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries. 
	 
	Redistribution and use in source and binary forms, with or without
	modification, are permitted (subject to the limitations in the
	disclaimer below) provided that the following conditions are met:
	 
		* Redistributions of source code must retain the above copyright
		  notice, this list of conditions and the following disclaimer.
	 
		* Redistributions in binary form must reproduce the above
		  copyright notice, this list of conditions and the following
		  disclaimer in the documentation and/or other materials provided
		  with the distribution.
	 
		* Neither the name of Qualcomm Technologies, Inc. nor the names of its
		  contributors may be used to endorse or promote products derived
		  from this software without specific prior written permission.
	 
	NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
	GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
	HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
	WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
	MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
	IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
	ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
	GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
	IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
	IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
	Author: Biswajit Roy (biswroy@qti.qualcomm.com)
			Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "QCommonConsoleGlobal.h"

// QCommon
#include "StringUtilities.h"

// Qt
#include <QList>
#include <QString>

struct Tab;

typedef QList<Tab> Tabs;

struct QCOMMONCONSOLE_EXPORT Tab
{
	Tab() = default;
	Tab(const Tab& copyMe) = default;
	~Tab() = default;

	static HashType makeHash(const Tab& tab);

	static Tab find(HashType hash, Tabs& tabs);
	static void addTab(Tab& tab, Tabs& tabs);
	static void removeTab(HashType hash, Tabs& tabs);
	static void updateTabName(HashType hash, QString& name, Tabs& tabs);
	static void setTabVisible(HashType hash, bool visible, Tabs& tabs);

	static QStringList toStringList(Tabs& tabs, bool configurableTabs = true);

	QString						_name;
	bool						_userTab{false};
	bool						_moveable{false};
	bool						_configurable{false};
	bool						_visible{true};
	int							_ordinal{0};

	// this for editing data
	HashType					_hash{0};
	bool						_deleted{false};
	bool						_added{false};
	QString						_newText;
};

#endif // TABS_H
