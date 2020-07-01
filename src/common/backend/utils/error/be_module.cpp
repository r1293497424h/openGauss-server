/*
 * Copyright (c) 2020 Huawei Technologies Co.,Ltd.
 *
 * openGauss is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *
 *          http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 * -------------------------------------------------------------------------
 *
 * be_module.cpp
 *
 * IDENTIFICATION
 *    src/common/backend/utils/error/be_module.cpp
 *
 * -------------------------------------------------------------------------
 */
#include "postgres.h"
#include "knl/knl_variable.h"

#include "gs_threadlocal.h"
#include "port.h"
#include "utils/be_module.h"

const module_data module_map[] = {{MOD_ALL, "ALL"},
    /* add your module name following */
    {MOD_DFS, "DFS"},
    {MOD_GUC, "GUC"},
    {MOD_HDFS, "HDFS"},
    {MOD_ORC, "ORC"},
    {MOD_SLRU, "SLRU"},
    {MOD_MEM, "MEM_CTL"},
    {MOD_AUTOVAC, "AUTOVAC"},
    {MOD_CACHE, "CACHE"},
    {MOD_ADIO, "ADIO"},
    {MOD_SSL, "SSL"},
    {MOD_GDS, "GDS"},
    {MOD_TBLSPC, "TBLSPC"},
    {MOD_WLM, "WLM"},
    {MOD_OBS, "OBS"},
    {MOD_EXECUTOR, "EXECUTOR"},
    {MOD_OPFUSION, "OPFUSION"},
    {MOD_GPC, "GPC"},
    {MOD_VEC_EXECUTOR, "VEC_EXECUTOR"},
    {MOD_STREAM, "STREAM"},
    {MOD_LLVM, "LLVM"},
    {MOD_OPT, "OPT"},
    {MOD_OPT_REWRITE, "OPT_REWRITE"},
    {MOD_OPT_JOIN, "OPT_JOIN"},
    {MOD_OPT_AGG, "OPT_AGG"},
    {MOD_OPT_SUBPLAN, "OPT_SUBPLAN"},
    {MOD_OPT_SETOP, "OPT_SETOP"},
    {MOD_OPT_SKEW, "OPT_SKEW"},
    {MOD_UDF, "UDF"},
    {MOD_COOP_ANALYZE, "COOP_ANALYZE"},
    {MOD_WLM_CP, "WLMCP"},
    {MOD_ACCELERATE, "ACCELERATE"},
    {MOD_MM, "MM"},
    {MOD_PARQUET, "PARQUET"},
    {MOD_PLANHINT, "PLANHINT"},
    {MOD_TRANS_SNAPSHOT, "SNAPSHOT"},
    {MOD_TRANS_XACT, "XACT"},
    {MOD_TRANS_HANDLE, "HANDLE"},
    {MOD_TRANS_CLOG, "CLOG"},
    {MOD_EC, "EC"},
    {MOD_REMOTE, "REMOTE"},
    {MOD_CN_RETRY, "CN_RETRY"},
    {MOD_PLSQL, "PLSQL"},
    {MOD_TS, "TEXTSEARCH"},
    {MOD_SEQ, "SEQ"},
    {MOD_REDO, "REDO"},
    {MOD_FUNCTION, "FUNCTION"},
    {MOD_PARSER, "PARSER"},
    {MOD_INSTR, "INSTR"},
    {MOD_INCRE_CKPT, "INCRE_CKPT"},
    {MOD_DW, "DBL_WRT"},
    {MOD_RTO, "RTO"},
    {MOD_HEARTBEAT, "HEARTBEAT"},
    {MOD_COMM_IPC, "COMM_IPC"},
    {MOD_COMM_PARAM, "COMM_PARAM"},
    {MOD_OPT_AI, "OPT_AI"},

    /* add your module name above */
    {MOD_MAX, "BACKEND"}};

/*
 * @Description: check whether module name is unique
 * @Return: if two modules have the same name, return false;
 *          otherwise any module name is unique, return true;
 * @See also:
 */
