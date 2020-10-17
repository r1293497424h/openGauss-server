select  
  pg_catalog.sysdate() as c0, 
  pg_catalog.sysdate() as c1
  from
  pg_catalog.gs_wlm_ec_operator_info as sample_0
  left join (select  
  case when (select pg_catalog.max(start_date) from pg_catalog.pg_job)
  = (select start_date from pg_catalog.pg_job limit 1 )
  then ref_0.nodename else ref_0.nodename end
  as c0
  from
  pg_catalog.gs_total_memory_detail as ref_0
  where false
  limit 121) as subq_0
  on (pg_catalog.polygon(
  cast(cast(null as circle) as circle)) |>> case when sample_0.average_peak_memory > cast(null as int8) then cast(null as polygon) else cast(null as polygon) end
  )
  where true;

select  pg_catalog.max(
      cast(cast(null as "date") as "date")) over (partition by ref_0.catchup_start order by ref_0.peer_role) as c0, 
      pg_catalog.inet_server_port() as c1, 
      ref_0.peer_role as c2, 
      ref_0.lwpid as c3, 
      pg_catalog.timeofday() as c4, 
      ref_0.catchup_end as c5, 
     case when true then ref_0.catchup_end else ref_0.catchup_end end
     as c6, 
     ref_0.catchup_start as c7
     from 
     pg_catalog.pg_get_senders_catchup_time as ref_0
     where cast(coalesce(pg_catalog.int4_bpchar(
     cast(ref_0.lwpid as int4)),
     pg_catalog.int1_bpchar(
     cast((select pg_catalog.bit_and(attcmprmode) from pg_catalog.pg_attribute)
     as int1))) as bpchar) < case when case when ref_0.catchup_end >= case when cast(null as circle) <> cast(null as circle) then cast(null as "date") else cast(null as "date") end
     then case when EXISTS (
     select  
     ref_1.ftoptions as c0, 
     ref_1.xc_node_id as c1, 
     ref_0.type as c2, 
     ref_0.catchup_end as c3, 
     ref_0.pid as c4, 
     ref_1.ftwriteonly as c5, 
     ref_0.catchup_start as c6, 
     ref_0.catchup_start as c7, 
     ref_0.pid as c8, 
     ref_1.xc_node_id as c9, 
     ref_1.ftserver as c10, 
     61 as c11
     from 
     pg_catalog.pg_foreign_table as ref_1
     where cast(null as polygon) >> cast(null as polygon)) then cast(null as raw) else cast(null as raw) end
     else case when EXISTS (
     select  ref_1.ftoptions as c0, 
             ref_1.xc_node_id as c1, 
             ref_0.type as c2, 
             ref_0.catchup_end as c3, 
             ref_0.pid as c4, 
             ref_1.ftwriteonly as c5, 
             ref_0.catchup_start as c6, 
             ref_0.catchup_start as c7, 
             ref_0.pid as c8, 
             ref_1.xc_node_id as c9, 
             ref_1.ftserver as c10, 
             61 as c11
             from 
             pg_catalog.pg_foreign_table as ref_1
             where cast(null as polygon) >> cast(null as polygon)) then cast(null as raw) else cast(null as raw) end
             end
             = cast(null as raw) then cast(null as bpchar) else cast(null as bpchar) end
             limit 77;

select  
    sample_0.adnum as c0, 
    (select starelid from pg_catalog.pg_statistic limit 1 offset 2) as c1, 
    subq_0.c0 as c2, 
    sample_0.adnum as c3, 
    subq_0.c0 as c4, 
    sample_0.adrelid as c5
from 
    pg_catalog.pg_attrdef as sample_0 tablesample system (7.1) 
    left join 
      (select  
          (select schemaname from pg_catalog.pg_stat_xact_sys_tables limit 1 offset 3)  as c0, 
          ref_0.catalog_name as c1
       from 
            information_schema.information_schema_catalog_name as ref_0
       where 
           (cast(null as circle) ~ cast(null as point)) 
           or 
           (
            (select group_buckets from pg_catalog.pgxc_group limit 1 offset 3)
             ~>~ 
            (select pg_catalog.listagg(null_frac) from pg_catalog.pg_stats)
            )
      ) as subq_0
on (case when true then cast(null as circle) 
         else cast(null as circle) end
    <> cast(coalesce(cast(null as circle),
       cast(null as circle)) as circle))
where 
    pg_catalog.now() <= pg_catalog.pg_systimestamp()