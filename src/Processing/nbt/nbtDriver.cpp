#include "nbtDriver.h"
#include <stdio.h>
#include "nbt.h"


static size_t writer_write(void* userdata, uint8_t* data, size_t size) {
    return fwrite(data, 1, size, static_cast<FILE*>(userdata));
}

void write_nbt_file(const char* name, nbt_tag_t* tag, int flags) {
    FILE* file = fopen(name, "wb");
    nbt_writer_t writer;

    writer.write = writer_write;
    writer.userdata = file;

    nbt_write(writer, tag, flags);
    fclose(file);
}

void SetTagName(nbt_tag_t* tag, const std::string name) {
    nbt_set_tag_name(tag, name.c_str(), name.size());
}

nbt_tag_t* CreateCompoundTag(std::string name, nbt_tag_t* append) {
    nbt_tag_t* tag = nbt_new_tag_compound();
    SetTagName(tag, name);
    if (append != NULL) {
        nbt_tag_compound_append(append, tag);
        return NULL;
    }
    return tag;
}

nbt_tag_t* CreateListTag(std::string name, nbt_tag_type_t type, nbt_tag_t* append) {
    nbt_tag_t* tag = nbt_new_tag_list(type);
    SetTagName(tag, name);
    if (append != NULL) {
        nbt_tag_compound_append(append, tag);
        return NULL;
    }
    return tag;
}

nbt_tag_t* CreateIntTag(std::string name, int val, nbt_tag_t* append) {
    nbt_tag_t* tag = nbt_new_tag_int(val);
    SetTagName(tag, name);
    if (append != NULL) {
        nbt_tag_compound_append(append, tag);
        return NULL;
    }
    return tag;
}

nbt_tag_t* CreateLongTag(std::string name, int64_t val, nbt_tag_t* append) {
    nbt_tag_t* tag = nbt_new_tag_long(val);
    SetTagName(tag, name);
    if (append != NULL) {
        nbt_tag_compound_append(append, tag);
        return NULL;
    }
    return tag;
}

nbt_tag_t* CreateStringTag(std::string name, std::string val, nbt_tag_t* append) {
    nbt_tag_t* tag = nbt_new_tag_string(val.c_str(), val.size());
    SetTagName(tag, name);
    if (append != NULL) {
        nbt_tag_compound_append(append, tag);
        return NULL;
    }
    return tag;
}

nbt_tag_t* CreateLongArrayTag(std::string name, int64_t vals[], int length, nbt_tag_t* append) {
    nbt_tag_t* tag = nbt_new_tag_long_array(vals, length);
    SetTagName(tag, name);
    if (append != NULL) {
        nbt_tag_compound_append(append, tag);
        return NULL;
    }
    return tag;
}

void CloseTag(nbt_tag_t* tag, nbt_tag_t* append, bool toCompound) {
    if (toCompound)
        nbt_tag_compound_append(append, tag);
    else
        nbt_tag_list_append(append, tag);
}

