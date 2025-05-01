/* 

  SjASMPlus Z80 Cross Compiler

  Copyright (c) 2025 Paul Wightmore

  This software is provided 'as-is', without any express or implied warranty.
  In no event will the authors be held liable for any damages arising from the
  use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it freely,
  subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not claim
	 that you wrote the original software. If you use this software in a product,
	 an acknowledgment in the product documentation would be appreciated but is
	 not required.

  2. Altered source versions must be plainly marked as such, and must not be
	 misrepresented as being the original software.

  3. This notice may not be removed or altered from any source distribution.

*/

// dialect.h

bool IsSjASMDialect(const char* name);
bool IsCometDialect(const char* name);
bool IsLermDialect(const char* name);

bool IsDirectiveSupported(const char* dirName);

bool SetDialect(const char *const_id, bool isStrict);
const char* GetDialectName();

enum EDialectDirectiveStrategy { DDS_ANY, DDS_WHITELIST, DDS_BLACKLIST };

using directive_list_t = std::set<std::string>;
using base_list_t = std::map<std::string, int>;

class CDialectDef {
public:
	CDialectDef(const CDialectDef&) = delete;
	CDialectDef(const char* name, const base_list_t& BasePrefixes, const base_list_t& BaseSuffixes, char GroupingSeparator, EDialectDirectiveStrategy directiveStrategy, const directive_list_t& directiveList, aint defaultOrg);
	~CDialectDef();

	const char* getID() const { return ID; }
	const base_list_t BasePrefixes;
	const base_list_t BaseSuffixes;
	const char GroupingSeparator;
	const EDialectDirectiveStrategy DirectiveStrategy;
	const directive_list_t DirectiveList;
	const aint DefaultOrg;

private:
	char* ID;
};

class CDialect {
public:
CDialect(const CDialect&) = delete;
	CDialect(const char* name, CDialect* parent, bool isStrict);
	~CDialect();
	char* ID;
	CDialect* Next;
	bool IsStrict;
	base_list_t BasePrefixes;
	base_list_t BaseSuffixes;
	char GroupingSeparator;
	EDialectDirectiveStrategy DirectiveStrategy;
	std::set<std::string> DirectiveList;
	aint DefaultOrg;
};