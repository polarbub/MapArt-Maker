#pragma once
#include "nbt.h"
#include "string"

static size_t writer_write(void* userdata, uint8_t* data, size_t size);

void write_nbt_file(const char* name, nbt_tag_t* tag, int flags);

void SetTagName(nbt_tag_t* tag, const std::string name);

nbt_tag_t* CreateCompoundTag(std::string name, nbt_tag_t* append = NULL);

nbt_tag_t* CreateListTag(std::string name, nbt_tag_type_t type, nbt_tag_t* append = NULL);

nbt_tag_t* CreateIntTag(std::string name, int val, nbt_tag_t* append = NULL);

nbt_tag_t* CreateLongTag(std::string name, int64_t val, nbt_tag_t* append = NULL);

nbt_tag_t* CreateStringTag(std::string name, std::string val, nbt_tag_t* append = NULL);

nbt_tag_t* CreateLongArrayTag(std::string name, int64_t vals[], int length, nbt_tag_t* append = NULL);

void CloseTag(nbt_tag_t* tag, nbt_tag_t* append, bool toCompound = true);