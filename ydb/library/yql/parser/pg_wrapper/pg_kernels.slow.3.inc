TExecFunc arrow_float4_to_char() { return MakeIndirectExec<true, false>(&float4_to_char); }
TExecFunc arrow_float8_to_char() { return MakeIndirectExec<true, false>(&float8_to_char); }
TExecFunc arrow_ginarrayconsistent() { return MakeIndirectExec<true, true>(&ginarrayconsistent); }
TExecFunc arrow_ginarrayextract() { return MakeIndirectExec<true, true>(&ginarrayextract); }
TExecFunc arrow_ginarrayextract_2args() { return MakeIndirectExec<true, true>(&ginarrayextract_2args); }
TExecFunc arrow_ginarraytriconsistent() { return MakeIndirectExec<true, true>(&ginarraytriconsistent); }
TExecFunc arrow_ginqueryarrayextract() { return MakeIndirectExec<true, true>(&ginqueryarrayextract); }
TExecFunc arrow_int4_to_char() { return MakeIndirectExec<true, false>(&int4_to_char); }
TExecFunc arrow_int8_to_char() { return MakeIndirectExec<true, false>(&int8_to_char); }
TExecFunc arrow_interval_to_char() { return MakeIndirectExec<true, false>(&interval_to_char); }
TExecFunc arrow_numeric_to_char() { return MakeIndirectExec<true, false>(&numeric_to_char); }
TExecFunc arrow_numeric_to_number() { return MakeIndirectExec<true, false>(&numeric_to_number); }
TExecFunc arrow_thesaurus_init() { return MakeIndirectExec<true, true>(&thesaurus_init); }
TExecFunc arrow_thesaurus_lexize() { return MakeIndirectExec<true, true>(&thesaurus_lexize); }
TExecFunc arrow_timestamp_to_char() { return MakeIndirectExec<true, false>(&timestamp_to_char); }
TExecFunc arrow_timestamptz_to_char() { return MakeIndirectExec<true, false>(&timestamptz_to_char); }
TExecFunc arrow_to_date() { return MakeIndirectExec<true, true>(&to_date); }
TExecFunc arrow_to_timestamp() { return MakeIndirectExec<true, true>(&to_timestamp); }
TExecFunc arrow_tsquery_rewrite() { return MakeIndirectExec<true, false>(&tsquery_rewrite); }
TExecFunc arrow_tsquery_rewrite_query() { return MakeIndirectExec<true, false>(&tsquery_rewrite_query); }
