static str keyword_as_str(keyword a) {
    assert(a < countof(keyword_names));
    str str = keyword_names[a];
    return str;
}

static keyword keyword_from_spelling(str a) {
    for_val_array(span_str, it, keyword_spellings) {
        for_val(str, s, it) {
            if (str_eq(s, a)) {
                return it_idx;
            }
        }
    }
    return 0;
}

static str operator_as_str(operator a) {
    assert(a < countof(operator_names));
    str str = operator_names[a];
    return str;
}

static operator operator_from_spelling(str a) {
    for_val_array(span_str, it, operator_spellings) {
        for_val(str, s, it) {
            if (str_eq(s, a)) {
                return it_idx;
            }
        }
    }
    return 0;
}
