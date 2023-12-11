static u64 map_hash(u64 val) {
    u64 ret = val * 0x9e3779b97f4a7c13;
    return ret;
}

static bool map_create(map *map, ix cap) {
    zero(map);
    u64 *keys = heap_alloc(cap * sizeof(u64), true);
    u64 *vals = heap_alloc(cap * sizeof(u64), true);

    bool ret = keys && vals;
    if (ret) {
        map->keys = keys;
        map->vals = vals;
        map->cap = cap;
        map->count = 0;
    } else {
        map_destroy(map);
    }

    return ret;
}

static void map_destroy(map *map) {
    heap_free(map->keys);
    heap_free(map->vals);
    zero(map);
}

static void map_rehash(map *src, map *dst) {
    for_range(ix, old_idx, 0, dst->cap) {
        map_put(src, dst->keys[old_idx], dst->vals[old_idx]);
    }
}

static ix map_lookup(map *map, u64 key) {
    ix ret = -1;
    u64 hash = map_hash(key);
    ix cap = map->cap;
    for_range(ix, probe, 0, map->cap) {
        ix idx = (hash + probe) % cap;
        if ((map->keys[idx] == 0) || (map->keys[idx] == key)) {
            ret = idx;
            break;
        }
    }
    return ret;
}

static bool map_put(map *map, u64 key, u64 val) {
    assert(key != 0); // NOTE(rune): Key 0 is reserved for empty slot.
    bool ret = false;
    if (key != 0) {
        if ((map->cap - map->count) <= (map->cap / 4)) {
            struct map expanded;
            if (map_create(&expanded, map->cap * 2)) {
                map_rehash(&expanded, map);
                map_destroy(map);
                *map = expanded;
            }
        }

        ix idx = map_lookup(map, key);
        if (idx != -1) {
            if (map->keys[idx] == 0) {
                map->keys[idx] = key;
                map->count++;
            }

            map->vals[idx] = val;
            ret = true;
        }
    }

    return ret;
}

static bool map_remove(map *map, u64 key) {
    bool ret = false;
    ix idx = map_lookup(map, key);
    if (idx != -1) {
        if (map->keys[idx] != 0) {
            map->keys[idx] = 0;
            map->count--;
        }

        ret = true;
    }
}

static bool map_get(map *map, u64 key, u64 *val) {
    bool ret = false;
    ix slot = map_lookup(map, key); // TODO(rune): Quadratic probe?
    if (slot != -1 && map->keys[slot] != 0) {
        *val = map->vals[slot];
        ret = true;
    }
    return ret;
}
