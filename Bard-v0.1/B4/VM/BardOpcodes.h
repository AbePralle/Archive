#ifndef BARD_OPCODES_H
#define BARD_OPCODES_H
//=============================================================================
// BardOpcodes.h
//
// This file is automatically generated - do not hand-modify as your changes
// may be overwritten.
//=============================================================================

#define BARD_OP_HALT                                                   0
#define BARD_OP_NOP                                                    1
#define BARD_OP_NATIVE_RETURN                                          2
#define BARD_OP_RETURN_NIL                                             3
#define BARD_OP_RETURN_THIS                                            4
#define BARD_OP_RETURN_OBJECT                                          5
#define BARD_OP_RETURN_REAL                                            6
#define BARD_OP_RETURN_INTEGER                                         7
#define BARD_OP_RETURN_COMPOUND_N_SLOTS                                8
#define BARD_OP_COMPOUND_RETURN_NIL_FP_ADJUST_N                        9
#define BARD_OP_COMPOUND_RETURN_OBJECT_FP_ADJUST_N                    10
#define BARD_OP_COMPOUND_RETURN_REAL_FP_ADJUST_N                      11
#define BARD_OP_COMPOUND_RETURN_INTEGER_FP_ADJUST_N                   12
#define BARD_OP_COMPOUND_RETURN_COMPOUND_A_SLOTS_FP_ADJUST_B          13
#define BARD_OP_THROW_MISSING_RETURN                                  14
#define BARD_OP_THROW_EXCEPTION                                       15
#define BARD_OP_JUMP_TO_OFFSET_N                                      16
#define BARD_OP_JUMP_IF_FALSE_TO_OFFSET_N                             17
#define BARD_OP_JUMP_IF_TRUE_TO_OFFSET_N                              18
#define BARD_OP_JUMP_IF_NULL_TO_OFFSET_N                              19
#define BARD_OP_JUMP_IF_NOT_NULL_TO_OFFSET_N                          20
#define BARD_OP_IF_FALSE_PUSH_FALSE_AND_JUMP_TO_OFFSET_N              21
#define BARD_OP_IF_TRUE_PUSH_TRUE_AND_JUMP_TO_OFFSET_N                22
#define BARD_OP_DUPLICATE_OBJECT                                      23
#define BARD_OP_POP_DISCARD                                           24
#define BARD_OP_POP_DISCARD_COUNT_N                                   25
#define BARD_OP_TRACE_ON                                              26
#define BARD_OP_TRACE_OFF                                             27
#define BARD_OP_FINITE_LOOP                                           28
#define BARD_OP_TYPE_CHECK                                            29
#define BARD_OP_INSTANCE_OF_TYPE_INDEX_N                              30
#define BARD_OP_SPECIALIZE_AS_TYPE_INDEX_N                            31
#define BARD_OP_CONVERT_INTEGER_TO_REAL                               32
#define BARD_OP_CONVERT_REAL_TO_INTEGER                               33
#define BARD_OP_CONVERT_INTEGER_TO_CHARACTER                          34
#define BARD_OP_CONVERT_INTEGER_TO_BYTE                               35
#define BARD_OP_PUSH_THIS                                             36
#define BARD_OP_PUSH_THIS_COMPOUND_N_SLOTS                            37
#define BARD_OP_PUSH_THIS_REAL                                        38
#define BARD_OP_PUSH_THIS_INTEGER                                     39
#define BARD_OP_PUSH_LITERAL_OBJECT_NULL                              40
#define BARD_OP_PUSH_LITERAL_STRING_N                                 41
#define BARD_OP_PUSH_LITERAL_REAL                                     42
#define BARD_OP_PUSH_LITERAL_INTEGER                                  43
#define BARD_OP_PUSH_LITERAL_INTEGER_N                                44
#define BARD_OP_PUSH_LITERAL_INTEGER_N_AS_REAL                        45
#define BARD_OP_PUSH_AND_CLEAR_N_SLOTS                                46
#define BARD_OP_READ_LOCAL_OBJECT_N                                   47
#define BARD_OP_READ_LOCAL_REAL_N                                     48
#define BARD_OP_READ_LOCAL_INTEGER_N                                  49
#define BARD_OP_READ_LOCAL_COMPOUND_SLOT_A_SIZE_B                     50
#define BARD_OP_WRITE_LOCAL_OBJECT_N                                  51
#define BARD_OP_WRITE_LOCAL_REAL_N                                    52
#define BARD_OP_WRITE_LOCAL_INTEGER_N                                 53
#define BARD_OP_WRITE_LOCAL_COMPOUND_SLOT_A_SIZE_B                    54
#define BARD_OP_CLEAR_LOCAL_OBJECT_N                                  55
#define BARD_OP_CLEAR_LOCAL_REAL_N                                    56
#define BARD_OP_CLEAR_LOCAL_INTEGER_N                                 57
#define BARD_OP_CLEAR_LOCAL_COMPOUND_SLOT_A_SIZE_B                    58
#define BARD_OP_ADD_ASSIGN_LOCAL_REAL_N                               59
#define BARD_OP_ADD_ASSIGN_LOCAL_INTEGER_N                            60
#define BARD_OP_SUBTRACT_ASSIGN_LOCAL_REAL_N                          61
#define BARD_OP_SUBTRACT_ASSIGN_LOCAL_INTEGER_N                       62
#define BARD_OP_MULTIPLY_ASSIGN_LOCAL_REAL_N                          63
#define BARD_OP_MULTIPLY_ASSIGN_LOCAL_INTEGER_N                       64
#define BARD_OP_DIVIDE_ASSIGN_LOCAL_REAL_N                            65
#define BARD_OP_DIVIDE_ASSIGN_LOCAL_INTEGER_N                         66
#define BARD_OP_MOD_ASSIGN_LOCAL_REAL_N                               67
#define BARD_OP_MOD_ASSIGN_LOCAL_INTEGER_N                            68
#define BARD_OP_POWER_ASSIGN_LOCAL_REAL_N                             69
#define BARD_OP_POWER_ASSIGN_LOCAL_INTEGER_N                          70
#define BARD_OP_BITWISE_AND_ASSIGN_LOCAL_INTEGER_N                    71
#define BARD_OP_BITWISE_OR_ASSIGN_LOCAL_INTEGER_N                     72
#define BARD_OP_BITWISE_XOR_ASSIGN_LOCAL_INTEGER_N                    73
#define BARD_OP_ADD_ASSIGN_PROPERTY_REAL_N                            74
#define BARD_OP_ADD_ASSIGN_PROPERTY_INTEGER_N                         75
#define BARD_OP_SUBTRACT_ASSIGN_PROPERTY_REAL_N                       76
#define BARD_OP_SUBTRACT_ASSIGN_PROPERTY_INTEGER_N                    77
#define BARD_OP_ADD_ASSIGN_THIS_PROPERTY_REAL_N                       78
#define BARD_OP_ADD_ASSIGN_THIS_PROPERTY_INTEGER_N                    79
#define BARD_OP_SUBTRACT_ASSIGN_THIS_PROPERTY_REAL_N                  80
#define BARD_OP_SUBTRACT_ASSIGN_THIS_PROPERTY_INTEGER_N               81
#define BARD_OP_INCREMENT_LOCAL_REAL_N                                82
#define BARD_OP_INCREMENT_LOCAL_INTEGER_N                             83
#define BARD_OP_DECREMENT_LOCAL_REAL_N                                84
#define BARD_OP_DECREMENT_LOCAL_INTEGER_N                             85
#define BARD_OP_INCREMENT_PROPERTY_REAL_N                             86
#define BARD_OP_INCREMENT_PROPERTY_INTEGER_N                          87
#define BARD_OP_INCREMENT_THIS_PROPERTY_REAL_N                        88
#define BARD_OP_INCREMENT_THIS_PROPERTY_INTEGER_N                     89
#define BARD_OP_DECREMENT_PROPERTY_REAL_N                             90
#define BARD_OP_DECREMENT_PROPERTY_INTEGER_N                          91
#define BARD_OP_DECREMENT_THIS_PROPERTY_REAL_N                        92
#define BARD_OP_DECREMENT_THIS_PROPERTY_INTEGER_N                     93
#define BARD_OP_READ_SETTING_OBJECT_OFFSET_N                          94
#define BARD_OP_READ_SETTING_REAL_OFFSET_N                            95
#define BARD_OP_READ_SETTING_INTEGER_OFFSET_N                         96
#define BARD_OP_READ_SETTING_CHARACTER_OFFSET_N                       97
#define BARD_OP_READ_SETTING_BYTE_OFFSET_N                            98
#define BARD_OP_READ_SETTING_COMPOUND_OFFSET_N                        99
#define BARD_OP_READ_PROPERTY_OBJECT_OFFSET_N                        100
#define BARD_OP_READ_PROPERTY_REAL_OFFSET_N                          101
#define BARD_OP_READ_PROPERTY_INTEGER_OFFSET_N                       102
#define BARD_OP_READ_PROPERTY_CHARACTER_OFFSET_N                     103
#define BARD_OP_READ_PROPERTY_BYTE_OFFSET_N                          104
#define BARD_OP_READ_PROPERTY_COMPOUND_OFFSET_N                      105
#define BARD_OP_READ_THIS_PROPERTY_OBJECT_OFFSET_N                   106
#define BARD_OP_READ_THIS_PROPERTY_REAL_OFFSET_N                     107
#define BARD_OP_READ_THIS_PROPERTY_INTEGER_OFFSET_N                  108
#define BARD_OP_READ_THIS_PROPERTY_CHARACTER_OFFSET_N                109
#define BARD_OP_READ_THIS_PROPERTY_BYTE_OFFSET_N                     110
#define BARD_OP_READ_THIS_PROPERTY_COMPOUND_OFFSET_N                 111
#define BARD_OP_WRITE_SETTING_OBJECT_OFFSET_N                        112
#define BARD_OP_WRITE_SETTING_REAL_OFFSET_N                          113
#define BARD_OP_WRITE_SETTING_INTEGER_OFFSET_N                       114
#define BARD_OP_WRITE_SETTING_CHARACTER_OFFSET_N                     115
#define BARD_OP_WRITE_SETTING_BYTE_OFFSET_N                          116
#define BARD_OP_WRITE_SETTING_COMPOUND_OFFSET_N                      117
#define BARD_OP_WRITE_PROPERTY_OBJECT_OFFSET_N                       118
#define BARD_OP_WRITE_PROPERTY_REAL_OFFSET_N                         119
#define BARD_OP_WRITE_PROPERTY_INTEGER_OFFSET_N                      120
#define BARD_OP_WRITE_PROPERTY_CHARACTER_OFFSET_N                    121
#define BARD_OP_WRITE_PROPERTY_BYTE_OFFSET_N                         122
#define BARD_OP_WRITE_PROPERTY_COMPOUND_OFFSET_N                     123
#define BARD_OP_WRITE_THIS_PROPERTY_OBJECT_OFFSET_N                  124
#define BARD_OP_WRITE_THIS_PROPERTY_REAL_OFFSET_N                    125
#define BARD_OP_WRITE_THIS_PROPERTY_INTEGER_OFFSET_N                 126
#define BARD_OP_WRITE_THIS_PROPERTY_CHARACTER_OFFSET_N               127
#define BARD_OP_WRITE_THIS_PROPERTY_BYTE_OFFSET_N                    128
#define BARD_OP_WRITE_THIS_PROPERTY_COMPOUND_OFFSET_N                129
#define BARD_OP_READ_COMPOUND_OBJECT_OFFSET_A_DELTA_SP_B             130
#define BARD_OP_READ_COMPOUND_REAL_OFFSET_A_DELTA_SP_B               131
#define BARD_OP_READ_COMPOUND_INTEGER_OFFSET_A_DELTA_SP_B            132
#define BARD_OP_READ_COMPOUND_SUBSET_OFFSET_A_DELTA_SP_B             133
#define BARD_OP_READ_THIS_COMPOUND_OBJECT_OFFSET_N                   134
#define BARD_OP_READ_THIS_COMPOUND_REAL_OFFSET_N                     135
#define BARD_OP_READ_THIS_COMPOUND_INTEGER_OFFSET_N                  136
#define BARD_OP_READ_THIS_COMPOUND_SUBSET_OFFSET_A_SLOT_COUNT_B      137
#define BARD_OP_CREATE_OBJECT_OF_TYPE_INDEX_N                        138
#define BARD_OP_READ_SINGLETON_OF_TYPE_INDEX_N                       139
#define BARD_OP_READ_SINGLETON_COMPOUND_OF_TYPE_INDEX_N              140
#define BARD_OP_NATIVE_CALL_INDEX_N                                  141
#define BARD_OP_DYNAMIC_CALL_WITH_N_PARAMETERS                       142
#define BARD_OP_ASPECT_CALL_WITH_N_PARAMETERS                        143
#define BARD_OP_STATIC_CALL_TO_METHOD_INDEX_N                        144
#define BARD_OP_NEGATE_REAL                                          145
#define BARD_OP_NEGATE_INTEGER                                       146
#define BARD_OP_NEGATE_LOGICAL                                       147
#define BARD_OP_LOGICALIZE_OBJECT                                    148
#define BARD_OP_LOGICALIZE_REAL                                      149
#define BARD_OP_LOGICALIZE_INTEGER                                   150
#define BARD_OP_EQ_REAL                                              151
#define BARD_OP_EQ_INTEGER                                           152
#define BARD_OP_NE_REAL                                              153
#define BARD_OP_NE_INTEGER                                           154
#define BARD_OP_LE_REAL                                              155
#define BARD_OP_LE_INTEGER                                           156
#define BARD_OP_GE_REAL                                              157
#define BARD_OP_GE_INTEGER                                           158
#define BARD_OP_LT_REAL                                              159
#define BARD_OP_LT_INTEGER                                           160
#define BARD_OP_GT_REAL                                              161
#define BARD_OP_GT_INTEGER                                           162
#define BARD_OP_IS_OBJECT                                            163
#define BARD_OP_IS_NOT_OBJECT                                        164
#define BARD_OP_ADD_REAL                                             165
#define BARD_OP_ADD_INTEGER                                          166
#define BARD_OP_SUBTRACT_REAL                                        167
#define BARD_OP_SUBTRACT_INTEGER                                     168
#define BARD_OP_MULTIPLY_REAL                                        169
#define BARD_OP_MULTIPLY_INTEGER                                     170
#define BARD_OP_DIVIDE_REAL                                          171
#define BARD_OP_DIVIDE_INTEGER                                       172
#define BARD_OP_MOD_REAL                                             173
#define BARD_OP_MOD_INTEGER                                          174
#define BARD_OP_POWER_REAL                                           175
#define BARD_OP_POWER_INTEGER                                        176
#define BARD_OP_BITWISE_AND_INTEGER                                  177
#define BARD_OP_BITWISE_OR_INTEGER                                   178
#define BARD_OP_BITWISE_XOR_INTEGER                                  179
#define BARD_OP_SHL_INTEGER                                          180
#define BARD_OP_SHR_INTEGER                                          181
#define BARD_OP_SHRX_INTEGER                                         182
#define BARD_OP_LOGICAL_NOT                                          183
#define BARD_OP_LOGICAL_XOR                                          184
#define BARD_OP_BITWISE_NOT_INTEGER                                  185
#define BARD_OP_CREATE_ARRAY_TYPE_N                                  186
#define BARD_OP_ARRAY_COUNT                                          187
#define BARD_OP_READ_ARRAY_ELEMENT_OBJECT                            188
#define BARD_OP_READ_ARRAY_ELEMENT_REAL                              189
#define BARD_OP_READ_ARRAY_ELEMENT_INTEGER                           190
#define BARD_OP_READ_ARRAY_ELEMENT_CHARACTER                         191
#define BARD_OP_READ_ARRAY_ELEMENT_BYTE                              192
#define BARD_OP_READ_ARRAY_ELEMENT_COMPOUND_N_SLOTS                  193
#define BARD_OP_WRITE_ARRAY_ELEMENT_OBJECT                           194
#define BARD_OP_WRITE_ARRAY_ELEMENT_REAL                             195
#define BARD_OP_WRITE_ARRAY_ELEMENT_INTEGER                          196
#define BARD_OP_WRITE_ARRAY_ELEMENT_CHARACTER                        197
#define BARD_OP_WRITE_ARRAY_ELEMENT_BYTE                             198
#define BARD_OP_WRITE_ARRAY_ELEMENT_COMPOUND_N_SLOTS                 199
#define BARD_OP_RESOLVE_POP_DISCARD_COMPOUND_TYPE_N                  200
#define BARD_OP_RESOLVE_READ_SETTING_INDEX_N                         201
#define BARD_OP_RESOLVE_READ_PROPERTY_INDEX_N                        202
#define BARD_OP_RESOLVE_READ_THIS_PROPERTY_INDEX_N                   203
#define BARD_OP_RESOLVE_WRITE_SETTING_INDEX_N                        204
#define BARD_OP_RESOLVE_WRITE_SETTING_COMPOUND_INDEX_N               205
#define BARD_OP_RESOLVE_WRITE_PROPERTY_INDEX_N                       206
#define BARD_OP_RESOLVE_WRITE_PROPERTY_COMPOUND_INDEX_N              207
#define BARD_OP_RESOLVE_WRITE_THIS_PROPERTY_INDEX_N                  208
#define BARD_OP_RESOLVE_INCREMENT_PROPERTY                           209
#define BARD_OP_RESOLVE_INCREMENT_THIS_PROPERTY                      210
#define BARD_OP_RESOLVE_DECREMENT_PROPERTY                           211
#define BARD_OP_RESOLVE_DECREMENT_THIS_PROPERTY                      212
#define BARD_OP_RESOLVE_ADD_ASSIGN_PROPERTY_REAL_INDEX_N             213
#define BARD_OP_RESOLVE_ADD_ASSIGN_PROPERTY_INTEGER_INDEX_N          214
#define BARD_OP_RESOLVE_SUBTRACT_ASSIGN_PROPERTY_REAL_INDEX_N        215
#define BARD_OP_RESOLVE_SUBTRACT_ASSIGN_PROPERTY_INTEGER_INDEX_N     216
#define BARD_OP_RESOLVE_ADD_ASSIGN_THIS_PROPERTY_REAL_INDEX_N        217
#define BARD_OP_RESOLVE_ADD_ASSIGN_THIS_PROPERTY_INTEGER_INDEX_N     218
#define BARD_OP_RESOLVE_SUBTRACT_ASSIGN_THIS_PROPERTY_REAL_INDEX_N   219
#define BARD_OP_RESOLVE_SUBTRACT_ASSIGN_THIS_PROPERTY_INTEGER_INDEX_N 220
#define BARD_OP_LAST_OP                                              221

extern const char* bard_op_names[];

#endif // BARD_OPCODES_H