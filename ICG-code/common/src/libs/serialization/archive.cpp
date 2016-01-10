#include "archive.h"
#include "xmliarchive.h"
#include "xmloarchive.h"
#include "textiarchive.h"
#include "textoarchive.h"
#include "biniarchive.h"
#include "binoarchive.h"

map<string, ArchiveTypeInfo*> ArchiveTypeRegistry::name_to_info;
map<string, string> ArchiveTypeRegistry::typeid_to_name;
map<string, size_t> ArchiveTypeRegistry::name_to_hash;
map<size_t, string> ArchiveTypeRegistry::hash_to_name;
hash<string> ArchiveTypeRegistry::hashFunc;
size_t ArchiveTypeRegistry::count = 1;
