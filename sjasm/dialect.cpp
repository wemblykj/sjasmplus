/*

  SjASMPlus Z80 Cross Compiler

  Copyright (c) 2004-2006 Aprisobal
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

  References:
    Lerm Z80 Toolkit v2 user manual - https://spectrumcomputing.co.uk/pub/sinclair/games-info/z/Z80Toolkit2.pdf
    Lerm Sam Assembler user manual - https://www.worldofsam.org/system/files/2023-12/LERM%20SAM%20ASSEMBLER%203.1%20Manual.pdf
	Coment - Z80 Assembler for the Sam Coupe - https://www.worldofsam.org/sites/default/files/dl-18/comet_v1-3_man.pdf
	Manual for SC_ASSEMBLER - https://www.worldofsam.org/system/files/2018-05/SC_ASSEMBLER%20notes%20and%20manual%20pdf.pdf
*/

// dialect.cpp

#include "sjdefs.h"

const char* DIALECT_SJASMPLUS_ID = "SJASMPLUS";

bool IsSjASMDialect(const char* name) {
	if (nullptr == name) return true;
	if (strcmp(name, DIALECT_SJASMPLUS_ID) == 0)
	{
		return true;
	}
	return false;
}

bool IsLermDialect(const char* name) {
	if (nullptr == name) return true;
	if (strcmp(name, "ZXLERM") &&
		strcmp(name, "SAMLERM"))
	{
		return false;
	}
	return true;
}

bool IsCometDialect(const char* name) {
	if (nullptr == name) return true;
	if (strcmp(name, "SAMCOMET"))
	{
		return false;
	}
	return true;
}

bool IsDirectiveSupported(const char* dirName) {
	if (nullptr == dirName) return true;
	if (Dialect->DirectiveStrategy == DDS_ANY) return true;

	bool isListed = Dialect->DirectiveList.find(dirName) != Dialect->DirectiveList.end();
	return (Dialect->DirectiveStrategy == DDS_WHITELIST) ? isListed : !isListed;
}

static void initRegularDialect(CDialect* const dialect, base_list_t basePrefixes, base_list_t baseSuffixes, char groupingSeparator, EDialectDirectiveStrategy directiveStrategy, const directive_list_t& directiveList, aint defaultOrg) {
	dialect->BasePrefixes = basePrefixes;
	dialect->BaseSuffixes = baseSuffixes;
	dialect->GroupingSeparator = groupingSeparator;
	dialect->DirectiveStrategy = directiveStrategy;
	dialect->DirectiveList = directiveList;
	dialect->DefaultOrg = defaultOrg;
}

directive_list_t tSjASMPlusDirectiveBlacklist = {
	"DUMP", "PUT", "MDAT"
};

const char NoGroupingSeperator = '\0';

static void initSjASMPlusDialect(CDialect* const dialect) {
	const base_list_t BasePrefixes = { {"%", 1} , {"0b", 1}, {"0q", 1}, { "#", 4 }, { "$", 4 }, { "0x", 4 } };
	const base_list_t BaseSuffixes = { {"b", 1}, {"q", 3}, {"o", 3}, { "h", 4 }, { "d", 0 } };
	const char GroupingSeparator = '\'';
	initRegularDialect(dialect, BasePrefixes, BaseSuffixes, GroupingSeparator, DDS_ANY, tSjASMPlusDirectiveBlacklist, - 1);
}

static void DialectSjASMPlus(CDialect** dialect, CDialect* parent, bool isStrict) {
	*dialect = new CDialect("SJASMPLUS", parent, isStrict);
	initSjASMPlusDialect(*dialect);
}

const base_list_t ZXLermBasePrefixes = { {"%", 1}, { "#", 4 } };

static void DialectZXLerm(CDialect** dialect, CDialect* parent, bool isStrict) {
	const directive_list_t ZXLermDirectiveBlacklist = {"DUMP", "PUT", "INCLUDE","DB", "DW", "DS"};
	const aint DefaultOrg = 27000;		// as per Lerm Assembler manual
	
	*dialect = new CDialect("ZXLERM", parent, isStrict);

	initRegularDialect(*dialect, ZXLermBasePrefixes, {}, NoGroupingSeperator, DDS_BLACKLIST, ZXLermDirectiveBlacklist, DefaultOrg);
}

static void DialectSamLerm(CDialect** dialect, CDialect* parent, bool isStrict) {
	const directive_list_t SamLermDirectiveBlacklist = {"DUMP", "MDAT", "INCLUDE"};

	const aint DefaultOrg = 32768;		// Documented as not being below 32768, assume default is 32768 for now (not specified in the manual)

	*dialect = new CDialect("SAMLERM", parent, isStrict);

	initRegularDialect(*dialect, ZXLermBasePrefixes, {}, NoGroupingSeperator,  DDS_BLACKLIST, SamLermDirectiveBlacklist, DefaultOrg);
}

