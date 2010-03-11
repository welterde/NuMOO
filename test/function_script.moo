; recycle($functests)
; $functests = renumber(create(#1))
; reset_max_object()

@verb $functests:test_disassemble
.program $functests:test_disassemble
{"disassemble", 2, 2, {1, -1}};
$assert:equal(disassemble($testdata, "disassemble_1"), $testdata.disassemble_1_expect, "trivial verb");
$assert:equal(disassemble($testdata, "disassemble_empty"), $testdata.disassemble_empty_expect, "empty verb");
$assert:eval_raises("disassemble($testdata, \"\");", E_VERBNF);
$assert:eval_raises("disassemble($testdata, 99);", E_VERBNF);
.

@verb $functests:test_log_cache_stats
.program $functests:test_log_cache_stats
{"log_cache_stats", 0, 0, {}};
$test:skip("not testable in-moo");
.

@verb $functests:test_verb_cache_stats
.program $functests:test_verb_cache_stats
{"verb_cache_stats", 0, 0, {}};
"Can't test exact values but we can test form";
"Should look something like this:";
{113, 126, 75, 12, {7501, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
vcs = verb_cache_stats();
$assert:ok(length(vcs) == 5, " cache has 5 elements");
$assert:ok(typeof(vcs[1]) == INT, "cache data type 1");
$assert:ok(typeof(vcs[2]) == INT, "cache data type 2");
$assert:ok(typeof(vcs[3]) == INT, "cache data type 3");
$assert:ok(typeof(vcs[4]) == INT, "cache data type 4");
$assert:ok(typeof(vcs[5]) == LIST, "cache data type 5");
"eh, good enough";
$assert:ok(length(vcs[5]) > 8, "length of histogram"); 
.

@verb $functests:test_call_function
.program $functests:test_call_function
{"call_function", 1, -1, {2}};
$assert:equal(call_function("abs", -123), 123);
$assert:eval_raises("call_function(\"abs\", -123, 123);", E_ARGS);
$assert:eval_raises("call_function(\"qwerty\", -123, 123);", E_INVARG);
$assert:eval_raises("call_function(\"\");", E_INVARG);
.

@verb $functests:test_raise
.program $functests:test_raise
{"raise", 1, 3, {-1, 2, -1}};
"half-assed ... not sure how to comprehensively test this";
"It's tested pretty incidentally by $assert anyway";
$test:skip("hard test, needs its own suite");
.

@verb $functests:test_suspend
.program $functests:test_suspend
{"suspend", 0, 1, {0}};
$test:skip("hard test, needs its own suite");
.

@verb $functests:test_read
.program $functests:test_read
{"read", 0, 2, {1, -1}};
$test:skip("hard test, needs its own suite");
.

@verb $functests:test_seconds_left
.program $functests:test_seconds_left
{"seconds_left", 0, 0, {}};
$assert:ok(seconds_left() > 0);
.

@verb $functests:test_ticks_left
.program $functests:test_ticks_left
{"ticks_left", 0, 0, {}};
$assert:ok(ticks_left() > 0);
.

@verb $functests:test_pass
.program $functests:test_pass
{"pass", 0, -1, {}};
$test:skip("Don't feel like testing this yet");
.

@verb $functests:test_set_task_perms
.program $functests:test_set_task_perms
{"set_task_perms", 1, 1, {1}};
set_task_perms($no_one);
$assert:eval_raises("notify(player, \"YOU SHOULD NOT SEE THIS\");", E_PERM);
.

@verb $functests:test_caller_perms
.program $functests:test_caller_perms
{"caller_perms", 0, 0, {}};
$test:skip("Already being tested by test suite");
.

@verb $functests:test_callers
.program $functests:test_callers
{"callers", 0, 1, {-1}};
$test:skip("Already being tested by test suite");
.

@verb $functests:test_task_stack
.program $functests:test_task_stack
{"task_stack", 1, 2, {0, -1}};
$test:skip("Needs to be part of suspend/resume task test");
.

@verb $functests:test_function_info
.program $functests:test_function_info
{"function_info", 0, 1, {2}};
$assert:equal(function_info("task_stack"), {"task_stack", 1, 2, {0, -1}});
.

@verb $functests:test_load_server_options
.program $functests:test_load_server_options
{"load_server_options", 0, 0, {}};
$test:skip();
.

@verb $functests:test_value_bytes
.program $functests:test_value_bytes
{"value_bytes", 1, 1, {-1}};
"This is pretty compiler/platform-dependent but it should usually be 16";
$assert:equal(value_bytes(0), 16);
$assert:equal(value_bytes("foo"), 20);
.

@verb $functests:test_value_hash
.program $functests:test_value_hash
{"value_hash", 1, 1, {-1}};
$assert:equal(value_hash(0), "CFCD208495D565EF66E7DFF9F98764DA");
$assert:equal(value_hash({"foo",123}), "2B92CFFFF46F5C6BA25D1CD05E040F65");
.

@verb $functests:test_string_hash
.program $functests:test_string_hash
{"string_hash", 1, 1, {2}};
$assert:equal(string_hash("foo"), "ACBD18DB4CC2F85CEDEF654FCCC4A4D8");
.

@verb $functests:test_binary_hash
.program $functests:test_binary_hash
{"binary_hash", 1, 1, {2}};
$assert:equal(binary_hash("foo"), string_hash("foo"));
$assert:not_equal(binary_hash("foo~DA"), string_hash("foo~DA"));
$assert:eval_raises("binary_hash(\"foo~\");", E_INVARG);
.

@verb $functests:test_decode_binary
.program $functests:test_decode_binary
{"decode_binary", 1, 2, {2, -1}};
$assert:equal(decode_binary("foo"), {"foo"});
$assert:equal(decode_binary("foo~0D"), {"foo", 13});
$assert:equal(decode_binary("f~00oo"), {"f", 0, "oo"});
$assert:equal(decode_binary("f~00oo", 1), {102, 0, 111, 111});
.

@verb $functests:test_encode_binary
.program $functests:test_encode_binary
{"encode_binary", 0, -1, {}};
"Takes an arbitrary number of 'chunks'.  These are from the docs:";
$assert:equal(encode_binary("~foo"), "~7Efoo");
$assert:equal(encode_binary({"foo", 10}, {"bar", 13}), "foo~0Abar~0D");
$assert:equal(encode_binary("foo", 10, "bar", 13), "foo~0Abar~0D");
.

@verb $functests:test_length
.program $functests:test_length
{"length", 1, 1, {-1}};
$assert:equal(length({1,2,3}), 3);
$assert:equal(length("foo"), 3)
$assert:equal(length(""), 0);
$assert:equal(length({}), 0);
$assert:equal(length({{{{{{{{{{}}}}}}}}}}), 1);
.

@verb $functests:test_setadd
.program $functests:test_setadd
{"setadd", 2, 2, {4, -1}};
l = {1,2,3};
$assert:equal(setadd(l, 3), l); 
$assert:equal(setadd(l, 3), {1,2,3}); 
$assert:equal(setadd(l, 5), {1,2,3,5}); 
$assert:equal(l, {1,2,3}); 
.

@verb $functests:test_setremove
.program $functests:test_setremove
{"setremove", 2, 2, {4, -1}};
l = {1,2,3};
$assert:equal(setremove(l, 2), {1,3}); 
$assert:equal(setremove(l, 1), {2,3}); 
$assert:equal(l, {1,2,3}); 
"setremove is documented to remove only the first occurrence";
$assert:equal(setremove({1,2,3,2}, 2), {1,3,2});
.

@verb $functests:test_listappend
.program $functests:test_listappend
{"listappend", 2, 3, {4, -1, 0}};
"The equivalences given in lambdacore's docs for listappend would make for a good generated test suite";
l = {1,2,3};
$assert:equal(listappend(l,666), {1,2,3,666});
$assert:equal(listappend(l,666,1), {1,666,2,3});
$assert:equal(listappend(l,666,99), {1,2,3,666});
$assert:equal(listappend(l,666,-99), {666,1,2,3});
.

@verb $functests:test_listinsert
.program $functests:test_listinsert
{"listinsert", 2, 3, {4, -1, 0}};
l = {1,2,3};
$assert:equal(listinsert(l,666), {666,1,2,3});
$assert:equal(listinsert(l,666,3), {1,2,666,3});
$assert:equal(listinsert(l,666,99), {1,2,3,666});
$assert:equal(listinsert(l,666,-99), {666,1,2,3});
.

@verb $functests:test_listdelete
.program $functests:test_listdelete
{"listdelete", 2, 2, {4, 0}};
l = {1,2,3};
$assert:equal(listdelete(l, 1), {2,3});
$assert:equal(listdelete(l, 2), {1,3});
$assert:equal(listdelete(l, 3), {1,2});
$assert:eval_raises("listdelete({1,2,3}, 99);", E_RANGE);
$assert:eval_raises("listdelete({}, 0);", E_RANGE);
.

@verb $functests:test_listset
.program $functests:test_listset
{"listset", 3, 3, {4, -1, 0}};
"Not that common anymore but it has its uses";
l = {1,2,3};
$assert:equal(listset(l,1,666), {666,2,3});
$assert:equal(listset(l,2,666), {1,666,3});
$assert:eval_raises("listset({1,2,3}, 4, 666);", E_RANGE);
$assert:eval_raises("listset({}, 0, 666);", E_RANGE);
.

@verb $functests:test_equal
.program $functests:test_equal
{"equal", 2, 2, {-1, -1}};.
$assert:ok(equal("foo", "foo"));
$assert:ok(equal({1,2}, listappend({1},2)));
$assert:ok(equal({"foo", "bar"}, {"fo"+"o", "b"+"ar"}));
$assert:ok(!equal("foo", "Foo"));
"note that int/float equality is normally NEVER true";
$assert:ok(!equal({1,2}, {1.0, 2.0}))
$assert:ok(!equal({"foo", "bar"}, {"Foo", "Bar"}));
.

@verb $functests:test_is_member
.program $functests:test_is_member
{"is_member", 2, 2, {-1, 4}};
$assert:ok(is_member({1,2,3}, 2));
$assert:ok(is_member({"foo", "bar"}, "bar"));
$assert:ok(!is_member({"foo", "bar"}, "Bar"));
"note that int/float equality is normally NEVER true";
$assert:ok(is_member({1,2,3}, 2.0));
.

@verb $functests:test_tostr
.program $functests:test_tostr
{"tostr", 0, -1, {}};
$assert:equal(tostr(""), "");
$assert:equal(tostr("foo", "bar"), "foobar");
$assert:equal(tostr("foo", "", "", "bar"), "foobar");
$assert:equal(tostr("foo", 1, E_NONE, "bar"), "foo1No Errorbar");
$assert:equal(tostr(#0), "#0");
$assert:ok(tostr(new_waif())[1..2] == "[[");
.

@verb $functests:test_toliteral
.program $functests:test_toliteral
{"toliteral", 1, 1, {-1}};
$assert:equal(toliteral(E_NONE), "E_NONE");
$assert:equal(toliteral(#0, "#0"));
$assert:equal(toliteral({#0}, "{#0}"));
$assert:equal(toliteral({}, "{}"));
.

@verb $functests:test_match
.program $functests:test_match
{"match", 2, 3, {2, 2, -1}};
"a full-blown regex test suite is not gonna happen here.  I am merely testing sanity";
$assert:equal(match("foobar", "f%(o*%)b"), {1, 4, {{2, 3}, {0, -1}, {0, -1}, {0, -1}, {0, -1}, {0, -1}, {0, -1}, {0, -1}, {0, -1}}, "foobar"});
$assert:eval_raises("match(\"foobar\", \"f%(o*b\");", E_INVARG);
.

@verb $functests:test_rmatch
.program $functests:test_rmatch
{"rmatch", 2, 3, {2, 2, -1}};
$assert:equal(rmatch("foobarbazfoooooooobarbaz", "f%(o*%)b"), {10, 19, {{11, 18}, {0, -1}, {0, -1}, {0, -1}, {0, -1}, {0, -1}, {0, -1}, {0, -1}, {0, -1}}, "foobarbazfoooooooobarbaz"});
$assert:eval_raises("rmatch(\"foobar\", \"f%(o*b\");", E_INVARG);
.

@verb $functests:test_substitute
.program $functests:test_substitute
{"substitute", 2, 2, {2, 4}};
subs = match("*** Welcome to LambdaMOO!!!", "%(%w*%) to %(%w*%)");
$assert:equals(substitute("I thank you for your %1 here in %2.", subs), "I thank you for your Welcome here in LambdaMOO.");
$assert:eval_raises("substitute(\"%1 %2\", {});", E_INVARG);
.

@verb $functests:test_crypt
.program $functests:test_crypt
{"crypt", 1, 2, {2, 2}};
"Crypt is highly platform-dependent, so I'm not going to test the algorithm, just that it results in something that isn't the original string";
$assert:not_equals(crypt("foo"), "foo");
$assert:not_equals(crypt("foo", "xy"), "foo");
$assert:not_equals(crypt("foo", "xy"), "xyfoo");
$assert:not_equals(crypt("foo", "xy"), "fooxy");
.

@verb $functests:test_index
.program $functests:test_index
{"index", 2, 3, {2, 2, -1}};

.

@verb $functests:test_rindex
.program $functests:test_rindex
{"rindex", 2, 3, {2, 2, -1}};
.

@verb $functests:test_strcmp
.program $functests:test_strcmp
{"strcmp", 2, 2, {2, 2}};
.

@verb $functests:test_strsub
.program $functests:test_strsub
{"strsub", 3, 4, {2, 2, 2, -1}};
.

@verb $functests:test_tochar
.program $functests:test_tochar
{"tochar", 1, 1, {-1}};
.

@verb $functests:test_charname
.program $functests:test_charname
{"charname", 1, 1, {2}};
.

@verb $functests:test_ord
.program $functests:test_ord
{"ord", 1, 1, {2}};
.

@verb $functests:test_encode_chars
.program $functests:test_encode_chars
{"encode_chars", 2, 2, {-1, 2}};
.

@verb $functests:test_decode_chars
.program $functests:test_decode_chars
{"decode_chars", 2, 3, {2, 2, -1}};
.

@verb $functests:test_server_log
.program $functests:test_server_log
{"server_log", 1, 2, {2, -1}};
.

@verb $functests:test_toint
.program $functests:test_toint
{"toint", 1, 1, {-1}};
.

@verb $functests:test_tonum
.program $functests:test_tonum
{"tonum", 1, 1, {-1}};
.

@verb $functests:test_tofloat
.program $functests:test_tofloat
{"tofloat", 1, 1, {-1}};
.

@verb $functests:test_min
.program $functests:test_min
{"min", 1, -1, {-2}};
.

@verb $functests:test_max
.program $functests:test_max
{"max", 1, -1, {-2}};
.

@verb $functests:test_abs
.program $functests:test_abs
{"abs", 1, 1, {-2}};
.

@verb $functests:test_random
.program $functests:test_random
{"random", 0, 1, {0}};
.

@verb $functests:test_time
.program $functests:test_time
{"time", 0, 0, {}};
.

@verb $functests:test_ftime
.program $functests:test_ftime
{"ftime", 0, 0, {}};
.

@verb $functests:test_ctime
.program $functests:test_ctime
{"ctime", 0, 2, {0, 2}};
.

@verb $functests:test_floatstr
.program $functests:test_floatstr
{"floatstr", 2, 3, {9, 0, -1}};
.

@verb $functests:test_sqrt
.program $functests:test_sqrt
{"sqrt", 1, 1, {9}};
.

@verb $functests:test_sin
.program $functests:test_sin
{"sin", 1, 1, {9}};
.

@verb $functests:test_cos
.program $functests:test_cos
{"cos", 1, 1, {9}};
.

@verb $functests:test_tan
.program $functests:test_tan
{"tan", 1, 1, {9}};
.

@verb $functests:test_asin
.program $functests:test_asin
{"asin", 1, 1, {9}};
.

@verb $functests:test_acos
.program $functests:test_acos
{"acos", 1, 1, {9}};
.

@verb $functests:test_atan
.program $functests:test_atan
{"atan", 1, 2, {9, 9}};
.

@verb $functests:test_sinh
.program $functests:test_sinh
{"sinh", 1, 1, {9}};
.

@verb $functests:test_cosh
.program $functests:test_cosh
{"cosh", 1, 1, {9}};
.

@verb $functests:test_tanh
.program $functests:test_tanh
{"tanh", 1, 1, {9}};
.

@verb $functests:test_asinh
.program $functests:test_asinh
{"asinh", 1, 1, {9}};
.

@verb $functests:test_acosh
.program $functests:test_acosh
{"acosh", 1, 1, {9}};
.

@verb $functests:test_atanh
.program $functests:test_atanh
{"atanh", 1, 1, {9}};
.

@verb $functests:test_exp
.program $functests:test_exp
{"exp", 1, 1, {9}};
.

@verb $functests:test_log
.program $functests:test_log
{"log", 1, 1, {9}};
.

@verb $functests:test_log10
.program $functests:test_log10
{"log10", 1, 1, {9}};
.

@verb $functests:test_ceil
.program $functests:test_ceil
{"ceil", 1, 1, {9}};
.

@verb $functests:test_floor
.program $functests:test_floor
{"floor", 1, 1, {9}};
.

@verb $functests:test_trunc
.program $functests:test_trunc
{"trunc", 1, 1, {9}};
.

@verb $functests:test_expm1
.program $functests:test_expm1
{"expm1", 1, 1, {9}};
.

@verb $functests:test_log1p
.program $functests:test_log1p
{"log1p", 1, 1, {9}};
.

@verb $functests:test_erf
.program $functests:test_erf
{"erf", 1, 1, {9}};
.

@verb $functests:test_erfc
.program $functests:test_erfc
{"erfc", 1, 1, {9}};
.

@verb $functests:test_lgamma
.program $functests:test_lgamma
{"lgamma", 1, 1, {9}};
.

@verb $functests:test_j
.program $functests:test_j
{"j", 2, 2, {0, 9}};
.

@verb $functests:test_y
.program $functests:test_y
{"y", 2, 2, {0, 9}};
.

@verb $functests:test_toobj
.program $functests:test_toobj
{"toobj", 1, 1, {-1}};
.

@verb $functests:test_typeof
.program $functests:test_typeof
{"typeof", 1, 1, {-1}};
.

@verb $functests:test_create
.program $functests:test_create
{"create", 1, 2, {1, 1}};
.

@verb $functests:test_recycle
.program $functests:test_recycle
{"recycle", 1, 1, {1}};
.

@verb $functests:test_object_bytes
.program $functests:test_object_bytes
{"object_bytes", 1, 1, {1}};
.

@verb $functests:test_valid
.program $functests:test_valid
{"valid", 1, 1, {1}};
.

@verb $functests:test_parent
.program $functests:test_parent
{"parent", 1, 1, {1}};
.

@verb $functests:test_children
.program $functests:test_children
{"children", 1, 1, {1}};
.

@verb $functests:test_chparent
.program $functests:test_chparent
{"chparent", 2, 2, {1, 1}};
.

@verb $functests:test_max_object
.program $functests:test_max_object
{"max_object", 0, 0, {}};
.

@verb $functests:test_players
.program $functests:test_players
{"players", 0, 0, {}};
.

@verb $functests:test_is_player
.program $functests:test_is_player
{"is_player", 1, 1, {1}};
.

@verb $functests:test_set_player_flag
.program $functests:test_set_player_flag
{"set_player_flag", 2, 2, {1, -1}};
.

@verb $functests:test_move
.program $functests:test_move
{"move", 2, 2, {1, 1}};
.

@verb $functests:test_properties
.program $functests:test_properties
{"properties", 1, 1, {1}};
.

@verb $functests:test_property_info
.program $functests:test_property_info
{"property_info", 2, 2, {1, 2}};
.

@verb $functests:test_set_property_info
.program $functests:test_set_property_info
{"set_property_info", 3, 3, {1, 2, 4}};
.

@verb $functests:test_add_property
.program $functests:test_add_property
{"add_property", 4, 4, {1, 2, -1, 4}};
.

@verb $functests:test_delete_property
.program $functests:test_delete_property
{"delete_property", 2, 2, {1, 2}};
.

@verb $functests:test_clear_property
.program $functests:test_clear_property
{"clear_property", 2, 2, {1, 2}};
.

@verb $functests:test_is_clear_property
.program $functests:test_is_clear_property
{"is_clear_property", 2, 2, {1, 2}};
.

@verb $functests:test_server_version
.program $functests:test_server_version
{"server_version", 0, 0, {}};
.

@verb $functests:test_renumber
.program $functests:test_renumber
{"renumber", 1, 1, {1}};
.

@verb $functests:test_reset_max_object
.program $functests:test_reset_max_object
{"reset_max_object", 0, 0, {}};
.

@verb $functests:test_memory_usage
.program $functests:test_memory_usage
{"memory_usage", 0, 0, {}};
.

@verb $functests:test_shutdown
.program $functests:test_shutdown
{"shutdown", 0, 1, {2}};
.

@verb $functests:test_dump_database
.program $functests:test_dump_database
{"dump_database", 0, 0, {}};
.

@verb $functests:test_db_disk_size
.program $functests:test_db_disk_size
{"db_disk_size", 0, 0, {}};
.

@verb $functests:test_open_network_connection
.program $functests:test_open_network_connection
{"open_network_connection", 0, -1, {}};
.

@verb $functests:test_connected_players
.program $functests:test_connected_players
{"connected_players", 0, 1, {-1}};
.

@verb $functests:test_connected_seconds
.program $functests:test_connected_seconds
{"connected_seconds", 1, 1, {1}};
.

@verb $functests:test_idle_seconds
.program $functests:test_idle_seconds
{"idle_seconds", 1, 1, {1}};
.

@verb $functests:test_connection_name
.program $functests:test_connection_name
{"connection_name", 1, 1, {1}};
.

@verb $functests:test_notify
.program $functests:test_notify
{"notify", 2, 3, {1, 2, -1}};
.

@verb $functests:test_boot_player
.program $functests:test_boot_player
{"boot_player", 1, 1, {1}};
.

@verb $functests:test_set_connection_option
.program $functests:test_set_connection_option
{"set_connection_option", 3, 3, {1, 2, -1}};
.

@verb $functests:test_connection_option
.program $functests:test_connection_option
{"connection_option", 2, 2, {1, 2}};
.

@verb $functests:test_connection_options
.program $functests:test_connection_options
{"connection_options", 1, 1, {1}};
.

@verb $functests:test_listen
.program $functests:test_listen
{"listen", 2, 3, {1, -1, -1}};
.

@verb $functests:test_unlisten
.program $functests:test_unlisten
{"unlisten", 1, 1, {-1}};
.

@verb $functests:test_listeners
.program $functests:test_listeners
{"listeners", 0, 0, {}};
.

@verb $functests:test_buffered_output_length
.program $functests:test_buffered_output_length
{"buffered_output_length", 0, 1, {1}};
.

@verb $functests:test_task_id
.program $functests:test_task_id
{"task_id", 0, 0, {}};
.

@verb $functests:test_queued_tasks
.program $functests:test_queued_tasks
{"queued_tasks", 0, 0, {}};
.

@verb $functests:test_kill_task
.program $functests:test_kill_task
{"kill_task", 1, 1, {0}};
.

@verb $functests:test_output_delimiters
.program $functests:test_output_delimiters
{"output_delimiters", 1, 1, {1}};
.

@verb $functests:test_queue_info
.program $functests:test_queue_info
{"queue_info", 0, 1, {1}};
.

@verb $functests:test_resume
.program $functests:test_resume
{"resume", 1, 2, {0, -1}};
.

@verb $functests:test_force_input
.program $functests:test_force_input
{"force_input", 2, 3, {1, 2, -1}};
.

@verb $functests:test_flush_input
.program $functests:test_flush_input
{"flush_input", 1, 2, {1, -1}};
.

@verb $functests:test_verbs
.program $functests:test_verbs
{"verbs", 1, 1, {1}};
.

@verb $functests:test_verb_info
.program $functests:test_verb_info
{"verb_info", 2, 2, {1, -1}};
.

@verb $functests:test_set_verb_info
.program $functests:test_set_verb_info
{"set_verb_info", 3, 3, {1, -1, 4}};
.

@verb $functests:test_verb_args
.program $functests:test_verb_args
{"verb_args", 2, 2, {1, -1}};
.

@verb $functests:test_set_verb_args
.program $functests:test_set_verb_args
{"set_verb_args", 3, 3, {1, -1, 4}};
.

@verb $functests:test_add_verb
.program $functests:test_add_verb
{"add_verb", 3, 3, {1, 4, 4}};
.

@verb $functests:test_delete_verb
.program $functests:test_delete_verb
{"delete_verb", 2, 2, {1, -1}};
.

@verb $functests:test_verb_code
.program $functests:test_verb_code
{"verb_code", 2, 4, {1, -1, -1, -1}};
.

@verb $functests:test_set_verb_code
.program $functests:test_set_verb_code
{"set_verb_code", 3, 3, {1, -1, 4}};
.

@verb $functests:test_eval
.program $functests:test_eval
{"eval", 1, 1, {2}};
.

@verb $functests:test_new_waif
.program $functests:test_new_waif
{"new_waif", 0, 0, {}};
.

