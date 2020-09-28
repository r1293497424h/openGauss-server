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
 * ---------------------------------------------------------------------------------------
 *
 * multi_redo_settings.h
 *
 *
 * IDENTIFICATION
 *        src/include/access/multi_redo_settings.h
 *
 * ---------------------------------------------------------------------------------------
 */

#ifndef MULTI_REDO_SETTINGS_H
#define MULTI_REDO_SETTINGS_H


/* if you change MOST_FAST_RECOVERY_LIMIT, remember to change max_recovery_parallelism in cluster_guc.conf */
static const int MOST_FAST_RECOVERY_LIMIT = 20;
static const int MAX_PARSE_WORKERS = 16;
static const int MAX_REDO_WORKERS_PER_PARSE = 8;




static const int TRXN_REDO_MANAGER_NUM = 1;
static const int TRXN_REDO_WORKER_NUM = 1;
static const int XLOG_READER_NUM = 3;

static const int MAX_EXTREME_THREAD_NUM = MAX_PARSE_WORKERS * MAX_REDO_WORKERS_PER_PARSE + MAX_PARSE_WORKERS + 
    MAX_PARSE_WORKERS + TRXN_REDO_MANAGER_NUM + TRXN_REDO_WORKER_NUM + XLOG_READER_NUM;

static const int MAX_RECOVERY_THREAD_NUM = (MAX_EXTREME_THREAD_NUM > MOST_FAST_RECOVERY_LIMIT) ? 
    MAX_EXTREME_THREAD_NUM : MOST_FAST_RECOVERY_LIMIT;

void ConfigRecoveryParallelism();


#endif