bool check_module_name_unique(void)
{
    for (int i = 0; i <= (int)MOD_MAX; ++i) {
        for (int j = i + 1; j <= (int)MOD_MAX; ++j) {
            if (0 == pg_strncasecmp(module_map[i].mod_name, module_map[j].mod_name, MODULE_NAME_MAXLEN)) {
                /* Oops, two module have the same name. */
                return false;
            }
        }
    }
    return true;
}

/*
 * @Description: find a module's id according to its name.
 *    1. module name is case-insensitive;
 *    2. simple line search is adopted because module_map[] is not so big;
 * @IN module_name: module name
 * @Return: module id
 * @See also:
 */
ModuleId get_module_id(const char* module_name)
{
    for (int mde_id = 0; mde_id < (int)MOD_MAX; ++mde_id) {
        if (0 == pg_strncasecmp(module_map[mde_id].mod_name, module_name, MODULE_NAME_MAXLEN)) {
            return (ModuleId)mde_id;
        }
    }
    /* invalid module id */
    return MOD_MAX;
}

/* 1 byte --> 8 bit, so byte position is (_m/8) */
#define BEMD_BITMAP_POS(_m) (((unsigned int)(_m)) >> 3)

/* BEMD_BITMAP_OFF() should be in [0,7] */
#define BEMD_BITMAP_OFF(_m) (((unsigned int)(_m)) & 0x07)

/* mask is 2^x where x is in [0, 7] */
#define BEMD_MASK(_m) ((unsigned char)(1 << BEMD_BITMAP_OFF(_m)))

/*
 * @Description: enable or disable all modules logging
 * @IN turn_on: true, enable all these modules logging;
 *             false, disable all these modules logging;
 * @See also:
 */
static void module_logging_init(bool turn_on)
{
    const unsigned char v = turn_on ? 0xFF : 0x00;
    for (int i = 0; i < (int)BEMD_BITMAP_SIZE; ++i) {
        u_sess->log_cxt.module_logging_configure[i] = v;
    }
}

/*
 * @Description: Given module id, query whether its logging is enable.
 * @IN module_id: module id
 * @Return: enable --> true; disable --> false;
 * @See also:
 */
bool module_logging_is_on(ModuleId module_id)
{
    /* MOD_MAX is a special id. at default it's on.
     * after 'off(ALL)' is set, it is turned off;
     * after 'on(ALL)' is set, it is switched to be on again.
     */
    return (0 != (BEMD_MASK(module_id) & u_sess->log_cxt.module_logging_configure[BEMD_BITMAP_POS(module_id)]));
}

/*
 * @Description: enable the module logging.
 * @IN module_id: module id
 * @See also:
 */
static inline void enable_module_logging(ModuleId module_id)
{
    u_sess->log_cxt.module_logging_configure[BEMD_BITMAP_POS(module_id)] |= BEMD_MASK(module_id);
}

/*
 * @Description: disable the module logging.
 * @IN module_id: module id
 * @See also:
 */
static inline void disable_module_logging(ModuleId module_id)
{
    u_sess->log_cxt.module_logging_configure[BEMD_BITMAP_POS(module_id)] &= (~BEMD_MASK(module_id));
}

/*
 * @Description: batch enable/disable modules logging.
 * @IN apply_all_modules: enable/disable all these existing modules, its a fastpath.
 * @IN turn_on: enable module logging which is in mods.
 * @IN mods: module id array
 * @IN nmods: size of module id array
 * @See also:
 */
void module_logging_batch_set(ModuleId* mods, int nmods, bool turn_on, bool apply_all_modules)
{
    if (apply_all_modules) {
        module_logging_init(turn_on);
    } else if (turn_on) {
        for (int i = 0; i < nmods; ++i) {
            enable_module_logging(mods[i]);
        }
    } else {
        for (int i = 0; i < nmods; ++i) {
            disable_module_logging(mods[i]);
        }
    }
}

/*
 * @Description: comm log, enable the module logging.
 * @IN module_id: module id
 * @See also:
 */
void module_logging_enable_comm(ModuleId module_id)
{
    enable_module_logging(module_id);
}
