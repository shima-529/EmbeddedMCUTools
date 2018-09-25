function print_global_macro(macro_name, id, type, comment) {
    if( !id && !type ) {
        printf("%s\n", comment);
    }else{
        printf("%s(%s, %s); %s\n", macro_name, id, type, comment);
    }
}
function print_export_macro(macro_name, id, type, comment) {
    if( !id && !type ) {
        printf("%s\n", comment);
    }else{
        sub(/\[[0-9]*\]/, "[]", id);
        printf("%s(%s, %s); %s\n", macro_name, id, type, comment);
    }
}


BEGIN {
    if( !macro ) {
        macro = "_MAIN";
    }
    if( !macrofile ) {
        macrofile = "instance/root.hpp";
    }
    n_attribute = 1;
    n_type = 2;
    n_id = 3;
    n_arraynum = 4;
    n_comment = 5;
}

!/^$/{
    cnt = 1;
    idx_of_comment = index($0, "//");

    if( $1 ~ /__attribute__\(\(.*\)\)/ ) {
        a[NR, n_attribute] = $cnt;
        cnt++;
    }
    if( idx_of_comment ) { # if comment exists
        a[NR, n_comment] = substr($0, idx_of_comment);
    }
    if( idx_of_comment != 1 ) { # if the line does not start from comment
        a[NR, n_type] = $cnt;
        cnt++;
        a[NR, n_id] = $cnt;
        cnt++;
    }
    match($0, /\[[0-9]+\]/);
    if( RLENGTH != -1 ) {
        a[NR, n_arraynum] = int(substr($0, RSTART + 1, RLENGTH - 2));
    }
}

END {
    printf("#include <%s>\n\n", macrofile);
    printf("#ifdef %s\n", macro);
    for(i=1; i<=NR; i++) {
        idenfiler[i] = a[i, n_id];
        gsub(/;/, "",  idenfiler[i]); # remove ";"
    }

    for(i=1; i<=NR; i++) {
        if( a[i, n_attribute] ) {
            printf a[i, n_attribute] " ";
        }
        print_global_macro("_global_instance", idenfiler[i], a[i, n_type], a[i, n_comment]);
    }
    print "#else /* _MAIN */";
    for(i=1; i<=NR; i++) {
        if( a[i, n_attribute] ) {
            printf a[i, n_attribute] " ";
        }
        print_export_macro("_export_instance", idenfiler[i], a[i, n_type], a[i, n_comment]);
    }

    printf("#endif /* %s */\n", macro);
}

# function print_macro(macro_name, id, type, comment) {
#     if( !id && !type ) {
#         printf("%s\n", comment);
#     }else{
#         printf("%s(%s, %s); %s\n", macro_name, id, type, comment);
#     }
# }
# 
# 
# BEGIN {
#     if( !macro ) {
#         macro = "_MAIN";
#     }
#     if( !macrofile ) {
#         macrofile = "instance/root.hpp";
#     }
#     n_attribute = 1;
#     n_type = 2;
#     n_id = 3;
#     n_comment = 4;
# }
# 
# !/^$/{
#     cnt = 1;
#     idx_of_comment = index($0, "//");
# 
#     if( $1 ~ /__attribute__\(\(.*\)\)/ ) {
#         a[NR, n_attribute] = $cnt;
#         cnt++;
#     }
#     if( idx_of_comment ) { # if comment exists
#         a[NR, n_comment] = substr($0, idx_of_comment);
#     }
#     if( idx_of_comment != 1 ) { # if the line does not start from comment
#         a[NR, n_type] = $cnt;
#         cnt++;
#         a[NR, n_id] = $cnt;
#     }
# }
# 
# END {
#     printf("#include <%s>\n\n", macrofile);
#     printf("#ifdef %s\n", macro);
#     for(i=1; i<=NR; i++) {
#         idenfiler[i] = a[i, n_id];
#         gsub(/;/, "",  idenfiler[i]); # remove ";"
#         gsub(/\[[^\]]*\]/, "[]", idenfiler[i]);
#     }
# 
#     for(i=1; i<=NR; i++) {
#         if( a[i, n_attribute] ) {
#             printf a[i, n_attribute] " ";
#         }
#         print_macro("_global_instance", idenfiler[i], a[i, n_type], a[i, n_comment]);
#     }
#     print "#else";
#     for(i=1; i<=NR; i++) {
#         if( a[i, n_attribute] ) {
#             printf a[i, n_attribute] " ";
#         }
#         print_macro("_export_instance", idenfiler[i], a[i, n_type], a[i, n_comment]);
#     }
# 
#     printf("#endif /* %s */\n", macro);
# }