static void DialectSamComet(CDialect** dialect, CDialect* parent, bool isStrict) {
	const base_list_t BasePrefixes = { {"%", 1}, { "&", 4 } };
	
	const directive_list_t SamCometDirectiveBlacklist = {"DISP", "ENT", "PUT", "DEFW", "DEFS", "DEFM", "DB", "DW", "DS"};
	const aint DefaultOrg = 32768;		// as per Comet Assembler manual

	*dialect = new CDialect("SAMCOMET", parent, isStrict);
	initRegularDialect(*dialect, BasePrefixes, {}, NoGroupingSeperator, DDS_BLACKLIST, SamCometDirectiveBlacklist, DefaultOrg);
}

static void DialectSCAssembler(CDialect** dialect, CDialect* parent, bool isStrict) {
	const base_list_t BasePrefixes = { {"%", 1}, { "#", 4 } };
	const directive_list_t SCAssemblerDirectiveBlacklist = { "DUMP", "INCLUDE","DEFB", "DEFW", "DEFM", "DEFS" };
	const aint DefaultOrg = 32768;		// as per SC_ASSEMBLER manual

	*dialect = new CDialect("SC_ASSEMBLER", parent, isStrict);

	initRegularDialect(*dialect, BasePrefixes, {}, NoGroupingSeperator, DDS_BLACKLIST, SCAssemblerDirectiveBlacklist, DefaultOrg);
}

static bool SetUserDefinedDialect(const char* id, CDialect** dialect, CDialect* parent, bool isStrict) {
	auto findIt = std::find_if(
		DefDialects.begin(), DefDialects.end(),
		[&](const CDialectDef* el) { return 0 == strcasecmp(id, el->getID()); }
	);
	if (DefDialects.end() == findIt) return false;	// not found
	const CDialectDef& def = **findIt;
	
	*dialect = new CDialect(def.getID(), parent, isStrict);
	initRegularDialect(*dialect, def.BasePrefixes, def.BaseSuffixes, def.GroupingSeparator, def.DirectiveStrategy, def.DirectiveList, def.DefaultOrg);
	
	return true;
}

bool SetDialect(const char* const_id, bool isStrict) {
	CDialect** dialect;
	CDialect* parent = nullptr;
	char* id = const_cast<char*>(const_id);		//TODO cmphstr for both const/nonconst variants?
	// ^ argument is const because of lua bindings

	if (!id || cmphstr(id, "none")) {
		DialectID = nullptr;
		Dialect = nullptr;
		return true;
	}

	if (!DialectID || strcmp(DialectID, id)) {	// different dialect than current, change to it
		DialectID = nullptr;
		dialect = &Dialects;
		// search for dialect
		while (*dialect) {
			parent = *dialect;
			if (!strcmp(parent->ID, id)) break;
			dialect = &(parent->Next);
		}
		if (nullptr == (*dialect)) {	// dialect not found
			if (cmphstr(id, "sjasmplus")) {
				DialectSjASMPlus(dialect, parent, isStrict);
			}
			else if (cmphstr(id, "comet")) {
				//if (IsSamCoupe(DeviceID));
				DialectSamComet(dialect, parent, isStrict);
			}
			else if (cmphstr(id, "lerm")) {
				//if (IsSamCoupe(DeviceID));
				//	DialectSamLerm(dialect, parent);
				// else
				DialectZXLerm(dialect, parent, isStrict);
			}
			else if (cmphstr(id, "sc_assembler")) {
				//if (IsSamCoupe(DeviceID));
				//	DialectSamLerm(dialect, parent);
				// else
				DialectSCAssembler(dialect, parent, isStrict);
			}
			else if (!SetUserDefinedDialect(id, dialect, parent, isStrict)) {
				return false;
			}
		}
		// set up the found/new dialect
		Dialect = (*dialect);
		DialectID = Dialect->ID;
	}
	return true;
}

const char* GetDialectName() {
	return DialectID ? DialectID : DIALECT_SJASMPLUS_ID;
}

std::vector<CDialectDef*> DefDialects;

CDialectDef::CDialectDef(const char* name, const base_list_t& basePrefixes, const base_list_t& baseSuffixes, char groupingSeparator, EDialectDirectiveStrategy directiveStrategy, const directive_list_t& directiveList, aint defaultOrg)
	: BasePrefixes(basePrefixes), BaseSuffixes(baseSuffixes), GroupingSeparator(groupingSeparator), DirectiveStrategy(directiveStrategy), DirectiveList(directiveList), DefaultOrg(defaultOrg) {
	assert(name);
	ID = STRDUP(name);
}

CDialectDef::~CDialectDef() {
	free(ID);
}

CDialect::CDialect(const char* name, CDialect* parent, bool isStrict)
	: Next(nullptr), IsStrict(isStrict), DirectiveStrategy(DDS_ANY), DefaultOrg(-1) {
	ID = STRDUP(name);
	if (parent) parent->Next = this;
}

CDialect::~CDialect() {
	if (Next) delete Next;
	free(ID);
}